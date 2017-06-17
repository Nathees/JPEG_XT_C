#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"

#include "marker_parser.h"

// Sub Header Files
#include "quantization_table_gen.h"
#include "huffman_table_gen.h"
#include "scan_frame_decoder.h"
#include "tone_table_gen.h"

//  **************** ALL Files Gloabal Variables Decleration ****************
int app11_processed_bytes;

// ***************** Local Gloabal Variable Decleration 
char residual_layer_flag = 0;
char resi_tbox_flag = 0;
int app11_len;


void marker_parser(void){

	// Skiping SOI Marker
	buff_index = buff_index + 3;

	while (buffer[buff_index] != 0xDA){

		switch (buffer[buff_index++]){
			case 0xC0	:	baseline_frame_decode(); break;
			case 0xC1	:	extended_frame_decode(); break;
			case 0xC2	:	progressive_frame_decode(); break;

			case 0xC4	:	huffman_table_gen(); break;
			case 0xDB  	:	quantization_table_gen();break;

			case 0xEB 	:	APP11_process(); break;
			default 	:	skip_marker(); break;
		}
		buff_index++;
	}
	buff_index++;
	scan_decoder();
}

void APP11_process(void){
	residual_layer_flag = 1;
	app11_processed_bytes = 0;

	app11_len = buffer[buff_index++];
	app11_len = app11_len << 8;
	app11_len = app11_len + buffer[buff_index++] - 2;		// subtract 2 for remove the quantization table defination length (16 bit)

	// SKIP 15 bytes
	buff_index = buff_index + 15;
	app11_processed_bytes = 15;

	switch (buffer[buff_index++]){
		case 0x45:	tone_table_gen(); break;
		case 0x49:	if (resi_tbox_flag) {
						get_residual_layer_bitstream();
					}
					else {
						residual_marker_parser(); 
					}
					break;
		default:	buff_index = buff_index + app11_len - app11_processed_bytes - 1; break;
	}
	residual_layer_flag = 0;
}

void residual_marker_parser(void){

	resi_tbox_flag = 1;

	// Skiping SOI Marker in Residual Layer
	buff_index = buff_index + 3;
	app11_processed_bytes = app11_processed_bytes + 3;

	while (buffer[buff_index] != 0xDA){

		switch (buffer[buff_index++]){
			case 0xC0	:	baseline_frame_decode(); break;
			case 0xC1	:	extended_frame_decode(); break;
			case 0xC2	:	progressive_frame_decode(); break;
		
			case 0xC4	:	huffman_table_gen(); break;
			case 0xDB	:	quantization_table_gen(); break;

			default		:	skip_marker(); break;
		}
		buff_index++;
		app11_processed_bytes = app11_processed_bytes + 2;
	}
	buff_index++; 
	app11_processed_bytes++;
	scan_decoder();

	index_resi = 0;
	get_residual_layer_bitstream();
}

void skip_marker(void){
	int skip_marker_len;
	skip_marker_len = buffer[buff_index++];
	skip_marker_len = (skip_marker_len << 8) + buffer[buff_index++];

	buff_index = buff_index + skip_marker_len - 2;
	app11_processed_bytes = app11_processed_bytes + skip_marker_len;
}

void get_residual_layer_bitstream(void){
	while (app11_processed_bytes < app11_len){
		buffer_resi[index_resi++] = buffer[buff_index++];
		app11_processed_bytes++;
	}
}