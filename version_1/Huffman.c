#include <stdio.h>
#include "Bitstream.h"
#include "Main.h"
#include "Huffman.h"
#include "IDCT.h"

unsigned char huffman_table_DC_1[65536][2] = { { 0 } }; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
unsigned char huffman_table_AC_1[65536][2] = { { 0 } };
unsigned char huffman_table_DC_2[65536][2] = { { 0 } }; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
unsigned char huffman_table_AC_2[65536][2] = { { 0 } };

int DIFF_Y;
int DIFF_Cb;
int DIFF_Cr;

int J;
unsigned char K;
int CODE;
unsigned char SI;
unsigned char HUFFSIZE;			//huffman code length
unsigned char Li;				//Li - No of huffman code length
unsigned char AC_termination;	// terminating variable

unsigned char VALUE;
int CODE_WORD;
unsigned char row, col;

int location;
int current_location = -1;
int state1_location = -1;
unsigned char extend_huff_size;
unsigned char extend_huff_value;

unsigned int bitstream;
unsigned int huff_chk_bitstream;
unsigned char bitstream_state = 0;
//******************************Below 4 functions involved in Huffman code generation*****************************************
void generate_huffcode(unsigned char table_destination_id){
	
	// Initialization of variables involed in Generation of table of Huffman codes
	location = 0;
	extend_huff_size = 0;
	extend_huff_value = 0;

	K = 0;
	CODE = 0;
	HUFFSIZE = huff_size[K];
	SI = HUFFSIZE;
	
	while (K < Li){
		State1(table_destination_id);
		HUFFSIZE = huff_size[K];
		if (HUFFSIZE != SI){
			State2();
			while (HUFFSIZE != SI)
				State2();
		}
	}
	state1_location = 65536;
	update_intermediate_space(table_destination_id);
}

void State1(unsigned char table_id){
	// choose the Huffman table array based on Tc and Th
	byte_file = buffer[count++]; app11_len--;
	state1_location = CODE;
	state1_location = state1_location << (16 - huff_size[K]);
	switch (table_id){

	case 0x00:	huffman_table_DC_1[state1_location][0] = huff_size[K]; huffman_table_DC_1[state1_location][1] = byte_file; break;
	case 0x01:	huffman_table_DC_2[state1_location][0] = huff_size[K]; huffman_table_DC_2[state1_location][1] = byte_file; break;
	case 0x10:	huffman_table_AC_1[state1_location][0] = huff_size[K]; huffman_table_AC_1[state1_location][1] = byte_file; break;
	case 0x11:	huffman_table_AC_2[state1_location][0] = huff_size[K]; huffman_table_AC_2[state1_location][1] = byte_file; break;
	default:	break;
	}
	update_intermediate_space(table_id);
	location = state1_location;
	extend_huff_size = huff_size[K];
	extend_huff_value = byte_file;
	CODE = CODE + 1;
	K = K + 1;
}
void State2(){
	CODE = CODE << 1;
	SI = SI + 1;
}

void update_intermediate_space(unsigned char table_id){
	if (extend_huff_size != 0){
		current_location = location;
		while (current_location != state1_location && current_location < 65536){
			switch (table_id){

			case 0x00:	huffman_table_DC_1[current_location][0] = extend_huff_size; huffman_table_DC_1[current_location][1] = extend_huff_value; break;
			case 0x01:	huffman_table_DC_2[current_location][0] = extend_huff_size; huffman_table_DC_2[current_location][1] = extend_huff_value; break;
			case 0x10:	huffman_table_AC_1[current_location][0] = extend_huff_size; huffman_table_AC_1[current_location][1] = extend_huff_value; break;
			case 0x11:	huffman_table_AC_2[current_location][0] = extend_huff_size; huffman_table_AC_2[current_location][1] = extend_huff_value; break;
			default:	break;
			}
			current_location++;
		}
	}
}


//******************************************Huffman Decoding*****************************************************************
void Huffman_decode(unsigned char comp, unsigned char DC_table_id, unsigned char AC_table_id, unsigned char Quantization_table_id){
	//**********************************************Reseting the Block************************************************
	Reset_Block();
	//**********************************************Decoding DC Elements************************************************
	DC_Huffman_decode(comp,DC_table_id, Quantization_table_id);
	//**********************************************Decoding AC Elements************************************************
	AC_Huffman_decode(AC_table_id, Quantization_table_id);
}

void Reset_Block(){
	for (J = 0; J < 8; J++)
		for (K = 0; K < 8; K++){
			block[J][K] = 0;
			block_temp[J][K] = 0;
			integer_block[J][K] = 0;
		}
}

void initial_load_bitstream(){
	bitstream = 0;
	huff_chk_bitstream = 0;
	for (row = 0; row < 4; row++){
		if (resi_tbox_flag){
			byte_file = buffer_resi[count_resi++];
		}
		else{
			byte_file = buffer[count++];
		}
		if (byte_file == 0xFF)
			remove_00_byte("Error in Encoded Bitstream\n");
		bitstream = bitstream + (byte_file << (24 - 8 * row));
	}
	bitstream_state = 32;
	huff_chk_bitstream = bitstream >> 16;
}

void shift(unsigned char shift){
	if (shift > 0){
		huff_chk_bitstream = 0;
		for (row = 0; row < shift; row++){
			if (bitstream_state == 24){

				if (resi_tbox_flag){
					byte_file = buffer_resi[count_resi++];
				}
				else{
					byte_file = buffer[count++];
				}

				if (byte_file == 0xFF){
					remove_00_byte("Error in Encoded Bitstream\n");  //count++;
				}
				bitstream = bitstream + byte_file;
				bitstream_state = 32;
			}
			bitstream = bitstream << 1;
			bitstream_state = bitstream_state - 1;
		}
		huff_chk_bitstream = (bitstream >> 16);
	}
}

void DC_Huffman_decode(unsigned char comp, unsigned char table_id, unsigned char Quantization_table_id){
	// Get Huffman Code Size and Huffman Value of particular huffman Check Bitstream
	switch (table_id){
	case 0x00:	HUFFSIZE = huffman_table_DC_1[huff_chk_bitstream][0]; VALUE = huffman_table_DC_1[huff_chk_bitstream][1]; break;
	case 0x01:	HUFFSIZE = huffman_table_DC_2[huff_chk_bitstream][0]; VALUE = huffman_table_DC_2[huff_chk_bitstream][1]; break;
		default:	break;
	}
	shift(HUFFSIZE);	// Shift the bits of the Huffman Code
	
	// Read the code word
	CODE_WORD = 0;
	CODE_WORD = huff_chk_bitstream >> (16 - VALUE);
	shift(VALUE);		// Shift the bits of the Huffman Value
	calculate_value();
	switch (comp)
	{
	case 0:	CODE_WORD = CODE_WORD + DIFF_Y; DIFF_Y = CODE_WORD; break;
	case 1: CODE_WORD = CODE_WORD + DIFF_Cb; DIFF_Cb = CODE_WORD; break;
	case 2: CODE_WORD = CODE_WORD + DIFF_Cr; DIFF_Cr = CODE_WORD; break;
	default: break;
	}

	// Integer Operation
	if (resi_tbox_flag)
		integer_block[0][0] = ((CODE_WORD *integer_modified_resi_quantization_table[0][Quantization_table_id] >> 6) + 1) >> 1;
	else
		integer_block[0][0] = ((CODE_WORD *integer_modified_quantization_table[0][Quantization_table_id] >> 6) + 1) >> 1;

	// Float Operation
	/*if (resi_tbox_flag)
		block[0][0] = resi_quantization_table[0][Quantization_table_id] * CODE_WORD;
	else
		block[0][0] = quantization_table[0][Quantization_table_id] * CODE_WORD;*/


	//block[0][0] = CODE_WORD * modified_quantization_table[0][Quantization_table_id]; // modified_quantization_table[0][Quantization_table_id] *  dequantization with Feig implementation for IDCT
	//integer_block[0][0] = CODE_WORD *integer_modified_quantization_table[0][Quantization_table_id];

	//integer_block[0][0] = integer_block[0][0] + 128;
	//integer_block[0][0] = CODE_WORD;
}

void AC_Huffman_decode(unsigned char table_id, unsigned char Quantization_table_id){
	AC_termination = 1;
	J = 0;
	while (AC_termination != 0 && J < 63){
		// Get Huffman Code Size and Huffman Value of particular huffman Check Bitstream
		switch (table_id){
		case 0x00:	HUFFSIZE = huffman_table_AC_1[huff_chk_bitstream][0]; VALUE = huffman_table_AC_1[huff_chk_bitstream][1]; break;
		case 0x01:	HUFFSIZE = huffman_table_AC_2[huff_chk_bitstream][0]; VALUE = huffman_table_AC_2[huff_chk_bitstream][1]; break;
		default:	break;
		}
		shift(HUFFSIZE);	// Shift the bits of the Huffman Code

		AC_termination = VALUE;
		J = J + (VALUE >> 4) + 1;		//Most significant 4 bits of VALUE indicating the runlength
		VALUE = VALUE & 0xF;			// Least significant 4 bits of VALUE contain the CODE_WOED length
		// Read the code word
		CODE_WORD = 0;
		CODE_WORD = huff_chk_bitstream >> (16 - VALUE);
		shift(VALUE);		// Shift the bits of the Huffman Value
		calculate_value();

		row = Zig_Zag[J] & 0xF;
		col = Zig_Zag[J] >> 4;

		// Integer Operation
		if (resi_tbox_flag)
			integer_block[row][col] = ((CODE_WORD *integer_modified_resi_quantization_table[J][Quantization_table_id] >> 6) + 1) >> 1;
		else
			integer_block[row][col] = ((CODE_WORD *integer_modified_quantization_table[J][Quantization_table_id] >> 6) + 1) >> 1;

		// Float Operation
		/*if (resi_tbox_flag)
			block[row][col] = resi_quantization_table[J][Quantization_table_id] * CODE_WORD;
		else
			block[row][col] = quantization_table[J][Quantization_table_id] * CODE_WORD;*/

		//block[row][col] = CODE_WORD * modified_quantization_table[J][Quantization_table_id]; // modified_quantization_table[J][Quantization_table_id] *  dequantization with Feig implementation for IDCT
		//integer_block[row][col] = CODE_WORD *integer_modified_quantization_table[J][Quantization_table_id];

		//integer_block[row][col] = CODE_WORD;
	}
}

void calculate_value(){
	int value_max, value_min;	//value max and min calculated based on size
	switch (VALUE)
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

	if (CODE_WORD < value_min){
		CODE_WORD = CODE_WORD - value_max;
	}
}