
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>



char* cdfunc(){
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
    char* output = "";
    return output;
}

char* pwdfunc(){
    char* directory = (char*)malloc(sizeof(char)*124);
    getcwd(directory, 124);
    return directory;
}

void execfunc(char* command){
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

int main(){
    int command_num = 1;
    while(1){
        char* input = (char*) malloc(sizeof(char)*128);
        printf("mysh (%d)>", command_num);
        fgets(input, 128*sizeof(char), stdin);
        char* command = strtok(input, " \n\t");
        char* output = (char*)malloc(sizeof(char)*124);
        if(strcmp(command, "exit") == 0){
            exit(0);
        }else if(strcmp(command, "cd") == 0){
            output = cdfunc();

        }else if(strcmp(command, "pwd") == 0){
            output = pwdfunc();
        }else{
            execfunc(command);
        }
        if(strcmp(output, "") != 0){
            printf("%s\n", output);
        }
        fflush(stdout);
        command_num++;
        free(input);

    }
}

