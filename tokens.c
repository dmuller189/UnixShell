
#include <stdio.h>
#include "svec.h"
#include "tokenizer.h"

#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/*
int 
main(int _ac, char* _av[])
{

    
    char line[100];

    while (1) {

        printf("tokens$ ");
    
        char* l = fgets(line, 96, stdin);

        if(!l) {
            break;
        }

        svec* ls = tokenize(l);

        fflush(stdout);

        for (int ii=ls->size; ii>0; --ii) {
             puts(svec_get(ls,ii-1));
        }
   }

    return 0;
}

*/

