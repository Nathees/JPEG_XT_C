#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "io_file.h"


FILE *input_file;
FILE *output_ppm;
FILE *output_pfm;

long filelen;

void read_jpeg(char *input_img){

	input_file = fopen(input_img, "rb"); 			 // Open the  jpeg file in binary mode

	if (!input_file){
		printf("Unable to the jpeg file\n");
		exit(0);
	}

	// Copy all the jpeg data to buffer
	fseek(input_file, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(input_file);             // Get the current byte offset in the file
	rewind(input_file);

	buffer = (unsigned char *)malloc((filelen + 1)*sizeof(char)); 		// Enough memory for file 
	buffer_resi = (unsigned char *)malloc((filelen + 1)*sizeof(char)); 	// Enough memory for file 

	fread(buffer, filelen, 1, input_file); 	// Read in the entire file
	fclose(input_file);
}
