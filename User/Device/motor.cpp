#include "motor.hpp"

Motor::Motor() : _enabled(false), _duty_a(0), _duty_b(0) {}

bool Motor::init(const Config &cfg)
{
  if (!cfg.pwm || !cfg.qei)
    return false;

  if (cfg.encoder_lines == 0 || cfg.control_period_ms == 0)
    return false;

  if (cfg.pwm_ch_a >= BspPwm::MAX_CH || cfg.pwm_ch_b >= BspPwm::MAX_CH)
    return false;

  _cfg = cfg;

  _pulses_per_rev = (float)cfg.encoder_lines * 4.0f * cfg.gear_ratio;

  _last_count = cfg.qei->get_count();
  _state      = {};

  _enabled = false;

  if (_cfg.enable_pin)
    _cfg.enable_pin->reset();

  return true;
}

void Motor::set_duty(uint32_t ch_a, uint32_t ch_b)
{
  if (ch_a > _cfg.pwm_max)
    ch_a = _cfg.pwm_max;

  if (ch_b > _cfg.pwm_max)
    ch_b = _cfg.pwm_max;

  _duty_a = ch_a;
  _duty_b = ch_b;
  _output_pwm(ch_a, ch_b);
}

void Motor::update()
{
  uint16_t cur   = _cfg.qei->get_count();
  uint16_t dif   = cur - _last_count;
  int16_t  delta = (dif > 32767) ? (int16_t)(dif - 65536) : (int16_t)dif;
  _last_count    = cur;

  _state.total_pulses += (int32_t)delta;
  _state.current_speed_rpm = _pulses_to_rpm(delta);
  _state.is_forward        = (delta >= 0);
}

void Motor::stop()
{
  _output_pwm(0, 0);

  if (_cfg.enable_pin)
    _cfg.enable_pin->reset();

  _enabled = false;
  _state   = {};
}

void Motor::enable()
{
  if (_cfg.enable_pin)
    _cfg.enable_pin->set();

  _enabled = true;
}

void Motor::disable()
{
  _output_pwm(0, 0);

  if (_cfg.enable_pin)
    _cfg.enable_pin->reset();

  _enabled = false;
}

float Motor::_pulses_to_rpm(int16_t delta) const
{
  /* RPM = (delta / pulses_per_rev) * (60000 / control_period_ms) */
  return (float)delta / _pulses_per_rev * (60000.0f / (float)_cfg.control_period_ms);
}

void Motor::_output_pwm(uint32_t a, uint32_t b)
{
  if (!_enabled)
  {
    a = 0;
    b = 0;
  }

  _cfg.pwm->set_duty(_cfg.pwm_ch_a, a);
  _cfg.pwm->set_duty(_cfg.pwm_ch_b, b);
  _state.pwm_a = a;
  _state.pwm_b = b;
}
