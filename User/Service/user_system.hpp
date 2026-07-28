/**
 * @file user_system.hpp
 * @brief 轻量级格式化输出 — 独立实现的 printf 子集，无动态内存分配
 *
 * 提供 sprintf 风格的格式化函数，专为嵌入式 OLED / 串口输出设计。
 * 不依赖 libc 的 printf，无堆内存分配，栈上转换数字。
 *
 * 支持的格式符:
 *   %d %i %u %x %X %f %s %c %%  以及 %ld %lu %lx %lX %.Nf
 *
 * @note 浮点 %f 使用软件转换，无 FPU 依赖，适用于 Cortex-M0+
 */

#ifndef __USER_SYSTEM_HPP__
#define __USER_SYSTEM_HPP__

#include <stdarg.h>
#include <stddef.h>

/**
 * @brief 用户系统工具命名空间
 */
namespace user_system
{

/**
 * @brief 格式化字符串写入缓冲区（va_list 版本）
 * @param buf   输出缓冲区
 * @param size  缓冲区大小（含 '\\0'）
 * @param fmt   格式化字符串
 * @param args  可变参数列表
 * @return 实际写入的字符数（不含 '\\0'），出错返回 -1
 */
int format_v(char *buf, size_t size, const char *fmt, va_list args);

/**
 * @brief 格式化字符串写入缓冲区
 * @param buf   输出缓冲区
 * @param size  缓冲区大小（含 '\\0'）
 * @param fmt   格式化字符串
 * @param ...   可变参数
 * @return 实际写入的字符数（不含 '\\0'），出错返回 -1
 */
int format(char *buf, size_t size, const char *fmt, ...);

} // namespace user_system

#endif
