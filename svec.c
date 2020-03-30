

#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <stdio.h>
#include <string.h>

#include "svec.h"


/*
 * Makes and returns an svec* strutrue with a default capacity of two
 */
svec*
make_svec()
{
    svec* sv = malloc(sizeof(svec));
    sv->data = malloc(2 * sizeof(char*));
    sv->size = 0;
    sv->capacity = 2;
    return sv;
}

/*
 * Frees all data of the given svec*
 */
void
free_svec(svec* sv)
{   
	if (sv) {
		for (int ii = 0; ii < sv->size; ++ii) {
			free(sv->data[ii]);
		}
	free(sv->data);
	free (sv);	
	}
}


/*
 * Gets the ii'th value in the given svec*
 */
char*
svec_get(svec* sv, int ii)
{
    assert(ii >= 0 && ii < sv->size);
    return sv->data[ii];
}


/**
 * puts an tiem into the svec* at the given index
 */
void
svec_put(svec* sv, int ii, char* item)
{
    assert(ii >= 0 && ii < sv->size);
    sv->data[ii] =  strdup(item);
}


/*
 * Puts an item of the end of the svec*
 * - grows the svec if not enough room to place the svec*
 */
void
svec_push_back(svec* sv, char* item)
{
    int ii = sv->size;
	
	// expand vector when size equals capacity
	if(sv->size == sv->capacity) {
		sv->data = realloc(sv->data, 2*sv->size*sizeof(char*));
		sv->capacity *= 2;
	}

    sv->size = ii + 1;
    svec_put(sv, ii, item);
}


/*
 * swaps the values at the indices of the svec*
 */
void
svec_swap(svec* sv, int ii, int jj)
{

	assert(sv->size > ii && sv->size > jj);
    char* temp = sv->data[ii];
	sv->data[ii] = sv->data[jj];
	sv->data[jj] = temp;
}
