#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"

// Sub Header Files
#include "quatization_table_gen.h"
#include "huffman_table_gen.h"
#include "scan_frame_decoder.h"
#include "tone_table_gen.h"

//  **************** ALL Files Gloabal Variables Decleration ****************
// Base Quantization Table
float base_float_quantization_table[64][2];
int base_int_quantization_table[64][2];
// Residual Quantization Table
float resi_float_quantization_table[64][2];
int resi_int_quantization_table[64][2];
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


// 
char residual_layer_flag = 0;
char resi_tbox_flag = 0;
int app11_len;
int app11_processed_bytes;


void marker_parser(void){

	// Skiping SOI Marker
	index = index + 3;

	while (buffer[index] != 0xDA){

		switch (buffer[index++]){
			case 0xC0	:	baseline_frame_decode(); break;
			case 0xC1	:	extended_frame_decode(); break;
			case 0xC2	:	progressive_frame_decode(); break;

			case 0xC4	:	huffman_table_gen(); break;
			case 0xDB  	:	quantization_table_gen();break;

			case 0xEB 	:	APP11_process(); break;
			default 	:	skip_marker(); break;
		}
		index++;
	}
	index++;
	scan_decoder();
}

void APP11_process(void){
	residual_layer_flag = 1;
	app11_processed_bytes = 0;

	app11_len = buffer[index++];
	app11_len = app11_len << 8;
	app11_len = app11_len + buffer[index++] - 2;		// subtract 2 for remove the quantization table defination length (16 bit)

	// SKIP 15 bytes
	index = index + 15;
	app11_processed_bytes = 15;

	switch (buffer[index++]){
		case 0x45:	tone_table_gen(); break;
		case 0x49:	if (resi_tbox_flag) {
						get_residual_layer_bitstream();
					}
					else {
						residual_marker_parser(); 
					}
					break;
		default:	index = index + app11_len - app11_processed_bytes - 1; break;
	}
	residual_layer_flag = 0;
}

void residual_marker_parser(void){

	resi_tbox_flag = 1;

	// Skiping SOI Marker in Residual Layer
	index = index + 3;
	app11_processed_bytes = app11_processed_bytes + 3;

	while (buffer[index] != 0xDA){

		switch (buffer[index++]){
			case 0xC0	:	baseline_frame_decode(); break;
			case 0xC1	:	extended_frame_decode(); break;
			case 0xC2	:	progressive_frame_decode(); break;
		
			case 0xC4	:	huffman_table_gen(); break;
			case 0xDB	:	quantization_table_gen(); break;

			default		:	skip_marker(); break;
		}
		index++;
		app11_processed_bytes = app11_processed_bytes + 2;
	}
	index++; 
	app11_processed_bytes++;
	scan_decoder();

	index_resi = 0;
	get_residual_layer_bitstream();
}

void skip_marker(void){
	int skip_marker_len;
	skip_marker_len = buffer[index++];
	skip_marker_len = (skip_marker_len << 8) + buffer[index++];

	index = index + skip_marker_len - 2;
	app11_processed_bytes = app11_processed_bytes + skip_marker_len;
}

void get_residual_layer_bitstream(void){
	while (app11_processed_bytes < app11_len){
		buffer_resi[index_resi++] = buffer[index++];
		app11_processed_bytes++;
	}
}