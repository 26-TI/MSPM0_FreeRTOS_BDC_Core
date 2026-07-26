#include "user_system.hpp"

#include <stdio.h>

namespace user_system
{
namespace
{
  static void append_char(char *buf, size_t &written, size_t size, char c)
  {
    if (written + 1 < size)
      buf[written++] = c;
  }

  static void append_string(char *buf, size_t &written, size_t size, const char *s)
  {
    if (!s)
      s = "(null)";
    while (*s && written + 1 < size)
      buf[written++] = *s++;
  }

  static void append_unsigned(char *buf, size_t &written, size_t size, unsigned long long value, int base, bool uppercase)
  {
    char digits[64];
    int  len = 0;

    do
    {
      unsigned long long rem = value % static_cast<unsigned long long>(base);
      digits[len++]          = (rem < 10U) ? static_cast<char>('0' + rem)
                                           : static_cast<char>((uppercase ? 'A' : 'a') + (rem - 10U));
      value /= static_cast<unsigned long long>(base);
    } while (value != 0U);

    while (len-- > 0)
      append_char(buf, written, size, digits[len]);
  }

  static void append_signed(char *buf, size_t &written, size_t size, long long value, int base)
  {
    if (value < 0)
    {
      append_char(buf, written, size, '-');
      value = -value;
    }
    append_unsigned(buf, written, size, static_cast<unsigned long long>(value), base, false);
  }
} // namespace

int format_v(char *buf, size_t size, const char *fmt, va_list args)
{
  if (!buf || !fmt || size == 0)
    return -1;

  size_t      written = 0;
  const char *p       = fmt;

  while (*p && written + 1 < size)
  {
    if (*p != '%')
    {
      append_char(buf, written, size, *p++);
      continue;
    }

    ++p;
    int  precision = -1;
    bool is_long   = false;

    while (*p == '0')
      ++p;

    while (*p >= '0' && *p <= '9')
      ++p;

    if (*p == '.')
    {
      ++p;
      precision = 0;
      while (*p >= '0' && *p <= '9')
      {
        precision = precision * 10 + (*p - '0');
        ++p;
      }
    }

    if (*p == 'l')
    {
      is_long = true;
      ++p;
    }

    char spec = *p++;
    switch (spec)
    {
      case 'd':
      case 'i':
      {
        long long value = is_long ? static_cast<long long>(va_arg(args, long)) : static_cast<long long>(va_arg(args, int));
        append_signed(buf, written, size, value, 10);
        break;
      }
      case 'u':
      {
        unsigned long long value = is_long ? static_cast<unsigned long long>(va_arg(args, unsigned long)) : static_cast<unsigned long long>(va_arg(args, unsigned));
        append_unsigned(buf, written, size, value, 10, false);
        break;
      }
      case 'x':
      case 'X':
      {
        unsigned long long value = is_long ? static_cast<unsigned long long>(va_arg(args, unsigned long)) : static_cast<unsigned long long>(va_arg(args, unsigned));
        append_unsigned(buf, written, size, value, 16, spec == 'X');
        break;
      }
      case 'f':
      {
        double value = va_arg(args, double);
        if (value < 0.0)
        {
          append_char(buf, written, size, '-');
          value = -value;
        }

        unsigned long long whole = static_cast<unsigned long long>(value);
        append_unsigned(buf, written, size, whole, 10, false);

        if (precision < 0)
          precision = 6;

        if (precision > 0)
        {
          append_char(buf, written, size, '.');
          double frac = value - static_cast<double>(whole);
          for (int i = 0; i < precision; ++i)
          {
            frac *= 10.0;
            int digit = static_cast<int>(frac);
            append_char(buf, written, size, static_cast<char>('0' + digit));
            frac -= static_cast<double>(digit);
          }
        }
        break;
      }
      case 's':
      {
        const char *s = va_arg(args, const char *);
        append_string(buf, written, size, s);
        break;
      }
      case 'c':
      {
        int c = va_arg(args, int);
        append_char(buf, written, size, static_cast<char>(c));
        break;
      }
      case '%':
        append_char(buf, written, size, '%');
        break;
      default:
        append_char(buf, written, size, '%');
        append_char(buf, written, size, spec);
        break;
    }
  }

  if (size > 0)
    buf[(written < size - 1) ? written : (size - 1)] = '\0';
  return static_cast<int>(written);
}

int format(char *buf, size_t size, const char *fmt, ...)
{
  va_list args;
  va_start(args, fmt);
  int ret = format_v(buf, size, fmt, args);
  va_end(args);
  return ret;
}
} // namespace user_system
