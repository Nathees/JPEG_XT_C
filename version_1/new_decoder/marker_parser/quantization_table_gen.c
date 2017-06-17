#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"
#include "quatization_table_gen.h"

int quanti_tab_len; 			// Quantization Marker Syntax Length
int processed_bytes; 			// Number of Processed Bytes
unsigned char Pq,Tq;			// Pq - element precision  Tq - table identifier
char no_dqt_tables; 			// Number of DQT Tables


const int cos_val[8] = { 23170, 32138, 30274, 27246, 23170, 18205, 12540, 6394 };  // Inger Operation Cosine Values
const float pi = 3.14159;


void quantization_table_gen(void){

	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0)
			printf("DQT Maker Process");
		else
			printf("APP11 - DQT Maker Process");
	#endif
	
	//************************************* calculate quantization table defination length (16 bit)**************************************
	quanti_tab_len = buffer[index++]; app11_processed_bytes++;
	quanti_tab_len = quanti_tab_len << 8;
	quanti_tab_len = quanti_tab_len + buffer[index++] - 2;	app11_processed_bytes++;	// subtract 2 for remove the quantization table defination length (16 bit)
	
	//*********************** identify Pq and Tq and finally add one to no_table due to Pq and Tq each 4 bits*****************************
	processed_bytes = 0;
	no_dqt_tables = 0;
	while (processed_bytes < quanti_tab_len){
		no_dqt_tables = no_dqt_tables + 1;  
		Pq = buffer[index++]; 
		Pq = Pq >> 4;
		Tq = byte_file;
		Tq = Tq & 0xF;
		app11_processed_bytes++;
		processed_bytes = processed_bytes + 1;

		if (byte_file == 0xFF)
			remove_00_byte("Error in DQT marker segment");

		//********************************************Generating Quantization Table *************************************** 
		for (char i = 0; i < 8; i++){
			for (char j = 0; j < 8; j++){
				row = Zig_Zag[i * 8 + j] & 0xF;
				col = Zig_Zag[i * 8 + j] >> 4;

				if(residual_layer_flag == 0){
					#if INTEGER_OPERATION
						base_int_quantization_table[i * 8 + j][Tq] = (((buffer[index++] * cos_val[col]) >> 4) + 1) >> 1; 
					#elif (IDCT_OPERATION == 1)
						if (col == 0)
							base_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0 * sqrt(2.0);
						else
							base_float_quantization_table[i * 8 + j][Tq] = ((float)buffer[index++] / 2.0)  * cos((float)pi * col / 16.0);
					#else
						if (row == 0 && col == 0)
							base_float_quantization_table[0][Tq] = (float)buffer[index++] / 8;
						else if (row == 0 || col == 0)
							base_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0 / sqrt(2.0);
						else
							base_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0;
	
						if (~(row == 0 && col == 0))
							base_float_quantization_table[i * 8 + j][Tq] = base_float_quantization_table[i * 8 + j][Tq] 
																	  * cos((float)pi * row / 16.0) * cos((float)pi * col / 16.0);
					#endif
				}
				else{
					#if INTEGER_OPERATION
						resi_int_quantization_table[i * 8 + j][Tq] = (((buffer[index++] * cos_val[col]) >> 4) + 1) >> 1; 
					#elif (IDCT_OPERATION == 1)
						if (col == 0)
							resi_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0 * sqrt(2.0);
						else
							resi_float_quantization_table[i * 8 + j][Tq] = ((float)buffer[index++] / 2.0)  * cos((float)pi * col / 16.0);
					#else
						if (row == 0 && col == 0)
							resi_float_quantization_table[0][Tq] = (float)buffer[index++] / 8;
						else if (row == 0 || col == 0)
							resi_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0 / sqrt(2.0);
						else
							resi_float_quantization_table[i * 8 + j][Tq] = (float)buffer[index++] / 4.0;
	
						if (~(row == 0 && col == 0))
							resi_float_quantization_table[i * 8 + j][Tq] = resi_float_quantization_table[i * 8 + j][Tq] 
																	  * cos((float)pi * row / 16.0) * cos((float)pi * col / 16.0);
					#endif
				}
			}
		}
		app11_processed_bytes = app11_processed_bytes + 64;
		processed_bytes = processed_bytes + 64;
	}

	// Tracking Operation
	#if TRACKING_ENABLE
		printf("\t-\t%d Tables\n",no_dqt_tables);
	#endif
}


void debug_quantization_table(void){

}
