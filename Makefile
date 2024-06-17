bin :
	xmake --rebuild --warning

ARGS ?= 

vm : bin
	./build/linux/x86_64/release/Virtual-Machine $(ARGS)

compile_commands :
	xmake project -k compile_commands

clean :
	xmake clean

