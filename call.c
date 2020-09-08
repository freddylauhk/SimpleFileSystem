#include "call.h"
const char *HD = "./HD";

int open_t(char *pathname, int flags)
{
	int inode_number;

	//Open the HD file
	int fd=open(HD, O_RDWR, 0660);
	if(!fd)
	{
		printf("Error in open()\n");
		return -1;
	}
	
	//read superblock
	struct superblock *superblock=(struct superblock *)malloc(sizeof(struct superblock));
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(read(fd, (struct superblock *)superblock, sizeof(struct superblock))<0)
	{
		printf("Error in read() of attainning superblock\n");
		return -1;
	}
	
	//read path
	int i;
	int count=0;
	char path[MAX_NESTING_DIR+1][10];
	char *temp;
	int isDir = -1;
	char *pathtemp = strdup(pathname);
	for(;;){
		temp = strsep(&pathtemp, "/");
		strcpy(path[count++],temp);
		if(pathtemp==NULL) break;
	}
	
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

	//find file
	inode_number = -1;
	DIR_NODE *mapping=(DIR_NODE *)malloc(sizeof(DIR_NODE));
	int tempcount=0;
	int tempstring = 0;

	if(count==2 && strcmp(path[1],"root")==0)
	{
		inode_number = 0;
	}
	else{
		for(i=2;i<count;i++)
		{
			tempstring = i;
			while(tempcount<temp_inode->file_num)
			{	
				if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
				{
					printf("Error in lseek() of attending directory");
					return -1;
				}	
				if(read(fd,(DIR_NODE *) mapping,sizeof(DIR_NODE))<0)
				{
					printf("Error in read() of attending mapping\n");
					return -1;
				}
				if(strcmp(mapping->dir,path[i])==0 && i==count-1){
					inode_number = mapping->inode_number;
					break;
				}
				else if(strcmp(mapping->dir,path[i])==0 && i!=count-1){
					if(lseek(fd, superblock->inode_offset+sizeof(struct inode)*mapping->inode_number, SEEK_SET)<0)
					{
						printf("Error in lseek() of attending directory\n");
						return -1;
					}
					if(read(fd,(struct inode *)temp_inode, sizeof(struct inode))<0)
					{
						printf("Error in read() of attending directory inode.\n");
						return -1;
					}
					tempcount=0;
					break;
				}
			
				tempcount++;
				
			}
			
		}
	}
	
	
	if(flags==0){
		
		//create an inode for new regular file
		struct inode *new_rfile=(struct inode *)malloc(sizeof(struct inode));
		new_rfile->i_number = superblock->next_available_inode;
		new_rfile->i_type = 0;
		new_rfile->i_size = MAX_FILE_SIZE;
		new_rfile->i_blocks = 1026;
		new_rfile->direct_blk[0] = superblock->next_available_blk++;
		new_rfile->direct_blk[1] = superblock->next_available_blk++;
		new_rfile->indirect_blk = superblock->next_available_blk+=1024;
		new_rfile->file_num = 0;
		time( &new_rfile->i_mtime );
		localtime( &new_rfile->i_mtime );

		//Write the new file inode to HD
		if(lseek(fd, superblock->inode_offset+(sizeof(struct inode)*new_rfile->i_number), SEEK_SET)<0)
		{
			printf("Error in lseek() of storing new inode\n");
			return -1;
		}
		if(write(fd, (void *)new_rfile, sizeof (struct inode))<0)
		{
			printf("Error in write() of storing new inode\n");
			return -1;
		}
		superblock->next_available_inode++;

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
		
		//change mapping if file exists
		if(inode_number != -1 && temp_inode->i_type == 0){
			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of attending mapping");
				return -1;
			}
			if(read(fd,(DIR_NODE *) mapping,sizeof(DIR_NODE))<0)
			{
				printf("Error in read() of attending mapping\n");
				return -1;
			}

			mapping->inode_number=new_rfile->i_number;

			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of attending mapping");
				return -1;
			}
			if(write(fd, (void *)mapping, sizeof(DIR_NODE))<0)
			{
				printf("Error in write() of updating mapping\n");
				return -1;
			}
		}
		else
		{
			//new mapping information
			DIR_NODE *new_mapping=(DIR_NODE *)malloc(sizeof(DIR_NODE));
			strcpy(new_mapping->dir,path[tempstring]);
			printf("%s\n",new_mapping->dir);
			new_mapping->inode_number = new_rfile->i_number;

			//update parent mapping
			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of storing mapping\n");
				return -1;
			}
			if(write(fd, (void *)new_mapping, sizeof(DIR_NODE))<0)
			{
				printf("Error in write() of updating mapping\n");
				return -1;
			}
			
			//update parent inode
			temp_inode->i_size+=sizeof(DIR_NODE);
			temp_inode->file_num++;
			if(lseek(fd,(superblock->inode_offset)+(sizeof(struct inode)*temp_inode->i_number),SEEK_SET)<0){
				printf("Error in lseek() of updating inode\n");
				return -1;
			}
			if(write(fd, (void *)temp_inode, sizeof(struct inode))<0)
			{
				printf("Error in write() of updating inode\n");
				return -1;
			}
			
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

		inode_number=new_rfile->i_number;
	}
	else if(flags == 1)
	{
		//build new file and config it with directory condfig
		struct inode *new_rfile=(struct inode *)malloc(sizeof(struct inode));
		new_rfile->i_number = superblock->next_available_inode;
		new_rfile->i_type = 1;
		new_rfile->i_size = sizeof(DIR_NODE)*2;
		new_rfile->i_blocks = 1;
		new_rfile->direct_blk[0] = superblock->next_available_blk++;
		new_rfile->direct_blk[1] = -1;
		new_rfile->indirect_blk = -1;
		new_rfile->file_num = 2;
		time( &new_rfile->i_mtime );
		localtime( &new_rfile->i_mtime );

		//Write the new file inode to HD
		if(lseek(fd, superblock->inode_offset+(sizeof(struct inode)*new_rfile->i_number), SEEK_SET)<0)
		{
			printf("Error in lseek() of storing new inode\n");
			return -1;
		}
		if(write(fd, (void *)new_rfile, sizeof (struct inode))<0)
		{
			printf("Error in write() of storing new inode\n");
			return -1;
		}
		superblock->next_available_inode++;

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
		
		//change mapping if file exists
		if(inode_number != -1 && temp_inode->i_type == 1){
			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of attending mapping");
				return -1;
			}
			if(read(fd,(DIR_NODE *) mapping,sizeof(DIR_NODE))<0)
			{
				printf("Error in read() of attending mapping\n");
				return -1;
			}

			mapping->inode_number=new_rfile->i_number;

			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of attending mapping");
				return -1;
			}
			if(write(fd, (void *)mapping, sizeof(DIR_NODE))<0)
			{
				printf("Error in write() of updating mapping\n");
				return -1;
			}
		}
		else
		{
			//new mapping information
			DIR_NODE *new_mapping=(DIR_NODE *)malloc(sizeof(DIR_NODE));
			strcpy(new_mapping->dir,path[tempstring]);
			printf("%s\n",new_mapping->dir);
			new_mapping->inode_number = new_rfile->i_number;

			//update parent mapping
			if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*temp_inode->direct_blk[0])+(sizeof(DIR_NODE)*tempcount), SEEK_SET)<0)
			{
				printf("Error in lseek() of storing mapping\n");
				return -1;
			}
			if(write(fd, (void *)new_mapping, sizeof(DIR_NODE))<0)
			{
				printf("Error in write() of updating mapping\n");
				return -1;
			}
			
			//update parent inode
			temp_inode->i_size+=sizeof(DIR_NODE);
			temp_inode->file_num++;
			if(lseek(fd,(superblock->inode_offset)+(sizeof(struct inode)*temp_inode->i_number),SEEK_SET)<0){
				printf("Error in lseek() of updating inode\n");
				return -1;
			}
			if(write(fd, (void *)temp_inode, sizeof(struct inode))<0)
			{
				printf("Error in write() of updating inode\n");
				return -1;
			}
			
		}
		//Initialize the directory name mapping
		DIR_NODE *self_mapping=(DIR_NODE *)malloc(sizeof(DIR_NODE));
		strcpy(self_mapping->dir, ".");
		self_mapping->inode_number = new_rfile->i_number;

		DIR_NODE *mapping_parent=(DIR_NODE *)malloc(sizeof(DIR_NODE));
		strcpy(mapping_parent->dir, "..");
		mapping_parent->inode_number = temp_inode->i_number;

		//write the mapping to the HD
		if(lseek(fd, (superblock->data_offset)+(superblock->blk_size*new_rfile->direct_blk[0]), SEEK_SET)<0)
		{
			printf("Error in lseek() of storing Root Directory mapping.\n");
			return -1;
		}
		if(write(fd, (void *)self_mapping, sizeof(DIR_NODE))<0)
		{
			printf("Error in write() of storing Root Directory\n");
			return -1;
		}
		if(write(fd, (void *)mapping_parent, sizeof(DIR_NODE))<0)
		{
			printf("Error in write() of storing Root Directory\n");
			return -1;
		}
		
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

		inode_number=new_rfile->i_number;
	}
	else if(flags == 2)
	{
		return inode_number;
	}
	
	
	close(fd);
        return inode_number;
}

int read_t(int inode_number, int offest, void *buf, int count)
{

	//Open the HD file
	int fd=open(HD, O_RDWR, 0660);
	if(!fd)
	{
		printf("Error in open()\n");
		return -1;
	}
	
	//read superblock
	struct superblock *superblock=(struct superblock *)malloc(sizeof(struct superblock));
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(read(fd, (struct superblock *)superblock, sizeof(struct superblock))<0)
	{
		printf("Error in read() of attainning superblock\n");
		return -1;
	}
	
	//read target i-node
	struct inode *temp_inode=(struct inode *)malloc(sizeof(struct inode));
	if(lseek(fd, superblock->inode_offset+(sizeof(struct inode)*inode_number), SEEK_SET)<0)
	{
		printf("Error in lseek() of attending directory\n");
		return -1;
	}
	if(read(fd,(struct inode *)temp_inode, sizeof(struct inode))<0)
	{
		printf("Error in read() of attending directory inode.\n");
		return -1;
	}
	
	int read_bytes = 0;
	// Read Directory file
	if(offest >= temp_inode->i_size)
	{
		return read_bytes;
	}
	if(temp_inode->i_type == 1){
		if(lseek(fd, superblock->data_offset+(superblock->blk_size*temp_inode->direct_blk[0])+offest,SEEK_SET)<0)
		{
			printf("Error in lseek() of attending directory data\n");
			return -1;
		}
		if(read(fd,(void *)buf, count)<0)
		{
			printf("Error in read() of attending directory data.\n");
			return -1;
		}
		read_bytes+=count;
	}

	// read regular file
	else if(temp_inode->i_type == 0){
		if(lseek(fd, superblock->data_offset+(superblock->blk_size*temp_inode->direct_blk[0])+offest,SEEK_SET)<0)
		{
			printf("Error in lseek() of attending directory data\n");
			return -1;
		}
		read_bytes=read(fd, buf, count);
	
		
	}
	close(fd);
	return read_bytes; 
}

int write_t(int inode_number, int offest, void *buf, int count)
{
	//Open the HD file
	int fd=open(HD, O_RDWR, 0660);
	if(!fd)
	{
		printf("Error in open()\n");
		return -1;
	}
	
	//read superblock
	struct superblock *superblock=(struct superblock *)malloc(sizeof(struct superblock));
	if(lseek(fd, SB_OFFSET, SEEK_SET)<0)
	{
		printf("Error in lseek() of storing SuperBlock\n");
		return -1;
	}
	if(read(fd, (struct superblock *)superblock, sizeof(struct superblock))<0)
	{
		printf("Error in read() of attainning superblock\n");
		return -1;
	}
	
	//read target i-node
	struct inode *temp_inode=(struct inode *)malloc(sizeof(struct inode));
	if(lseek(fd, superblock->inode_offset+(sizeof(struct inode)*inode_number), SEEK_SET)<0)
	{
		printf("Error in lseek() of attending directory\n");
		return -1;
	}
	if(read(fd,(struct inode *)temp_inode, sizeof(struct inode))<0)
	{
		printf("Error in read() of attending directory inode.\n");
		return -1;
	}
	
	int write_bytes = 0;
	
	int i;
	if(offest >= temp_inode->i_size)
	{
		return write_bytes;
	}
	if(lseek(fd, superblock->data_offset+superblock->blk_size*temp_inode->direct_blk[0]+offest,SEEK_SET)<0)
	{
		printf("Error in lseek() of storing directory data\n");
		return -1;
	}
	int can_write = temp_inode->i_size-offest;
	if(can_write >= count)
	{
		write_bytes=write(fd,buf,count);
	}
	else{
		write_bytes=write(fd,buf,can_write);
	}
	close(fd);
	return write_bytes;
}



