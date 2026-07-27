/**
 * @file bsp_key.cpp
 * @brief MSPM0 BspKey 实现 — 软件消抖按键
 *
 * @note 消抖状态机:
 * @code
 *   poll() 每周期:
 *     raw = DL_GPIO_readPins()       // 读原始电平
 *     active = activeLow ? !raw : raw // 转换为按下=true
 *
 *     if (active == _lastStable)      // 与上次稳定值相同
 *       cnt=0, holdCnt++             // 保持中
 *       if (holdCnt >= longPressCnt && !longFired)
 *         → LONG_PRESS               // 长按触发（仅一次）
 *
 *     else                            // 与上次不同 → 可能是抖动
 *       cnt++
 *       if (cnt >= debounceCnt)       // 消抖确认
 *         _lastStable = active        // 更新稳定值
 *         → PRESSED 或 RELEASED
 * @endcode
 *
 * @note 纯裸机操作 (DL_GPIO_readPins)，无系统阻塞，无 ISR。
 */

#include "bsp_key.hpp"

/* ================================================================
 *  初始化
 * ================================================================ */

/**
 * @brief 绑定引脚并重置消抖状态
 * @param cfg 引脚 + 消抖/长按参数
 *
 * @note 读取 GPIO 初始电平作为 _lastStable，
 *       避免启动时误触发按下/释放事件。
 */
void BspKey::init(const Config &cfg)
{
  _port           = cfg.port;
  _pin            = cfg.pin;
  _active_low     = cfg.active_low;
  _debounce_cnt   = cfg.debounce_cnt;
  _long_press_cnt = cfg.long_press_cnt;
  _cnt            = 0U;
  _hold_cnt       = 0U;
  _long_fired     = false;

  /* 以当前电平作为初始稳定状态 */
  bool raw     = (DL_GPIO_readPins(_port, _pin) != 0U);
  _last_stable = _active_low ? !raw : raw;

  /* 创建按键计数信号量（最大计数 2: 短按=1, 长按=2） */
  if (!_sem)
  {
    _sem = xSemaphoreCreateCounting(2, 0);
  }
}

/* ================================================================
 *  消抖轮询
 * ================================================================ */

/**
 * @brief 单次消抖轮询（裸机非阻塞，立即返回）
 *
 * 状态转换图:
 * @code
 *   IDLE ──(cnt>=N, active)──► PRESSED  ──(hold++)──► ... ──► LONG_PRESS
 *   PRESSED ──(cnt>=N, !active)──► RELEASED ──► IDLE
 * @endcode
 *
 * @return 本次触发的事件，大部分情况返回 NONE
 */
BspKey::Event BspKey::poll()
{
  bool raw    = (DL_GPIO_readPins(_port, _pin) != 0U);
  bool active = _active_low ? !raw : raw; /* true = 按键按下 */

  Event result = NONE;

  /* ---- 阶段一: 电平与上次一致 → 稳定 ---- */
  if (active == _last_stable)
  {
    _cnt = 0U; /* 清零抖动计数 */
    if (active)
    {
      _hold_cnt++; /* 按下保持中 */
      if (!_long_fired && _hold_cnt >= _long_press_cnt)
      {
        _long_fired = true; /* 标记已触发，本次按下不再重复 */
        result = LONG_PRESS;
      }
    }
  }
  /* ---- 阶段二: 电平与上次不同 → 可能是抖动 ---- */
  else
  {
    _cnt++;
    if (_cnt >= _debounce_cnt)
    {
      /* ---- 阶段三: 消抖确认，状态切换 ---- */
      _cnt = 0U;

      if (active && !_last_stable)
      {
        /* 确认按下 */
        _last_stable = true;
        _hold_cnt    = 0U;
        _long_fired  = false;
        result = PRESSED;
      }
      else if (!active && _last_stable)
      {
        /* 确认释放 → 根据是否长按过给出不同计数值 */
        _last_stable = false;
        _hold_cnt    = 0U;
        result = RELEASED;

        if (_sem)
        {
          BaseType_t count = _long_fired ? 2 : 1;
          for (BaseType_t i = 0; i < count; i++)
            xSemaphoreGive(_sem);
        }
      }
    }
  }

  return result;
}
