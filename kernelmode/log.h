#pragma once
#include <ntifs.h>

template <typename... Args>
void log(const char* format, Args... args)
{
	//DbgPrintEx(0, 0, "[SYNWARE] ");
	//DbgPrintEx(0, 0, format, args...);
	//DbgPrintEx(0, 0, "\n");
}