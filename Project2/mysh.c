#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

char error_message[30] = "An error has occurred\n";
int processes[20]; 
int curr_process_index = 0;

void pipefunc(char** arguments){

    int pipefd[2];
    pipe(pipefd);
    int pid, pid2,i;

    pid = fork();

    if(pid ==0){ 
        //LHS
        close(pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        if(execvp(arguments[0],arguments)){
            write(STDERR_FILENO, error_message, strlen(error_message));
            
            kill(getpid(), SIGINT);
        }
        //free(arguments);
        //free(infile);
        //free(outfile);
        //free(rhs_arguments);
        exit(0);
    }


    pid2 = fork();
    //second child 
    if(pid2 == 0){
        //RHS
        char** rhs_arguments = (char**) malloc(sizeof(char*)* 64);
        for(i=0;i<256;i++){
            char* arg = (char*) malloc(128);
            arg = strtok(NULL, " \n\t");

            if(arg == NULL){
                rhs_arguments[i]= arg;
                break;
            }else{ 
                rhs_arguments[i]= arg;
                //                     free(arg);
            }
        }


        if(rhs_arguments[0] == NULL){
            write(STDERR_FILENO, error_message, strlen(error_message));

          
            kill(getpid(), SIGINT);
        }



        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        if(execvp(rhs_arguments[0], rhs_arguments)){
            write(STDERR_FILENO, error_message, strlen(error_message));
           
            kill(getpid(), SIGINT);
        }
        
        exit(0);
    }
    //parent
    close(pipefd[1]);
    close(pipefd[0]);
    waitpid(-1, NULL, 0);
    // waitpid(pid2, NULL, 0);

    if(waitpid(pid, NULL, WNOHANG) == 0){

        kill(pid, SIGKILL);
    }

    waitpid(-1,NULL, 0);


}

void cdfunc(){
    char* directory = strtok(NULL, " \n\t");
    if(directory == NULL){
        directory = getenv("HOME");
        chdir(directory);

    }else if(directory[0] == '/'){
        chdir(directory);    
        if(chdir(directory) == -1)
            write(STDERR_FILENO, error_message, strlen(error_message));
    }
    else{
        char* path = (char*) malloc(strlen(directory));
        strcat(path, "./");
        strcat(path, directory);
        if(chdir(path) == -1)
            write(STDERR_FILENO, error_message, strlen(error_message));
        free(path);
    }
}

void pwdfunc(){
    char* directory = (char*)malloc(sizeof(char)*124);
    if(strtok(NULL, " \n\t") != NULL){
        write(STDERR_FILENO, error_message, strlen(error_message));
        return;
    }
    getcwd(directory, 124);
    printf("%s\n",directory);
    free(directory);
}

void execfunc(char* command){

    int i;
    int fd;
    int out_redirection = 0;
    int in_redirection = 0;
    int pipeBool = 0;
    int background = 0;
    char* infile = (char*) malloc(sizeof(char)*128);
    char* outfile = (char*) malloc(sizeof(char)*128);
    char** arguments = (char**) malloc(64*sizeof(char*));
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
            if(outfile == NULL){
                write(STDERR_FILENO, error_message, strlen(error_message));
                if(arguments != NULL){
                    // free(arguments);
                }
                if(infile != NULL){
                    // free(infile);
                }
                return;
            }
        }else if (strcmp(arg, "<") == 0){
            in_redirection = 1;
            infile = strtok(NULL, " \n\t");
            if(infile == NULL){
                write(STDERR_FILENO, error_message, strlen(error_message));
                if(arguments != NULL){
                    //  free(arguments);
                }
                if(outfile != NULL){
                    // free(outfile);
                }
                return;
            }
        }else if(strcmp(arg, "|") == 0){
            pipeBool = 1;
            arguments[i] = NULL;
            break;
        }else if(strcmp(arg, "&") == 0){
            background = 1;
        }
        else{
            if(in_redirection == 1 || out_redirection == 1){
                write(STDERR_FILENO, error_message, strlen(error_message));

                if(arguments != NULL){
                    //    free(arguments);
                }
                if(infile != NULL){
                    // free(infile);
                }
                if(outfile != NULL){
                    // free(outfile);
                }
                return;
            }
            arguments[i]= arg;
            //            free(arg);
        }
    }

    if(pipeBool == 1){
        pipefunc(arguments);
    }else{
        int pid;
        pid= fork();
        if(pid == 0){
            //child of fork
            //out redirection
            if(out_redirection == 1){
                fd = open(outfile, O_WRONLY|O_CREAT|O_TRUNC, S_IRUSR|S_IWUSR);
                dup2(fd, 1);
                close(fd);
            }

            //in redirection
            if(in_redirection == 1){
                fd = open(infile, O_RDONLY);
                if(fd == -1){
                    write(STDERR_FILENO, error_message, strlen(error_message));
                    if(arguments != NULL){
                        //    free(arguments);
                    }
                    if(infile != NULL){
                        //  free(infile);
                    }
                    if(outfile != NULL){
                        // free(outfile);
                    }

                    exit(0);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }



            //regular process
            if(execvp(command,arguments)){
                write(STDERR_FILENO, error_message, strlen(error_message));
                if(arguments != NULL){
                    //  free(arguments);
                }
                if(infile != NULL){
                    //    free(infile);
                }
                if(outfile != NULL){
                    //      free(outfile);
                }
                kill(getpid(), SIGINT);
            }
            if(!background){
                if(arguments != NULL){
                    //     free(arguments);
                }
                if(infile != NULL){
                    //   free(infile);
                }
                if(outfile != NULL){
                    //  free(outfile);
                }
                exit(0);
            }


        }else{
            //parent of fork


            if(!background){
                waitpid(pid,NULL,0);
            }else{
                processes[curr_process_index] = pid;
                curr_process_index++; 

            }

        }

    }

    if(arguments != NULL){
        // free(arguments);
    }
    if(infile != NULL){
        // free(infile);
    }
    if(outfile != NULL){
        //  free(outfile);
    }
}



void exitfunc(){

    int j;

    for(j = 0; j<curr_process_index; j++){
        kill(processes[j], SIGKILL);
    }
  
    exit(0);


}

void cleanup(){

    int i;
    for(i =0; i< curr_process_index; i++){
        if( waitpid(processes[i], NULL, WNOHANG)!=0){
            int j;
            
            
            for(j = i+1; j<curr_process_index; j++){
                    processes[j-1] = processes[j];

            }
            curr_process_index--;

        }


    }


}


int main(int argc, char* argv[]){
    if(argc > 1){
        write(STDERR_FILENO, error_message, strlen(error_message));
        exit(1);
    }

   
    int command_num = 1;
    while(1){
        char* input = (char*) malloc(sizeof(char)*1000);
        printf("mysh (%d)> ", command_num);
        fflush(stdout);
        fgets(input, 1000*sizeof(char), stdin);
        if( strlen(input) > 129){
            write(STDERR_FILENO, error_message, strlen(error_message));
            command_num++;
            free(input);
            continue;
        }


        char* command = strtok(input, " \n\t");
        if(command == NULL){
            continue;
        }
        else if(strcmp(command, "exit") == 0)
            exitfunc();
        else if(strcmp(command, "cd") == 0)
            cdfunc();
        else if(strcmp(command, "pwd") == 0)
            pwdfunc();
        else
            execfunc(command);
        

        cleanup();
        free(input);
        command_num++;

    }
}


