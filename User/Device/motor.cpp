#include "motor.hpp"

void Motor::Motor_Init(const Config &cfg)
{
  _cfg = cfg;

  _cfg.enable_pin->reset();

  _state.target = 0;
  _state.speed  = 0;
}


void Motor::PID_Init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable)
{
  _pid.init(p, i, d, t, out_max, i_max, i_epsilon_enable);
}

void Motor::update()
{
  float pid_out = 0;

  _state.speed = (_cfg.qei->get_count() - 32767.0f) * (1000.0f / _cfg.control_period_ms) / (_cfg.encoder_lines * 4 * _cfg.gear_ratio);
  _cfg.qei->set_count(32767);

  if (_pid_enable == 1)
  {
    pid_out = _pid.calc(_state.target, _state.speed);

    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500 + pid_out);
  }

  _pid_out = pid_out;
}


void Motor::enable()
{
  if (_pid_enable == 0)
  {
    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
    _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
    _cfg.enable_pin->set();
  }
}

void Motor::disable()
{
  _pid_enable = 0;
  _cfg.enable_pin->reset();
  _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
  _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
}

void Motor::speed_control_enable()
{
  if (_pid_enable == 0)
  {
    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500);
    _cfg.pwm->set_duty(_cfg.pwm_ch_a, 2500);
    _cfg.enable_pin->set();
    _pid_enable = 1;
  }
}

void Motor::set_speed(float target)
{
  _state.target = target;
}
//
// _state.pwm_a = a;
// _state.pwm_b = b;
