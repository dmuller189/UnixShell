
#ifndef AST_H
#define AST_H
#include "svec.h"
#include "hashmap.h"


/*
 * Represents an recursize abstract syntax tree
 * - op is the value at the root node holding a shell operator (e.g ";", "|", "<"...)
 * - left and right are recursively valid ast's, and are null on leaf nodes
 * - cmd is used for representing leaf node values, and is otherwise null 
 */
typedef struct ast {

	char* op;

	struct ast* left;
	struct ast* right;

	struct command* cmd;	
} ast;

/*
 * Represents a lead node
 * - acts as a simple shell command without an operator
 */
typedef struct command {

	char* func;
	char** args;

} command;


ast* make_ast_command(svec* tokens);
ast* make_ast_op(char* op, ast* left, ast* right);

ast* parse(svec* tokens, hashmap* map);	//parses tokens to build ast
void free_ast(ast* tree);	//frees ast	
void free_command(command* cmd);
void print_ast(ast* tree);	//prints ast


#endif
