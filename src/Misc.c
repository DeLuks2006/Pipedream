#include "../include/Misc.h"

ULONG pdFowlerA(LPCSTR String, ULONG Hash) {
	if (*String == '\0') {
		return Hash;
	}

	if (Hash == 0) {
		Hash = 0x811c9dc5;
	}

	Hash ^= (UCHAR)*String;
	Hash *= 0x01000193;

	return pdFowlerA(++String, Hash);
}

ULONG pdFowlerW(LPCWSTR String, ULONG Hash) {
	if (*String == L'\0') {
		return Hash;
	}

	if (Hash == 0) {
		Hash = 0x811c9dc5;
	}

	Hash ^= (UCHAR)*String;
	Hash *= 0x01000193;

	return pdFowlerW(++String, Hash);
}

PVOID pdCopyMemoryEx(PVOID Destination, PVOID Source, SIZE_T Length) {
	if (Length == 0) {
		return Destination;
	}

	*(PBYTE)Destination = *(PBYTE)Source;
	pdCopyMemoryEx((BYTE*)Destination + 1, (BYTE*)Source + 1, --Length);

	return Destination;
}
