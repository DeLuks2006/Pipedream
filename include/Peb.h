#pragma once
#include "Native.h"
#include "Misc.h"

PVOID pdGetModuleHandle(ULONG Hash, PLIST_ENTRY StartListEntry, PLIST_ENTRY ListEntry);

DWORD64 pdGetProcAddress(PVOID Module, ULONG Hash, ULONG Index);