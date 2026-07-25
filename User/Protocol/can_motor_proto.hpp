/**
 * @file can_motor_proto.hpp
 * @brief 统一 CAN 电机帧协议 — 步进 + DC 减速电机
 *
 * ## 帧格式 (固定 8 字节)
 *
 *    [0]     cmd       指令码
 *    [1-2]   p1        int16_t, 大端 (MSB 在前)
 *    [3-4]   p2        int16_t, 大端
 *    [5-6]   p3        int16_t, 大端
 *    [7]     checksum  XOR(字节 0..6)
 *
 * ## CAN ID 分配
 *
 *    DC 电机    0x100 + nodeId   例: 0x101, 0x102 ...
 *    步进电机   0x200 + nodeId   例: 0x201, 0x202 ...
 *
 *    同一 ID 双向复用 —— 上位机发命令用此 ID, 从机回状态也用此 ID。
 *    靠 ID 高位区分电机类型, 靠低位 nodeId 区分编号。
 *
 * ## 通信模式
 *
 *    上位机 (本机) 主动发命令帧, 从机被动应答状态帧。
 *    上位机不等待应答 —— 命令发出即返回, 状态通过 Ctrl 任务 1ms 轮询收集。
 *    从机自己跑 PID 闭环, 上位机只管发目标值。
 *
 * ## 指令码一览
 *
 *    步进 (0x0x):
 *      0x01  SPEED     恒速转动        p1 = rpm × 10
 *      0x02  POSITION  角度定位        p1 = 角度° × 100, p2 = 圈数, p3 = 最高 rpm
 *      0x03  STOP      停止            (步进+DC 共用)
 *      0x04  ENABLE    使能/失能      p1 = 1 使能 / 0 失能 (共用)
 *      0x05  QUERY     查询状态        (共用)
 *      0x06  TIMED     定时转动        p1 = 角度° × 100, p2 = 时长 ms
 *
 *    DC (0x1x):
 *      0x10  DUTY      开环占空比      p1 = 0~1000 (0.0% ~ 100.0%)
 *      0x11  RPM       闭环转速        p1 = 目标 rpm × 10, 从机自算 PID
 */

#ifndef __CAN_MOTOR_PROTO_HPP__
#define __CAN_MOTOR_PROTO_HPP__

#include <stdint.h>
#include <stdbool.h>

/* ================================================================
 *  指令码
 * ================================================================ */

enum : uint8_t {
    /* ---- 步进电机 (0x0x) ---- */
    CMD_SPEED    = 0x01,  ///< 恒速,        p1=rpm×10
    CMD_POSITION = 0x02,  ///< 角度定位,    p1=角度°×100, p2=圈数, p3=限速rpm
    CMD_STOP     = 0x03,  ///< 停止         (步进+DC 共用)
    CMD_ENABLE   = 0x04,  ///< 使能/失能    p1=1使能 0失能 (共用)
    CMD_QUERY    = 0x05,  ///< 查询状态     (共用)
    CMD_TIMED    = 0x06,  ///< 定时转动     p1=角度°×100, p2=时长ms

    /* ---- DC 减速电机 (0x1x) ---- */
    CMD_DUTY     = 0x10,  ///< 开环占空比    p1=0~1000 (0.0%~100.0%)
    CMD_RPM      = 0x11,  ///< 闭环转速      p1=目标rpm×10, 从机PID自调速
};

/** QUERY 指令的子查询码 */
enum : uint8_t {
    QUERY_ALL   = 0x00,  ///< 查询全部状态
    QUERY_ANGLE = 0x01,  ///< 仅查角度 (步进)
    QUERY_SPEED = 0x02,  ///< 仅查转速
    QUERY_TURNS = 0x03,  ///< 仅查圈数 (步进)
};

/* ================================================================
 *  CAN ID 计算 (inline, 零开销)
 * ================================================================ */

/** 命令/状态帧 CAN ID: base + nodeId */
inline uint32_t motor_cmd_id(uint8_t nodeId)  { return 0x100 + nodeId; }
inline uint32_t motor_stat_id(uint8_t nodeId) { return 0x200 + nodeId; }

/* ================================================================
 *  帧编解码辅助 (inline)
 * ================================================================ */

/** XOR 校验: 字节 0~6 异或 → 填入字节 7 */
inline uint8_t can_checksum(const uint8_t d[8])
    { return d[0] ^ d[1] ^ d[2] ^ d[3] ^ d[4] ^ d[5] ^ d[6]; }

/** 将 int16_t 以大端写入 buf[pos] 和 buf[pos+1] */
inline void pack_i16(uint8_t *buf, int pos, int16_t v)
    { buf[pos] = (uint8_t)(v >> 8); buf[pos+1] = (uint8_t)v; }

/** 从 buf[pos] 和 buf[pos+1] 以大端读出 int16_t */
inline int16_t unpack_i16(const uint8_t *buf, int pos)
    { return (int16_t)(((uint16_t)buf[pos] << 8) | buf[pos+1]); }

/** 构建 CAN 命令帧: cmd + 3个参数 + checksum → d[8] */
void build_motor_frame(uint8_t d[8], uint8_t cmd,
                     int16_t p1, int16_t p2, int16_t p3);

/* ================================================================
 *  步进电机状态帧
 *
 *  帧布局: [0]state [1-2]angle×100 [3-4]rpm×10 [5-6]turns [7]ck
 * ================================================================ */

struct StepperStatus
{
    uint8_t state = 0;   ///< 0=空闲 1=运动中 2=已到位 3=故障
    float   angle = 0;   ///< 当前角度 °
    float   rpm   = 0;   ///< 当前转速 RPM
    int16_t turns = 0;   ///< 累计圈数 (正转递增, 反转递减)
    bool    ck_ok  = false; ///< 校验和正确?

    bool is_done()   const { return ck_ok && state == 2; }   ///< 是否已到位
    bool is_moving() const { return ck_ok && state == 1; }   ///< 是否运动中
};

/** 解析步进状态帧 d[8] → StepperStatus */
void parse_stepper_status(const uint8_t d[8], StepperStatus *st);

/* ================================================================
 *  DC 减速电机状态帧 (精简版)
 *
 *  帧布局: [0]state [1]dir [2-6]保留 [7]ck
 *
 *  设计意图: 从机自己跑 PID, 上位机只管发指令。
 *           状态帧只回状态+方向确认存活, 后续可按需扩展。
 * ================================================================ */

struct DcMotorStatus
{
    uint8_t state = 0;   ///< 0=idle (停)  1=running (运行)  2=fault (故障)
    uint8_t dir   = 0;   ///< 0=停  1=CW (正转/前进)  2=CCW (反转/后退)
    bool    _ck_ok  = false; ///< 校验和正确?

    bool is_running() const { return _ck_ok && state == 1; } ///< 是否运行中
};

/** 解析 DC 状态帧 d[8] → DcMotorStatus */
void parse_dc_motor_status(const uint8_t d[8], DcMotorStatus *st);

/* ================================================================
 *  设备路由表 (无虚函数, 用类型标记 + void*)
 * ================================================================ */

/** 电机类型标记 */
enum MotorType : uint8_t {
    MT_STEPPER = 0,  ///< 步进电机
    MT_DC      = 1,  ///< DC 减速电机
};

/** 路由表条目: 一个 CAN 设备 */
struct MotorEntry {
    MotorType type;    ///< 电机类型
    void     *ptr;     ///< 指向 StepperMotor* 或 DcMotor*
};

/** 收帧 → 按 can_id 匹配 → 分发 (定义在 .cpp) */
void motor_can_dispatch(uint32_t id, const uint8_t d[8],
                      MotorEntry *entries, uint8_t count);

#endif
