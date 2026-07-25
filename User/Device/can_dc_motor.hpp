/**
 * @file can_dc_motor.hpp
 * @brief DC 减速电机设备 — CAN 远程直流电机节点的本地代理 (无虚函数)
 *
 * ## 用法
 * @code
 *   DcMotor dc;
 *   dc.init({1, 0x100, canSend});    // nodeId=1, CAN ID=0x101
 *   dc.set_duty(500);
 *   dc.set_rpm(120);
 * @endcode
 */

#ifndef __DC_MOTOR_HPP__
#define __DC_MOTOR_HPP__

#include <stdint.h>
#include <stdbool.h>
#include "../Protocol/can_motor_proto.hpp"

class DcMotor
{
public:
    /** CAN 发送回调类型 */
    typedef bool (*SendFn)(uint32_t id, const uint8_t *data, uint8_t len);

    struct Config
    {
        uint8_t  node_id;   ///< CAN 节点号 (1~8)
        uint16_t id_base;   ///< CAN ID 基数: 0x100
        SendFn   send_fn;   ///< → bsp_can.send
    };

    bool init(const Config &cfg);
    uint32_t can_id() const { return (uint32_t)_id_base + _node_id; }

    /* ---- 通用命令 ---- */
    void enable(bool on = true);   ///< CMD_ENABLE
    void stop();                   ///< CMD_STOP

    /* ---- DC 专用命令 ---- */
    void set_duty(uint16_t duty);  ///< 开环占空比 0~1000
    void set_rpm(float rpm);       ///< 闭环转速, 从机自算 PID

    /* ---- 状态帧 ---- */
    void handle_status(const uint8_t d[8]); ///< 解析并缓存状态

    uint8_t get_state() const;     ///< 0=idle 1=running 2=fault
    uint8_t get_direction() const; ///< 0=停 1=CW 2=CCW
    bool    has_status() const;    ///< 校验和是否通过
    bool    is_running() const;

private:
    void _send_cmd(uint8_t cmd, int16_t p1, int16_t p2, int16_t p3);

    uint8_t        _node_id = 0;
    uint16_t       _id_base = 0x100;
    SendFn         _send_fn = nullptr;
    bool           _ready   = false;   ///< init 是否成功
    DcMotorStatus  _status;
};

#endif
