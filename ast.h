
#ifndef AST_H
#define AST_H
#include "svec.h"
#include "hashmap.h"

union container;
struct node;
struct ast;


/*
 * Represents a leaf node
 * - acts as a simple shell command without an operator
 * 	- func: the shell command
 *	- args: the shell arguments of the command
 */
typedef struct command {
	char* func;
	char** args;
} command;

/*
 * Represents a node on the syntax tree 
 * - op is the value at the root node holding a shell operator (e.g ";", "|", "<"...)
 * - left and right are recursively valid ast's, 
 */
typedef struct node {
	char* op;
	struct ast* left;
	struct ast* right;	
} node;


//represents either a node or leaf value in the syntax tree
typedef union container {
	node* node;
	command* leaf;
}container;


/*
 * complete representation of a general abstract syntax tree
 *	- contains either a leaf or node, dterined by type
 *
 */
typedef struct ast {
	int type; //leaf or node? (0 = leaf, 1 = node)
	container* value; //the leaf or node value
} ast;






ast* make_ast_command(svec* tokens);
ast* make_ast_op(char* op, ast* left, ast* right);
ast* parse(svec* tokens, hashmap* map);	
void free_ast(ast* tree);	
void print_ast(ast* tree);	


#endif
