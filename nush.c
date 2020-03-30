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

char* readCArgs(int size, char* argv[]); 
void buildAndRun(char* line);
int execTree(ast* tree);

/*
 *executes a simple shell command (no special operators)
 */
int simpleCommand(ast* tree) {


	/* TODO - variables feature with hashmap
	if(strcmp("=", tree->cmd->func) {
		addToHash(tree,
	}
	*/

	//do nothing on null data
	if(tree == 0 || tree->cmd == 0) {
			return 0;
	}

	//passed empty comand
	if(strcmp("", tree->cmd->func) == 0 || tree->cmd->func == 0) {
		return 0;
	}

	//special case of 'false' command
    if (strcmp("false",tree->cmd->func) == 0) {
        return -1;
    }

    //special case of 'true' command
    if (strcmp("true",tree->cmd->func) == 0) {
        return 0;
    }

    char* cmd = tree->cmd->func;
    int cpid;

    if((cpid = fork())) {
        //parent process
        int st;
        waitpid(cpid, &st,0);

    } else {
        //child process
        return  execvp(cmd, tree->cmd->args);
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

   int first =  execTree(tree->left);
   int sec =  execTree(tree->right);

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

    int goodFirst = execTree(tree->left);
    int sec = -1;
    if(goodFirst == 0) {
      sec =  execTree(tree->right);
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

    int first = execTree(tree->left);

    if(first != 0) {
        execTree(tree->right);
    }
}

/*
 * Executes case of ast split of '&' operator
 */ 
int backgroundCommand(ast* tree) { 
    int cpid;

    if((cpid = fork())) {
    //parent         
	return 0;	
    } else {
    //child
    execTree(tree->left);
    _exit(0);

    }
    return 0;
}

/*
 * Executes case of ast split on '>'  operator
 */
int redirectOutputCommand(ast* tree) { 

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
		fd = open(tree->right->cmd->func, O_WRONLY | O_CREAT | O_TRUNC, 0644);
		dup2(fd, 1);
		close(fd);

        execTree(tree->left);
	
    }
}

/*
 * Executes case of ast split on '<' operator
 */
int redirectInputCommand(ast* tree) {

    int cpid;

    if((cpid = fork())) {

        int st;
        waitpid(cpid, &st, 0);

    }else {

		int fd;
        fd = open(tree->right->cmd->func, O_RDONLY, 0);
	
        dup2(fd, 0);
        close(fd);

        execTree(tree->left);
    }
}


/*
 * Executes case of ast split on '|' operator
 */
int pipeCommand(ast* tree) {
    
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
			execTree(tree->right);
			waitpid(cpid2, 0, 0);
		} else {
			//child.child
			dup2(fdrs[1], 1);
			close(fdrs[0]);
			execTree(tree->left);
		}
	}
}


/*
 * handles special case of changing directory command
 */
int handleCD(ast* tree) { 

    if(chdir(tree->cmd->args[1]) != 0) {
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
    if(tree->op == 0 && strcmp(tree->cmd->func, "cd") == 0) {
        return handleCD(tree);
    }

    //handle exit
    if(tree->op == 0 && strcmp("exit", tree->cmd->func) == 0) {
        return handleExit(tree);
    }

    //simple command case
    if(tree->op == 0 && tree->cmd != 0) {
        return simpleCommand(tree);
        
    } 

    
    if(strcmp(";", tree->op) == 0) {
       return semiColonCommand(tree);
    }

    if (strcmp("&&", tree->op) == 0) {
        return andCommand(tree);
    }

    if(strcmp("||", tree->op) == 0) {
        return orCommand(tree);
    }

    if(strcmp("&", tree->op) == 0) {
        return backgroundCommand(tree);
    }

    if(strcmp(">", tree->op) == 0) {

        return redirectOutputCommand(tree);
    }
    
    if(strcmp("<", tree->op) == 0){
        return redirectInputCommand(tree);
    }

    if(strcmp("|", tree->op) == 0) {
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
