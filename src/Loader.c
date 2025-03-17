#include "../include/Loader.h"

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
	prbcRelocCtx->status = prbcRelocCtx->pdNtReadVirtualMemory(
		(HANDLE)((HANDLE)-1),
		(PVOID)((DWORD_PTR)prbcRelocCtx->prcRelocCtx->lpImgBase + pdwRelocRVA),
		&pdwPatchPtr,
		sizeof(DWORD_PTR),
		NULL
	);
	if (prbcRelocCtx->status != STATUS_SUCCESS) {
		return;
	}

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


void pdImportFunction(PIMPORT_CTX ctx, LPVOID lpImgBase, PVOID hLib, PIMAGE_THUNK_DATA tThunk, PIMAGE_THUNK_DATA tLookupThunk) {
	PIMAGE_IMPORT_BY_NAME impFnName;
	ANSI_STRING ansString;
	LPCSTR strFnOrdinal;
	PVOID pFunction;

	if (tThunk->u1.AddressOfData == 0) {
		return;
	}

	if (IMAGE_SNAP_BY_ORDINAL(tLookupThunk->u1.Ordinal)) {
		ctx->status = ctx->pdLdrGetProcedureAddress(hLib, NULL, IMAGE_ORDINAL(tLookupThunk->u1.Ordinal), &pFunction);
		if (ctx->status == STATUS_SUCCESS) {
			tThunk->u1.Function = (ULONGLONG)pFunction;
		}
	}
	else {
		impFnName = (PIMAGE_IMPORT_BY_NAME)((DWORD_PTR)lpImgBase + tLookupThunk->u1.AddressOfData);
		RtlInitAnsiString(&ansString, impFnName->Name); // <------------------------------------------------------------------------------------ MY IMPLEMENTATION SHOULD BE HERE 
		ctx->status = ctx->pdLdrGetProcedureAddress(hLib, &ansString, 0, &pFunction);
		if (ctx->status == STATUS_SUCCESS) {
			tThunk->u1.Function = (ULONGLONG)pFunction;
		}
	}

	return pdImportFunction(ctx, lpImgBase, hLib, tThunk+1, tLookupThunk+1);
}


void pdLoadImports(PIMPORT_CTX ctx, PIMAGE_IMPORT_DESCRIPTOR pidImportDescriptor, LPVOID lpImgBase) {
	LPCSTR strLibName;
	PVOID hLibrary;
	PIMAGE_THUNK_DATA tLookupThunk;
	PIMAGE_THUNK_DATA tThunk;
	ANSI_STRING AnsString;
	UNICODE_STRING ucString;

	// HERE IMPLZEROMEMORY2 <-------------------------------------------------------------------------------

	if (pidImportDescriptor->Name == '\0') {
		return;
	}
	
	RtlInitAnsiString(&AnsString, (LPCSTR)pidImportDescriptor->Name + (DWORD_PTR)lpImgBase); // <------------- MY VERSION OF INITANSISTRING HERE PLS

	ctx->status = ctx->pdRtlAnsiStringToUnicodeString(&ucString, &AnsString, TRUE);
	if (ctx->status != STATUS_SUCCESS) {
		return;
	}

	ctx->status = ctx->pdLdrLoadDll(0, 0, &ucString, &hLibrary);
	if (ctx->status != STATUS_SUCCESS) {
		return;
	}

	if (hLibrary) {
		tThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)lpImgBase + pidImportDescriptor->FirstThunk);
		tLookupThunk = (PIMAGE_THUNK_DATA)((DWORD_PTR)lpImgBase + pidImportDescriptor->OriginalFirstThunk);
		pdImportFunction(ctx, lpImgBase, hLibrary, tThunk, tLookupThunk);
	}


	return pdLoadImports(ctx, pidImportDescriptor++, lpImgBase);
}
