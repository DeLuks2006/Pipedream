#include <Windows.h>

typedef struct BASE_RELOCATION_BLOCK {
	DWORD dwPageAddress;
	DWORD dwBlockSize;
} BASE_RELOCATION_BLOCK, *PBASE_RELOCATION_BLOCK;

typedef struct BASE_RELOCATION_ENTRY {
	USHORT usOffset : 12;
	USHORT usType : 4;
} BASE_RELOCATION_ENTRY, *PBASE_RELOCATION_ENTRY;

typedef struct RELOC_CTX {
	SIZE_T szRelocsProcessed;
	IMAGE_DATA_DIRECTORY iddRelocDir;
	DWORD_PTR pdwRelocTable;
	LPVOID lpImgBase;
} RELOC_CTX, *PRELOC_CTX;

typedef struct RELOC_BLOCK_CTX {
	PRELOC_CTX prcRelocCtx;
	INT iCounter;
	DWORD dwRelocCount;
	PBASE_RELOCATION_ENTRY preRelocEntries;
	PBASE_RELOCATION_BLOCK prbRelocBlock;
	DWORD_PTR pdwDelta;
} RELOC_BLOCK_CTX, *PRELOC_BLOCK_CTX;

/// <summary>
/// Copies the sections of a given buffer to allocated memory
/// </summary>
/// <param name="shSection">- Pointer to the first section of the buffer</param>
/// <param name="dwNumSections">- Total number of sections</param>
/// <param name="dwSectionsProcessed">- This needs to be 0</param>
/// <param name="lpImgBase">- Pointer to the image base</param>
/// <param name="lpFile">- Pointer to the buffer</param>
void pdCopySections(PIMAGE_SECTION_HEADER shSection, DWORD dwNumSections, DWORD dwSectionsProcessed, LPVOID lpImgBase, LPVOID lpFile);

/// <summary>
/// Performs relocations on given relocation block of an image.
/// </summary>
/// <param name="prbcRelocCtx">- Pointer to relocation block ctx</param>
void pdRelocateBlock(PRELOC_BLOCK_CTX prbcRelocCtx);

/// <summary>
/// Performs relocation on given image,
/// </summary>
/// <param name="prcRelocCtx">- Pointer to relocation ctx</param>
/// <param name="pdwDelta">- Pointer to delta image base</param>
void pdPerformRelocs(PRELOC_CTX prcRelocCtx, DWORD_PTR pdwDelta);

/// <summary>
/// Loads all imported functions from a given DLL
/// </summary>
/// <param name="lpImgBase">- Pointer to base address of image</param>
/// <param name="hLib">- Handle to module from which the functions are imported</param>
/// <param name="tThunk">- Pointer to thunk of passed image</param>
void pdImportFunction(LPVOID lpImgBase, HMODULE hLib, PIMAGE_THUNK_DATA tThunk);

/// <summary>
/// Resolves all imported DLLs and their functions from a given image
/// </summary>
/// <param name="pidImportDescriptor">- Pointer to import descriptor</param>
/// <param name="lpImgBase">- Pointer to base address of image</param>
void pdLoadImports(PIMAGE_IMPORT_DESCRIPTOR pidImportDescriptor, LPVOID lpImgBase);