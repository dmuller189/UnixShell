# UnixShell
Implementation of A UNIX Style Shell in C with accompanying vector and hashmap implementations

Type 'make' of 'make nush' to build. Rrun with ./nush on command line.

#OR

Give command line arguments in the form of a file	
	- e.g ./nush tests/01-echo.out


Type 'make clean' to remove non source-code files

#TODO

	- Shell Variables 
	- Shell parentheses  
	- Fix background procesing ( & operator )
	- evaluate parsing and order of operations for shell operators
	- Valgrind checks and memory leaks
	- static analysis and clang checks
	- some possible hashmap bugs with reading over tombstone values

Run 'make test' to execute shell tests.  Some failing at the moment
