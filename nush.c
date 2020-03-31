#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include "tokenizer.h"
#include "svec.h"
#include "ast.h"
#include "hashmap.h"
#include <assert.h>

char* readCArgs(int size, char* argv[]); 
void buildAndRun(char* line);
int execTree(ast* tree);

/*
 *executes a simple shell command (no special operators)

 tree is therefore of type leaf
 */
int simpleCommand(ast* tree) {


	/* TODO - variables feature with hashmap
	if(strcmp("=", tree->cmd->func) {
		addToHash(tree,
	}
	*/

	//do nothing on null data
	if(tree == 0 ||tree->value == 0  || tree->value->leaf == 0) {
			return 0;
	}

	assert(tree->type == 0);

	//passed empty comand
	if(strcmp("", tree->value->leaf->func) == 0 || tree->value->leaf->func == 0) {
		return 0;
	}

	//special case of 'false' command
    if (strcmp("false",tree->value->leaf->func) == 0) {
        return -1;
    }

    //special case of 'true' command
    if (strcmp("true",tree->value->leaf->func) == 0) {
        return 0;
    }

    char* cmd = tree->value->leaf->func;
    int cpid;

    if((cpid = fork())) {
        //parent process
        int st;
        waitpid(cpid, &st,0);

    } else {
        //child process
        return  execvp(cmd, tree->value->leaf->args);
        exit(-1);
    }
    return 0; 
}

/*
 * driver to construct tokens, build the abstract syntax tree, then evaluate the tree
 */
void buildAndRun(char* line) {

    svec* tokens = tokenize(line);
	hashmap* map = make_hashmap();
    ast* tree = parse(tokens, map);  
    execTree(tree);                
    free_ast(tree);
//	free_hashmap(map);
//    free_svec(tokens);
}

/*
 * Executes case of tree split of ';' operator
 */
int semiColonCommand(ast* tree) {
assert(tree->type == 1);	

   int first =  execTree(tree->value->node->left);
   int sec =  execTree(tree->value->node->right);

   if (first == 0 && sec == 0) {
       return 0;
   }
   else {
       return -1;
   }
}

/*
 * Executes case of ast split of '&&' operator
 */
int andCommand(ast* tree) { 
assert(tree->type == 1);	
    int goodFirst = execTree(tree->value->node->left);
    int sec = -1;
    if(goodFirst == 0) {
      sec =  execTree(tree->value->node->right);
    }

    if (sec == 0) {
        return 0;
    } else {
        return -1;
    }
}

/**
 * Executes case of ast split of '||' operator
 */
int orCommand(ast* tree) {

    int first = execTree(tree->value->node->left);

    if(first != 0) {
        execTree(tree->value->node->right);
    }
}

/*
 * Executes case of ast split of '&' operator
 */ 
int backgroundCommand(ast* tree) {
assert(tree->type == 1);	
    int cpid;

    if((cpid = fork())) {
    //parent         
	return 0;	
    } else {
    //child
    execTree(tree->value->node->left);
    _exit(0);

    }
    return 0;
}

/*
 * Executes case of ast split on '>'  operator
 */
int redirectOutputCommand(ast* tree) { 
assert(tree->type == 1);	
    int cpid;

    if((cpid = fork())) {
		//parent
        int st;
        waitpid(cpid, &st,0);
    } else {
		//child

        //change file descriptor to accomplish redirect
        //int fd = open(tree->right->cmd->func, O_CREAT | O_APPEND | O_WRONLY , 0644);
       
		int fd;
		fd = open(tree->value->node->right->value->leaf->func, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(fd, 1);
		close(fd);

        execTree(tree->value->node->left);
	
    }
}

/*
 * Executes case of ast split on '<' operator
 */
int redirectInputCommand(ast* tree) {
assert(tree->type == 1);	
    int cpid;

    if((cpid = fork())) {

        int st;
        waitpid(cpid, &st, 0);

    }else {

		int fd;
        fd = open(tree->value->node->right->value->leaf->func, O_RDONLY, 0);
	
        dup2(fd, 0);
        close(fd);

        execTree(tree->value->node->left);
    }
}


/*
 * Executes case of ast split on '|' operator
 */
int pipeCommand(ast* tree) {
   
        assert(tree->type == 1);	
	int cpid;
	int cpid2;
	int fdrs[2];

	if(cpid = fork()) {
		//parent
		waitpid(cpid,0,0);
	} else {
		//child
		pipe(fdrs);		
		if(cpid2 = fork()) {
			//child/parent
			dup2(fdrs[0], 0);
			close(fdrs[1]);
			execTree(tree->value->node->right);
			waitpid(cpid2, 0, 0);
		} else {
			//child.child
			dup2(fdrs[1], 1);
			close(fdrs[0]);
			execTree(tree->value->node->left);
		}
	}
}


/*
 * handles special case of changing directory command
 */
int handleCD(ast* tree) {

	assert(tree->type == 0);	

    if(chdir(tree->value->leaf->args[1]) != 0) {
        return -1;
    }
    return 0;
}

/*
 * Handles special case of exit command;
 */
int handleExit(ast* tree) {
    exit(0);
    return 0;
}


/*
 * Executes the given ast
 * - Recursively evaluates tree on operator in leaf node
 */
int execTree(ast* tree) {


	//protect again null data
	if(tree == 0) {
		return 0;
	}	

    //handling cd 
    if(tree->value->node->op == 0 && strcmp(tree->value->leaf->func, "cd") == 0) {
        return handleCD(tree);
    }

    //handle exit
    if(tree->type == 0 && strcmp("exit", tree->value->leaf->func) == 0) {
        return handleExit(tree);
    }

    //simple command case
    if(tree->type == 0 && tree->value->leaf != 0) {
        return simpleCommand(tree); 
    } 

    

	//rest must be case with operators

	assert(tree->type == 1);

    if(strcmp(";", tree->value->node->op) == 0) {
       return semiColonCommand(tree);
    }

    if (strcmp("&&", tree->value->node->op) == 0) {
        return andCommand(tree);
    }

    if(strcmp("||", tree->value->node->op) == 0) {
        return orCommand(tree);
    }

    if(strcmp("&", tree->value->node->op) == 0) {
        return backgroundCommand(tree);
    }

    if(strcmp(">", tree->value->node->op) == 0) {

        return redirectOutputCommand(tree);
    }
    
    if(strcmp("<", tree->value->node->op) == 0){
        return redirectInputCommand(tree);
    }

    if(strcmp("|", tree->value->node->op) == 0) {
        return pipeCommand(tree);
    }


    return 0;
}

/*
 * Entry point ot the programi
 *  - reads user input, executes command until exit
 *  - option to read from a file and execute line by line 
 */
int main(int argc, char* argv[]){
    
    char cmd[256];
   
    //read from a file 
    if(argc > 1) {

        FILE* fh = fopen(argv[1], "r");           

        if(!fh) {
            perror("open failed");
            return 1;
        } else {

            char temp[256];
            
            while(1) {

                char* line = fgets(temp, 256, fh);  

                if(!line) {
                    break;
                }

                buildAndRun(temp);
            }
            return 0;
        }
    }

    //take user input interactively
    if(argc == 1) {

        while(1) {

            printf("nush$");                
            fflush(stdout);                 
            char* rv = fgets(cmd, 256, stdin);        

            //user gives empty line
            if(cmd[0] == '\n') {
                continue;
            }
            if (!rv) {
                break;
            }
           buildAndRun(cmd);
        }
    }
    return 0;
}
