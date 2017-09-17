#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
int main(int argc, char *argv[]){


    if(argc >5){
	fprintf(stderr,"Usage: shuffle -i inputfile -o outputfile\n");
	exit(1);
    }	    
    FILE* in_file;
    FILE* out_file;
  
    

    if(strcmp(argv[1],"-i")==0){

       in_file = fopen(argv[2],"r");
       if(in_file == NULL){
	       fprintf(stderr, "Error: Cannot open file %s\n", argv[2]);
       	       exit(1);
       }
       out_file = fopen(argv[4], "w");
       if(out_file == NULL){
	       fprintf(stderr, "Error: Cannot open file  %s\n", argv[4]);
	       exit(1);
       }
    }else if(strcmp(argv[1],"-o")==0 ){

	in_file = fopen(argv[4], "r");
	if(in_file == NULL){
		fprintf(stderr,"Error: Cannot open file %s\n", argv[4]);
		exit(1);
        }
	out_file = fopen(argv[2], "w");
	if(out_file == NULL){
		fprintf(stderr, "Error: Cannot open file %s\n", argv[2]);
		exit(1);
	}
    }else{
	fprintf(stderr,"Usage: shuffle -i inputfile -o outputfile\n");
	exit(1);

    }

    char*  word;
    char*  words;
    char** ptrs_to_words;
    int* char_per_line;
    int num_lines;
    int char_num;
    int j;
    int file_size;
    struct stat in_file_info;
    int curr_line;
    num_lines =0;
    char_num = 0;
    	    

    fstat(fileno(in_file), &in_file_info);

    file_size = in_file_info.st_size;
    
    words = (char*)malloc(file_size);	   
    int temp = fread(words, sizeof(char), file_size, in_file);	    

    if(temp != file_size){
    //just used to get rid of a warning message for not using the return 
    //value of fread()
    }  
    
    int k=0;
    while(words[k]!='\0'){

	if(words[k]=='\n'){
	num_lines++;

	}


	k++;

    }

    ptrs_to_words = (char**)malloc(sizeof(char*)*num_lines);
    char_per_line = (int*)malloc(sizeof(int)*num_lines);
    word = (char*)malloc(sizeof(char)*512);
    j=0;
    curr_line=0;
    while(words[j] != '\0'){

	if(words[j] !='\n'){
	 
	 	
	 word[char_num] = words[j];
	 char_num++;



	}else{
	 	
	 
	 word[char_num] = words[j];
	 char_num++;
	 	 
    	 ptrs_to_words[curr_line] = word;
	 char_per_line[curr_line] = char_num;
	
	 char_num = 0;
	 curr_line++;
	 word = (char*)malloc(sizeof(char)*512);

	} 
       j++;	

    }
   
   for(j=0; j<num_lines; j++){
	if(j >= num_lines-j-1){
		if(j == num_lines-j-1){
		   fwrite(ptrs_to_words[j], sizeof(char), char_per_line[j], out_file);
		  
		}

		j = num_lines;
	
	}else{	
	   
	 fwrite(ptrs_to_words[j],sizeof(char),char_per_line[j], out_file);
	 
	 fwrite(ptrs_to_words[num_lines-j-1],sizeof(char), char_per_line[num_lines-j-1] , out_file);
 	 	 
	}

   }	

    
   for(j=0; j<num_lines; j++){
      free(ptrs_to_words[j]);
   }
    free(ptrs_to_words);
    free(word);
    free(char_per_line);
    free(words);
    fclose(in_file);
    fclose(out_file);
    return 0;

}
