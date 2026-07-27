#include "motor.hpp"

void Motor::Motor_Init(const Config &cfg)
{
  _cfg = cfg;

  _cfg.enable_pin->reset();

  _state.target = 0;
  _state.speed  = 0;
}


void Motor::PIDL_Init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable)
{
  _pidl.init(p, i, d, t, out_max, i_max, i_epsilon_enable);
}

void Motor::PIDH_Init(float p, float i, float d, float t, float out_max, float i_max, uint8_t i_epsilon_enable)
{
  _pidh.init(p, i, d, t, out_max, i_max, i_epsilon_enable);
}

void Motor::update()
{
  float pid_out = 0;

  _state.speed = (_cfg.qei->get_count() - 32767.0f) * (1000.0f / _cfg.control_period_ms) / (_cfg.encoder_lines * 4 * _cfg.gear_ratio);
  _cfg.qei->set_count(32767);

  if (_pid_enable == 1)
  {
    const float abs_speed = (_state.speed > 0.0f) ? _state.speed : -_state.speed;
    const uint16_t switch_threshold = (_pid_change_time > 0U) ? _pid_change_time : 5U;

    // 原本是高速状态，速度低于低速门——累加切换计数器
    if ((abs_speed < _low_speed_threshold) && (_pid_state == 2))
    {
      _pid_change_cnt++;
      if (_pid_change_cnt >= switch_threshold)
      {
        _pid_state = 1;
        _pid_change_cnt = 0;
        _pidl.bumpless_switch_from(_pidh);
      }
    }
    // 原本是低速状态，速度高于高速门——累加切换计数器
    else if ((abs_speed > _high_speed_threshold) && (_pid_state == 1))
    {
      _pid_change_cnt++;
      if (_pid_change_cnt >= switch_threshold)
      {
        _pid_state = 2;
        _pid_change_cnt = 0;
        _pidh.bumpless_switch_from(_pidl);
      }
    }
    else
      _pid_change_cnt = 0;


    if (_pid_state == 2)  pid_out = _pidh.calc(_state.target, _state.speed);
    else                  pid_out = _pidl.calc(_state.target, _state.speed);

    _cfg.pwm->set_duty(_cfg.pwm_ch_b, 2500 + pid_out);
  }

}


void Motor::enable()
{
  if (_pid_enable == 0)
  {
    _pidl.clear();
    _pidh.clear();
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
  _pidl.clear();
  _pidh.clear();
}

void Motor::speed_control_enable()
{
  if (_pid_enable == 0)
  {
    _pidl.clear();
    _pidh.clear();
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

void Motor::Set_Pid_Change_Time(uint16_t cnt)
{
  _pid_change_time = cnt;
}


