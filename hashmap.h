#ifndef HASHMAP_H
#define HASHMAP_H

#include <stdbool.h>

/*
 * Represents a pair in the hashmap's association array
 */
typedef struct hashmap_pair {

    char key[5]; 	//null terminated string key
    int  val;		//value
    bool used;		//slot in use?
    bool tomb;		//has this slot been deleted?
} hashmap_pair;


/**
 *- hashmap struct using an assosiation table of hashmap_pair
 *- load factor always at or below 0.5
 */
typedef struct hashmap {
  
    long cap;
    long size;
    struct hashmap_pair** data;

} hashmap;

hashmap* make_hashmap();
void free_hashmap(hashmap* hh);
int hashmap_has(hashmap* hh, char* kk);
int hashmap_get(hashmap* hh, char* kk);
void hashmap_put(hashmap* hh, char* kk, int vv);
void hashmap_del(hashmap* hh, char* kk);
hashmap_pair hashmap_get_pair(hashmap* hh, int ii);
void hashmap_dump(hashmap* hh);

#endif
