
PL/0 Parser
due March 8th
System Software
Euripedes Montagne

Written by
- Romsev Charles
- Brandon Sheridan

Run instructions

1) compile the file 
$ gcc parsercodegen.c -o a.out

2) run the compiled code with the input of the source code file
$ ./a.out [name of input file]

Problems:
    The program will attempt to make an intermediate file "tokens.txt"
    if it fails to make this file it may be trying to make it into a directory that doesn't exist
    