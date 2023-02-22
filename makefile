#just realized we're gonna have to wait after we put the parsercodegen machine code in a file for vm.c
#so that we can make this makefile

pl0: vm.out	#finally run the instruction code thru the vm
	./vm.out

vm.out: HW1/vm.c
	gcc HW1/vm.c -o vm.out

parser: parser.out #run the parser so we can extract the machine code to pass into vm
	./parser.out

parser.out: HW3/parsercodegen.c
	gcc HW3/parsercodegen.c -o parser.out

# clean:
# 	rm parser.out



#----------ChatGPT Wrote this--------------------
# # Define compiler and flags
# CC = gcc
# CFLAGS = -Wall -Wextra -Wpedantic

# # Define the targets and dependencies
# all: parsercodegen vm

# # Compile parsercodegen.c and generate code
# parsercodegen: parsercodegen.c
# 	$(CC) $(CFLAGS) -o parsercodegen parsercodegen.c
# 	./parsercodegen > codegen_output.txt

# # Compile vm.c and run it using code generated by parsercodegen
# vm: vm.c
# 	$(CC) $(CFLAGS) -o vm vm.c
# 	./vm codegen_output.txt

# # Clean up intermediate files
# clean:
# 	rm -f parsercodegen vm codegen_output.txt