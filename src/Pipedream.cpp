#include "../include/PeUtils.hpp"
#include "../include/Macros.hpp"

typedef BOOLEAN ( WINAPI* DLLMAIN_T ) (
	HMODULE ImageBase,
	DWORD Reason,
	LPVOID Parameter
);

void Pipedream(void) {
	// Loader Vars
	DLLMAIN_T Ent{};
	LPVOID pe_base{};
	RELOC_CTX rcRelocCtx{};
	DWORD dwOldProtect{};
	// PE bullshit
	PIMAGE_DOS_HEADER Dos = C_PTR( G_END(  ) );
	PIMAGE_NT_HEADERS NtH = C_PTR( U_PTR( Dos ) + Dos->e_lfanew );
	IMAGE_DATA_DIRECTORY import_dir{};
	PIMAGE_IMPORT_DESCRIPTOR import_desc{};
	SIZE_T szSizeImage{};
	DWORD_PTR pdwDelta{};
	

	// Calc memory for image
	szSizeImage = (((NtH->OptionalHeader.SizeOfImage) + 0x1000 - 1) & ~(0x1000 - 1));

	// Alloc memory for image
	pe_base = VirtualAlloc(
		(LPVOID)NtH->OptionalHeader.ImageBase,
		szSizeImage,
		MEM_RESERVE | MEM_COMMIT,
		PAGE_READWRITE
	);
	if (pe_base == NULL) {
		return;
	}

	// Copy sections
	pdCopySections(IMAGE_FIRST_SECTION(NtH), NtH->FileHeader.NumberOfSections, 0, pe_base, C_PTR( G_END(  ) ));

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
	VirtualProtect(pe_base, szSizeImage, PAGE_EXECUTE_READ, &dwOldProtect);

	// Execute
	Ent = C_PTR( U_PTR( pe_base ) + NtH->OptionalHeader.AddressOfEntryPoint );
	Ent( G_SYM( Start ), 1, NULL );
	Ent( G_SYM( Start ), 4, NULL );

	return;
}