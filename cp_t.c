#include "call.h"

int main (int argc, char *argv[])
{
    	/*
        Get the source pathname from argv[1], and get the destination pathname from argv[2]. Two pathname are both absolute path.
        e.g. ./cp_t [src_path] [dest_path]
        */
	char src_path[MAX_COMMAND_LENGTH];
	strcpy(src_path, argv[1]);
	
	char dest_path[MAX_COMMAND_LENGTH];
	strcpy(dest_path, argv[2]);
	
	/*
	Open source file with a valid pathname(only for regular file),
	then read its content to a buf
	*/
	int src_inode=open_t (src_path, 2);
	char buf[MAX_FILE_SIZE];
    	int read_size=read_t(src_inode, 0, buf, MAX_FILE_SIZE);
	
	/*
	Create a new regular file with a valid input filename,
	write the buf to the destination file
	*/
	int dest_inode=open_t (dest_path, 0);
	if(dest_inode < 0){
		printf("open_t() failed\n");
		return -1;
	}
      
    	int write_size= write_t(dest_inode, 0, buf, read_size);
	if(write_size < 0){
           printf("write_t() failed\n");
		return -1;
   
	}
	return 1;
}

