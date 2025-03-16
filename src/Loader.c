#include "../include/Loader.h"

PVOID pdCopyMemoryEx(PVOID Destination, PVOID Source, SIZE_T Length) {
  if (Length == 0) {
    return Destination;
  }

  *(PBYTE)Destination = *(PBYTE)Source;
  pdCopyMemoryEx((PBYTE)++Destination, (PBYTE)++Source, --Length);

  return Destination;
}

void pdCopySections(PIMAGE_SECTION_HEADER shSection, DWORD dwNumSections, DWORD dwSectionsProcessed, LPVOID lpImgBase, LPVOID lpFile) {
	LPVOID lpDestination;
	LPVOID lpBytes;

	if (dwNumSections == dwSectionsProcessed) {
		return;
	}

	lpDestination = (LPVOID)((DWORD_PTR)lpImgBase + (DWORD_PTR)shSection->VirtualAddress);
	lpBytes = (LPVOID)((DWORD_PTR)lpFile + (DWORD_PTR)shSection->PointerToRawData);
	pdCopyMemoryEx(lpDestination, lpBytes, shSection->SizeOfRawData);

	pdCopySections(shSection++, dwNumSections, dwSectionsProcessed, lpImgBase, lpFile);
}

void pdRelocateBlock(PRELOC_BLOCK_CTX prbcRelocCtx) {
	DWORD_PTR pdwRelocRVA;
	DWORD_PTR pdwPatchPtr;

	if (prbcRelocCtx->iCounter == prbcRelocCtx->dwRelocCount) {
		return;
	}

	prbcRelocCtx->prcRelocCtx->szRelocsProcessed += sizeof(BASE_RELOCATION_ENTRY);

	if (prbcRelocCtx->preRelocEntries[prbcRelocCtx->iCounter].usType == 0) {
		return pdRelocateBlock(prbcRelocCtx);
	}
	
	pdwRelocRVA = prbcRelocCtx->prbRelocBlock->dwPageAddress + prbcRelocCtx->preRelocEntries[prbcRelocCtx->iCounter].usOffset;
	ReadProcessMemory(
		(HANDLE)((HANDLE)-1), // GetCurrentProcess
		(LPCVOID)((DWORD_PTR)prbcRelocCtx->prcRelocCtx->lpImgBase + pdwRelocRVA),
		&pdwPatchPtr,
		sizeof(DWORD_PTR),
		NULL
	);

	pdwPatchPtr += prbcRelocCtx->pdwDelta;
	pdCopyMemoryEx((PVOID)((DWORD_PTR)prbcRelocCtx->prcRelocCtx->lpImgBase + pdwRelocRVA), &pdwPatchPtr, sizeof(DWORD_PTR));
	
	return pdRelocateBlock(prbcRelocCtx);
}

void pdPerformRelocs(PRELOC_CTX prcRelocCtx, DWORD_PTR pdwDelta) {
	RELOC_BLOCK_CTX rbcRelocCtx;

	if (prcRelocCtx->szRelocsProcessed == prcRelocCtx->iddRelocDir.Size) {
		return;
	}

	rbcRelocCtx.prcRelocCtx = prcRelocCtx;
	rbcRelocCtx.prbRelocBlock = (PBASE_RELOCATION_BLOCK)(prcRelocCtx->pdwRelocTable + prcRelocCtx->szRelocsProcessed);
	rbcRelocCtx.prcRelocCtx->szRelocsProcessed += sizeof(BASE_RELOCATION_BLOCK);
	rbcRelocCtx.dwRelocCount = (rbcRelocCtx.prbRelocBlock->dwBlockSize - sizeof(PBASE_RELOCATION_BLOCK) / sizeof(PBASE_RELOCATION_ENTRY));
	rbcRelocCtx.preRelocEntries = (PBASE_RELOCATION_ENTRY)(prcRelocCtx->pdwRelocTable + prcRelocCtx->szRelocsProcessed);
	rbcRelocCtx.pdwDelta = pdwDelta;

	pdRelocateBlock(&rbcRelocCtx);

	return pdPerformRelocs(prcRelocCtx, pdwDelta);
}


void pdImportFunction(LPVOID lpImgBase, HMODULE hLib, PIMAGE_THUNK_DATA tThunk) {
	LPCSTR strFnOrdinal;
	PIMAGE_IMPORT_BY_NAME impFnName;

	if (tThunk->u1.AddressOfData == 0) {
		return;
	}

	if (IMAGE_SNAP_BY_ORDINAL(tThunk->u1.Ordinal)) {
		strFnOrdinal = (LPCSTR)IMAGE_ORDINAL(tThunk->u1.Ordinal);
		tThunk->u1.Function = (DWORD_PTR)GetProcAddress(hLib, strFnOrdinal);
	}
	else {
		impFnName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)lpImgBase + tThunk->u1.AddressOfData);
		tThunk->u1.Function = (DWORD_PTR)GetProcAddress(hLib, impFnName->Name);
	}

	return pdImportFunction(lpImgBase, hLib, tThunk);
}


void pdLoadImports(PIMAGE_IMPORT_DESCRIPTOR pidImportDescriptor, LPVOID lpImgBase) {
	LPCSTR strLibName;
	HMODULE hLibrary;
	PIMAGE_THUNK_DATA tThunk;

	if (pidImportDescriptor->Name == '\0') {
		return;
	}

	strLibName = (LPCSTR)pidImportDescriptor->Name + (DWORD_PTR)lpImgBase;
	hLibrary = LoadLibraryA(strLibName);

	if (hLibrary) {
		tThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)lpImgBase + pidImportDescriptor->FirstThunk);
		pdImportFunction(lpImgBase, hLibrary, tThunk);
	}


	return pdLoadImports(pidImportDescriptor++, lpImgBase);
}
