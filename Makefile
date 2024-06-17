bin :
	xmake --rebuild --warning

ARGS ?= 

test : bin
	xmake run test $(ARGS)

Virtual-Machine : bin
	xmake run Virtual-Machine $(ARGS)

compile_commands :
	xmake project -k compile_commands

clean :
	xmake clean

