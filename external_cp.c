#include "call.h"

int main (int argc, char *argv[])
{
	/*
	Get the source pathname from argv[1], and get the destination pathname from argv[2]. Two pathname are both absolute path.
	e.g. ./external_cp [src_path] [dest_path]
	*/
	char src[MAX_COMMAND_LENGTH];
	strcpy(src, argv[1]);
	
	char dest[MAX_COMMAND_LENGTH];
	strcpy(dest, argv[2]);

	/*
	Open source file and get its file descriptor
	*/
	int src_fd=open(src, O_RDONLY, 0660);
	
	if(src_fd<0){
		printf("open() failed\n");
		return -1;
	}

	/*
	Allocate a buf with MAX_FILE_SIZE.
	Read up to MAX_FILE_SIZE bytes from source file to buf
	*/
	char buf[MAX_FILE_SIZE];
	int read_size=read(src_fd, buf, MAX_FILE_SIZE);
	if(read_size < 0){
		printf("read() failed\n");
		return -1;
	}
	/*
	Create a new regular file with a valid input filename.
	Write the buf to the destination file
	*/
	int dest_inode=open_t (dest, 0);
	if(dest_inode == -1){
		printf("open_t() failed\n");
		return -1;

	}
	int write_size=write_t(dest_inode,0,buf,read_size);
	if(write_size == -1){
		printf("write_t() failed\n");
		return -1;
	}
	return 1;
}

