#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

void clean (char *str){
  for(int i=0; i<strlen(str); i++){
    if(ispunct(str[i])) str[i]=' ';
  }
}

void grep(char* needle, char* filename){
  size_t  haystack_size = 0;
  ssize_t read;
  char*   haystack = NULL;
  FILE*   file = fopen(filename, "r");

  if ( file == NULL ) {
    printf("error: could not open %s\n", filename);
    exit(1); // ends the program
  }

  int line = 1;
  int flag_inic=0;
  while ((read = getline(&haystack, &haystack_size, file)) != -1) {
    char* ptr = haystack;
    ptr[strlen(ptr)-1]='\0';
    clean(ptr);
    char *token;
    
    token=strtok(ptr," ");
    while( token != NULL ) {
      //printf("%s",token);
      if(strcmp(token,needle)==0){
        if(!flag_inic){
          printf("[%d", line);
          flag_inic=1;
        }
        printf(",%d",(int)(token-ptr+1));
      }
      //column+=strlen(token)+1; //por causa do espaço
      token=strtok(NULL," ");
    }
    if(flag_inic) {
      printf("]\n");
      flag_inic=0;
    }
    line++;
  }
  free(haystack);
  fclose(file);
}

int main(int argc, char *argv[]) {
  grep(argv[1], argv[2]);
  return 0;
}
