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


void duplicate_dir_address(uint addrs, void* img_ptr, struct superblock* sb,struct dinode* di ){

	struct dinode* di2 = (struct dinode*) (img_ptr + (2*BSIZE));
	for(int k = 0; k<sb->ninodes; k++){
		if(di2 != di){
			for(int l = 0; l<NDIRECT+1; l++){
				if(di2->addrs[l] != 0){
					if(di2->addrs[l] == addrs){
						fprintf(stderr, "ERROR: direct address used more than once.\n");
						exit(1);
					}
				}
			}
			
			if(di2->addrs[NDIRECT] != 0){
				uint* indir = (uint*) (img_ptr + (di2->addrs[NDIRECT] *BSIZE));
				for(int z = 0; z< NINDIRECT; z++){
					if(*indir != 0){
						if(*indir == addrs){
							fprintf(stderr, "ERROR: direct address used more than once.\n");
							exit(1);
						}
					}
					indir++;
				}
			}
		}
	di2++;

	}


}

void duplicate_indir_address(uint addrs, void* img_ptr, struct superblock* sb,struct dinode* di ){

	struct dinode* di2 = (struct dinode*) (img_ptr + (2*BSIZE));
	for(int k = 0; k<sb->ninodes; k++){
		if(di2 != di){
			for(int l = 0; l<NDIRECT+1; l++){
				if(di2->addrs[l] != 0){
					if(di2->addrs[l] == addrs){
						fprintf(stderr, "ERROR: indirect address used more than once.\n");
						exit(1);
					}
				}
			}
			
			if(di2->addrs[NDIRECT] != 0){
				uint* indir = (uint*) (img_ptr + (di2->addrs[NDIRECT] *BSIZE));
				for(int z = 0; z< NINDIRECT; z++){
					if(*indir != 0){
						if(*indir == addrs){
							fprintf(stderr, "ERROR: indirect address used more than once.\n");
							exit(1);
						}
					}
					indir++;
				}
			}
		}
	di2++;

	}


}

void check_directory_link(short nlink, void* img_ptr, struct superblock* sb, int inode_num){
	short curr_link = 0;	
	struct dinode* di2 = (struct dinode*) (img_ptr + (2*BSIZE));
	for(int i = 0; i<sb->ninodes; i++){
		if(di2->type == T_DIR){
			for(int j = 0; j<NDIRECT; j++){
				if(di2->addrs[j] !=0){
					struct dirent* dre = (struct dirent*)(img_ptr + (di2->addrs[j] *BSIZE));
					for(int z = 0; z<DPB; z++){
						if(dre->inum == inode_num){
							curr_link++;
						}
						dre++;
					}

				}
			}
			
			if(di2->addrs[NDIRECT] != 0){
				uint* indir = (uint*) (img_ptr + (di2->addrs[NDIRECT] *BSIZE));
				for(int k = 0; k<NINDIRECT; k++){
					if(*indir != 0){
						struct dirent* dre = (struct dirent*) (img_ptr + (*indir * BSIZE));
						for(int y = 0; y<DPB; y++){
							if(dre->inum == inode_num){
								curr_link++;
							}		
							dre++;
						}
					}
					indir++;
				}

			}

		}

		di2++;
	}

	if(curr_link != nlink){
		fprintf(stderr, "ERROR: bad reference count for file.\n");
		exit(1);

	}

}


void check_multi_dir_ref(void* img_ptr, struct superblock* sb, int inode_num){

	short curr_link = 0;	
	struct dinode* di2 = (struct dinode*) (img_ptr + (2*BSIZE));
	for(int i = 0; i<sb->ninodes; i++){
		if(di2->type == T_DIR){
			for(int j = 0; j<NDIRECT; j++){
				if(di2->addrs[j] !=0){
					struct dirent* dre = (struct dirent*)(img_ptr + (di2->addrs[j] *BSIZE));
					//skipping . and ..					
					dre++;
					dre++;
					for(int z = 2; z<DPB; z++){
						if(dre->inum == inode_num){
							curr_link++;
						}
						dre++;
					}

				}
			}
			
			if(di2->addrs[NDIRECT] != 0){
				uint* indir = (uint*) (img_ptr + (di2->addrs[NDIRECT] *BSIZE));
				for(int k = 0; k<NINDIRECT; k++){
					if(*indir != 0){
						struct dirent* dre = (struct dirent*) (img_ptr + (*indir * BSIZE));
						for(int y = 0; y<DPB; y++){
							if(dre->inum == inode_num){
								curr_link++;
							}		
							dre++;
						}
					}
					indir++;
				}

			}

		}

		di2++;
	}

	if(curr_link >1){
		fprintf(stderr, "ERROR: directory appears more than once in file system.\n");
		exit(1);

	}




}
int main(int argc, char* argv[]){


    if(argc > 2){
        fprintf(stderr, "Usage: xv6_fsck <file_system_image>.\n");
        exit(1);
    }

    
    int fd = open(argv[1], O_RDONLY);
    if(fd < 0){
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



    //#5
    di = (struct dinode*) (img_ptr + (2*BSIZE));
	
	for(i = 0; i< sb->ninodes; i++){
		if((di->type == T_DIR) || (di->type == T_FILE) || (di->type == T_DEV)){

			int j;
			for(j = 0; j< NDIRECT+1; j++){
				if(di->addrs[j] != 0){
				char* bitmap = (char*) (img_ptr +(BBLOCK(di->addrs[j], sb->ninodes) * BSIZE));
				
				int bi = di->addrs[j]%BPB;
				int m = 1 << (bi %8);
					if((m &bitmap[bi/8])== 0){
						fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
						exit(1);
					}
						
				}
			}
			if(di->addrs[NDIRECT] != 0){
				uint* indir = (uint*)(img_ptr + (di->addrs[NDIRECT]*BSIZE));
				for(int k = 0; k< NINDIRECT; k++){
				char* bitmap = (char*) (img_ptr +(BBLOCK(*indir, sb->ninodes) * BSIZE));
				int bi = *indir%BPB;
				int m = 1 << (bi %8);
					if((m &bitmap[bi/8])== 0){
						fprintf(stderr, "ERROR: address used by inode but marked free in bitmap.\n");
						exit(1);
					}
				indir++;
				}
			}
			
		}
		di++;	
	}


   	//#6
	
   	
	int data_inuse ;
	int data_inuse_in;
	//printf("%d", BPB%sb->size);
	for(uint bi = sb->size - sb->nblocks; bi< BPB; bi++){
		data_inuse = 0;
		data_inuse_in = 0;
		char* bitmap = (char*) (img_ptr + (BBLOCK(bi, sb->ninodes)*BSIZE));
		int m = 1 <<((bi%BPB)%8);
		//printf("bit index: %d, bit in bitmap %u\n", bi, m&bitmap[bi/8]);
		if((m & bitmap[(bi%BPB)/8]) != 0){
			di = (struct dinode*) (img_ptr + (2*BSIZE));
			for(i = 0; i<sb->ninodes; i++){
				if((di->type == T_DIR) || (di->type == T_FILE) || (di->type == T_DEV)){			
					int j;
					for(j = 0; j<NDIRECT+1; j++){
						
						if(di->addrs[j] == bi){
							//printf("BLOCK NUMBER %d, bit index %d\n", di->addrs[j], bi);
							data_inuse = 1;
							break;
							
						}
					}
					if(di->addrs[NDIRECT] != 0){
						uint* indir = (uint*) (img_ptr + (di->addrs[NDIRECT] *BSIZE));
						for(int k =0; k<NINDIRECT; k++){
							if(*indir == bi){
								data_inuse_in = 1;
								break;
								
							}
							indir++;
						}
					}

				}
				di++;
			}
			int data_ok = data_inuse || data_inuse_in;

			if(!data_ok){
				fprintf(stderr,"ERROR: bitmap marks block in use but it is not in use.\n");
				exit(1);

			}

	
		}
		
	}


	//#7 & #8
	di = (struct dinode*) (img_ptr + (2*BSIZE));
		
	for(i = 0; i<sb->ninodes; i++){
		if((di->type == T_DIR) || (di->type == T_FILE) || (di->type == T_DEV)){
			for(int j = 0; j<NDIRECT+1; j++){
				if(di->addrs[j] != 0){
					
					duplicate_dir_address(di->addrs[j], img_ptr, sb, di);

				}
			}

			if(di->addrs[NDIRECT] !=0){
				
				uint* indir = (uint*) (img_ptr + (di->addrs[NDIRECT]*BSIZE));
				for(int z = 0; z< NINDIRECT; z++){
					duplicate_indir_address(*indir, img_ptr, sb, di);
					indir++;
				}
				
			}			
		}
		di++;
	}


	//#9
	di = (struct dinode*) (img_ptr + (2*BSIZE));
	int in_dir;
	di++;
	di++;
	//skipping root
	for(i = 2; i< sb->ninodes; i++){
		in_dir = 0;
		if((di->type == T_DIR) || (di->type == T_FILE) || (di->type == T_DEV)){
			
			struct dinode* dir = (struct dinode*) (img_ptr+2*BSIZE);			
			for(int k = 0; k < sb->ninodes; k++){
				if(dir->type == T_DIR){
					for(int j = 0; j<NDIRECT; j++){					
						struct dirent* dre = (struct dirent*) (img_ptr+dir->addrs[j]*BSIZE);
						// skipping . and ..
						dre++;
						dre++;
						for(int y = 0; y< DPB; y++){
							if(dre->inum == i){
								in_dir =1;
								break;
							}
							dre++;
						}
					}
					if(dir->addrs[NDIRECT] != 0){
						uint * indir = (uint*) (img_ptr + dir->addrs[NDIRECT] *BSIZE);
						for(int z=0; z<NINDIRECT; z++){
							struct dirent* dre = (struct dirent*) (img_ptr + (*indir*BSIZE));
														
							for(int y = 0; y<DPB; y++){
								if(dre->inum == i){
									in_dir = 1;
									break;
								}
								dre++;
							}

							indir++;
						}
			
					}
				}
			dir++;

			}
			if(!in_dir){
				fprintf(stderr, "ERROR: inode marked use but not found in a directory.\n");
				exit(1);
			}
		}

		

		di++;
	}
	

	//#10
/*
	di = (struct dinode*) (img_ptr + (2*BSIZE));
	for(i = 0; i< sb->ninodes; i++){
		if(di->type == T_DIR){		
			for(int j=0; j<NDIRECT; j++){
				struct dirent* dre = (struct dirent*) (img_ptr + di->addrs[j]*BSIZE);

				for(int z = 0; z< DPB; z++){
					int inode_num = dre->inum;
					if(inode_num != 0){
						struct dinode* di2 = (struct dinode*) (img_ptr + (IBLOCK(inode_num)*BSIZE));
						for(int a = 0; a<inode_num; a++){						
								di2++;
						}
						printf("inode number: %d. Block inode is in: %lu. Inode type: %d. directory entry name %s\n", inode_num, IBLOCK(inode_num), di2->type, dre->name);
						if(di2->type == 0){
							//fprintf(stderr, "ERROR: inode referred to in directory but marked free.\n");
							//exit(1);
						}
					}
					dre++;

				}
			}

			if(di->addrs[NDIRECT] !=0){
				uint* indir = (uint*) (img_ptr + (di->addrs[NDIRECT] * BSIZE));
				for(int k = 0; k<NINDIRECT; k++){
					struct dirent* dre = (struct dirent*) (img_ptr + (*indir*BSIZE));
					for(int g =0; g<DPB; g++){
						int inode_num = dre->inum;
						if(inode_num != 0){
							struct dinode* di2 = (struct dinode*) (img_ptr + (IBLOCK(inode_num)*BSIZE));
							di2 = di2 + inode_num;
							//printf("INDIRECT: inode number: %d. Block inode is in: %lu. Inode type: %d.\n", inode_num, IBLOCK(inode_num), di2->type);
							if(di2->type == 0){
								//fprintf(stderr, "ERROR: inode referred to in directory but marked free.\n");
								//exit(1);
							}
						}
						dre++;
					}					
					
					indir++;
				}

			}
		}
		printf("ACTUAL: inode num :%d, block inode is in: %lu, inode type: %d\n", i , IBLOCK(i), di->type);
		di++;
	}


	
*/




	//#11

	di = (struct dinode*) (img_ptr + (2*BSIZE));
	for(i = 0; i<sb->ninodes; i++){
		if(di->type == T_FILE){
			check_directory_link(di->nlink, img_ptr, sb,i);			

		}
		
		di++;
	}


	//#12

	di = (struct dinode*) (img_ptr + (2*BSIZE));
	for(i = 0; i<sb->ninodes; i++){
		if(di->type == T_DIR){
			check_multi_dir_ref(img_ptr, sb,i);			

		}
		
		di++;
	}




}
