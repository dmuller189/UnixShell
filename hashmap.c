
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


//some prototyping
hashmap_pair* make_empty_pair();
hashmap* make_hashmap_presize(int nn); 

/*
 * grows the hashmap when the load capacity exceeds .5
 * - Doubles the capacity, then copies over old values into grown hashmap memory
 */
void map_grow(hashmap* hh) {

		long nn = hh->cap;
		hashmap_pair** data = hh->data;
	
		//new data storage
		hh->data = malloc(2*nn*sizeof(hashmap_pair*));
		hh->cap = 2*nn;
		hh->size = 0;

		// - filling with 'empty' value pairs
		for(long ii = 0; ii < 2*nn; ++ii) {

			hh->data[ii] = make_empty_pair();
		}

		//iterate through old data to put into new table
		for(long jj = 0; jj < nn; ++jj) {
			if(data[jj]->used) {
					hashmap_put(hh, data[jj]->key, data[jj]->val);
			}
			free(data[jj]);
		}
		free(data);
}


/*
 * basic hash function
 */
long
hash(char* key, long cap) 
{
    // Produce an appropriate hash value.
    long hs = 0;
	for(long ii=0; key[ii]; ++ii) {
		hs = hs*31 + key[ii];
	}	
	return hs % cap;
}


/*
 * makes a hashmap of size nn with all empty value
 */
hashmap*
make_hashmap_presize(int nn) 
{
    hashmap* hh = malloc(sizeof(hashmap));
    //Allocate and initialize a hashmap with capacity 'nn'.
    hh->cap = nn;
    hh->size = 0;
	hh->data = malloc(nn * sizeof(hashmap_pair*));

	for(long ii = 0; ii < nn; ++ii) {
		hh->data[ii] = make_empty_pair();
	}
   
    return hh;
}

hashmap*
make_hashmap()
{
    return make_hashmap_presize(4);
}


/*
 * free all alocated data of the given hashmap
 */
void
free_hashmap(hashmap* hh)
{
	if(hh) {

		for(int i = 0; i < hh->cap; ++i) {
			free(hh->data[i]);
		}
		free(hh->data);
		free(hh);
	}
}


/*
 * Returns whether the given hashmap has the key-value pair
 */
int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}


/*
 * Gets the hashmap value associated with the key
 */
int
hashmap_get(hashmap* hh, char* kk)
{
    // Return the value associated with the
    // key kk.
    // Note: return -1 for key not found.
	long key = hash(kk, hh->cap);


	long limit = hh->cap;
	long ii = key;
	
	for (ii; ii < limit; ++ii) {
	
		//found key
		if(hh->data[ii]->used && strlen(hh->data[ii]->key)==strlen(kk) && (strncmp(hh->data[ii]->key, kk,strlen(kk)) == 0) ){		
			return hh->data[ii]->val;
		}

		//if reached bottom of table, loop back to top
		if(ii+1 >= hh->cap) {
			ii = 0;
			limit = key;
		}

		//found empty slot, return -1
		if(! hh->data[ii]->used) {

			return -1;
		}
	}
	return -1;
}


/*
 * Makes a hasmap_pair* with the given inputs
 */
hashmap_pair* make_pair(char* key, int val, bool used, bool tomb){

	hashmap_pair* hp = malloc(sizeof(hashmap_pair));

	strcpy(hp->key, key);
	hp->val = val;
	hp->used = used;
	hp->tomb = tomb;
	return hp;
}

hashmap_pair* make_empty_pair() {
		return make_pair("empt",0,false,false);
}


/*
 * Inserts the key-value pair in the hashmap at the first
 * available slot at or after the hash index.
 * - Replaces any existing values for an existing key
 */
void
hashmap_put(hashmap* hh, char* kk, int vv)
{
	
	//grow if going to exceed load capacity
	if(    ((1.0 * hh->size) / (1.0 * hh->cap)) >= .5) {
			map_grow(hh);
	}

	long hkey = hash(kk, hh->cap);
	long limit = hh->cap;
	long ii = hkey;

	//insert at right slot via linear probing
	for(ii; ii < limit; ++ii) {

		char* key = (char*) hh->data[ii]->key;
		//if key exists, update with next value
		if( strlen(key)==strlen(kk) 
			&& (strncmp(key, kk, strlen(kk))==0))//same string values
	   	{
			hh->data[ii]->val = vv;
			return;
		}


		if(! hh->data[ii]->used) { //puts in first unused slot
	
			strcpy(hh->data[ii]->key, kk);
			hh->data[ii]->val = vv;
			hh->data[ii]->used = true;
			hh->data[ii]->tomb = false;
			hh->size += 1;
			return;
		}

		//loop back to top of table when reached end
		if(ii+1 >= hh->cap) {
				ii = 0;
				limit = hkey;
		}
	}
}


/*
 * Remoes any value associated with the key in the hashmap
 */
void
hashmap_del(hashmap* hh, char* kk)
{
	
	int hkey = hash(kk, hh->cap);
	long limit = hh->cap;
	long i = hkey;

	//if being used, delete the value of the key
	for(i; i < limit; ++i) {

		//if found key, delete associated value 
		if(  (strncmp(hh->data[i]->key, kk, strlen(kk)) == 0)  ) {
			strcpy(hh->data[i]->key, "empt");
			hh->data[i]->used = false;
			hh->data[i]->tomb = true;
			hh->size -= 1;
			break;
		}

		//circle to top of association table if key not found by bottom
		if(i+1 >= hh->cap) {
				i = 0;
				limit = hh->cap;
		}
	}
}

/*
 * gets the i'th pair of the hashmap
 */
hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    // Get the {k,v} pair stored in index 'ii'.
	hashmap_pair ans = *(hh->data[ii]);
	return ans;
}


/**
 * prints a representation of the hashmap values in storage order
 *  - Debugging tool
 */
void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
	long ii = 0;
	while(hh->data[ii]) {
		if(hh->data[ii]->used) {
			printf("key: %s, val: %ld\n", hh->data[ii]->key, hh->data[ii]->val);
		}
		ii++;
	}
}

