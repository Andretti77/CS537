#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/wait.h>

int main(){
    int command_num = 1;
    while(1){
        char* input = (char*) malloc(sizeof(char)*128);
        printf("mysh (%d)>", command_num);
        fgets(input, 128*sizeof(char), stdin);
        char* command = strtok(input, " \n");
       


        if(strcmp(command, "exit") == 0){
                exit(0);
        }else if(strcmp(command, "cd") == 0){
                char* directory = strtok(NULL, " \n");
                if(directory == NULL){
                    directory = getenv("HOME");
                    chdir(directory);

                }else{
                    char* path = (char*) malloc(strlen(directory));
                    strcat(path, "./");
                    strcat(path, directory);
                    chdir(path);
                    free(path);
                }   

        }else if(strcmp(command, "pwd") == 0){
                char* directory = (char*)malloc(sizeof(char)*124);
                getcwd(directory, 124);
                printf("%s\n", directory);
                free(directory);

        }else{
            char** arguments = (char**) malloc(64*sizeof(char*));
            int i;
            arguments[0] = command;
            for(i=1; i<256; i++){

                char* arg = (char*) malloc(128);
                arg = strtok(NULL, " \n");
                arguments[i]= arg;
                if(arg == NULL){
                    break;
                }
                
            }
       
            int pid;

            pid= fork();

            if(pid == 0){   
                execvp(command,arguments);
            }else{
                waitpid(pid, NULL, 0);
            }


        }

        fflush(stdout);
        command_num++;
    


        free(input);

    }
}
