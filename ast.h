
#ifndef AST_H
#define AST_H
#include "svec.h"
#include "hashmap.h"

typedef struct ast {

	char* op;

	struct ast* left;
	struct ast* right;

	struct command* cmd;	
} ast;

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
