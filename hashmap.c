
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <bsd/string.h>
#include <string.h>

#include "hashmap.h"


//some prototyping
hashmap_pair* make_empty_pair();
hashmap* make_hashmap_presize(int nn); 

//grows the hashmap when the load capacity exceeps .5
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

long
hash(char* key, long cap) //more or less the same from class notes, with a different prime# and other small changes
{
    // Produce an appropriate hash value.
    long hs = 0;
	for(long ii=0; key[ii]; ++ii) {
		hs = hs*31 + key[ii];
	}	
	return hs % cap;
}

hashmap*
make_hashmap_presize(int nn) //set data to be all 'empty' pairs (unused)
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


void
free_hashmap(hashmap* hh)
{
    //  Free all allocated data.
	if(hh) {

		for(int i = 0; i < hh->cap; ++i) {
			free(hh->data[i]);
		}
		free(hh->data);
		free(hh);
	}
}

int
hashmap_has(hashmap* hh, char* kk)
{
    return hashmap_get(hh, kk) != -1;
}

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


//creates a pair in the hashmap
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


void
hashmap_put(hashmap* hh, char* kk, int vv)
{
    // Insert the value 'vv' into the hashmap
    // for the key 'kk', replacing any existing value
    // for that key.	
	
	//grow if going to exceed load capacity
	if(    ((1.0 * hh->size) / (1.0 * hh->cap)) >= .5) {
			map_grow(hh);
	}

	long hkey = hash(kk, hh->cap);

	long limit = hh->cap;
	long ii = hkey;

	for(ii; ii < limit; ++ii) {

		//if key exists, updata with next value
		if( strlen(hh->data[ii]->key)==strlen(kk) //same line length
					   	&& (strncmp( hh->data[ii]->key, kk, strlen(kk))==0))//same string values
	   	{
			hh->data[ii]->val = vv;
			return;
		}


		if(! hh->data[ii]->used ) { //puts in first unused slot
	
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



void
hashmap_del(hashmap* hh, char* kk)
{
    // Remove any value associated with
    // this key in the map.
	
	int hkey = hash(kk, hh->cap);


	long limit = hh->cap;
	long i = hkey;


	//if being used, delete the value of the key
	for(i; i < limit; ++i) {

	
		//if found key, delete associated value
		if(  (strncmp(hh->data[i]->key, kk, strlen(kk)) == 0)  ) {
			strcpy(hh->data[i]->key, "empt");
			hh->data[i]->used = false;
			hh->size -= 1;
			break;
		}

		if(i+1 >= hh->cap) {
				i = 0;
				limit = hh->cap;
		}
	}
}

hashmap_pair
hashmap_get_pair(hashmap* hh, int ii)
{
    // Get the {k,v} pair stored in index 'ii'.
	hashmap_pair ans = *(hh->data[ii]);
	return ans;
}

void
hashmap_dump(hashmap* hh)
{
    printf("== hashmap dump ==\n");
    //  Print out all the keys and values currently
    // in the map, in storage order. Useful for debugging.
	

	long ii = 0;
	while(hh->data[ii]) {
		if(hh->data[ii]->used) {
			printf("key: %s, val: %ld\n", hh->data[ii]->key, hh->data[ii]->val);
		}
		ii++;
	}
}

