# UnixShell
Implementation of A UNIX Style Shell in C with accompanying vector and hashmap implementations


## Explanation
- nush.c's main function is the entry point to the program
- ast.h and ast.c are a header and source file for the abstract syntax tree of the program
- hashmap.h and hashmap.c implement a hashmap from char pointer to int
- svec.h and svec.c implemenet a variable length vector of char pointers / strings
- tokenizer.h and tokenizer.h tokenizes input into a vector of tokens


## To Use
Type 'make' or 'make nush' to build. Run with ./nush on command line on a linux system or equivalent.

OR

Give command line arguments in the form of a file	
	- e.g ./nush tests/01-echo.out


Type 'make clean' to remove non source-code files

# TODO

- Shell Variables 
	- read '=' operator when parsing 
	- read '$xxx' variables and make appropriate swap with the hashmap
- Shell parentheses
- evaluate parsing logic and order of operations for shell operators
	- exit operator with && and || operators
- Valgrind checks and memory leaks
- some possible hashmap bugs with reading over tombstone values
- adapt hashmap for <string, string> mappings


Run 'make test' to execute shell tests.  Some failing at the moment
