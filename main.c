#include<stdlib.h>
#include<unistd.h>
#include<sys/wait.h>
#include<string.h>
#include <stdio.h>
#include<fcntl.h>

#define MAX_CHAR 4096
#define MAX_ARGS 128

void run_pipe(char *aRGS[],char *aRGS1[]){
    int pipefd[2];
    if(pipe(pipefd)==-1){
        printf("pipe failed\n");
        exit(1);
    }
    pid_t pid1 = fork();
    if(pid1==-1){
        perror("fork failed\n");
        close(pipefd[0]);
        close(pipefd[1]);
        return;
    }
    if(pid1==0){
        dup2(pipefd[1],STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        if(execvp(aRGS[0],aRGS)==-1){
            perror("Execvp failed");
            exit(1);
        }
    }
    pid_t pid2 = fork();
    if(pid2==-1){
        perror("fork failed\n");
        exit(1);
    }
    if(pid2==0){
        dup2(pipefd[0],STDIN_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);
        if(execvp(aRGS1[0],aRGS1)==-1){
            perror("Execvp failed\n");
            exit(1);
        }
    }
    close(pipefd[0]);
    close(pipefd[1]);
    wait(NULL);
    wait(NULL);
}

int main()
{
    while(1){
        printf(">>>");
        fflush(stdout);
        char input[MAX_CHAR];
        if(fgets(input,MAX_CHAR,stdin)==NULL){
            printf("Input failed\n");
            break;
        }
        input[strcspn(input,"\n")]='\0';
        
        char* aRGS[MAX_ARGS];
        int trav_ptr = 0;
        char* tok_ptr = strtok(input, " ");
        while(tok_ptr!=NULL){
            aRGS[trav_ptr++] = tok_ptr;
            tok_ptr= strtok(NULL, " ");
        }
        aRGS[trav_ptr]=NULL;
        if(aRGS[0]==NULL){
            continue;
        }
        if(strcmp(aRGS[0],"cd")==0){
            aRGS[1]= aRGS[1]?aRGS[1]:getenv("HOME");
            if(chdir(aRGS[1])==-1){
                printf("cd failed\n");
            }
            continue;
        }
        int pipe_flag = 0;
        //check for pipe (only one pipe implementation)
        int pipe_arg_ptr = 0;
        for(pipe_arg_ptr=0;pipe_arg_ptr<trav_ptr;pipe_arg_ptr++){
            if(strcmp(aRGS[pipe_arg_ptr],"|")==0){
                aRGS[pipe_arg_ptr++]=NULL;
                pipe_flag=1;
                break;
            }
        }
        if(pipe_flag==1){
            if (pipe_arg_ptr >= trav_ptr || aRGS[pipe_arg_ptr] == NULL) {
                fprintf(stderr, "syntax error\n");
            } else {
                run_pipe(&aRGS[0], &aRGS[pipe_arg_ptr]);
            }
            continue;
    
        }
        
        char* inputFile;
        char* outputFile;
        int out_redirect_flag = 0;
        int in_redirect_flag = 0;
        for(int i = 0;i<trav_ptr;i++){
            if(aRGS[i]==NULL){continue;}
            if(strcmp(aRGS[i],">")==0){
                aRGS[i]=NULL;
                outputFile = aRGS[++i];
                out_redirect_flag = 1;
            }
            else if(strcmp(aRGS[i],"<")==0){
                aRGS[i]=NULL;
                inputFile = aRGS[++i];
                in_redirect_flag= 1;
            }
        }
        pid_t pid = fork();
        if(pid==-1){
            perror("Fork failed\n");
            continue;
        }
        if(pid==0){
            
        if(in_redirect_flag==1){
            int fd1 = open(inputFile,O_RDONLY);
            if (fd1 < 0) {
                perror("opening input file failed\n");
                exit(1);
            }
            dup2(fd1, STDIN_FILENO);
            close(fd1);
        }
        if(out_redirect_flag==1){
           int fd2 = open(outputFile, O_WRONLY|O_CREAT|O_TRUNC, 0644);
           if (fd2 < 0) {
                perror("opening output file failed\n");
                exit(1);
            }
           dup2(fd2, STDOUT_FILENO);
           close(fd2);
        }
            if(execvp(aRGS[0],aRGS)==-1){
                printf("execvp failed\n");
                exit(1);
            }
        }
        wait(NULL);
    }
    return 0;
}
