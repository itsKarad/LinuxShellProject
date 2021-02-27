#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <memory.h>
#include <stdbool.h>
#include <sys/wait.h>


char* charAppend(char*, char);
char* strAppend(char*, char*);
char* getNewArray();
bool charInSet(char, char[]);
