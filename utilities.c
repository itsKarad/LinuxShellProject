#include "utilities.h"

//======= Constants ==========
static int const MAX_LINE_LENGTH = 100;
//============================

char* charAppend(char* s, char c)
{
	int len = strlen(s);
	s[len] = c;
	return s;
}

char* strAppend(char* str1, char* str2)
{
	char* str3 = (char*)malloc(sizeof(char*)*(strlen(str1)+strlen(str2)));
	strcpy(str3, str1);
	strcat(str3, str2);
	//printf("%s appended to %s --> %s\n",str1,str2,str3);
	return str3;
}

char* getNewArray()
{
	char *arr = (char*)malloc(sizeof(char*)*MAX_LINE_LENGTH);
	return arr;
}

// returns true if character c is equivalent to any element in "set"
bool charInSet(char c, char set[])
{
	int i;
	for(i=0; i<strlen(set); i++)
	{
		if(c == set[i])
			return true;
	}
	return false;
}