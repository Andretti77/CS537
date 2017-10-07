
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>

void cdfunc(){
    char* directory = strtok(NULL, " \n\t");
    if(directory == NULL){
        directory = getenv("HOME");
        chdir(directory);
        free(directory);
        
    }else{
        char* path = (char*) malloc(strlen(directory));
        strcat(path, "./");
        strcat(path, directory);
        chdir(path);
        free(path);
    }
}

void pwdfunc(){
    char* directory = (char*)malloc(sizeof(char)*124);
    getcwd(directory, 124);
    printf("%s\n",directory);
}

void execfunc(char* command){
    char** arguments = (char**) malloc(64*sizeof(char*));
    int i;
    int fd;
    int out_redirection = 0;
    int in_redirection = 0;
    char* infile = (char*) malloc(sizeof(char)*128);
    char* outfile = (char*) malloc(sizeof(char)*128);
    char* infile_arguments = (char*) malloc(sizeof(char)*128);
    arguments[0] = command;
    for(i=1; i<256; i++){
        char* arg = (char*) malloc(128);
        arg = strtok(NULL, " \n");
        if(arg == NULL){
            arguments[i]= arg;
            break;
        }
        if(strcmp(arg, ">") == 0){
            out_redirection = 1;
            outfile = strtok(NULL, " \n\t");
        }else if (strcmp(arg, "<") == 0){
            in_redirection = 1;
            infile = strtok(NULL, " \n\t");
        }
        else{
            arguments[i]= arg;
        }
    }
    int pid;
    pid= fork();
    if(pid == 0){
        if(out_redirection == 1){
            fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC);
            dup2(fd, 1);
            close(fd);
        }
        if(in_redirection == 1){
            fd = open(infile, O_RDONLY);
            dup2(fd, 0);
            close(fd);
            fgets(infile_arguments, 128, stdin);
            i = i-1;
            arguments[i] = strtok(infile_arguments, " \n\t");
            i++;
            printf("%s %d\n", arguments[i], i);
            for(; i<128; i++){
                char* arg = (char*) malloc(128);
                arg = strtok(NULL, " \n\t");
                if(arg == NULL){
                    arguments[i]= arg;
                    break;
                }
                if(strcmp(arg, ">") == 0){
                    out_redirection = 1;
                    outfile = strtok(NULL, " \n\t");
                }
                else{
                    arguments[i]= arg;
                }

            }
            
            
            
        }
        execvp(command,arguments);
    }else{
        waitpid(pid, NULL, 0);
    }
    free(arguments);
}

int main(){
    int command_num = 1;
    while(1){
        char* input = (char*) malloc(sizeof(char)*128);
        printf("mysh (%d)>", command_num);
        fgets(input, 128*sizeof(char), stdin);
        char* command = strtok(input, " \n\t");
        if(strcmp(command, "exit") == 0)
            exit(0);
        else if(strcmp(command, "cd") == 0)
            cdfunc();
        else if(strcmp(command, "pwd") == 0)
            pwdfunc();
        else
            execfunc(command);
        
        fflush(stdout);
        command_num++;
        free(input);

    }
}

