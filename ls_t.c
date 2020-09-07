#include "call.h"

int main (int argc, char *argv[])
{
	/*
	Get inode number according to the pathname from argv[1].
	Return value -1 means that there is no requested inode number.
	*/
	char pathname[MAX_COMMAND_LENGTH];
	strcpy(pathname, argv[1]);

	/*
	Invoke open_t() function to attain the inode number of an existing valid directory 
	*/
	int inode_number=open_t(pathname, 2);
	if(inode_number<0)
        {
                printf("open_t() failed\n");
                return  -1;
        }
	
	/*
	Define a temporary struct for entry in directory file
	*/
	DIR_NODE *temp= (DIR_NODE *)malloc (sizeof(DIR_NODE));
    
	/*
	Attain all the entries in a directory file by reading its block content.
	Only print out file names of entries.
	*/
	int entry_num=0;
	while(1)
	{
		int ret=read_t(inode_number, entry_num*sizeof(DIR_NODE), temp, sizeof(DIR_NODE));
		if(ret<=0)
			return -1;
		else 
			printf("%s\n", temp->dir);
		entry_num++;
        }
   
	return 1;
}

