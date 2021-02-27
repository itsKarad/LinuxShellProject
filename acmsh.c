#include<stdio.h>
#include<stdlib.h>
#include <string.h>
#include<unistd.h>
#include<sys/wait.h>
#include<sys/types.h>
#include<signal.h>
#include"./linkedlist.h"


static char ** history;
static int command_count=0;

int sh_cd(char** args);
int sh_pwd();
int sh_about();
int sh_history(char** args);
int sh_again(char** args);
int sh_exit(char **args);
int sh_bg(char **args);
int sh_bglist(char **args);
int sh_kill(char **args);
int sh_help();

int sh_execute(char ** args);
int sh_launch(char **args);
char** sh_split_line(char* line);
char *sh_read_line();





char* builtin_str[]={
	"cd",
	"exit",
    "bg",
    "bglist",
    "kill",
	"pwd",
	"history",
	"again",
	"help",
	"about"
};
int (*builtin_func[]) (char**)={
	&sh_cd,
	&sh_exit,
    &sh_bg,
    &sh_bglist,
    &sh_kill,
	&sh_pwd,
	&sh_history,
	&sh_again,
	&sh_help,
	&sh_about
};


int sh_cd(char** args)
{
	if(args[1]==NULL)
	{
		fprintf(stderr,"sh: expected argument to \"cd\"\n");
	}
	else
	{
		if(chdir(args[1])!=0)
		{
			perror("Invalid Call");
		}
	}
	return 1;
}
int sh_pwd(){
    char pwd[2000];
    if(getcwd(pwd,sizeof(pwd))==0){// in case of error return value
        return 0;
    }
    printf("%s\n",pwd);
    return 1;
}
int sh_about(){
    printf("╔═══════════════╗\n");
    printf("Made by Aditya Karad\n");
    printf("╚═══════════════╝\n");
    return 1;
}
int sh_history(char** args){
    printf("Your history:\n");
    int i;
    if (args[1] == NULL) {
		i=0;
	} 
    else 
    {
		i = command_count - atoi(args[1]);
		if(i<0)
            i=0;
	}
	for(int j = i; j < command_count; j++)
		printf("%d:%s\n", j+1, history[j]);
	return 1;
}
int sh_again(char** args){
	if(args[1]==NULL){
		perror("Err : expected arguement to again command\n");
		return 1;
	}
	else{
		int idx=atoi(args[1]);
		idx++;
		if(idx<=0 || idx>command_count){
			printf("Invalid argument\n");
			return 1;
		}
		idx--;
		printf("%s\n",history[command_count-idx]);
		char **args1 = sh_split_line(history[command_count-idx]);
		if(strcmp(args1[0],"again")==0){
			printf("Cannot execute command again, again!\n");
			return 1;
		}
		int status = sh_execute(args1);
		free(args1);
		return status;		
	}
	return 1;
}

int sh_exit(char **args)
{
	return 0;
}

process* headProcess=NULL;
int sh_bg(char **args)
{
//args -- bg echo "hello"
    ++args;
//args -- echo "hello"
    char *firstCmd= args[0];//echo
    int childpid=fork();
    if (childpid>=0)
    {
        if (childpid==0)
        {
            if (execvp(firstCmd,args)<0)
            {
                perror("Error on execvp\n");
                exit(0);
            }

        }
        else{
                if (headProcess==NULL)
                {
                    headProcess=create_list(childpid,firstCmd);
                }
                else{
                    add_to_list(childpid,firstCmd,true);
                }
        }
    }
    else{
        perror("fork() error");
    }
    return 1;
}
int sh_bglist(char **args)
{
    print_list();
    return 1;
}
int sh_kill(char **args)
{
    // kill 1575
    char* pidCmd=args[1];
    if(!pidCmd)//kill 
    {
        printf("Please specify a pid\n");
    }
    else
    {
        int pid=atoi(pidCmd);
        process* target= search_in_list(pid, &headProcess);
        if (target!=NULL)
        {
            if (kill(target->pid,SIGTERM)>=0)
            {
                delete_from_list(pid);
            }
            else{
                perror("Could not kill pid specified\n");
            }
        }
        else{
            printf("Specify a pid which is present in the list.\nType \"bglist\" to see active processes\n");
        }
    }
    return 1;
}



int find_size(){
    int len=sizeof(builtin_str)/sizeof(char *);
    return len;
}
int sh_help(){
    printf("\nWelcome to ACM Shell, you can use the following commands:\n");
    int n=find_size();
    for(int i=0;i<n;i++){
        printf("%s\n" ,builtin_str[i]);
    }
    return 1;// successful
}


char *sh_read_line()
{
	char*line=NULL;
	ssize_t bufsize =0;
	if(getline(&line,&bufsize,stdin)==-1)
	{
		if(feof(stdin))
			exit(EXIT_SUCCESS);
		else
		{
			perror("acm-sh: getline\n");
			exit(EXIT_FAILURE);
		}
	}
	return line;
} 

#define SH_TOKEN_BUFSIZE 64
#define SH_TOK_DELIM " \t\r\n"

char** sh_split_line(char* line)
{
	int bufsize=SH_TOKEN_BUFSIZE,position=0;
	char ** tokens=malloc(bufsize * sizeof(char*));
	char *token;

	if(!token)
	{
		fprintf(stderr,"acm-sh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	//strtok :- Library Function 
	token=strtok(line,SH_TOK_DELIM);
	while(token!=NULL)
	{
		tokens[position]=token;
		token=strtok(NULL,SH_TOK_DELIM);

		position++;
	}
	tokens[position]=NULL;
	return tokens;
}

int sh_launch(char **args)
{
	pid_t pid;
	int status;

	pid=fork();
	if(pid==0)
	{
		if(execvp(args[0],args)==-1)
		{
			printf("Invalid Command\n");
		}
		exit(EXIT_FAILURE);
	}
	else if(pid<0)
	{
		perror("acm-sh");
	}
	else
	{
		do
		{
			waitpid(pid,&status,WUNTRACED);
		}
		while(!WIFEXITED(status)&&!WIFSIGNALED(status));
	}
	return 1;
}

int sh_execute(char ** args)
{
	int i;
	if(args[0]==NULL)
	{
		return 1;
	}

	for(int i=0;i<find_size();i++)
	{
		if(strcmp(args[0],builtin_str[i])==0)
			return (*builtin_func[i])(args);
	}

	return sh_launch(args);

}

void broadcastTermination(int pid,int status){
    if (WIFEXITED(status)){
        printf("exited, status=%d\n", WEXITSTATUS(status));
    }
    else if (WIFSIGNALED(status)){
        printf("killed by signal %d\n", WTERMSIG(status));
    }
    else if(WIFSTOPPED(status))
    {
        printf("stopped by signal %d\n", WSTOPSIG(status));
    }
    else if(WIFCONTINUED(status))
    {
        printf("continued\n");
    }
    delete_from_list(pid);
}

static void signalHandler(int sig){
    int pid;
    int status;
    pid=waitpid(-1,&status,WNOHANG);
    broadcastTermination(pid,status);
}
#define sh_HIST_BUFFER 101
int main(int argc,char **argv)
{
	char *line;
	char **args;
	int status;
	int history_len = sh_HIST_BUFFER;
	history = (char **)malloc(sizeof(char *)*history_len);
	command_count = 0;
    signal(SIGCHLD,signalHandler);
	do{
		
		printf("ACM-SH > ");
		line=sh_read_line();
		history[command_count] = (char *)malloc(sizeof(*line));
		strcpy(history[command_count], line);
		args=sh_split_line(line);
		status=sh_execute(args);

		free(line);
		free(args);
		command_count++;
        if(command_count>=history_len){
            // shift
            for(int i=0;i<history_len-1;i++){
                history[i]=history[i+1];
            }
            command_count--;
        }
	}
	while(status);

	return 0;
}