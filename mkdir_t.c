#include "call.h"

int main (int argc, char *argv[])
{
    	/*
	Creat a new directory file with a valid input pathname.
	a. The last part of the pathname is the name of new directory file.
	   e.g. Input Pathname: /root/abc/new_dir 
	        "new_dir" is the name of new directory file under the parent directory "root/abc".
        b. The argument flags 1 means that a new directory file will be created
	*/
	int inode_number=open_t(argv[1], 1);
	if(inode_number<0)
        {
                printf("Found inode failed\n");
                return -1;
        }

	return 1;
}

