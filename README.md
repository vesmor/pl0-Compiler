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

##  Contributors
* [Romsev Charles](https://github.com/vesmor)
* [Brandon Sheridan](https://github.com/UandWhatArmy)
&nbsp;

