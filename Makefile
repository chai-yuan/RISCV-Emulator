bin :
	xmake -r

ARGS = ?

run : bin
	./build/linux/x86_64/release/RISCV-Emulator $(ARGS)
