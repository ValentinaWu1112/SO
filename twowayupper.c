#include <sys/wait.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <fcntl.h>
#include <ctype.h>

#define LINESIZE 1024
#define PP_RD 0
#define PP_WR 1

void upper (char *line){
  for(int i=0; i<strlen(line); i++){
    line[i]=toupper(line[i]);
  }
}

int main(int argc, char* argv[]){
  int fi[2], fo[2];
  pid_t pid;
  char line[LINESIZE];

  if(pipe(fi) < 0){
    perror("pipe error");
    exit(EXIT_FAILURE);
  }

  if(pipe(fo)<0){
    perror("pipe error");
    exit(EXIT_FAILURE);
  }

  if((pid=fork())<0){
    perror("fork error");
    exit(EXIT_FAILURE);
  }
  else if(pid>0){ //Parent
    close(fi[PP_RD]);
    close(fo[PP_WR]);
    int nbytes;
    while ((nbytes=read(STDIN_FILENO, line, LINESIZE))>0) {
      if(write(fi[PP_WR], line, nbytes)<0){
        fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
      }
      if((nbytes=read(fo[PP_RD], line, nbytes))>0){
        if(write(STDOUT_FILENO, line, nbytes)<0){
          fprintf(stderr, "Unable to read from pipe: %s", strerror(errno));
        }
      }
    }
    close(fo[PP_RD]);
    close(fi[PP_WR]);

    if(waitpid(pid, NULL, 0)<0){
      fprintf(stderr, "Cannot wait for children: %s\n", strerror(errno));
    }

    exit(EXIT_SUCCESS);
  }
  else{ //Child
    close(fi[PP_WR]);
    close(fo[PP_RD]);
    int nbytes;
    while((nbytes=read(fi[PP_RD], line, LINESIZE))>0){
      upper(line);
      if(write(fo[PP_WR], line, nbytes)<0){
        fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
      }
    }
    close(fo[PP_WR]);
    close(fi[PP_RD]);

    exit(EXIT_SUCCESS);
  }
}
