#include "tokenizer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>

char* read_line(const char* ls, long ii);
char* read_operator(const char* ls, long ii);
int isOperator(char n);
int multiOp(char n);

/**
 * produces a svec* of tokens from a char pointer
 *
 * drawn from class notes scrach 09 tokenize.c
 *
 * A valid token is one of:
 * 	-	&
 *	-	&&
 *	-   |
 *	-	||
 *	-	<
 *	-	>
 *	-	;
 *	- continuous set of characters seperated by a space ' '
 *
 * Therefore tokens are split according to these rules
 */
svec* tokenize(const char* ls) {

	svec* v = make_svec();
	long nn = strlen(ls);
	long ii = 0;

	while (ii < nn) {
	
		//read white space
		if(isspace(ls[ii])) {
			++ii;
			continue;
		}

		//read non-operator characters
		if(!isOperator(ls[ii])) {
			char* word = read_line(ls, ii);
			svec_push_back(v, word);
			ii += strlen(word);
			free(word);
			continue;
		}
		
		//else, must be an operator
		char* op = read_operator(ls, ii);
		svec_push_back(v, op);
		ii += strlen(op);
		free(op);		
	}
	return v;
}

/**
 * reads the given line for the next operator token starting from index ii of the given line
 */
char* 
read_operator(const char* ls, long ii) {

		int jj = 0;
		while(isOperator(ls[ii + jj]) && (ls[ii + jj] != '\0')) {
			++jj;

			//break if single character operator, or seen a double operator 
			if(!multiOp(ls[ii + jj-1]) || jj == 2) {
					break;
			}
		}

		char* op = malloc(jj + 1);
		memcpy(op, ls+ii, jj);
		op[jj] = 0;
		return op;
}


/*
 *reads the given line for the next non-operator token starting at index ii of the given line
 */
char* 
read_line(const char* ls, long ii) {
	
	int jj = 0;

	while(
					(isOperator(ls[ii + jj]) == 0) &&
					!isspace(ls[ii + jj]) &&
					(ls[ii + jj] != '\0')) {
		++jj;
	}

	char* word = malloc(jj+1);
	memcpy(word, ls + ii, jj);
	word[jj] = 0;
	return word;
}

/*
 *The operators '&', '&&', '|', and '||', are all valid 
 This function decides is the given char is a member of these two operators
 */
int multiOp(char n) {
	return (n == '&' || n == '|');
}


/*
 * is the char a valid shell operator
 */
int isOperator(char n) {
	int ans =   (n == '<' ||
			n == '>' ||
			n == '|' ||
			n == '&' ||
			n == ';');
	return ans;
}
