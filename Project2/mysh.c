#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>

int main(){
    int command_num = 1;
    char* command = (char*)malloc(128*sizeof(char));
    while(1){
        printf("mysh (%d)>", command_num);
        fgets(command, 128*sizeof(char), stdin);
        
       


        if(strstr(command, "exit") != NULL){
                exit(0);
        }else if(strstr(command, "cd") != NULL){
                strtok(command, " ");
                char* directory = strtok(NULL, " ");
                if(directory == NULL){
                    directory = getenv("HOME");
                    chdir(directory);
                }else{
                    char* curr_dir = (char*)malloc(128);
                    getcwd(curr_dir, 124);
                    char* path = (char*) malloc(strlen(directory)+strlen(curr_dir));
                    strcat(path,curr_dir+16);
                    strcat(path, "/");
                    strcat(path, directory);
                    chdir(path);
                            
                    
                }   

        }else if(strstr(command, "pwd")!=NULL){
                char* directory = (char*)malloc(sizeof(char)*124);
                getcwd(directory, 124);
                printf("%s\n", directory);

        }

        fflush(stdout);
        command_num++;

            


    }


    free(command);


}
