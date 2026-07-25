#ifndef __USER_SYSTEM_HPP__
#define __USER_SYSTEM_HPP__

#include <stdarg.h>
#include <stddef.h>

namespace user_system
{
int format(char *buf, size_t size, const char *fmt, ...);
int format_v(char *buf, size_t size, const char *fmt, va_list args);
} // namespace user_system

#endif
