#just realized we're gonna have to wait after we put the parsercodegen machine code in a file for vm.c
#so that we can make this makefile


parser: parser.out #run the parser so we can extract the machine code to pass into vm
	./parser.out HW3/input.txt

parser.out: HW3/parsercodegen.c
	gcc HW3/parsercodegen.c -o parser.out
