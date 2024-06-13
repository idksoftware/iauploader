#include "ReturnCode.h"
#include <memory>
#include <stdarg.h>  // For va_start, etc.
#include <string.h>

std::string ReturnCodeObject::m_errorstring;
ReturnCode ReturnCodeObject::m_returnCode = 0;

void ReturnCodeObject::setReturn(ReturnCode returnCode, const char* fmt, ...)
{
	m_returnCode = returnCode;
	size_t final_n, n = (strlen(fmt) * 2); // Reserve two times as much as the length of the fmt_str //

	va_list ap;
	while (true)
	{
		//formatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr
		auto formatted = std::make_unique<char[]>(n);

		va_start(ap, fmt);
		final_n = vsnprintf(formatted.get(), n, fmt, ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
		{
			n += abs(static_cast<int>(final_n - n + 1));
		}
		else
		{
			m_errorstring = formatted.get();
			break;
		}
	}
}

void ReturnCodeObject::setReturnString(const char* fmt, ...)
{
	size_t final_n, n = (strlen(fmt) * 2); // Reserve two times as much as the length of the fmt_str //

	va_list ap;
	while (true)
	{
		//formatted.reset(new char[n]); // Wrap the plain char array into the unique_ptr
		auto formatted = std::make_unique<char[]>(n);

		va_start(ap, fmt);
		final_n = vsnprintf(formatted.get(), n, fmt, ap);
		va_end(ap);
		if (final_n < 0 || final_n >= n)
		{
			n += abs(static_cast<int>(final_n - n + 1));
		}
		else
		{
			m_errorstring = formatted.get();
			break;
		}
	}

}

