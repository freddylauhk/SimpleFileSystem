#include "call.h"

int main(int argc, char * argv[]) 
{
	
   	/*
	 Get the inode number according to the pathname from argv[1].
        Return value -1 means that there is no requested inode number.
	*/
	char pathname[MAX_COMMAND_LENGTH];
	strcpy(pathname, argv[1]);

	int inode_number=open_t(pathname, 2);
    	if(inode_number<0){
		printf("open_t() failed.\n");
		return -1;
	}
      	
   	/*
	Read the content of the requested file to the buf,
	then print out its content one character by one character
	*/
	char buf[MAX_FILE_SIZE];
	int read_size=read_t(inode_number, 0, buf, MAX_FILE_SIZE);
	int i;
	for(i=0; i<read_size; i++)
		printf("%c",buf[i]);
	
	return 1;
}

