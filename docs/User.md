# CAN 电机协议使用手册

## 一、协议概述

本协议用于上位机 (MSPM0G3507) 通过 CAN 总线控制远程电机节点。支持两类电机：

| 电机类型 | 控制方式 | 从机能力 |
|----------|---------|----------|
| 步进电机 | 位置/速度/定时转动 | 从机执行运动规划 + 编码器闭环 |
| DC 减速电机 | 开环占空比 / 闭环转速 | 从机自带 PID + 编码器 |

通信模式：**上位机主动发命令，从机被动应答状态**。上位机不等待应答，状态通过 1ms 轮询收集。

---

## 二、CAN ID 分配

```
DC 电机:    0x100 + nodeId     例: 0x101 (节点1), 0x102 (节点2)
步进电机:   0x200 + nodeId     例: 0x201 (节点1), 0x202 (节点2)
```

- 同一 ID 双向复用：上位机发命令用此 ID，从机回状态也用此 ID
- 高位区分电机类型（0x1xx = DC，0x2xx = 步进），低位是节点编号
- 两个设备可以有相同的 nodeId 只要类型不同（例：DC#1=0x101, 步进#1=0x201）

---

## 三、CAN 帧格式

### 标准 8 字节

```
┌──────┬──────────┬──────────┬──────────┬──────────┐
│ [0]  │  [1-2]   │  [3-4]   │  [5-6]   │   [7]    │
│ cmd  │   p1     │   p2     │   p3     │ checksum │
│ u8   │  i16 BE  │  i16 BE  │  i16 BE  │   u8     │
└──────┴──────────┴──────────┴──────────┴──────────┘
```

- **cmd**: 指令码 (1 字节)
- **p1/p2/p3**: 参数，int16_t，大端序 (高字节在前)
- **checksum**: `XOR(字节0,1,2,3,4,5,6)`

### 帧编解码

```cpp
// 编码: 参数 → CAN 帧
uint8_t d[8];
buildMotorFrame(d, CMD_SPEED, rpm*10, 0, 0);
bsp_can.send(0x201, d, 8);

// 解码: CAN 帧 → 状态
StepperStatus st;
parseStepperStatus(d, &st);
if (st.ckOk) { ... }
```

---

## 四、指令码全集

### 步进电机命令

| 指令 | 码 | p1 | p2 | p3 | 说明 |
|------|-----|----|----|-----|------|
| `CMD_SPEED` | 0x01 | rpm×10 | 0 | 0 | 恒速转动，正转→正数，反转→负数 |
| `CMD_POSITION` | 0x02 | 角度°×100 | 圈数 | 限速rpm | 定位转动 |
| `CMD_STOP` | 0x03 | 0 | 0 | 0 | 停止 (步进+DC共用) |
| `CMD_ENABLE` | 0x04 | 1/0 | 0 | 0 | 使能/失能 (共用) |
| `CMD_QUERY` | 0x05 | 子码 | 0 | 0 | 查询状态 (共用) |
| `CMD_TIMED` | 0x06 | 角度°×100 | 时长ms | 0 | 定时转动 |

**子查询码** (CMD_QUERY 的 p1):

| 子码 | 含义 |
|------|------|
| 0x00 | 查询全部 |
| 0x01 | 仅查角度 |
| 0x02 | 仅查转速 |
| 0x03 | 仅查圈数 |

### DC 减速电机命令

| 指令 | 码 | p1 | p2 | p3 | 说明 |
|------|-----|----|----|-----|------|
| `CMD_DUTY` | 0x10 | 占空比 0~1000 | 0 | 0 | 开环控制，1000=100% |
| `CMD_RPM` | 0x11 | 目标rpm×10 | 0 | 0 | 闭环控制，从机自算PID |
| `CMD_STOP` | 0x03 | 0 | 0 | 0 | 停止 |
| `CMD_ENABLE` | 0x04 | 1/0 | 0 | 0 | 使能/失能 |
| `CMD_QUERY` | 0x05 | 子码 | 0 | 0 | 查询状态 |

---

## 五、状态帧

### 步进电机状态 (从机 → 上位机)

```
┌──────┬──────────┬──────────┬──────────┬──────────┐
│ [0]  │  [1-2]   │  [3-4]   │  [5-6]   │   [7]    │
│state │ angle×100│ rpm×10   │  turns   │ checksum │
└──────┴──────────┴──────────┴──────────┴──────────┘
```

| 字段 | 说明 |
|------|------|
| state | 0=空闲, 1=运动中, 2=已到位, 3=故障 |
| angle | 当前角度 (°) ×100 |
| rpm | 当前转速 (RPM) ×10 |
| turns | 累计圈数 (正转递增, 反转递减) |

```cpp
StepperStatus st;
parseStepperStatus(d, &st);
// st.angle  → 当前角度
// st.rpm    → 当前转速
// st.isDone()   → 是否到位
// st.isMoving() → 是否运动中
```

### DC 电机状态 (从机 → 上位机)

```
┌──────┬──────┬──────────┬──────────┐
│ [0]  │ [1]  │  [2-6]   │   [7]    │
│state │ dir  │  保留    │ checksum │
└──────┴──────┴──────────┴──────────┘
```

| 字段 | 说明 |
|------|------|
| state | 0=idle, 1=running, 2=fault |
| dir | 0=停, 1=CW(正转), 2=CCW(反转) |
| 2-6 | 保留, 后续扩展 |

> 设计意图: 从机自己跑 PID，上位机只管发指令。状态帧极简，后续需要编码器/转速可扩展字段。

---

## 六、代码使用

### 初始化

```cpp
#include "can_stepper_motor.hpp"
#include "can_dc_motor.hpp"

// CAN 发送桥接
static bool canSend(uint32_t id, const uint8_t *d, uint8_t len) {
    return bsp_can.send(id, d, len);
}

// 设备实例
StepperMotor g_step1, g_step2;
DcMotor      g_dc1, g_dc2;

// 路由表
static MotorEntry g_devices[] = {
    {MT_STEPPER, &g_step1},
    {MT_STEPPER, &g_step2},
    {MT_DC,      &g_dc1},
    {MT_DC,      &g_dc2},
};

void initMotors() {
    bsp_can.init();
    bsp_can.enable_irq();

    g_step1.init({1, 0x200, canSend});  // → CAN ID 0x201
    g_step2.init({2, 0x200, canSend});  // → CAN ID 0x202
    g_dc1.init(  {1, 0x100, canSend});  // → CAN ID 0x101
    g_dc2.init(  {2, 0x100, canSend});  // → CAN ID 0x102
}
```

### 每 1ms 收状态

```cpp
// Ctrl 任务
uint32_t rx_id;
uint8_t  d[8], len;
while (bsp_can.recv_read(&rx_id, d, &len))
    motorCanDispatch(rx_id, d, g_devices, 4);
```

### 控制电机

```cpp
// === 步进 ===
g_step1.enable();
g_step1.setSpeed(60);               // 恒速 60 RPM
g_step1.moveTo(90);                 // 就近转到 90°
g_step1.moveAbs(180, 3, 45);        // 绝对: 3圈180°, 限速45 RPM
g_step1.timedMove(90, 2.0f);        // 2秒内转到 90°
g_step1.stop();
g_step1.disable();

// 读状态
float angle = g_step1.getAngle();   // 当前角度
float rpm   = g_step1.getSpeed();   // 当前转速
bool  done  = g_step1.isDone();     // 到位?

// === DC 电机 ===
g_dc1.enable();
g_dc1.setDuty(300);                 // 30% 占空比 (开环)
g_dc1.setRpm(150);                  // 150 RPM (闭环, 从机算 PID)
g_dc1.stop();
g_dc1.disable();

// 读状态
uint8_t state = g_dc1.getState();       // 0=idle 1=running 2=fault
uint8_t dir   = g_dc1.getDirection();   // 0=停 1=CW 2=CCW
bool running  = g_dc1.isRunning();
```

---

## 七、CAN 帧示例 (裸字节)

### 步进

| 操作 | CAN ID | 数据帧 | 说明 |
|------|--------|--------|------|
| 使能 | 0x201 | `04 00 01 00 00 00 00 05` | cmd=0x04, p1=1 |
| 正转 60 RPM | 0x201 | `01 02 58 00 00 00 00 5B` | cmd=0x01, p1=600 |
| 反转 60 RPM | 0x201 | `01 FD A8 00 00 00 00 54` | p1=-600 |
| 停止 | 0x201 | `03 00 00 00 00 00 00 03` | cmd=0x03 |
| 失能 | 0x201 | `04 00 00 00 00 00 00 04` | p1=0 |
| GOTO 90° | 0x201 | `02 23 28 00 00 00 00 09` | p1=9000 |
| GOTO 180° | 0x201 | `02 46 50 00 00 00 00 14` | p1=18000 |
| 3圈90°,限速60 | 0x201 | `02 23 28 00 03 00 3C 36` | p1=9000, p2=3, p3=60 |
| 2秒到90° | 0x201 | `06 23 28 07 D0 00 00 DA` | p1=9000, p2=2000ms |
| 查询全部 | 0x201 | `05 00 00 00 00 00 00 05` | cmd=0x05, p1=0 |

### DC 电机

| 操作 | CAN ID | 数据帧 | 说明 |
|------|--------|--------|------|
| 使能 | 0x101 | `04 00 01 00 00 00 00 05` | 同步进 |
| 50%占空比 | 0x101 | `10 01 F4 00 00 00 00 E5` | cmd=0x10, p1=500 |
| 100%占空比 | 0x101 | `10 03 E8 00 00 00 00 FB` | p1=1000 |
| 120 RPM | 0x101 | `11 04 B0 00 00 00 00 A5` | cmd=0x11, p1=1200 |
| 停止 | 0x101 | `03 00 00 00 00 00 00 03` | 同步进 |

---

## 八、新增电机类型

1. 在 `can_motor_proto.hpp` 加命令码
2. 在 `can_motor_proto.hpp` 加状态结构体
3. 在 `can_motor_proto.cpp` 加 `parseXxxStatus()` 实现
4. 新建 `Device/can_xxx_motor.hpp/.cpp` 设备类
5. 在 `can_motor_proto.hpp` 的 `MotorType` 枚举加一行
6. 在 `can_motor_proto.cpp` 的 `motorCanDispatch()` 加一个 case

---

## 九、文件速查

| 文件 | 内容 |
|------|------|
| `Protocol/can_motor_proto.hpp` | 指令码、帧格式、Status 结构体、MotorEntry、dispatch 声明 |
| `Protocol/can_motor_proto.cpp` | buildMotorFrame、parseXxxStatus、motorCanDispatch |
| `Device/can_stepper_motor.hpp` | StepperMotor 接口 |
| `Device/can_stepper_motor.cpp` | StepperMotor 实现 |
| `Device/can_dc_motor.hpp` | DcMotor 接口 |
| `Device/can_dc_motor.cpp` | DcMotor 实现 |
| `App/main.cpp` | 设备实例 + 路由表 + initMotors() |
