#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "include/fs.h"

#define T_DIR 1
#define T_FILE 2
#define T_DEV 3
#define DPB (BSIZE/sizeof(struct dirent))
int main(int argc, char* argv[]){


    if(argc > 2){
        fprintf(stderr, "Usage: xv6_fsck <file_system_image>.\n");
        exit(1);
    }

    
    int fd = open(argv[1], O_RDONLY);
    if(fd <= 0){
		fprintf(stderr, "image not found.\n");
		exit (1);
    }
    int rc;
    struct stat stat_bf;
    rc = fstat(fd, &stat_bf);
    assert(rc == 0);
    void * img_ptr = mmap(NULL, stat_bf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    
    assert(img_ptr != MAP_FAILED);
    

    struct superblock *sb;
    sb = (struct superblock*) (img_ptr + BSIZE);

    int i;

    
    struct dinode* di = (struct dinode*)(img_ptr+ (2* BSIZE));
    
    //#1
    for(i = 0; i< sb->ninodes; i++){
        //printf("INODE %d, TYPE %d\n", i, di->type); 
        if((di->type !=  0) && (di->type != T_DIR) && (di->type != T_FILE) && (di->type != T_DEV)) {
           fprintf(stderr,"ERROR: bad inode.\n");
            exit(1);
        }
        else
            di++;
    }    
    
    //#2

    //printf("The size of the fs is %d. There are %d data blocks. There are %d non-data blocks.\n",sb->size, sb->nblocks, (sb-> size - sb->nblocks));
    di = (struct dinode*) (img_ptr + (2*BSIZE));
    for(i = 0; i< sb->ninodes; i++){
        if((di->type == T_DIR) || (di->type == T_FILE) || (di->type == T_DEV)){
            int j;
            for(j = 0; j< NDIRECT+1; j++){
             	//printf("inode: %d, index: %d, value: %d\n",i, j, di->addrs[j]);
                if(((di->addrs[j]< (sb-> size - sb->nblocks)) && (di->addrs[j] != 0)) || (di->addrs[j] > sb->size)) {
                    
                    if(j != NDIRECT){
                        fprintf(stderr,"ERROR: bad direct address in inode.\n");
                    }else{
                        fprintf(stderr,"ERROR: bad indirect address in inode.\n");
                    }
                    exit(1);      
                }

            }
			if(di->addrs[NDIRECT] != 0){
				uint* indir = (uint*)(img_ptr + (di->addrs[NDIRECT]*BSIZE));
				for(int k = 0; k< NINDIRECT; k++){
					if(((*indir< (sb-> size - sb->nblocks)) && (*indir != 0)) || (*indir > sb->size)){
						fprintf(stderr,"ERROR: bad indirect address in inode.\n");
						exit(1);
					}
					indir++;

				}
			}

        }

        di++;

    }
    
	//#4 first so it does not say not root directory stuff
	di = (struct dinode*) (img_ptr + (2*BSIZE));
	int dir_dot = 0;
	int dir_dotdot = 0;
	for(i = 0; i< sb->ninodes; i++){
		if(di->type == T_DIR){
			
            int j;
            for(j = 0; j < NDIRECT+1; j++){
               
		            struct dirent * dre = (struct dirent*) (img_ptr + (di->addrs[j]*BSIZE));
					for(int z = 0; z< DPB; z++){
		            	//printf("index: %d, entry address: %d, inode number of dir entry: %d, name of dir entry: %s\n", j,di->addrs[j],dre->inum, dre->name);
						if((strcmp(".", dre->name) == 0) && (dre->inum == i)){
							dir_dot = 1;
						}
						
						if(strcmp("..", dre->name)==0){
							dir_dotdot = 1;
						}
						dre++; 
					}
					

				
            }

        }
		
		
        di++;

    }
	
	int dir_ok = dir_dot && dir_dotdot;
	if(!dir_ok){
		fprintf(stderr, "ERROR: directory not properly formatted.\n");
		exit(1);
	}
    //#3
    
    di = (struct dinode*) (img_ptr + (2*BSIZE));
	int root_inode = 0;
	int root_parent = 0;
	
    for(i = 0; i< sb->ninodes; i++){
        if(di->type == T_DIR){
            
			if(i == 1){
				root_inode = 1;
				int j;
		        for(j = 0; j < NDIRECT+1; j++){
				        struct dirent * dre = (struct dirent*) (img_ptr + (di->addrs[j]*BSIZE));
						for(int z = 0; z< DPB; z++){
				        	//printf("index: %d, entry address: %d, inode number of dir entry: %d, name of dir entry: %s\n", j,di->addrs[j],dre->inum, dre->name);
						
							if((dre->inum == 1) && (strcmp("..", dre->name) == 0)){
								root_parent = 1;

							}
							
							dre++;
						}
				}
			}
		}
		di++;

	}

	int root_ok = root_inode && root_parent;
	if(!root_ok){
		fprintf(stderr, "ERROR: root directory does not exist.\n");
		exit(1);
	}


	

}
