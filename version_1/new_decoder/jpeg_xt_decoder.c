#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "jpeg_xt_decoder.h"

// Sub Header Files
#include "marker_parser/marker_parser.h"
#include "io_file/io_file.h"
#include "huffman_decoder/huffman_decoder.h"
#include "idct/idct.h"
#include "upsample/upsample.h"
#include "color_transform/color_transform.h"


//  ******************************** Extern Variables Decleration ********************************

// **************** Table Memory Decleration ****************
#if INTEGER_OPERATION
	int base_int_quantization_table[64][2];
	int resi_int_quantization_table[64][2];
#else
	float base_float_quantization_table[64][2];
	float resi_float_quantization_table[64][2];
#endif

// Base Huffman Table
unsigned char base_huffman_table_DC_1[65536][2] = { { 0 } }; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
unsigned char base_huffman_table_AC_1[65536][2] = { { 0 } };
unsigned char base_huffman_table_DC_2[65536][2] = { { 0 } }; 
unsigned char base_huffman_table_AC_2[65536][2] = { { 0 } };
// Residual Huffman Table
unsigned char resi_huffman_table_DC_1[65536][2] = { { 0 } }; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
unsigned char resi_huffman_table_AC_1[65536][2] = { { 0 } };
unsigned char resi_huffman_table_DC_2[65536][2] = { { 0 } }; 
unsigned char resi_huffman_table_AC_2[65536][2] = { { 0 } };

// TONE table
int tone_table[256];


// ************** Controlling Image Array Control Variables **************
unsigned char *buffer;		// Contains the all image
unsigned char *buffer_resi;  // Contains the Residual layer image
int buff_index;
int index_resi;
unsigned char byte_file;

// ************** Image Component Specific Parameters  **************
// Common Component Specific Parameters
unsigned char Nf_Ns;
// Base Layer Component Specific Parameters
unsigned char base_hori_samp_factor[3]; 	
unsigned char base_vert_samp_factor[3];
unsigned char base_dqt_id[3];
unsigned char base_huff_dc_id[3];
unsigned char base_huff_ac_id[3];
// Residual Layer Component Specific Parameters
unsigned char resi_hori_samp_factor[3];
unsigned char resi_vert_samp_factor[3];
unsigned char resi_dqt_id[3];
unsigned char resi_huff_dc_id[3];
unsigned char resi_huff_ac_id[3];

// ************** Image Size Control Variables  **************
int img_height;
int img_width;
int img_mcu_height;
int img_mcu_width;

// ************** Common Control Variables ********************
unsigned char row, col;

// ************** Zig-Zag Control Variable ********************
const unsigned char Zig_Zag[64] = { 0x00, 0x10, 0x01, 0x02, 0x11, 0x20, 0x30, 0x21,   //0
									0x12, 0x03, 0x04, 0x13, 0x22, 0x31, 0x40, 0x50,   //1
									0x41, 0x32, 0x23, 0x14, 0x05, 0x06, 0x15, 0x24,	  //2
									0x33, 0x42, 0x51, 0x60, 0x70, 0x61, 0x52, 0x43,	  //3
									0x34, 0x25, 0x16, 0x07, 0x17, 0x26, 0x35, 0x44,	  //4
									0x53, 0x62, 0x71, 0x72, 0x63, 0x54, 0x45, 0x36,	  //5
									0x27, 0x37, 0x46, 0x55, 0x64, 0x73, 0x74, 0x65,	  //6
									0x56, 0x47, 0x57, 0x66, 0x75, 0x76, 0x67, 0x77 }; //7

// ********************************  8 x 8 Block Decleraion    ********************************
#if INTEGER_OPERATION
	int base_int_block[8][8];
	int resi_int_block[8][8];
#else
	float base_float_block[8][8];
	float resi_float_block[8][8];
#endif

// **************  Identifying the current decoding layer  ******************
unsigned char base_resi_layer; //( 0 - Base layer & 1 - Residual Layer)

// **************  Identifying ldr or hdr image  ******************
unsigned char ldr_hdr_img = 0; //( 0 - LDR Image & 1 - HDR Image)

// ************** Upsampled Block Array decleration **************
unsigned char base_upsample_y_block[32][8];
unsigned char base_upsample_cb_block[32][8];
unsigned char base_upsample_cr_block[32][8];
unsigned char resi_upsample_y_block[32][8];
unsigned char resi_upsample_cb_block[32][8];
unsigned char resi_upsample_cr_block[32][8];

// ************** Decoded RGB Channel **************
int channel_r[8000][6000];
int channel_g[8000][6000];
int channel_b[8000][6000];

// ******************************** Local Variables Decleration ********************************
char argument[20]; // Identifying Input Argument

int img_mcu_x, img_mcu_y, img_comp, img_hori_sam_fact, img_vert_sam_fact;

// Local Functions Declerations
void initial_setup(void);
void base_decoder(void);
void resi_decoder(void);


int main(int argc, char* argv[]){



	/*if(argc <= 2){ 				// Checking the sufficient Arguments
		printf("Not Sufficient Arguments Entered\n");
		exit(0);
	}
	else if(argc == 3){ 		// Checking the correctness of passed arguments (input & output files)
		strcpy(argument, argv[1]);
		if(argument[0] == '-'){
			printf("Entered Argument is wrong\n");
			exit(0);
		}
		strcpy(argument, argv[2]);
		if(argument[0] == '-'){
			printf("Entered Argument is wrong\n");
			exit(0);
		}
		else{
		}
	}*/

	read_jpeg(argv[1]);
	marker_parser();
	initial_setup();

	for (img_mcu_y = 0; img_mcu_y < img_mcu_height; img_mcu_y++){		//img_mcu_height
		for (img_mcu_x = 0; img_mcu_x < img_mcu_width; img_mcu_x++){	//img_mcu_width

			for (img_comp = 0; img_comp < 3; img_comp++){				// No of Components
				for (img_vert_sam_fact = 0; img_vert_sam_fact < base_vert_samp_factor[img_comp]; img_vert_sam_fact++){ 
					for (img_hori_sam_fact = 0; img_hori_sam_fact < base_hori_samp_factor[img_comp]; img_hori_sam_fact++){
						
						for(base_resi_layer = 0; base_resi_layer <= ldr_hdr_img; base_resi_layer++){
							if(base_resi_layer == 0)
								base_decoder();
							else{
								#if RESIDUAL_DECODE_ENABLE
									resi_decoder();
								#endif
							}
						}

					}
				}
			}
		}
	}
	printf("%.2X\n", buffer[buff_index - 4]);
	printf("%.2X\n", buffer[buff_index - 3]);

	printf("%.2X\n", buffer_resi[index_resi - 4]);
	printf("%.2X\n", buffer_resi[index_resi - 3]);
	return 0;
}


void initial_setup(void){
	initial_load_bitstream();
	identify_upsample_type();

	// Tracking Operation
	#if TRACKING_ENABLE
		printf("Start Decoding.....\n");
	#endif
}

void base_decoder(void){
	huffman_decoder(img_comp);
	idct();
	upsample(img_comp);
	color_transform(img_mcu_y, img_mcu_x);
}

void resi_decoder(void){
	huffman_decoder(img_comp);
	idct();
	upsample(img_comp);
}

