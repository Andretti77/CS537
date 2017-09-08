#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]){

    FILE *in_file;

    in_file = fopen(argv[1],"r");

    char* word = NULL;

    char* words[300];

    int line_length;
    unsigned long int len;
    int num_lines;
    int i;

    num_lines = 0;


	

    while((line_length = getline(&word, &len, in_file)) != -1){
	
	    words[num_lines] = strdup(word);
	    num_lines++;

     

    }

    
   for(i=0; i<num_lines; i++){
	free(words[i]);
	

   }

    
    free(word);
    fclose(in_file);
    return 0;

}
