#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include "include/fs.h"

#define T_DIR 1
#define T_FILE 2
#define T_DEV 3

int main(int argc, char* argv[]){


    if(argc > 2){
        fprintf(stderr, "Usage: xv6_fsck <file_system_image>.\n");
        exit(1);
    }

    
    int fd = open(argv[1], O_RDONLY);

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

    //printf("There are %d blocks. There are %d data blocks.\n", sb->nblocks, (sb->nblocks - sb->ninodes -2)/2);
    struct dinode* di2 = (struct dinode*) (img_ptr + (2*BSIZE));
    for(i = 0; i< sb->ninodes; i++){
        if((di2->type == T_DIR) || (di2->type == T_FILE) || (di2->type == T_DEV)){
            int j;
            for(j = 0; j< NDIRECT +1; j++){
      //        printf("inode: %d, index: %d, value: %d\n",i, j, di2->addrs[j]);
                if(di2->addrs[j]> sb->nblocks) {
                    if(j != NDIRECT)
                        fprintf(stderr,"ERROR: bad direct address in inode.\n");
                    else
                        fprintf(stderr,"ERROR: bad indirect address in inode.\n");

                    exit(1);
                }

            }

        }

        di2++;

    }
    

    //#3

}
