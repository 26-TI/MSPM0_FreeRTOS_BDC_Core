/**
 * @file motor_proto_test.cpp
 * @brief CAN 电机协议测试 — 软件验证 + 回环测试
 *
 * 测试内容:
 *   1. 软件验证: 帧编码/解码、校验和、所有指令码
 *   2. DcMotor 回环: enable / set_duty / set_rpm / stop
 *   3. StepperMotor 回环: enable / set_speed / move_to / move_abs / timed_move / stop
 *
 * @note 使用 CAN 回环模式，无需外部硬件。
 *       测试结果通过 UART2 输出。
 */

#include "test.hpp"
#include "bsp_cfg.hpp"
#include "can_motor_proto.hpp"
#include "can_dc_motor.hpp"
#include "can_stepper_motor.hpp"

#include "task.h"
#include <string.h>

/* ---- CAN 发送回调 (桥接 BspCan 成员函数到 MotorDevice::SendFn 函数指针) ---- */

static bool canSend(uint32_t id, const uint8_t *data, uint8_t len)
{
  return bsp_can.send(id, data, len);
}

/* ---- 测试计数器 ---- */

static uint32_t g_pass = 0;
static uint32_t g_fail = 0;

#define TEST_ASSERT(cond, msg)        \
  do                                  \
  {                                   \
    if (cond)                         \
    {                                 \
      g_pass++;                       \
    }                                 \
    else                              \
    {                                 \
      g_fail++;                       \
      bsp_uart4.uart_printf(          \
        "  FAIL [%lu]: %s\r\n",       \
        g_pass + g_fail, msg);        \
    }                                 \
  } while (0)

/* ================================================================
 *  Phase 1: 软件层帧编解码验证 (不涉及 CAN 硬件)
 * ================================================================ */

static void test_frame_encode_decode(void)
{
  bsp_uart4.uart_printf("\r\n--- Phase 1: Frame Encode/Decode ---\r\n");

  /* ---- 1.1 build_motor_frame + 校验和 ---- */
  {
    bsp_uart4.uart_printf("[1.1] build_motor_frame + checksum\r\n");
    uint8_t d[8];

    // CMD_SPEED (0x01): p1=600 (60.0 RPM)
    build_motor_frame(d, CMD_SPEED, 600, 0, 0);
    TEST_ASSERT(d[0] == CMD_SPEED, "SPEED cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "SPEED checksum");

    // CMD_POSITION (0x02): p1=9000 (90.00°), p2=3圈, p3=45 RPM
    build_motor_frame(d, CMD_POSITION, 9000, 3, 45);
    TEST_ASSERT(d[0] == CMD_POSITION, "POSITION cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "POSITION checksum");

    // CMD_STOP (0x03)
    build_motor_frame(d, CMD_STOP, 0, 0, 0);
    TEST_ASSERT(d[0] == CMD_STOP, "STOP cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "STOP checksum");

    // CMD_ENABLE (0x04): p1=1 (使能)
    build_motor_frame(d, CMD_ENABLE, 1, 0, 0);
    TEST_ASSERT(d[0] == CMD_ENABLE, "ENABLE cmd byte");
    TEST_ASSERT(d[1] == 0 && d[2] == 1, "ENABLE p1=1 (大端)");
    TEST_ASSERT(d[7] == can_checksum(d), "ENABLE checksum");

    // CMD_ENABLE: p1=0 (失能)
    build_motor_frame(d, CMD_ENABLE, 0, 0, 0);
    TEST_ASSERT(d[0] == CMD_ENABLE, "DISABLE cmd byte");
    int16_t p1 = unpack_i16(d, 1);
    TEST_ASSERT(p1 == 0, "DISABLE p1=0");

    // CMD_QUERY (0x05)
    build_motor_frame(d, CMD_QUERY, 0, 0, 0);
    TEST_ASSERT(d[0] == CMD_QUERY, "QUERY cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "QUERY checksum");

    // CMD_TIMED (0x06): p1=9000 (90.00°), p2=2000 (2000ms)
    build_motor_frame(d, CMD_TIMED, 9000, 2000, 0);
    TEST_ASSERT(d[0] == CMD_TIMED, "TIMED cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "TIMED checksum");

    // CMD_DUTY (0x10): p1=500 (50.0%)
    build_motor_frame(d, CMD_DUTY, 500, 0, 0);
    TEST_ASSERT(d[0] == CMD_DUTY, "DUTY cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "DUTY checksum");

    // CMD_RPM (0x11): p1=1200 (120.0 RPM)
    build_motor_frame(d, CMD_RPM, 1200, 0, 0);
    TEST_ASSERT(d[0] == CMD_RPM, "RPM cmd byte");
    TEST_ASSERT(d[7] == can_checksum(d), "RPM checksum");
  }

  /* ---- 1.2 大端 int16 编解码往返 ---- */
  {
    bsp_uart4.uart_printf("[1.2] int16 big-endian pack/unpack\r\n");
    uint8_t buf[2];

    pack_i16(buf, 0, 0x1234);
    TEST_ASSERT(buf[0] == 0x12 && buf[1] == 0x34, "pack_i16 0x1234");
    TEST_ASSERT(unpack_i16(buf, 0) == 0x1234, "unpack_i16 0x1234");

    pack_i16(buf, 0, -100);
    TEST_ASSERT(unpack_i16(buf, 0) == -100, "unpack_i16 -100");

    pack_i16(buf, 0, 0);
    TEST_ASSERT(unpack_i16(buf, 0) == 0, "unpack_i16 0");
  }

  /* ---- 1.3 StepperStatus 解析 ---- */
  {
    bsp_uart4.uart_printf("[1.3] parse_stepper_status\r\n");
    uint8_t d[8];
    StepperStatus st;

    // 有效帧: state=1(运动中), angle=90.00°, rpm=60.0, turns=5
    d[0] = 1;
    pack_i16(d, 1, 9000);
    pack_i16(d, 3, 600);
    pack_i16(d, 5, 5);
    d[7] = can_checksum(d);

    parse_stepper_status(d, &st);
    TEST_ASSERT(st.ck_ok, "Stepper ck_ok=true");
    TEST_ASSERT(st.state == 1, "Stepper state=1 (运动中)");
    TEST_ASSERT(st.angle > 89.9f && st.angle < 90.1f, "Stepper angle=90.0");
    TEST_ASSERT(st.rpm > 59.9f && st.rpm < 60.1f, "Stepper rpm=60.0");
    TEST_ASSERT(st.turns == 5, "Stepper turns=5");
    TEST_ASSERT(st.is_moving(), "Stepper is_moving=true");
    TEST_ASSERT(!st.is_done(), "Stepper is_done=false");

    // 校验失败帧
    d[7] ^= 0xFF;
    parse_stepper_status(d, &st);
    TEST_ASSERT(!st.ck_ok, "Stepper bad checksum -> !ck_ok");
  }

  /* ---- 1.4 DcMotorStatus 解析 ---- */
  {
    bsp_uart4.uart_printf("[1.4] parse_dc_motor_status\r\n");
    uint8_t d[8];
    DcMotorStatus st;

    // 有效帧: state=1(running), dir=1(CW)
    memset(d, 0, sizeof(d));
    d[0] = 1;
    d[1] = 1;
    d[7] = can_checksum(d);

    parse_dc_motor_status(d, &st);
    TEST_ASSERT(st._ck_ok, "DC _ck_ok=true");
    TEST_ASSERT(st.state == 1, "DC state=1 (running)");
    TEST_ASSERT(st.dir == 1, "DC dir=1 (CW)");
    TEST_ASSERT(st.is_running(), "DC is_running=true");
  }
}

/* ================================================================
 *  Phase 2: DcMotor CAN 回环测试
 * ================================================================ */

static void test_dc_motor_loopback(void)
{
  bsp_uart4.uart_printf("\r\n--- Phase 2: DC Motor Loopback ---\r\n");

  bsp_can.set_loopback(true);

  DcMotor dc;
  bool ok = dc.init({1, 0x100, canSend});
  TEST_ASSERT(ok, "DcMotor init");
  TEST_ASSERT(dc.can_id() == 0x101, "DcMotor can_id=0x101");

  /* ---- 2.1 ENABLE ---- */
  {
    bsp_uart4.uart_printf("[2.1] enable(true)\r\n");
    dc.enable(true);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "ENABLE frame received");
    TEST_ASSERT(id == 0x101, "ENABLE can_id=0x101");
    TEST_ASSERT(data[0] == CMD_ENABLE, "ENABLE cmd=0x04");
    TEST_ASSERT(data[7] == can_checksum(data), "ENABLE checksum OK");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 1, "ENABLE p1=1");
  }

  /* ---- 2.2 SET DUTY (50.0%) ---- */
  {
    bsp_uart4.uart_printf("[2.2] set_duty(500) = 50.0%%\r\n");
    dc.set_duty(500);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "DUTY frame received");
    TEST_ASSERT(id == 0x101, "DUTY can_id=0x101");
    TEST_ASSERT(data[0] == CMD_DUTY, "DUTY cmd=0x10");
    TEST_ASSERT(data[7] == can_checksum(data), "DUTY checksum OK");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 500, "DUTY p1=500");
  }

  /* ---- 2.3 SET RPM (120 RPM) ---- */
  {
    bsp_uart4.uart_printf("[2.3] set_rpm(120)\r\n");
    dc.set_rpm(120.0f);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "RPM frame received");
    TEST_ASSERT(id == 0x101, "RPM can_id=0x101");
    TEST_ASSERT(data[0] == CMD_RPM, "RPM cmd=0x11");
    TEST_ASSERT(data[7] == can_checksum(data), "RPM checksum OK");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 1200, "RPM p1=1200 (120.0 rpm)");
  }

  /* ---- 2.4 STOP ---- */
  {
    bsp_uart4.uart_printf("[2.4] stop()\r\n");
    dc.stop();
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "STOP frame received");
    TEST_ASSERT(id == 0x101, "STOP can_id=0x101");
    TEST_ASSERT(data[0] == CMD_STOP, "STOP cmd=0x03");
    TEST_ASSERT(data[7] == can_checksum(data), "STOP checksum OK");
  }

  /* ---- 2.5 DISABLE ---- */
  {
    bsp_uart4.uart_printf("[2.5] enable(false)\r\n");
    dc.enable(false);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "DISABLE frame received");
    TEST_ASSERT(data[0] == CMD_ENABLE, "DISABLE cmd=0x04");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 0, "DISABLE p1=0");
  }

  bsp_can.set_loopback(false);
}

/* ================================================================
 *  Phase 3: StepperMotor CAN 回环测试
 * ================================================================ */

static void test_stepper_loopback(void)
{
  bsp_uart4.uart_printf("\r\n--- Phase 3: Stepper Motor Loopback ---\r\n");

  bsp_can.set_loopback(true);

  StepperMotor step;
  bool ok = step.init({1, 0x200, canSend});
  TEST_ASSERT(ok, "StepperMotor init");
  TEST_ASSERT(step.can_id() == 0x201, "StepperMotor can_id=0x201");

  /* ---- 3.1 ENABLE ---- */
  {
    bsp_uart4.uart_printf("[3.1] enable(true)\r\n");
    step.enable(true);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "ENABLE frame received");
    TEST_ASSERT(id == 0x201, "ENABLE can_id=0x201");
    TEST_ASSERT(data[0] == CMD_ENABLE, "ENABLE cmd=0x04");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 1, "ENABLE p1=1");
  }

  /* ---- 3.2 SET SPEED ---- */
  {
    bsp_uart4.uart_printf("[3.2] set_speed(60 rpm)\r\n");
    step.set_speed(60.0f);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "SPEED frame received");
    TEST_ASSERT(id == 0x201, "SPEED can_id=0x201");
    TEST_ASSERT(data[0] == CMD_SPEED, "SPEED cmd=0x01");
    TEST_ASSERT(data[7] == can_checksum(data), "SPEED checksum OK");
    int16_t p1 = unpack_i16(data, 1);
    TEST_ASSERT(p1 == 600, "SPEED p1=600 (60.0 rpm)");
  }

  /* ---- 3.3 MOVE TO (就近定位) ---- */
  {
    bsp_uart4.uart_printf("[3.3] move_to(90 deg, max 45 rpm)\r\n");
    step.move_to(90.0f, 45.0f);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "move_to frame received");
    TEST_ASSERT(id == 0x201, "move_to can_id=0x201");
    TEST_ASSERT(data[0] == CMD_POSITION, "move_to cmd=POSITION(0x02)");
    TEST_ASSERT(data[7] == can_checksum(data), "move_to checksum OK");

    int16_t p1 = unpack_i16(data, 1); // angle×100
    int16_t p2 = unpack_i16(data, 3); // turns (就近=0)
    int16_t p3 = unpack_i16(data, 5); // max_rpm
    TEST_ASSERT(p1 == 9000, "move_to p1=9000 (90.00 deg)");
    TEST_ASSERT(p2 == 0, "move_to p2=0 (就近, 不指定圈数)");
    TEST_ASSERT(p3 == 45, "move_to p3=45 (max rpm)");
  }

  /* ---- 3.4 MOVE ABS (绝对多圈) ---- */
  {
    bsp_uart4.uart_printf("[3.4] move_abs(180 deg, 3 turns, 45 rpm)\r\n");
    step.move_abs(180.0f, 3, 45.0f);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "move_abs frame received");
    TEST_ASSERT(data[0] == CMD_POSITION, "move_abs cmd=POSITION");
    TEST_ASSERT(data[7] == can_checksum(data), "move_abs checksum OK");

    int16_t p1 = unpack_i16(data, 1);
    int16_t p2 = unpack_i16(data, 3);
    int16_t p3 = unpack_i16(data, 5);
    TEST_ASSERT(p1 == 18000, "move_abs p1=18000 (180.00 deg)");
    TEST_ASSERT(p2 == 3, "move_abs p2=3 (3 turns)");
    TEST_ASSERT(p3 == 45, "move_abs p3=45 (max rpm)");
  }

  /* ---- 3.5 TIMED MOVE ---- */
  {
    bsp_uart4.uart_printf("[3.5] timed_move(90 deg, 2.0 s)\r\n");
    step.timed_move(90.0f, 2.0f);
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "timed_move frame received");
    TEST_ASSERT(data[0] == CMD_TIMED, "timed_move cmd=TIMED(0x06)");
    TEST_ASSERT(data[7] == can_checksum(data), "timed_move checksum OK");

    int16_t p1 = unpack_i16(data, 1);
    int16_t p2 = unpack_i16(data, 3);
    TEST_ASSERT(p1 == 9000, "timed_move p1=9000 (90.00 deg)");
    TEST_ASSERT(p2 == 2000, "timed_move p2=2000 (2000 ms)");
  }

  /* ---- 3.6 STOP ---- */
  {
    bsp_uart4.uart_printf("[3.6] stop()\r\n");
    step.stop();
    vTaskDelay(pdMS_TO_TICKS(10));

    uint32_t id;
    uint8_t  data[8], len;
    TEST_ASSERT(bsp_can.recv_read(&id, data, &len, pdMS_TO_TICKS(50)),
                "STOP frame received");
    TEST_ASSERT(data[0] == CMD_STOP, "STOP cmd=0x03");
    TEST_ASSERT(data[7] == can_checksum(data), "STOP checksum OK");
  }

  bsp_can.set_loopback(false);
}

/* ================================================================
 *  测试任务入口
 * ================================================================ */

void motor_proto_test_task(void *arg)
{
  (void)arg;

  bsp_uart4.uart_printf(
    "\r\n"
    "========================================\r\n"
    "  CAN Motor Protocol Test\r\n"
    "  Protocol: can_motor_proto\r\n"
    "  Devices:  DcMotor + StepperMotor\r\n"
    "  Mode:     Loopback (no ext HW)\r\n"
    "========================================\r\n");

  g_pass = 0;
  g_fail = 0;

  /* 等待 CAN 外设就绪 */
  vTaskDelay(pdMS_TO_TICKS(100));

  /* 启用 CAN RX 中断 — 回环帧通过 ISR 写入 StreamBuffer，recv_read 才能拿到数据 */
  bsp_can.enable_irq();

  /* 运行 3 个测试阶段 */
  test_frame_encode_decode();
  test_dc_motor_loopback();
  test_stepper_loopback();

  /* 结果汇总 */
  bsp_uart4.uart_printf(
    "\r\n========================================\r\n"
    "  Results: %lu PASS, %lu FAIL\r\n"
    "========================================\r\n",
    g_pass, g_fail);

  if (g_fail == 0)
  {
    bsp_uart4.uart_printf("  ALL TESTS PASSED!\r\n\r\n");
  }
  else
  {
    bsp_uart4.uart_printf("  %lu TEST(S) FAILED!\r\n\r\n", g_fail);
  }

  /* 测试完成，挂起自身 */
  vTaskSuspend(NULL);
}
