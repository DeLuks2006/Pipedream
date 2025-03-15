CC := clang -target x86_64-w64-mingw32
ASM := nasm

CFLAGS	:= $(CFLAGS) -Os -fno-asynchronous-unwind-tables -nostdlib 
CFLAGS 	:= $(CFLAGS) -fno-ident -fpack-struct=8 -falign-functions=1
CFLAGS  := $(CFLAGS) -s -ffunction-sections -falign-jumps=1 -w
CFLAGS	:= $(CFLAGS) -falign-labels=1 -fPIC -Wl,-TSectionLink.ld
LFLAGS	:= $(LFLAGS) -Wl,-s,--no-seh,--enable-stdcall-fixup

OUT := x64_Pipedream.exe
BIN := x64_Pipedream.bin

all:
	$(ASM) -f win64 asm/Entry.asm -o x64_Entry.o
	$(ASM) -f win64 asm/GetIp.asm -o x64_GetIp.o
	$(CC) src/*.c x64_Entry.o x64_GetIp.o -o $(OUT) $(CFLAGS) $(LFLAGS) -I.
	python3 scripts/extract.py -f $(OUT) -o $(BIN)

clean:
	rm -rf *.o
	rm -rf *.bin
	rm -rf *.exe

