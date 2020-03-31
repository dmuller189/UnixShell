#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashmap.h"
int wordIsOperator(char* word);
void free_command(ast* com);


/**
 * Returns an svec* sliced according to the given bounds and input svec*
 */
svec* slice(svec* toks, int i0, int i1) { 

	svec* ans = make_svec();

	for(int i = i0; i <= i1; ++i) {
		svec_push_back(ans, svec_get(toks, i));
	}
	return ans;
}



/**
 * Searches for a shell operator in the tokens list
 * Looks for '&' and ';' operators first
 * Returns the index of the first occurance, else -1
 */
int containsOperator(svec* toks) {

	int ans = 0;

	if(toks->size == 0) {
		return -1;
	}

	//parse tokens and find the index of the leftmost operator
	//if '&' of ';' is found, return that index
	for(int ii = toks->size-1; ii >= 0; --ii) {
		
		char* tok = svec_get(toks, ii);	
		if(wordIsOperator(tok)) {
			ans = ii;
			if(strcmp("&",tok)==0 || strcmp(";", tok)==0) {
				return ans;
			}
		}
	}
	return ans == 0 ? -1 : ans;
}

/*
 * Determines is the given string is a valid shell operator
 */
int wordIsOperator(char* word) {
	return ( 
		strcmp("<", word)  == 0 ||
		strcmp(">", word)  == 0 ||
		strcmp("&", word)  == 0 ||
		strcmp("&&", word) == 0 ||
		strcmp("|", word)  == 0 ||
		strcmp("||", word) == 0||
		strcmp(";", word) == 0
	      );
}


/**
 *Parses tokens to build an ast, splittingon shell operators
 *	- searches for shell operator
 *		- if found, splits on operator and recusively adds left and right ast
 *		- if not found, a command leaf is added to the data structure
 *Inspired by lecture notes on parsing an arithmetic expression
 */
ast* parse(svec* tokens, hashmap* map) {

	//look for shell operators in tokens
	int co =  containsOperator(tokens);

	//no shell operators found
	if(co == -1) {
		return make_ast_command(tokens);
	} 
	else {

		//build ast and split on found operator
		svec* left = slice(tokens, 0, co-1); //inclusive of both bounds
		svec* right;

		if(co+1 > tokens->size - 1) {
			right = make_svec();
		} else {
			right = slice(tokens, co+1, tokens->size - 1);
		}

		//recursively build abstract syntax tree
		ast* tree = make_ast_op(svec_get(tokens, co), parse(left, map), parse(right,map));
		free_svec(left);
		free_svec(right);
		
		return tree;
	}	
}

/*
 * makes an abstract syntax tree split on the given operator
 * 	- root node has the value of the operator
 * 	- left and right branches are valid abstract syntax tree's
 */
ast* make_ast_op(char* op, ast* left, ast* right){
	ast* tree = malloc(sizeof(ast));
	tree->type = 1;

	container* c = malloc(sizeof(container));

	node* n = malloc(sizeof(node));
	n->op = strdup(op);
	n->left = left;
	n->right = right;

	c->node = n;

	tree->value = c;

	return tree;
}



/*
 * returns a list of strings (char**) representing the arguments 
 * to be passed into execvp()
 */
char** buildCommandArgs(svec* tokens, char* funcName)  {

	if(tokens->size == 0) {

		char** ars = 0;
		return ars;
	}

	int size = tokens->size - 1;

	char** ars = malloc((2+size) * sizeof(char*));

	ars[0] = strdup(funcName);

	for (int i = 0; i < size; ++i) {
		ars[i+1] = strdup(svec_get(tokens, i+1));
	}

	ars[size + 1] = NULL;

	return ars;
}

/*
 * Build an abstract syntax tree representing a leaf node - a simple command with no shell operators
 */
ast*  make_ast_command(svec* tokens) {

	ast* tree = malloc(sizeof(ast));
	tree->type = 0;

	container* c = malloc(sizeof(container)); 	

	command* leaf = malloc(sizeof(command));
	
	if(tokens->size == 0) {
		leaf->func = 0;
	} else {
		leaf->func = strdup(svec_get(tokens, 0));
	}

	leaf->args = buildCommandArgs(tokens, leaf->func);

	c->leaf = leaf;

	tree->value = c;

	return tree;
}


/**
 * Frees all associated memory with the abstract syntax tree
 */
void free_ast(ast* tree) {

	if(tree == 0) {
		return;
	}

	if(tree->type == 0) { //leaf
		free_command(tree);
	} else { //must be a node

		free_ast(tree->value->node->left);
		free_ast(tree->value->node->right);
		free(tree->value->node->op);
		free(tree->value->node);
		free(tree->value);
		free(tree);
	}
}

/*
 * Frees all memory from the given command structure, a leaf
 */
void free_command(ast* tree) {

	assert(tree->type == 0);

	if(tree == 0) {
		return;
	}

	assert(tree->type == 0);

	if(tree->value->leaf->func) {
		free(tree->value->leaf->func);
	}

	if(tree->value->leaf->args) {
		for (int i = 0; tree->value->leaf->args[i] != 0; ++i) {
			if(tree->value->leaf->args[i] == 0) {
				break;
			}
			free(tree->value->leaf->args[i]);
		}
	}

	free(tree->value->leaf->args);
	free(tree->value->leaf);
	free(tree->value);
	free(tree);
}


