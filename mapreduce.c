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

#define PP_RD 0
#define PP_WR 1

int main(int argc, char *argv[]) {

  FILE *file = fopen(argv[1],"r");
  if(file==NULL){
    perror("Error in opening file");
    exit(EXIT_FAILURE);
  }

  size_t  str_size = 0;
  ssize_t read_file;
  char* str = NULL;

  read_file = getline(&str, &str_size, file);
  int n = atoi(str);

  int matriz[n][n];

  int l=0, c=0;

  while ((read_file = getline(&str, &str_size, file)) != -1) {
    char* ptr = str;

    char *token;
    token=strtok(ptr," ");

    while( token != NULL ) {
      matriz[l][c]=atoi(token);
      c++;
      token=strtok(NULL," ");
    }
    l++;
    c=0;
  }
  free(str);
  fclose(file);

  int m = atoi(argv[2]);
  int k = atoi(argv[3]);

  int fd[2];
  if (pipe(fd) < 0) {
    perror("pipe error");
    exit(EXIT_FAILURE);
  }

  for(int f=0; f<m; f++){
    pid_t pid;

    if ((pid = fork()) < 0) {
      perror("fork error");
      exit(EXIT_FAILURE);
    }
    if(pid==0){
      close(fd[PP_RD]);
      int parcial=0;
      for(int i=0; i<n; i++){
        if(i%m==f){
          for(int j=0; j<n; j++){
            if(matriz[i][j]>k) parcial++;
          }
        }
      }
      if (write(fd[PP_WR], &parcial, sizeof(int)) < 0) {
        fprintf(stderr, "Unable to write to pipe: %s\n", strerror(errno));
      }
      close(fd[PP_WR]);
      exit(EXIT_SUCCESS);
    }
  }

  close(fd[PP_WR]);
  int total=0;
  int parcial;
  for(int f=0; f<m; f++){
    if (read(fd[PP_RD], &parcial, sizeof(int)) < 0 ) {
      fprintf(stderr, "Unable to read from pipe: %s\n", strerror(errno));
    }
    total+=parcial;
  }
  close(fd[PP_RD]);

  printf("%d\n",total);

  for(int f=0; f<m; f++){
    if(waitpid(-1,NULL,0)<0){
      fprintf(stderr, "Cannot wait for child: %s\n",strerror(errno));
    }
  }

  exit(EXIT_SUCCESS);


  return 0;
}
