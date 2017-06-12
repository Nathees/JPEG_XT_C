#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "Main.h"
#include "Bitstream.h"
#include "quatization_table_gen.h"

int quanti_tab_len; 			// Quantization Marker Syntax Length
int no_table; 					// Number of Quantization Tables
unsigned char Pq,Tq;			// Pq - element precision  Tq - table identifier


const int cos_val[8] = { 23170, 32138, 30274, 27246, 23170, 18205, 12540, 6394 };  // Inger Operation Cosine Values
const float pi = 3.14159;

float float_quantization_table[64][2];
int int_quantization_table[64][2];

void quantization_table_gen(void){
	
	//************************************* calculate quantization table defination length (16 bit)**************************************
	quanti_tab_len = buffer[count++]; app11_len--;
	quanti_tab_len = quanti_tab_len << 8;
	quanti_tab_len = quanti_tab_len + buffer[count++] - 2;	app11_len--;	// subtract 2 for remove the quantization table defination length (16 bit)
	
	//*********************** identify Pq and Tq and finally add one to no_table due to Pq and Tq each 4 bits*****************************
	no_table = 0;
	while (no_table < quanti_tab_len){
		byte_file = buffer[count++]; 
		Pq = byte_file;
		Pq = Pq >> 4;
		Tq = byte_file;
		Tq = Tq & 0xF;
		no_table = no_table + 1;

		if (byte_file == 0xFF)
			remove_00_byte("Error in DQT marker segment");

		//********************************************Generating Quantization Table *************************************** 
		for (char i = 0; i < 8; i++){
			for (char j = 0; j < 8; j++){
				row = Zig_Zag[i * 8 + j] & 0xF;
				col = Zig_Zag[i * 8 + j] >> 4;

				#if INTEGER_OPERATION
					int_quantization_table[i * 8 + j][Tq] = (((buffer[count++] * cos_val[col]) >> 4) + 1) >> 1; 
				#elif (IDCT_OPERATION == 1)
					if (col == 0)
						float_quantization_table[i * 8 + j][Tq] = (float)buffer[count++] / 4.0 * sqrt(2.0);
					else
						float_quantization_table[i * 8 + j][Tq] = ((float)buffer[count++] / 2.0)  * cos((float)pi * col / 16.0);
				#else
					if (row == 0 && col == 0)
						float_quantization_table[0][Tq] = (float)buffer[count++] / 8;
					else if (row == 0 || col == 0)
						float_quantization_table[i * 8 + j][Tq] = (float)buffer[count++] / 4.0 / sqrt(2.0);
					else
						float_quantization_table[i * 8 + j][Tq] = (float)buffer[count++] / 4.0;

					if (~(row == 0 && col == 0))
						float_quantization_table[i * 8 + j][Tq] = float_quantization_table[i * 8 + j][Tq] 
																  * cos((float)pi * row / 16.0) * cos((float)pi * col / 16.0);
				#endif
			}
		}
		no_table = no_table + 64;
	}
}


void debug_quantization_table(void){

}
