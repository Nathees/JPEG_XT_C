#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"

char residual_layer_flag = 0;


void marker_parser(void){
	index++;					// Skipping FF Byte
	while (buffer[index] != 0xDA){
		
		if (resi_tbox_flag && buffer[index] != 0xEB){
			residual_decode_process();
			resi_tbox_flag = 0;
			printf("residual layer decoding Finished\n");
		}

		switch (buffer[index++]){
		case 0xC0	:	Baseline_Frame(); break;
		case 0xC1	:	Extended_Frame(); break;
		case 0xC2	:	Progressive_Frame(); break;

		case 0xC4	:	Huffman(); break;
		case 0xDB  :	Quantization();break;

		case 0xEB:	residual_marker_parser(); break;
		default:	skip_marker(); break;
		}
		index++;
	}
	index++;
	Scan();
}

void residual_marker_parser(void){

}

void skip_marker(void){
	marker_len = 0;
	marker_len = buffer[count++];
	marker_len = (marker_len << 8) + buffer[count++];
	count = count + marker_len - 2;
}

