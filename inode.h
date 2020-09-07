#ifndef _INODE_H_
#define _INODE_H_
#include <time.h>

typedef int file_type;

struct inode 
{
	int i_number;
	time_t i_mtime;
	file_type i_type;
	int i_size;
	int i_blocks;
	int direct_blk[2];
	int indirect_blk;
	int file_num;
};

typedef struct dir_mapping
{
	char dir[10];
	int inode_number;
}DIR_NODE;



#endif
