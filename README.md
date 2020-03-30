# UnixShell
Implementation of A UNIX Style Shell in C with accompanying vector and hashmap implementations

Type 'make' or 'make nush' to build. Run with ./nush on command line on a linux system or equivalent.

OR

Give command line arguments in the form of a file	
	- e.g ./nush tests/01-echo.out


Type 'make clean' to remove non source-code files

# TODO

	- Shell Variables 
		- read '=' operator when parsing 
		- read '$xxx' variables and make appropriate swap
	- Shell parentheses
	- evaluate parsing logic and order of operations for shell operators
		- exit operator with && and || operators
	- Valgrind checks and memory leaks
	- some possible hashmap bugs with reading over tombstone values
	- adapt hashmap for <string, string> mappings


Run 'make test' to execute shell tests.  Some failing at the moment
