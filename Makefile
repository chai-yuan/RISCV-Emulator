bin :
	xmake --rebuild --warning

ARGS ?= 

run : bin
	./build/linux/x86_64/debug/RISCV-Emulator $(ARGS)
