#include "ast.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashmap.h"
int wordIsOperator(char* word);
void free_command(command* com);

svec* slice(svec* toks, int i0, int i1) { 

	svec* ans = make_svec();

	for(int i = i0; i <= i1; ++i) {
		svec_push_back(ans, svec_get(toks, i));
	}
	return ans;
}



/**
 * Searches for a shell operator and returns the index of the first occurance, else -1
 */
int containsOperator(svec* toks) {


	if(toks->size == 0) {
		return -1;
	}

	for(int ii = 0; ii < toks->size; ++ii) {
		if(wordIsOperator(svec_get(toks, ii))) {
			return ii;
		}
	}
	return -1;
}
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
 TODO - handle the right side of an operator being null / empty 
 		- as to not break slicing 
 */
ast* parse(svec* tokens, hashmap* map) {

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

		ast* tree = make_ast_op(svec_get(tokens, co), parse(left, map), parse(right,map));
		//free some things(left and right)... TODO
		free_svec(left);
		free_svec(right);
		

		return tree;
	}	
}

ast* make_ast_op(char* op, ast* left, ast* right){
	ast* tree = malloc(sizeof(ast));
	tree->op = strdup(op);
	tree->left = left;
	tree->right = right;
	tree->cmd = 0;
	return tree;
}

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

ast* make_ast_command(svec* tokens) {

	ast* tree = malloc(sizeof(ast));
	tree->op = 0;
	tree->left = 0;
	tree->right = 0;
	
	command* c = malloc(sizeof(command));

	if(tokens->size == 0) {
		c->func = 0;
	} else {
		c->func = strdup(svec_get(tokens, 0));
	}
//	c->args = malloc(sizeof(char*) * (tokens->size + 2)); //TODO - verify length	
	c->args = buildCommandArgs(tokens, c->func);

	tree->cmd = c;

	return tree;
}


void free_ast(ast* tree) { //TODO

	if(tree == 0) {
		return;
	}

	if(tree->op == 0) {
		free(tree->right);
		free(tree->left);
		free_command(tree->cmd);
	} else {

		free_ast(tree->left);
		free_ast(tree->right);
		free(tree->op);
	
	}
	free(tree);
}

void free_command(command* com) {

	if(com == 0) {
		return;
	}

	if(com->args == 0 && com->func == 0) {
			free(com);
			return;
	}

		
	free(com->func);

	if(com->args != 0) {
		for(int i = 0; com->args[i] != 0; ++i) {
			if(com->args[0] == 0) {
				break;
			}	
			free(com->args[i]);
		}
	}

	free(com->args);
	free(com);


}


