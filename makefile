#makefile created with the help of chatGPT

HW1_DIR = HW1
HW3_DIR = HW3

all: codegen vm

.PHONY: codegen
codegen: $(HW3_DIR)/parsercodegen.c $(HW3_DIR)/input.txt
	cd $(HW3_DIR) && gcc -g parsercodegen.c -o parsercodegen && ./parsercodegen error4.txt

.PHONY: vm
vm: $(HW1_DIR)/vm.c $(HW1_DIR)/input.txt
	cd $(HW1_DIR) && gcc -o vm vm.c && ./vm input.txt
.PHONY: codegen
codegen: $(HW3_DIR)/parsercodegen.c $(HW3_DIR)/input.txt
	cd $(HW3_DIR) && gcc -o -g parsercodegen parsercodegen.c && ./parsercodegen error4.txt

.PHONY: clean
clean:
	rm -f $(HW3_DIR)/parsercodegen $(HW1_DIR)/vm
