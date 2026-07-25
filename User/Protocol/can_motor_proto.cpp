/**
 * @file can_motor_proto.cpp
 * @brief 统一 CAN 电机帧协议 — 编解码 + 路由分发
 *
 * 所有编解码函数为纯数据操作, 无硬件依赖, 无全局状态。
 * 可直接复用到从机 (MSPM0 电机节点) 固件中。
 */

#include "can_motor_proto.hpp"
#include "../Device/can_stepper_motor.hpp" // dispatch 需要 StepperMotor::can_id / handle_status
#include "../Device/can_dc_motor.hpp"      // dispatch 需要 DcMotor::can_id / handle_status

/* ================================================================
 *  命令帧编码: 上层语义 → 8 字节 CAN 数据
 * ================================================================ */

void build_motor_frame(uint8_t d[8], uint8_t cmd,
                     int16_t p1, int16_t p2, int16_t p3)
{
    d[0] = cmd;
    pack_i16(d, 1, p1);   // 大端, 高字节在前
    pack_i16(d, 3, p2);
    pack_i16(d, 5, p3);
    d[7] = can_checksum(d); // XOR(0..6)
}

/* ================================================================
 *  步进状态帧解码: 8 字节 CAN 数据 → StepperStatus
 * ================================================================ */

void parse_stepper_status(const uint8_t d[8], StepperStatus *st)
{
    st->ck_ok  = (d[7] == can_checksum(d)); // 先验校验和, 不通过则以下字段不可信
    st->state = d[0];                      // 0=空闲 1=运动中 2=已到位 3=故障
    st->angle = (float)unpack_i16(d, 1) / 100.0f;   // angle×100 → °
    st->rpm   = (float)unpack_i16(d, 3) / 10.0f;    // rpm×10   → RPM
    st->turns = unpack_i16(d, 5);                    // 累计圈数
}

/* ================================================================
 *  DC 状态帧解码: 8 字节 CAN 数据 → DcMotorStatus
 * ================================================================ */

void parse_dc_motor_status(const uint8_t d[8], DcMotorStatus *st)
{
    st->_ck_ok  = (d[7] == can_checksum(d)); // 校验, 不通过则字段不可信
    st->state = d[0];                      // 0=idle 1=running 2=fault
    st->dir   = d[1];                      // 0=停 1=CW 2=CCW
}

/* ================================================================
 *  CAN 帧路由: 收帧 → 按类型+can_id 匹配 → 分发
 *
 *  O(n) 遍历 MotorEntry[], n ≤ 8, 每 1ms 最多一帧, 无开销。
 *  用 void* + MotorType 标记代替虚函数, M0+ 零额外开销。
 * ================================================================ */

void motor_can_dispatch(uint32_t id, const uint8_t d[8],
                      MotorEntry *entries, uint8_t count)
{
    for (uint8_t i = 0; i < count; i++)
    {
        void *p = entries[i].ptr;
        if (!p) continue;

        switch (entries[i].type)
        {
        case MT_STEPPER: {
            auto *m = (StepperMotor *)p;
            if (m->can_id() == id) { m->handle_status(d); return; }
            break;
        }
        case MT_DC: {
            auto *m = (DcMotor *)p;
            if (m->can_id() == id) { m->handle_status(d); return; }
            break;
        }
        }
    }
}
