#include <stdio.h>
#include "math.h"
#include <time.h>

#include "Bitstream.h"
#include "Main.h"
#include "Huffman.h"
#include "IDCT.h"
#include "Upsampling.h"

FILE *file_jpeg;
//FILE *fp1;		// For Feig_IDCT
//FILE *fp2;		// For Norm_IDCT
//FILE *fp3;		// For Dequantized_block
//FILE *fp4;		// For Dequantized_Feig_block
//FILE *fp5;		// For Y, Cb, Cr components
FILE *ppm;		// For make the RGB file in PPM mode 

// Global variable decleration
unsigned char *buffer;		// Contains the all image
unsigned char *buffer_resi; // Contain the Residual Layer Information

long count;			// control the buffer
long count_resi;	// control the residual buffer

int marker_len;

unsigned char byte_file;									// value of a byte from the file
unsigned char marker_id;									// value of marker identifier

int MCU_X, MCU_Y; //MCUs for X and Y

float block[8][8];
int integer_block[8][8];
float block_temp[8][8];

FILE *tone_map;

//int **RGB_R;
//int **RGB_G;
//int **RGB_B;
/*int RGB_R[8000][8000];
int RGB_G[8000][8000];
int RGB_B[8000][8000];*/

int resi_y[8000][8000];
int resi_cb[8000][8000];
int resi_cr[8000][8000];

int integer_R[8000][8000];
int integer_G[8000][8000];
int integer_B[8000][8000];

int integer_block_upsampling_cb_1[16][8];
int integer_block_upsampling_cb_2[16][8];
int integer_block_upsampling_cr_1[16][8];
int integer_block_upsampling_cr_2[16][8];

int integer_block_y[8][8];
int integer_block_cb[8][8];
int integer_block_cr[8][8];

int integer_block_r[8][8];
int integer_block_g[8][8];
int integer_block_b[8][8];

const unsigned char Zig_Zag[64] = { 0x00, 0x10, 0x01, 0x02, 0x11, 0x20, 0x30, 0x21,   //0
									0x12, 0x03, 0x04, 0x13, 0x22, 0x31, 0x40, 0x50,   //1
									0x41, 0x32, 0x23, 0x14, 0x05, 0x06, 0x15, 0x24,	  //2
									0x33, 0x42, 0x51, 0x60, 0x70, 0x61, 0x52, 0x43,	  //3
									0x34, 0x25, 0x16, 0x07, 0x17, 0x26, 0x35, 0x44,	  //4
									0x53, 0x62, 0x71, 0x72, 0x63, 0x54, 0x45, 0x36,	  //5
									0x27, 0x37, 0x46, 0x55, 0x64, 0x73, 0x74, 0x65,	  //6
									0x56, 0x47, 0x57, 0x66, 0x75, 0x76, 0x67, 0x77 }; //7

// Function Decleration 

void residual_decode_process();
float sixteento32float(int x);

void open_file(char *directory);
void copy_file_to_memory(void);
//unsigned char read_nxt_byte();
void skip_byte(void);

void check_SOI_marker();
void create_dynamic_array_RGB();
void YCbCr_to_RGB();

void generate_PPM_File(char *name, unsigned char type);
void generate_PFM_File(char *name);

void calculate_psnr();


int main(){
	count = 0;
	open_file("Cafe.jpg"); // LG0056_c
	copy_file_to_memory(); 
	check_SOI_marker();
	
	// *********************************  Reading Marker Syntax Details    ************************************************

	clock_t t;
	t = clock();

	count++;		// Skipping FF Byte
	while (buffer[count] != 0xDA){
		
		if (resi_tbox_flag && buffer[count] != 0xEB){
			//residual_decode_process();
			resi_tbox_flag = 0;
			printf("residual layer decoding Finished\n");
		}

		switch (buffer[count++]){
		case 0xC0	:	Baseline_Frame(); printf("Basline frame procees\n"); break;
		case 0xC1	:	Baseline_Frame();  /*Extended_Frame();*/ printf("Extended frame procees\n"); break;
		case 0xC2	:	Progressive_Frame(); printf("Progressive frame procees\n");  break;

		case 0xC4	:	Huffman(); printf("Huffman syntax procees\n");  break;
		case 0xDB  :	Quantization(); printf("Quantization Syntax procees\n");  break;

		case 0xEB:	APP11_process();  printf("APP11 Handling\n"); break;
		default:	/*printf("Skip Process\n");*/ skip_byte(); break;
		}
		count++;
	}
	count++;
	Scan();
	initial_load_bitstream();		// Load the first 4 bytes of encoded bitstream to the bitstream variable
	
	
	//************************************************Operations related to the decoding process**********************************************
	//Component specification parameter - unsigned char comp_specific_param[3][6];	
	//[0]-component Identifier, [1]-H, [2]-V,[3]-Quantization tble identifier, [4]-DC identifier, [5]-AC identifier
	// Calculatiing horizontal (MCU_X) and vertical (MCU_Y) blocks for the image
	printf("X = %d,  Y = %d\n", X, Y);
	if (X % 8)
		MCU_X = X / 8 + 1;
	else
		MCU_X = X / 8;
	if (Y % 8)
		MCU_Y = Y / 8 + 1;
	else
		MCU_Y = Y / 8;
	// Calculatiing horizontal (MCU_X) and vertical (MCU_Y) MCUs for the image
	if (MCU_X % comp_specific_param[0][1])
		MCU_X = MCU_X / comp_specific_param[0][1] + 1;
	else
		MCU_X = MCU_X / comp_specific_param[0][1];
	if (MCU_Y % comp_specific_param[0][2])
		MCU_Y = MCU_Y / comp_specific_param[0][2] + 1;
	else
		MCU_Y = MCU_Y / comp_specific_param[0][2];

	DIFF_Y = 0;
	DIFF_Cb = 0;
	DIFF_Cr = 0;

	unsigned char upsampling_type = identify_upsampling_type();	// Identify the upsampling type
	printf("upsampling type %d\n", upsampling_type);
	
	//create_dynamic_array_RGB();				// Create dynamic array to store R, G and B components
	

	printf("MCU_X = %d,  MCU_Y = %d\n", MCU_X, MCU_Y);
	//**************************************************Decoding Starts from here**********************************************
	int x, y;
	unsigned char comp, H, V;

	//FILE *huffman_blks;
	//huffman_blks = fopen("Base_rgb_decoded_blks.txt", "wb");
	/*for (row = 0; row < 64; row++){
		printf("%x\n", integer_modified_quantization_table[row][0]);
	}*/
	
	for (x = 0; x < MCU_Y; x++){//MCU_Y
		for (y = 0; y < MCU_X; y++){//MCU_X
			for (comp = 0; comp < Nf; comp++){//Nf
				for (V = 0; V < comp_specific_param[comp][2]; V++){ //run upto ith component vertical sampling factor  comp_specific_param[comp][2]
					for (H = 0; H < comp_specific_param[comp][1]; H++){ //run upto ith component horizontal sampling factor comp_specific_param[comp][1]
						
						Huffman_decode(comp, comp_specific_param[comp][4], comp_specific_param[comp][5], comp_specific_param[comp][3]);
						
					
						Integer_IDCT(comp);

						if (upsampling_type != 1 && comp != 0){
							upsampling_cbcr_integer(comp);
						}

						if (comp == 2){
							color_transform_integer(x, y);
							tone_mapping(x, y);							
						}
					}
				}
			}
		}
	}
	
	//fclose(huffman_blks);
	

	//t = clock() - t;
	//double time_taken = ((double)t) / CLOCKS_PER_SEC;

	//printf("\nTime taken = %f\n", time_taken);

	/*if (upsampling_type != 1){
		upsampling_CbCr(upsampling_type);
	}*/

	//YCbCr_to_RGB();
	//generate_PPM_File("output_float.ppm",1); // Float operation output Image
	//generate_PPM_File("hdr.ppm",2); // Integer operation output Image

	generate_PFM_File("hdr_output.pfm");

	//calculate_psnr();
	return 0;
}

void residual_decode_process(){
	count_resi = 0;
	initial_load_bitstream();		// Load the first 4 bytes of encoded bitstream to the bitstream variable

	int x, y;
	unsigned char comp, H, V;


	//tone_map = fopen("Tone_Mapped", "wb");

	for (x = 0; x < MCU_Y - 1; x++){//MCU_Y
		for (y = 0; y < MCU_X; y++){//MCU_X
			for (comp = 0; comp < Nf; comp++){//Nf
				for (V = 0; V < comp_specific_param[comp][2]; V++){ //run upto ith component vertical sampling factor  comp_specific_param[comp][2]
					for (H = 0; H < comp_specific_param[comp][1]; H++){ //run upto ith component horizontal sampling factor comp_specific_param[comp][1]

						Huffman_decode(comp, comp_specific_param[comp][4], comp_specific_param[comp][5], comp_specific_param[comp][3]);
						Integer_IDCT(comp);
						if (comp == 2){
							resi_layer_transfer_element(x, y);
						}
					}
				}
			}
		}
	}
}

void open_file(char *directory){							// open file

	file_jpeg = fopen(directory, "rb"); 			 		// Open the  jpeg file in binary mode

	if (!file_jpeg){
		printf("Unable to open file\n");
	}
	else{
		printf("Success\n");
	}
}

void copy_file_to_memory(void){
	long filelen;
	fseek(file_jpeg, 0, SEEK_END);          // Jump to the end of the file
	filelen = ftell(file_jpeg);             // Get the current byte offset in the file
	rewind(file_jpeg);

	buffer = (char *)malloc((filelen + 1)*sizeof(char)); 	// Enough memory for file 
	buffer_resi = (char *)malloc((filelen + 1)*sizeof(char)); 	// Enough memory for file 

	fread(buffer, filelen, 1, file_jpeg); 				// Read in the entire file
	fclose(file_jpeg);
}

void skip_byte(void){
	marker_len = 0;
	marker_len = buffer[count++];
	marker_len = (marker_len << 8) + buffer[count++];
	count = count + marker_len - 2;
}

void check_SOI_marker(){
	if (buffer[count++] == 0xFF){					// check the first byte is 0xFF
		if (buffer[count++] != 0xD8){				// check the second byte is 0xD8
			printf("Error in file\n");
		}
	}
	else	printf("Error in file\n");
}

void create_dynamic_array_RGB(){
	/*RGB_R = (int **)malloc(sizeof(int *) * Y);
	RGB_G = (int **)malloc(sizeof(int *) * Y);
	RGB_B = (int **)malloc(sizeof(int *) * Y);
	for (int i = 0; i < Y; i++){
		RGB_R[i] = (int *)malloc(X * sizeof(int));
		RGB_G[i] = (int *)malloc(X * sizeof(int));
		RGB_B[i] = (int *)malloc(X * sizeof(int));
	}*/
	
	/*integer_R = (int **)malloc(sizeof(int *) * Y); 
	integer_G = (int **)malloc(sizeof(int *) * Y); 
	integer_B = (int **)malloc(sizeof(int *) * Y); 
	for (int i = 0; i < Y; i++){
		integer_R[i] = (int *)malloc(X * sizeof(int));
		integer_G[i] = (int *)malloc(X * sizeof(int));
		integer_B[i] = (int *)malloc(X * sizeof(int));
	}*/
}

void YCbCr_to_RGB(){
	/*for (int i = 0; i < Y; i++){
		for (int j = 0; j < X; j++){
			RGB_R[i][j] = (int)round((YCbCr_Y[i][j] + 1.402*(YCbCr_Cr[i][j] - 128)));
			if (RGB_R[i][j] >255)
				RGB_R[i][j] = 255;
			else if (RGB_R[i][j] < 0)
				RGB_R[i][j] = 0;
			RGB_G[i][j] = (int)round((YCbCr_Y[i][j] - 0.34414*(YCbCr_Cb[i][j] - 128) - 0.71414*(YCbCr_Cr[i][j] - 128)));
			if (RGB_G[i][j] >255)
				RGB_G[i][j] = 255;
			else if (RGB_G[i][j] < 0)
				RGB_G[i][j] = 0;
			RGB_B[i][j] = (int)round((YCbCr_Y[i][j] + 1.772*(YCbCr_Cb[i][j] - 128)));
			if (RGB_B[i][j] >255)
				RGB_B[i][j] = 255;
			else if (RGB_B[i][j] < 0)
				RGB_B[i][j] = 0;
		}
	}*/
}

float sixteento32float(int x){
	//float y = 3.0;
	unsigned char byte4[4], bytef[4] = { 0 }, byte1;
	unsigned int ex16, ex32;
	float val;
	if (x < 0)
		x = -(x + 1);
	memcpy(byte4, &x, 4);
	bytef[3] = byte4[1] & 0x80;
	ex16 = (byte4[1] & 0x7c) >> 2;
	ex32 = ex16 - 15 + 127;
	bytef[3] = bytef[3] | ex32 >> 1;
	bytef[2] = (ex32 & 0x01) << 7;
	bytef[2] = bytef[2] | ((byte4[1] & 0x03) << 5);
	bytef[2] = bytef[2] | ((byte4[0] & 0xf8) >> 3);
	bytef[1] = (byte4[0] & 0x07) << 5;
	bytef[0] = 0;


	//printf("ex16 %d\n",ex16);
	//printf("%hx %hx %hx %hx\n",byte4[0],byte4[1],byte4[2],byte4[3]);
	//printf("%hx %hx %hx %hx\n",bytef[0],bytef[1],bytef[2],bytef[3]);
	memcpy(&val, bytef, 4);
	//printf("%f\n",bytef);
	//printf("%f\n",val);
	//printf("%d %f\n",x,val);
	return val;
}


void generate_PPM_File(char *name, unsigned char type){
	ppm = fopen(name, "wb");
	//ppm = fopen("Decoded_Image.ppm", "wb");
	unsigned char ppm_syntax;
	unsigned char no_of_digits;
	int divider=0;
	int length;
	//************************************************************************
	ppm_syntax = 80; //P in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 54; //6 in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 10; //Line feed character in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	//************************************************************************
	// Image dimension  WIDTH
	divider = 10; no_of_digits = 1;
	while (X / divider != 0){
		divider = divider * 10;
		no_of_digits++;
	}
	length = X;
	while (no_of_digits > 1){
		divider = divider / 10;
		ppm_syntax = length / divider + 48; // Adding 48 for convertion of character to ASCII
		length = length % divider;
		fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
		no_of_digits--;
	}
	ppm_syntax = length + 48; // Adding 48 for convertion of character to ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	//************************************************************************
	ppm_syntax = 32; //space character in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	//************************************************************************
	// Image dimension  Height
	divider = 10; no_of_digits = 1;
	while (Y / divider != 0){
		divider = divider * 10;
		no_of_digits++;
	}
	length = Y;
	while (no_of_digits > 1){
		divider = divider / 10;
		ppm_syntax = length / divider + 48; // Adding 48 for convertion of character to ASCII
		length = length % divider;
		fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
		no_of_digits--;
	}
	ppm_syntax = length + 48; // Adding 48 for convertion of character to ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	//************************************************************************
	ppm_syntax = 10; //Line feed character in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 50; //2 in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 53; //5 in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 53; //5 in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	ppm_syntax = 10; //Line feed character in ASCII
	fwrite(&ppm_syntax, sizeof(unsigned char), 1, ppm);
	//************************************************************************
	// UPLOADING RGB PIXELS to PPM
	for (int i = 0; i < Y; i++){
		for (int j = 0; j < X; j++){
			if (type == 1){
				/*fwrite(&RGB_R[i][j], 1, 1, ppm);
				fwrite(&RGB_G[i][j], 1, 1, ppm);
				fwrite(&RGB_B[i][j], 1, 1, ppm);*/
			}
			if (type == 2){
				fwrite(&integer_R[i][j], 1, 1, ppm);
				fwrite(&integer_G[i][j], 1, 1, ppm);
				fwrite(&integer_B[i][j], 1, 1, ppm);
			}
		}
	}
	fclose(ppm);
}

void generate_PFM_File(char *name){
	FILE * pfm;
	char byte20[20];
	unsigned int i, j, k, l, count;

	float hdr_r, hdr_g, hdr_b;
	//unsigned char R, G, B;
	//int intR,intG, intB;

	pfm = fopen(name, "wb");
	k = 6;
	l = 2;

	fwrite("PF\n", 3, 1, pfm);
	count = sprintf(byte20, "%d", X);
	fwrite(byte20, 1, count, pfm);
	fwrite(" ", 1, 1, pfm);
	count = sprintf(byte20, "%d", Y);
	fwrite(byte20, 1, count, pfm);

	fwrite("\n-1.000000\n", 11, 1, pfm);

	for (i = 0; i < Y; i++)
		for (j = 0; j < X; j++)
		{
			{
				hdr_r = sixteento32float(integer_R[i][j]);
				hdr_g = sixteento32float(integer_G[i][j]);
				hdr_b = sixteento32float(integer_B[i][j]);

				fwrite(&hdr_r, 4, 1, pfm);
				fwrite(&hdr_g, 4, 1, pfm);
				fwrite(&hdr_b, 4, 1, pfm);
			}

		}
	fclose(pfm);
}

void calculate_psnr(){
	float MSE_R = 0, MSE_G = 0, MSE_B = 0;
	int diff_r = 0, diff_g = 0, diff_b = 0;

	float PSNR_R, PSNR_G, PSNR_B;

	for (int i = 0; i < Y; i++){
		for (int j = 0; j < X; j++){
			//diff_r = RGB_R[i][j] - integer_R[i][j];
			//diff_g = RGB_G[i][j] - integer_G[i][j];
			//diff_b = RGB_B[i][j] - integer_B[i][j];

			MSE_R = MSE_R + pow(diff_r, 2);
			MSE_G = MSE_G + pow(diff_g, 2);
			MSE_B = MSE_B + pow(diff_b, 2);
		}
	}
	MSE_R = MSE_R / (X * Y);
	MSE_G = MSE_G / (X * Y);
	MSE_B = MSE_B / (X * Y);

	PSNR_R = 20 * log10(255) - 10 * log10(MSE_R);
	PSNR_G = 20 * log10(255) - 10 * log10(MSE_G);
	PSNR_B = 20 * log10(255) - 10 * log10(MSE_B);

	printf("PSNR_R = %.2f\n", PSNR_R);
	printf("PSNR_G = %.2f\n", PSNR_G);
	printf("PSNR_B = %.2f\n", PSNR_B);
	float psnr;
	psnr = (PSNR_R + PSNR_G + PSNR_B) / 3;
	printf("\n\nAverage PSNR = %.2f\n\n", psnr);
}