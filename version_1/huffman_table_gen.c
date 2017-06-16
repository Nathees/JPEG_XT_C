#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "Bitstream.h"
#include "huffman_table_gen.h"

int huff_tab_len; 				// Huffman Marker Syntax Length
int no_table; 					// Number of Quantization Tables
unsigned char Tc_Th;			// Tc - Table class (1 - AC, 0 - DC)  Th - table identifier

// Huffman Table Array Decleration
unsigned char huffman_table_DC_1[65536][2] = { { 0 } }; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
unsigned char huffman_table_AC_1[65536][2] = { { 0 } };
unsigned char huffman_table_DC_2[65536][2] = { { 0 } }; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
unsigned char huffman_table_AC_2[65536][2] = { { 0 } };

// Temperary Huffman Size array
unsigned char huff_size[256];

// Control Variables related to huffman table generation
unsigned char J,K;
int CODE;
unsigned char SI;
unsigned char HUFFSIZE;			//huffman code length
unsigned char Li;				//Li - No of huffman code length

// Control Variable for updating intermediate huffman table spaces
int location;
int current_location = -1;
int state1_location = -1;
unsigned char extend_huff_size;
unsigned char extend_huff_value;


void huffman_table_gen(void){
	//*************************************Calculate huffman table defination length (16 bit)*******************************************
	huff_tab_len = buffer[count++];  app11_len--;
	huff_tab_len = huff_tab_len << 8;
	huff_tab_len = huff_tab_len + buffer[count++] - 2;	 app11_len--;	// subtract 2 for remove the quantization table defination length (16 bit)

	//*********************** identify Tc and Th and finally add one to no_table due to Tc and Th each 4 bits**************************
	no_table = 0;
	while (no_table < huff_tab_len){
		Tc_Th = buffer[count++]; app11_len--;
		no_table = no_table + 1;

		//*****************************Identify Li and store the HUFFSIZE to huffman_table[K][1]**************************************
		J = 0;
		Li = 0;
		K = 0;
		for (J = 1; J < 17; J++){
			byte_file = buffer[count++]; app11_len--;
			Li = Li + byte_file;
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			while (byte_file){
				huff_size[K] = J;
				K = K + 1;
				byte_file = byte_file - 1;
			}
		}
		no_table = no_table + 16;				// add 16 for Li values each have 8 bits
		no_table = no_table + Li;
		generate_huffcode();				//generate huff code for each Vi,j value and store it to Huffman table
	}
}

void generate_huffcode(void){
	// Initialization of variables involed in Generation of table of Huffman codes
	location = 0;
	extend_huff_size = 0;
	extend_huff_value = 0;

	K = 0;
	CODE = 0;
	HUFFSIZE = huff_size[K];
	SI = HUFFSIZE;
	
	while (K < Li){
		State1();
		HUFFSIZE = huff_size[K];
		if (HUFFSIZE != SI){
			State2();
			while (HUFFSIZE != SI)
				State2();
		}
	}
	state1_location = 65536;
	update_intermediate_space();
}

void State1(void){
	// choose the Huffman table array based on Tc and Th
	byte_file = buffer[count++]; app11_len--;
	state1_location = CODE;
	state1_location = state1_location << (16 - huff_size[K]);

	switch (Tc_Th){
		case 0x00:	huffman_table_DC_1[state1_location][0] = huff_size[K]; 
					huffman_table_DC_1[state1_location][1] = byte_file; break;
		case 0x01:	huffman_table_DC_2[state1_location][0] = huff_size[K]; 
					huffman_table_DC_2[state1_location][1] = byte_file; break;
		case 0x10:	huffman_table_AC_1[state1_location][0] = huff_size[K]; 
					huffman_table_AC_1[state1_location][1] = byte_file; break;
		case 0x11:	huffman_table_AC_2[state1_location][0] = huff_size[K]; 
					huffman_table_AC_2[state1_location][1] = byte_file; break;
		default:	break;
	}

	update_intermediate_space();
	location = state1_location;
	extend_huff_size = huff_size[K];
	extend_huff_value = byte_file;
	CODE = CODE + 1;
	K = K + 1;
}
void State2(void){
	CODE = CODE << 1;
	SI = SI + 1;
}

void update_intermediate_space(void){
	if (extend_huff_size != 0){
		current_location = location;
		while (current_location != state1_location && current_location < 65536){

			switch (Tc_Th){
				case 0x00:	huffman_table_DC_1[current_location][0] = extend_huff_size; 
							huffman_table_DC_1[current_location][1] = extend_huff_value; break;
				case 0x01:	huffman_table_DC_2[current_location][0] = extend_huff_size; 
							huffman_table_DC_2[current_location][1] = extend_huff_value; break;
				case 0x10:	huffman_table_AC_1[current_location][0] = extend_huff_size; 
							huffman_table_AC_1[current_location][1] = extend_huff_value; break;
				case 0x11:	huffman_table_AC_2[current_location][0] = extend_huff_size; 
							huffman_table_AC_2[current_location][1] = extend_huff_value; break;
				default:	break;
			}

			current_location++;
		}
	}
}