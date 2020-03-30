#ifndef TOKENIZER_H
#define TOKENIZER_H

#include "svec.h"


/*
 * returns an svec* structure representing the tokens taken from user input
 * - each item in the svec* vector is a token
 */
svec* tokenize(const char* line);


#endif
