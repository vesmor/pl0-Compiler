# PL/0 Machine

This is one of my most favorite projects that I've done. A compiler made in class that targets the pl/0 educational programming language. Compiles down to a custom 'interpreted' assembly ran by a virtual machine made in class.

## Features
### Variable Declarations
#### Constants
Constants can be declared but not reassigned.
```sh
const u = 5;
```
#### Variables
variables can be declared and reassigned and even assigned to each other.
```sh
var x, y;
y := 5;
y := 6;
x := 8;

x := y + 2;
```
### Functions

Functions are declared by procedure and their name. They can be called later with the keyword "call"
```sh
procedure functionname;
    ...
;

call functionname
```
### Recursive calls

Functions can be called within themselves and all their variables and other information will persist on the call stack.
```sh
procedure recursiveFunction;

    call recursiveFunction;

;
```


### Comments
As always needed for documentation, comments.
```sh
/*this is a comment*/
begin
    ...
end.
```

&nbsp;

## How to run
- Compile the compiler.c and the vm.c program
```sh
$ gcc compiler.c -o compiler
```
```sh
$ gcc vm.c -o vm
```
- Run the compiler with the desired input file
```sh
$ ./compiler <input file>
```

- This will create a "tokens.txt" file and an "elf.text" file
- Run the vm executable along with the elf.text file that was created earlier
```sh
$ ./vm elf.text
```
And you should get a runnable program that shows each instruction executed and a cool representation of the call stack, and other machine code info.

&nbsp;

##  Contributors
* [Romsev Charles](https://github.com/vesmor)
* [Brandon Sheridan](https://github.com/UandWhatArmy)
&nbsp;

