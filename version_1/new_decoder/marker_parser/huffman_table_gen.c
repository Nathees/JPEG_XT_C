#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"
#include "huffman_table_gen.h"

int huff_tab_len; 				// Huffman Marker Syntax Length
int processed_bytes; 			// Number of Processed Bytes
unsigned char Tc_Th;			// Tc - Table class (1 - AC, 0 - DC)  Th - table identifier
char no_dht_tables; 			// Number of DHT Tables



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

	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0)
			printf("DHT Maker Process");
		else
			printf("APP11 - DHT Maker Process");
	#endif

	//*************************************Calculate huffman table defination length (16 bit)*******************************************
	huff_tab_len = buffer[index++];  app11_processed_bytes++;
	huff_tab_len = huff_tab_len << 8;
	huff_tab_len = huff_tab_len + buffer[index++] - 2;	 app11_processed_bytes++;	// subtract 2 for remove the quantization table defination length (16 bit)

	//*********************** identify Tc and Th and finally add one to no_table due to Tc and Th each 4 bits**************************
	processed_bytes = 0;
	no_dht_tables = 0;
	while (processed_bytes < huff_tab_len){
		no_dht_tables = no_dht_tables + 1;
		Tc_Th = buffer[index++]; 
		app11_processed_bytes++;
		processed_bytes = processed_bytes + 1;

		//*****************************Identify Li and store the HUFFSIZE to huffman_table[K][1]**************************************
		J = 0;
		Li = 0;
		K = 0;
		for (J = 1; J < 17; J++){
			byte_file = buffer[index++]; 
			Li = Li + byte_file;
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			while (byte_file){
				huff_size[K] = J;
				K = K + 1;
				byte_file = byte_file - 1;
			}
		}
		app11_processed_bytes = app11_processed_bytes + 16;
		processed_bytes = processed_bytes + 16;				// add 16 for Li values each have 8 bits
		processed_bytes = processed_bytes + Li;
		generate_huffcode();				//generate huff code for each Vi,j value and store it to Huffman table
	}

	// Tracking Operation
	#if TRACKING_ENABLE
		printf("\t-\t%d Tables\n",no_dht_tables);
	#endif
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
	byte_file = buffer[index++]; app11_processed_bytes++;
	state1_location = CODE;
	state1_location = state1_location << (16 - huff_size[K]);

	if(residual_layer_flag == 0){
		switch (Tc_Th){
			case 0x00:	base_huffman_table_DC_1[state1_location][0] = huff_size[K]; 
						base_huffman_table_DC_1[state1_location][1] = byte_file; break;
			case 0x01:	base_huffman_table_DC_2[state1_location][0] = huff_size[K]; 
						base_huffman_table_DC_2[state1_location][1] = byte_file; break;
			case 0x10:	base_huffman_table_AC_1[state1_location][0] = huff_size[K]; 
						base_huffman_table_AC_1[state1_location][1] = byte_file; break;
			case 0x11:	base_huffman_table_AC_2[state1_location][0] = huff_size[K]; 
						base_huffman_table_AC_2[state1_location][1] = byte_file; break;
			default:	break;
		}
	}
	else{
		switch (Tc_Th){
			case 0x00:	resi_huffman_table_DC_1[state1_location][0] = huff_size[K]; 
						resi_huffman_table_DC_1[state1_location][1] = byte_file; break;
			case 0x01:	resi_huffman_table_DC_2[state1_location][0] = huff_size[K]; 
						resi_huffman_table_DC_2[state1_location][1] = byte_file; break;
			case 0x10:	resi_huffman_table_AC_1[state1_location][0] = huff_size[K]; 
						resi_huffman_table_AC_1[state1_location][1] = byte_file; break;
			case 0x11:	resi_huffman_table_AC_2[state1_location][0] = huff_size[K]; 
						resi_huffman_table_AC_2[state1_location][1] = byte_file; break;
			default:	break;
		}
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

			if(residual_layer_flag == 0){
				switch (Tc_Th){
					case 0x00:	base_huffman_table_DC_1[current_location][0] = extend_huff_size; 
								base_huffman_table_DC_1[current_location][1] = extend_huff_value; break;
					case 0x01:	base_huffman_table_DC_2[current_location][0] = extend_huff_size; 
								base_huffman_table_DC_2[current_location][1] = extend_huff_value; break;
					case 0x10:	base_huffman_table_AC_1[current_location][0] = extend_huff_size; 
								base_huffman_table_AC_1[current_location][1] = extend_huff_value; break;
					case 0x11:	base_huffman_table_AC_2[current_location][0] = extend_huff_size; 
								base_huffman_table_AC_2[current_location][1] = extend_huff_value; break;
					default:	break;
				}
			}
			else{
				switch (Tc_Th){
					case 0x00:	resi_huffman_table_DC_1[current_location][0] = extend_huff_size; 
								resi_huffman_table_DC_1[current_location][1] = extend_huff_value; break;
					case 0x01:	resi_huffman_table_DC_2[current_location][0] = extend_huff_size; 
								resi_huffman_table_DC_2[current_location][1] = extend_huff_value; break;
					case 0x10:	resi_huffman_table_AC_1[current_location][0] = extend_huff_size; 
								resi_huffman_table_AC_1[current_location][1] = extend_huff_value; break;
					case 0x11:	resi_huffman_table_AC_2[current_location][0] = extend_huff_size; 
								resi_huffman_table_AC_2[current_location][1] = extend_huff_value; break;
					default:	break;
				}
			}

			current_location++;
		}
	}
}