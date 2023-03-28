#makefile created with the help of chatGPT

HW1_DIR = HW1
HW3_DIR = HW3
HW4_DIR = HW4

all: proc vm

.PHONY: proc
proc: $(HW4_DIR)/parsercodegen.c $(HW4_DIR)/input.txt
	cd $(HW4_DIR) && gcc -g parsercodegen.c -o parsercodegen && ./parsercodegen input.txt out/input_out.txt

.PHONY: proc_debug
proc: $(HW4_DIR)/parsercodegen.c $(HW4_DIR)/input.txt
	cd $(HW4_DIR) && gcc -g parsercodegen.c -o parsercodegen && ./parsercodegen input.txt out/input_out.txt

.PHONY: codegen
codegen: $(HW3_DIR)/parsercodegen.c $(HW3_DIR)/input.txt
	cd $(HW3_DIR) && gcc -g parsercodegen.c -o parsercodegen && ./parsercodegen cases/input.txt out/input_out.txt

.PHONY: vm
vm: $(HW1_DIR)/vm.c $(HW1_DIR)/input2.txt
	cd $(HW1_DIR) && gcc -o vm vm.c && ./vm ../$(HW4_DIR)/output.txt

.PHONY: clean
clean:
	rm -f $(HW3_DIR)/parsercodegen $(HW1_DIR)/vm $(HW4_DIR)/parsercodegen
