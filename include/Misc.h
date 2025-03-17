#pragma once
#include "Native.h"

ULONG pdFowlerA(LPCSTR String, ULONG Hash);

ULONG pdFowlerW(LPCWSTR String, ULONG Hash);

PVOID pdCopyMemoryEx(PVOID Destination, PVOID Source, SIZE_T Length);