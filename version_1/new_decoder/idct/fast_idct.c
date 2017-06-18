#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "idct.h"
#include "fast_idct.h"

// Integer IDCT Control Variables
const int cos_val[8] = { 23170, 32138, 30274, 27246, 23170, 18205, 12540, 6394 };
const int int_C2 = 60547;  	// 2*cos(pi/8) = 1.8477590650225735122563663787936 * 2 ^ 17 	= 242189.47617063875539846645400123  	= 242189	(17)
const int int_C4 = 46381;  	// sqrt(2) = 1.4142135623730950488016887242097 * 2 ^ 16 		= 92681.900023683157118267472229807    	= 92682		(16)
const int int_C6 = 25080;  	// 2*sin(pi/8) = 0.7653668647301795434569199680608 * 2 ^  16 	= 200636.33138782818623997082810733		= 200636	(18)
const int int_Q = 35468;  	// Q1 = C2 - C6 = 1.0823922002923939687994464107328 * 2 ^ 16 	= 70935.655238362331139240519973785		= 70936		(16)
const int int_R = 42813; 	// R1 = C2 + C6 = 2.6131259297527530557132863468544 * 2 ^ 16 	= 171253.82093227642425922593402745		= 171253	(16)


int idct_temp[8][8], idct_temp1[8][8];

int stage1_0, stage1_1, stage1_2, stage1_3, stage1_4, stage1_5, stage1_6, stage1_7, stage1_8;
int wire1, wire2, wire3, wire4;

int stage2_0, stage2_1, stage2_2, stage2_3, stage2_4, stage2_5, stage2_6, stage2_7;
int wire5, wire6;

int wire7, wire8, wire9, wire10, wire11;

void fast_int_idct(void){

	block_to_temp_data_transfer();
	
	RTL_function(); 		// 1D Row IDCT
	col_row_rotation(); 	// Row to Column Covertion

	// Cosine Multiplication of Col Operation
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			idct_temp[row][col] = idct_temp[row][col] * cos_val[col];
			idct_temp[row][col] = ((idct_temp[row][col] >> 15) + 1) >> 1;
		}
	}

	RTL_function(); 		// 1D Column IDCT
	col_row_rotation(); 	// Column to Row Covertion

	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			idct_temp[row][col] = ((idct_temp[row][col] >> 3) + 1)>>1; // ((rtl_block[row][col] >> 3) + 1) >> 1;
			idct_temp[row][col] = idct_temp[row][col] + 128; // Level Shift
			if (idct_temp[row][col] < 0)
				idct_temp[row][col] = 0;
			else if (idct_temp[row][col] > 255)
				idct_temp[row][col] = 255;
		}
	}

	temp_to_block_data_transfer();
}

void block_to_temp_data_transfer(void){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			if(base_resi_layer == 0)
				idct_temp[row][col] = base_int_block[row][col];
			else
				idct_temp[row][col] = resi_int_block[row][col];
		}
	}
}
void temp_to_block_data_transfer(void){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			if(base_resi_layer == 0)
				base_int_block[row][col] = idct_temp[row][col];
			else
				resi_int_block[row][col] = idct_temp[row][col];
		}
	}
}

void col_row_rotation(void){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			idct_temp1[row][col] = idct_temp[row][col];
		}
	}
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			idct_temp[row][col] = idct_temp1[col][row];
		}
	}
}

void RTL_function(void){
	for (row = 0; row < 8; row++){
		stage1();
		stage2();
		stage3();
	}
}
void stage1(void){
	wire1 = idct_temp[row][5] - idct_temp[row][3];
	wire2 = idct_temp[row][1] - idct_temp[row][7];
	wire3 = idct_temp[row][1] + idct_temp[row][7];
	wire4 = idct_temp[row][3] + idct_temp[row][5];

	stage1_0 = idct_temp[row][0];
	stage1_1 = idct_temp[row][4];
	stage1_2 = idct_temp[row][2] - idct_temp[row][6];
	stage1_3 = idct_temp[row][2] + idct_temp[row][6];
	stage1_4 = wire1;
	stage1_5 = wire3 - wire4;
	stage1_6 = wire2;
	stage1_7 = wire3 + wire4;
	stage1_8 = wire1 + wire2;
}
void stage2(void){
	wire5 = ((int_C6*stage1_8 >> 14) + 1) >> 1;
	wire6 = stage1_0 + stage1_1;

	stage2_0 = -(((int_Q*stage1_4 >> 14) + 1) >> 1) - wire5;
	stage2_1 = (((int_R*stage1_6 >> 13) + 1) >> 1) - wire5;
	stage2_2 = stage1_0 - stage1_1;
	stage2_3 = (((int_C4*stage1_2 >> 14) + 1) >> 1) - stage1_3;
	stage2_4 = wire6 + stage1_3;
	stage2_5 = wire6 - stage1_3;
	stage2_6 = stage1_7;
	stage2_7 = stage1_7 + (((int_C4*stage1_5 >> 14) + 1) >> 1);
}
void stage3(void){
	wire7 = stage2_2 + stage2_3;
	wire8 = stage2_1 - stage2_6;
	wire9 = stage2_2 - stage2_3;
	wire10 = stage2_1 - stage2_7;
	wire11 = stage2_0 - wire10;

	idct_temp[row][0] = stage2_4 + stage2_6;
	idct_temp[row][1] = wire7 + wire8;
	idct_temp[row][2] = wire9 - wire10;
	idct_temp[row][3] = stage2_5 - wire11;
	idct_temp[row][4] = stage2_5 + wire11;
	idct_temp[row][5] = wire9 + wire10;
	idct_temp[row][6] = wire7 - wire8;
	idct_temp[row][7] = stage2_4 - stage2_6;
}