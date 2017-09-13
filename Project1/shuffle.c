#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char *argv[]){

    FILE* in_file;
    FILE* out_file;

    in_file = fopen(argv[2],"r");
    out_file = fopen(argv[4], "w");

    char  word[512];
    char  words[300];
    char* ptrs_to_words[300];
    //char* last_word;
    //int line_length;
   // unsigned long int len;
    int num_lines;
    int char_num;
    int j;
    //int k;
    num_lines =0;
    char_num = 0;
    	    

    
	   
    fread(words, sizeof(char), 512, in_file);	    
	
    for(j=0; j<sizeof(words); j++){

	if(words[j] !='\n'){
	 
	 	
	 word[char_num] = words[j];
	 char_num++;



	}else{
	 	
	 word[char_num] = words[j];

	 //printf("%s", word);	 
    	 ptrs_to_words[num_lines] = strdup(word);
	 //printf("%s", ptrs_to_words[num_lines]);
	 char_num = 0;
	 num_lines++;

	}  

    }

   
    

   

  


 	 
  
   //fwrite(ptrs_to_words[0], sizeof(char)*512, 1, out_file);
   for(j=0; j<num_lines/2; j++){
	printf("%s", ptrs_to_words[0]); 

	fwrite(ptrs_to_words[j],sizeof(char),512, out_file);
	fwrite(ptrs_to_words[num_lines-j-1],sizeof(char), 512 , out_file);	


   }	

 //  }

    
    fclose(in_file);
    fclose(out_file);
    return 0;

}
