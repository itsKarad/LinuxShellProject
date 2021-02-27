#include "linkedlist.h"
#include <signal.h>

struct process *head = NULL;
struct process *curr = NULL;
char cwd[1024];

struct process* create_list(int pid, char* firstCmd)
{
	getcwd(cwd, sizeof(cwd));
	char* path = charAppend(cwd, '/');
	path = strAppend(cwd, firstCmd);
    //printf("\n creating list with headnode as [%d] %s\n",pid, path);
    struct process *ptr = (struct process*)malloc(sizeof(struct process));
    if(NULL == ptr)
    {
        printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->pid = pid;
	ptr->path = path;
    ptr->next = NULL;

    head = curr = ptr;
    return ptr;
}

struct process* add_to_list(int pid, char* firstCmd, bool add_to_end)
{
	getcwd(cwd, sizeof(cwd));
	char* path = charAppend(cwd, '/');
	path = strAppend(cwd, firstCmd);
	
    if(NULL == head)
    {
        return (create_list(pid, path));
    }

    struct process *ptr = (struct process*)malloc(sizeof(struct process));
    if(NULL == ptr)
    {
        //printf("\n Node creation failed \n");
        return NULL;
    }
    ptr->pid = pid;
	ptr->path = path;
    ptr->next = NULL;

    if(add_to_end)
    {
		//printf("\n Adding node to end of list with pidue [%d]\n",pid);
        curr->next = ptr;
        curr = ptr;
    }
    else
    {
		//printf("\n Adding node to beginning of list with pidue [%d]\n",pid);
        ptr->next = head;
        head = ptr;
    }
	//printf("Added pid [%d] %s to list\n",ptr->pid, ptr->path);
    return ptr;
}

struct process* search_in_list(int pid, struct process **prev)
{
    struct process *ptr = head;
    struct process *tmp = NULL;
    bool found = false;

    //printf("\n Searching the list for pidue [%d] \n",pid);

    while(ptr != NULL)
    {
        if(ptr->pid == pid)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}

int delete_from_list(int pid)
{
    struct process *prev = NULL;
    struct process *del = NULL;

    del = search_in_list(pid,&prev);
    if(del == NULL)
    {
        return -1;
    }
    else
    {
        if(prev != NULL)
            prev->next = del->next;

        if(del == curr)
        {
            curr = prev;
        }
        else if(del == head)
        {
            head = del->next;
        }
    }

	//printf("\n Deleted pid [%d] from list\n",del->pid);
	
	if(del == head){
		//printf("deleting head\n");
		head=NULL;
	}
	free(del);
    del = NULL;
    return 0;
}

void print_list(void)
{
    struct process *ptr = head;
	int length=0;
	printf("\n =============== Background Process List =============== \n");
    while(ptr != NULL)
    {
		length++;
        printf("\n [%d] %s\n",ptr->pid, ptr->path);
        ptr = ptr->next;
    }
	
	printf(" Total Background Jobs: %d\n",length);
	printf(" ======================================================= \n\n");
    return;
}

void kill_list()
{
	process* ptr = head;
	while(ptr != NULL)
	{
		if(kill(ptr->pid, SIGTERM) >= 0){
			delete_from_list(ptr->pid);
		}else{
			perror("Attempting to kill entire list");
			printf("Could not kill process %d\n",ptr->pid);
		}
		ptr = ptr->next;
	}
	
}
