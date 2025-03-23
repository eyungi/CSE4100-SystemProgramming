/* $begin shellmain */
#include "myshell.h"
#include<errno.h>
#define MAXARGS   128

/* Function prototypes */
void eval(char *cmdline);
int parseline(char *buf, char **argv);
int builtin_command(char **argv); 
char*** seper_pipe(char ** argv, int * argc);
void operate_commands(char *** cmds, int argc);

int main() 
{
    char cmdline[MAXLINE]; /* Command line */

    while (1) {
	/* Read */
	printf("CSE4100-SP-P2> ");                   
	fgets(cmdline, MAXLINE, stdin); 
	if (feof(stdin))
	    exit(0);

	/* Evaluate */
	eval(cmdline);
    } 
}
/* $end shellmain */

char*** seper_pipe(char **argv, int * argc){
    int i,j;
    char*** cmds = (char***)malloc(sizeof(char**)*MAXARGS);
    for(i=0; i<MAXARGS; i++){
        cmds[i] = (char**)malloc(sizeof(char*)*MAXARGS);
        for(j=0; j<MAXARGS ;j++){
            cmds[i][j] = (char*)malloc(sizeof(char)*MAXARGS);
        }
    }
    i=0, j=0;
    while(*argv != 0){
        if(*argv[0] == '|'){
            cmds[i][j] = 0;
            argv++;
            i++;
            j = 0;
        }
        else{
            strcpy(cmds[i][j],*argv);
            j++;
            argv++;
        }
    }
    *argc = i+1;
    cmds[i][j] = 0;
    cmds[i+1][0] = 0;
    return cmds;
}

void operate_commands(char *** cmds, int argc){
    int fds[2];
    int fd;
    int status;
    pid_t pid;

    fd = 0;
    for(int i=0; i< argc; ){
        pipe(fds);
        if((pid = fork()) == 0){
            if(cmds[i+1][0] != 0) 
                dup2(fds[1],1);
            dup2(fd,0);
            close(fds[0]);
            if (!builtin_command(cmds[i])) {
				if (execvp(cmds[i][0], cmds[i]) < 0) {
					printf("%s: Command not found.\n", cmds[i][0]);
					exit(0);
				}
			}
			else
				exit(0);
        }
        else{
            waitpid(pid,&status,0);
            close(fds[1]);
            fd = fds[0];
            i++;
        }
    }
}

/* $begin eval */
/* eval - Evaluate a command line */
void eval(char *cmdline) 
{
    char *argv[MAXARGS]; /* Argument list execve() */
    char buf[MAXLINE];   /* Holds modified command line */
    int bg;              /* Should the job run in bg or fg? */
    pid_t pid;           /* Process id */
    int child_status;
    int argc = 1;
    char *** cmds;
    int i=0;

    strcpy(buf, cmdline);
    bg = parseline(buf, argv);

    if (argv[0] == NULL)  
	    return;   /* Ignore empty lines */
    if (!builtin_command(argv)){
        cmds = seper_pipe(argv,&argc);
        operate_commands(cmds,argc);
    }    

    return;
}

/* If first arg is a builtin command, run it and return true */
int builtin_command(char **argv) 
{
    if (!strcmp(argv[0], "exit")) /* quit command */
	    exit(0);  
    if (!strcmp(argv[0], "&"))    /* Ignore singleton & */
	    return 1;
    if(!strcmp(argv[0],"cd")){
        chdir(argv[1]);
        return 1;
    }
    return 0;                     /* Not a builtin command */
}
/* $end eval */

/* $begin parseline */
/* parseline - Parse the command line and build the argv array */
int parseline(char *buf, char **argv) 
{
    char *delim;         /* Points to first space delimiter */
    int argc;            /* Number of args */
    int bg;              /* Background job? */
    char * pipe;
    char * col;
    buf[strlen(buf)-1] = ' ';  /* Replace trailing '\n' with space */
    while (*buf && (*buf == ' ')) /* Ignore leading spaces */
	    buf++;

    /* Build the argv list */
    argc = 0;
    pipe = strchr(buf,'|');
    while ((delim = strchr(buf, ' '))) {
        if(*buf == '\'') {
            buf++;
            delim = strchr(buf,'\'');
        }
        else if(*buf == '\"'){
            buf++;
            delim = strchr(buf,'\"');
        }
        //ls -al| grep myshell
        if(pipe > buf && pipe < delim){
            argv[argc++] = buf;
            *pipe = '\0';
            argv[argc++] = "|";
            buf = pipe + 1;
            pipe = strchr(buf,'|');
        }
        if(*buf == '|'){
            argv[argc++] = "|";
            buf++;
        }
        else{
            argv[argc++] = buf;
	        *delim = '\0';
	        buf = delim + 1;
        }
	    while (*buf && (*buf == ' ')) /* Ignore spaces */
            buf++;
    }
    argv[argc] = NULL;
    
    if (argc == 0)  /* Ignore blank line */
	    return 1;

    /* Should the job run in the background? */
    if ((bg = (*argv[argc-1] == '&')) != 0)
	    argv[--argc] = NULL;

    return bg;
}
/* $end parseline */


