#include "utilities.h"

typedef struct process
{
	int pid;
	char* path;
	struct process* next;
} process;

struct process* create_list(int, char*);
struct process* add_to_list(int, char*, bool);
struct process* search_in_list(int, struct process **);
int delete_from_list(int);
void print_list(void);
void kill_list();
