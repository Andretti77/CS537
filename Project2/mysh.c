#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(){
    int command_num = 1;
    while(1){
        char* input = (char*) malloc(sizeof(char)*128);
        printf("mysh (%d)>", command_num);
        fgets(input, 128*sizeof(char), stdin);
        char* command = strtok(input, " \n\t");
       


        if(strcmp(command, "exit") == 0){
                exit(0);
        }else if(strcmp(command, "cd") == 0){
                char* directory = strtok(NULL, " \n\t");
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
            int fd;
            int out_redirection=0;
            char* outfile;
            printf("HERE\n");
            arguments[0] = command;
            for(i=1; i<256; i++){
                char* arg = (char*) malloc(128);
                arg = strtok(NULL, " \n\t");
                if(strcmp(arg, ">") == 0){
                    out_redirection = 1;
                    outfile = strtok(NULL, " \n\t");
                    fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC);
                    



                }else{
                    
                    arguments[i]= arg;
                
                    if(arg == NULL){
                        break;
                    }

                }
                
            }
            printf("HERE\n"); 
            int pid;

            pid= fork();
            
            if(pid == 0){
                if(out_redirection){
                    dup2(fd, 1);
                    printf("HERE\n");
                    close(fd);
                }
                printf("BEFORE EXECVP\n");   
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
