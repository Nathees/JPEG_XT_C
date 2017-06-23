#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "huffman_decoder.h"

unsigned char compenent = 0, huff_dc_table_id = 0, huff_ac_table_id = 0, quant_table_id = 0;


// Variables Used for huffman decoder
unsigned int huff_base_chk_bitstream;
unsigned int huff_resi_chk_bitstream;
unsigned int base_bitstream;
unsigned int resi_bitstream;
unsigned char base_bitstream_state;
unsigned char resi_bitstream_state;

unsigned char huffsize;
unsigned char huff_value;
int code_word;


// Block Termination Control variables
unsigned char eob_flag;
unsigned char blk_count;

// Difference Value of each Y Cb and Cr components
int base_diff_y;
int base_diff_cb;
int base_diff_cr;
int resi_diff_y;
int resi_diff_cb;
int resi_diff_cr;

// Claculate Value Control Variables
int value_max, value_min;	//value max and min calculated based on size

void initial_load_bitstream(void){
	base_bitstream = 0;
	resi_bitstream = 0;
	huff_base_chk_bitstream = 0;
	huff_resi_chk_bitstream = 0;
	base_diff_y 	= 0;
	base_diff_cb 	= 0;
	base_diff_cr 	= 0;
	resi_diff_y 	= 0;
	resi_diff_cb 	= 0;
	resi_diff_cr 	= 0;

	for(row = 0; row < 4; row++){
		byte_file = buffer[buff_index++];
		if (byte_file == 0xFF)
			buff_index++;
		base_bitstream = base_bitstream + (byte_file << (24 - 8 * row));
	}
	base_bitstream_state = 32;
	huff_base_chk_bitstream = base_bitstream >> 16;

	index_resi = 0;
	for(row = 0; row < 4; row++){
		byte_file = buffer_resi[index_resi++];
		if (byte_file == 0xFF)
			index_resi++;
		resi_bitstream = resi_bitstream + (byte_file << (24 - 8 * row));
	}
	resi_bitstream_state = 32;
	huff_resi_chk_bitstream = resi_bitstream >> 16;
}

void huffman_decoder(unsigned char comp){

	compenent = comp;

	if(base_resi_layer == 0){
		huff_dc_table_id = resi_huff_dc_id[compenent];
		huff_ac_table_id = resi_huff_ac_id[compenent];
		quant_table_id = resi_dqt_id[compenent];
	}
	else{
		huff_dc_table_id = resi_huff_dc_id[compenent];
		huff_ac_table_id = resi_huff_ac_id[compenent];
		quant_table_id = resi_dqt_id[compenent];
	}

	reset_block();

	dc_huffman_decode();
	ac_huffman_decode();
}


void reset_block(void){
	for(row = 0; row < 8; row ++){
		for(col = 0; col < 8; col++){
			#if INTEGER_OPERATION
				if(base_resi_layer == 0)
					base_int_block[row][col] = 0;
				else
					resi_int_block[row][col] = 0;
			#else
				if(base_resi_layer == 0)
					base_float_block[row][col] = 0;
				else
					resi_float_block[row][col] = 0;
			#endif
		}
	}
}

void dc_huffman_decode(void){

	// Get Huffman Code Size and Huffman Value of particular huffman Check Bitstream
	if(base_resi_layer == 0){
		switch (huff_dc_table_id){
			case 0x00:	huffsize 	= base_huffman_table_DC_1[huff_base_chk_bitstream][0]; 
						huff_value 	= base_huffman_table_DC_1[huff_base_chk_bitstream][1];
						break;
			case 0x01:	huffsize 	= base_huffman_table_DC_2[huff_base_chk_bitstream][0]; 
						huff_value 	= base_huffman_table_DC_2[huff_base_chk_bitstream][1];
						break;
			default:	break;
		}
	}
	else{
		switch (huff_dc_table_id){
			case 0x00:	huffsize 	= resi_huffman_table_DC_1[huff_resi_chk_bitstream][0]; 
						huff_value 	= resi_huffman_table_DC_1[huff_resi_chk_bitstream][1];
						break;
			case 0x01:	huffsize 	= resi_huffman_table_DC_2[huff_resi_chk_bitstream][0]; 
						huff_value 	= resi_huffman_table_DC_2[huff_resi_chk_bitstream][1]; 
						break;
			default:	break;
		}
	}
	shift(huffsize);		// Shift the bits of the Huffman Code
	
	// Read the code word
	code_word = 0;
	if(base_resi_layer == 0)
		code_word = huff_base_chk_bitstream >> (16 - huff_value);
	else
		code_word = huff_resi_chk_bitstream >> (16 - huff_value);
	shift(huff_value);		// Shift the bits of the Huffman Value
	calculate_value();

	if(base_resi_layer == 0){
		switch (compenent)
		{
			case 0:	code_word = code_word + base_diff_y; 	base_diff_y = code_word; break;
			case 1: code_word = code_word + base_diff_cb; 	base_diff_cb = code_word; break;
			case 2: code_word = code_word + base_diff_cr; 	base_diff_cr = code_word; break;
			default: break;
		}
	}
	else{
		switch (compenent)
		{
			case 0:	code_word = code_word + resi_diff_y; 	resi_diff_y = code_word; break;
			case 1: code_word = code_word + resi_diff_cb; 	resi_diff_cb = code_word; break;
			case 2: code_word = code_word + resi_diff_cr; 	resi_diff_cr = code_word; break;
			default: break;
		}
	}
	
	// Loading the 8x8 block with huffman decoded DC value
	#if INTEGER_OPERATION
		if(base_resi_layer == 0)
			base_int_block[0][0] = ((code_word * base_int_quantization_table[0][quant_table_id] >> 6) + 1) >> 1;
		else
			resi_int_block[0][0] = ((code_word * resi_int_quantization_table[0][quant_table_id] >> 6) + 1) >> 1;
	#else
		if(base_resi_layer == 0)
			base_float_block[0][0] = (float)code_word * base_float_quantization_table[0][quant_table_id];
		else
			resi_float_block[0][0] = (float)code_word * resi_float_quantization_table[0][quant_table_id];
	#endif
}

void ac_huffman_decode(void){
	eob_flag = 1;
	blk_count = 0;

	while (eob_flag != 0 && blk_count < 63){

		// Get Huffman Code Size and Huffman Value of particular huffman Check Bitstream
		if(base_resi_layer == 0){
			switch (huff_ac_table_id){
				case 0x00:	huffsize 	= base_huffman_table_AC_1[huff_base_chk_bitstream][0]; 
							huff_value 	= base_huffman_table_AC_1[huff_base_chk_bitstream][1];
							break;
				case 0x01:	huffsize 	= base_huffman_table_AC_2[huff_base_chk_bitstream][0]; 
							huff_value 	= base_huffman_table_AC_2[huff_base_chk_bitstream][1]; 
							break;
				default:	break;
			}
		}
		else{
			switch (huff_ac_table_id){
				case 0x00:	huffsize 	= resi_huffman_table_AC_1[huff_resi_chk_bitstream][0]; 
							huff_value 	= resi_huffman_table_AC_1[huff_resi_chk_bitstream][1];
							break;
				case 0x01:	huffsize 	= resi_huffman_table_AC_2[huff_resi_chk_bitstream][0]; 
							huff_value 	= resi_huffman_table_AC_2[huff_resi_chk_bitstream][1]; 
							break;
				default:	break;
			}
		}
		shift(huffsize);		// Shift the bits of the Huffman Code

		eob_flag = huff_value;
		blk_count = blk_count + (huff_value >> 4) + 1;		//Most significant 4 bits of huff_value indicating the runlength
		huff_value = huff_value & 0xF;						// Least significant 4 bits of huff_value contain the CODE_WOED length
		
		// Read the code word
		code_word = 0;
		if(base_resi_layer == 0)
			code_word = huff_base_chk_bitstream >> (16 - huff_value);
		else
			code_word = huff_resi_chk_bitstream >> (16 - huff_value);
		shift(huff_value);		// Shift the bits of the Huffman Value
		calculate_value();

		row = Zig_Zag[blk_count] & 0xF;
		col = Zig_Zag[blk_count] >> 4;

		// Loading the 8x8 block with huffman decoded AC values
		#if INTEGER_OPERATION
			if(base_resi_layer == 0)
				base_int_block[row][col] = ((code_word * base_int_quantization_table[blk_count][quant_table_id] >> 6) + 1) >> 1;
			else
				resi_int_block[row][col] = ((code_word * resi_int_quantization_table[blk_count][quant_table_id] >> 6) + 1) >> 1;
		#else
			if(base_resi_layer == 0)
				base_float_block[row][col] = (float)code_word * base_float_quantization_table[blk_count][quant_table_id];
			else
				resi_float_block[row][col] = (float)code_word * resi_float_quantization_table[blk_count][quant_table_id];
		#endif
	}
}


void shift(unsigned char shift){
	if (shift > 0){

		if (base_resi_layer == 0){
			huff_base_chk_bitstream = 0;
			for (row = 0; row < shift; row++){
				if (base_bitstream_state == 24){
					byte_file = buffer[buff_index++];
					if (byte_file == 0xFF)
						buff_index++;
					base_bitstream = base_bitstream + byte_file;
					base_bitstream_state = 32;
				}
				base_bitstream = base_bitstream << 1;
				base_bitstream_state = base_bitstream_state - 1;
			}
			huff_base_chk_bitstream = (base_bitstream >> 16);
		}
		else{
			huff_resi_chk_bitstream = 0;
			for (row = 0; row < shift; row++){
				if (resi_bitstream_state == 24){
					byte_file = buffer_resi[index_resi++];
					if (byte_file == 0xFF)
						index_resi++;
					resi_bitstream = resi_bitstream + byte_file;
					resi_bitstream_state = 32;
				}
				resi_bitstream = resi_bitstream << 1;
				resi_bitstream_state = resi_bitstream_state - 1;
			}
			huff_resi_chk_bitstream = (resi_bitstream >> 16);
		}

	}
}

void calculate_value(void){
	
	switch (huff_value)
	{
	case 0:	value_min = 0; value_max = 0; break;
	case 1: value_min = 1; value_max = 1; break;
	case 2: value_min = 2; value_max = 3; break;
	case 3: value_min = 4; value_max = 7; break;
	case 4: value_min = 8; value_max = 15; break;
	case 5: value_min = 16; value_max = 31; break;
	case 6: value_min = 32; value_max = 63; break;
	case 7: value_min = 64; value_max = 127; break;
	case 8: value_min = 128; value_max = 255; break;
	case 9: value_min = 256; value_max = 511; break;
	case 10: value_min = 512; value_max = 1023; break;
	case 11: value_min = 1024; value_max = 2047; break;
	default: break;
	}

	if (code_word < value_min){
		code_word = code_word - value_max;
	}
}