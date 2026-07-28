/**
 * @file can_motor_proto.cpp
 * @brief CAN 电机帧协议 — 编解码实现 (纯协议层, 无设备依赖)
 */

#include "can_motor_proto.hpp"

void CanMotorProto::build_frame(uint8_t d[8], uint8_t cmd,
                                int16_t p1, int16_t p2, int16_t p3)
{
    d[0] = cmd;
    pack_i16(d, 1, p1);
    pack_i16(d, 3, p2);
    pack_i16(d, 5, p3);
    d[7] = 0;
}

void CanMotorProto::parse_stepper(const uint8_t d[8], StepperStatus *st)
{
    st->state = d[0];
    st->angle = (float)unpack_i16(d, 1) / 100.0f;
    st->rpm   = (float)unpack_i16(d, 3) / 10.0f;
    st->turns = unpack_i16(d, 5);
}

void CanMotorProto::parse_dc(const uint8_t d[8], DcMotorStatus *st)
{
    st->rpm       = (float)unpack_i16(d, 0) / 100.0f;
    st->current_a = (float)unpack_i16(d, 2) / 1000.0f;
    st->voltage_v = (float)unpack_i16(d, 4) / 1000.0f;
}
