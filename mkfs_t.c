#include <fcntl.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include "./superblock.h"
#include "./inode.h"

const char *HD = "./HD";

int main()
{
	//Please input your code
	//Flush the superblock region and initialize the root directory in the file.


	//Open the HD file
	int fd=open(HD, O_RDWR, 0660);
	if(!fd)
	{
		printf("Error in open()\n");
		return -1;
	}

	//Define a new superblock structure
	struct superblock *superblock=(struct superblock *)malloc(sizeof(struct superblock));
	superblock->inode_offset = INODE_OFFSET;
	superblock->data_offset = DATA_OFFSET;
	superblock->max_inode = MAX_INODE;
	superblock->max_data_blk = MAX_DATA_BLK;
	superblock->next_available_inode = 0;
	superblock->next_available_blk = 0;
	superblock->blk_size = BLOCK_SIZE;

	//Write the superblock to the region.
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(write(fd, (void *)superblock, sizeof (struct superblock))<0)
	{
		printf("Error in write() of storing SuperBlock\n");
		return -1;
	}
	
	//Read SuperBlock
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(read(fd, (struct superblock *)superblock, sizeof(struct superblock))<0)
	{
		printf("Error in read() of attainning SuperBlock\n");
		return -1;
	}

	//print for debug
	printf("inode_offset:%d\n data_offset:%d\n max_inode:%d\n max_data_blk:%d\n next_available_inode:%d\n next_available_blk:%d\n blk_size:%d\n\n",superblock->inode_offset,superblock->data_offset,superblock->max_inode,superblock->max_data_blk,superblock->next_available_inode,superblock->next_available_blk,superblock->blk_size);

	//Initialize the root directory.
	struct inode *root=(struct inode *)malloc(sizeof(struct inode));
	root->i_number = superblock->next_available_inode;
	root->i_type = 1;
	root->i_size = sizeof(DIR_NODE)*2;
	root->i_blocks = 1;
	root->direct_blk[0] = 0;
	root->direct_blk[1] = -1;
	root->indirect_blk = -1;
	root->file_num = 2;

	time( &root->i_mtime );
	printf ( "i-Node root created at %s\n", asctime (localtime( &root->i_mtime )) );
	

	//Initialize the root directory name mapping
	DIR_NODE *root_mapping=(DIR_NODE *)malloc(sizeof(DIR_NODE));
	strcpy(root_mapping->dir, ".");
	root_mapping->inode_number = 0;

	DIR_NODE *root_mapping_parent=(DIR_NODE *)malloc(sizeof(DIR_NODE));
	strcpy(root_mapping_parent->dir, "..");
	root_mapping_parent->inode_number = 0;

	//Write the root directory to HD
	if(lseek(fd, superblock->inode_offset, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing Root Directory\n");
		return -1;
	}
	if(write(fd, (void *)root, sizeof(struct inode))<0)
	{
		printf("Error in write() of storing Root Directory\n");
		return -1;
	}
	superblock->next_available_inode++;
	//testing
	//read root directory
	struct inode *temp_inode=(struct inode *)malloc(sizeof(struct inode));
	if(lseek(fd, superblock->inode_offset, SEEK_SET)<0)
	{
		printf("Error in lseek() of attending root directory\n");
		return -1;
	}
	if(read(fd,(struct inode *)temp_inode, sizeof(struct inode))<0)
	{
		printf("Error in read() of attending root directory inode.\n");
		return -1;
	}


	if(lseek(fd, superblock->data_offset, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing Root Directory mapping.\n");
		return -1;
	}
	//Write the root directory mapping to HD
	if(write(fd, (void *)root_mapping, sizeof(DIR_NODE))<0)
	{
		printf("Error in write() of storing Root Directory\n");
		return -1;
	}
	if(write(fd, (void *)root_mapping_parent, sizeof(DIR_NODE))<0)
	{
		printf("Error in write() of storing Root Directory\n");
		return -1;
	}


	//Update superblock
	
	superblock->next_available_blk++;

	//Update superblock to HD
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(write(fd, (void *)superblock, sizeof (struct superblock))<0)
	{
		printf("Error in write() of storing SuperBlock\n");
		return -1;
	}

	//Read SuperBlock
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(read(fd, (struct superblock *)superblock, sizeof(struct superblock))<0)
	{
		printf("Error in read() of attainning the POLYU Information\n");
		return -1;
	}

	//print for debug
	printf("inode_offset:%d\n data_offset:%d\n max_inode:%d\n max_data_blk:%d\n next_available_inode:%d\n next_available_blk:%d\n blk_size:%d\n\n",superblock->inode_offset,superblock->data_offset,superblock->max_inode,superblock->max_data_blk,superblock->next_available_inode,superblock->next_available_blk,superblock->blk_size);
	printf("SFS built!\n");
	close(fd);
	return 0;
}
