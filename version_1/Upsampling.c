#include <stdio.h>
#include <stdlib.h>

#include "Bitstream.h"
#include "Main.h"
#include "Huffman.h"
#include "Upsampling.h"

float **YCbCr_Y;
float **YCbCr_Cb;
float **YCbCr_Cr;

int row_blk, col_blk;

void create_dynamic_array_YCbCr(){
	YCbCr_Y = (float **)malloc(sizeof(float *) * Y);
	YCbCr_Cb = (float **)malloc(sizeof(float *) * Y);
	YCbCr_Cr = (float **)malloc(sizeof(float *) * Y);
	for (int i = 0; i < Y; i++){
		YCbCr_Y[i] = (float *) malloc(X * sizeof(float));
		YCbCr_Cb[i] = (float *) malloc(X * sizeof(float));
		YCbCr_Cr[i] = (float *) malloc(X * sizeof(float));
	}
}


unsigned char identify_upsampling_type(void){
	/*			H1		V1		H2		V2		H3		V3
	type 1	:	1		1		1		1		1		1
	type 2	:	2		2		2		1		2		1
	type 3	:	2		2		1		2		1		2
	type 4	:	2		2		1		1		1		1
	*/
	// comp_specific_param[3][6]:- [1]-H, [2]-V
	if (comp_specific_param[0][1] == 1){			//H1 == 1
		return 1; // return type 1: H1 = 1
	}
	else{											//H1 == 2
		if (comp_specific_param[1][1] == 1){		//H2 == 1	
			if (comp_specific_param[1][2] == 1){	//V2 == 1
				return 4; // return type 4: H1 = 2, H2 = 1, V2 = 1
			}
			else{									//V2 == 2
				return 3; // return type 3: H1 = 2, H2 = 1, V2 = 2
			}
		}
		else{										//H2 == 2
			return 2; // return type 2: H1 = 1, H2 = 2
		}
	}
}

void transfer_element(unsigned char upsampling_type, int mcu_y, int mcu_x, unsigned char component_id, unsigned char vert_blk, unsigned char hori_blk){

	switch (upsampling_type)
	{
	case 1: row_blk = (mcu_y * 8); //V1 = V2 = V3 = 1
			col_blk = (mcu_x * 8); //H1 = H2 = H3 = 1
			for (row = 0; row < 8; row++){
				if (Y > row_blk + row){
					for (col = 0; col < 8; col++){
						if (X > col_blk + col){
							switch (component_id)
							{
							case 0:	YCbCr_Y[row_blk + row][col_blk + col] = block[row][col]; break;
							case 1:	YCbCr_Cb[row_blk + row][col_blk + col] = block[row][col]; break;
							case 2:	YCbCr_Cr[row_blk + row][col_blk + col] = block[row][col]; break;
							default: break;
							}
						}
					}
				}
			}
			break;

	case 2: switch (component_id)
			{
			case 0:		row_blk = (mcu_y * 2 * 8) + (8 * vert_blk); //V1 = 2
						col_blk = (mcu_x * 2 * 8) + (8 * hori_blk); //H1 = 2
						for (row = 0; row < 8; row++){
							if (Y > row_blk + row){
								for (col = 0; col < 8; col++){
									if (X > col_blk + col){
										YCbCr_Y[row_blk + row][col_blk + col] = block[row][col];
									}
								}
							}
						}
						break;
			default:	row_blk = (mcu_y * 1 * 16);					//V2 = V3 = 1
						col_blk = (mcu_x * 2 * 8) + (8 * hori_blk); //H2 = H3 = 2
						for (row = 0; row < 8; row++){
							if (Y > row_blk + (row * 2)){
								for (col = 0; col < 8; col++){
									if (X > col_blk + col){
										if (component_id == 1)
											YCbCr_Cb[row_blk + (row * 2)][col_blk + col] = block[row][col];
										else
											YCbCr_Cr[row_blk + (row * 2)][col_blk + col] = block[row][col];
									}
								}
							}
						}
						break;
			}

	case 3: switch (component_id)
			{
			case 0:		row_blk = (mcu_y * 2 * 8) + (8 * vert_blk); //V1 = 2
						col_blk = (mcu_x * 2 * 8) + (8 * hori_blk); //H1 = 2
						for (row = 0; row < 8; row++){
							if (Y > row_blk + row){
								for (col = 0; col < 8; col++){
									if (X > col_blk + col){
										YCbCr_Y[row_blk + row][col_blk + col] = block[row][col];
									}
								}
							}
						}
						break;
			default:	row_blk = (mcu_y * 2 * 8) + (8 * vert_blk);			//V2 = V3 = 2
						col_blk = (mcu_x * 1 * 16);							//H2 = H3 = 1
						for (row = 0; row < 8; row++){
							if (Y > row_blk + row){
								for (col = 0; col < 8; col++){
									if (X > col_blk + (col * 2)){
										if (component_id == 1)
											YCbCr_Cb[row_blk + row][col_blk + (col * 2)] = block[row][col];
										else								 
											YCbCr_Cr[row_blk + row][col_blk + (col * 2)] = block[row][col];
									}
								}
							}
						}
						break;
			}

	case 4:
		switch (component_id)
			{
			case 0:		row_blk = (mcu_y * 2 * 8) + (8 * vert_blk); //V1 = 2
						col_blk = (mcu_x * 2 * 8) + (8 * hori_blk); //H1 = 2
						for (row = 0; row < 8; row++){
							if (Y > row_blk + row){
								for (col = 0; col < 8; col++){
									if (X > col_blk + col){
										YCbCr_Y[row_blk + row][col_blk + col] = block[row][col];
									}
								}
							}
						}
						break;
			default:	row_blk = (mcu_y * 2 * 8) + (8 * vert_blk);			//V2 = V3 = 2
						col_blk = (mcu_x * 2 * 8) + (8 * hori_blk);			//H2 = H3 = 2
						for (row = 0; row < 8; row++){
							if (Y > row_blk + (row * 2)){
								for (col = 0; col < 8; col++){
									if (X > col_blk + (col * 2)){
										if (component_id == 1)
											YCbCr_Cb[row_blk + (row * 2)][col_blk + (col * 2)] = block[row][col];
										else				   
											YCbCr_Cr[row_blk + (row * 2)][col_blk + (col * 2)] = block[row][col];
									}
								}
							}
						}
						break;
			}
	default: break;
	}
}


void upsampling_CbCr(unsigned char upsampling_type){
	int i, j;
	for (i = 0; i <  Y - 2; i = i + 2){
		for (j = 0; j < X - 2; j = j + 2){
			switch (upsampling_type)
			{
			case 2: YCbCr_Cb[i + 1][j] = (YCbCr_Cb[i][j] + YCbCr_Cb[i + 2][j]) / 2;// Cb component col upsampling
					YCbCr_Cr[i + 1][j] = (YCbCr_Cr[i][j] + YCbCr_Cr[i + 2][j]) / 2;// Cr component col upsampling
					j--; //to increment J one by one
					break;
			case 3: YCbCr_Cb[i][j + 1] = (YCbCr_Cb[i][j] + YCbCr_Cb[i][j + 2]) / 2;// Cb component row upsampling
					YCbCr_Cr[i][j + 1] = (YCbCr_Cr[i][j] + YCbCr_Cr[i][j + 2]) / 2;// Cr component row upsampling
					i--; //to increment I one by one
					break;													
			case 4: YCbCr_Cb[i][j + 1] = (YCbCr_Cb[i][j] + YCbCr_Cb[i][j + 2]) / 2;// Cb component row upsampling 
					YCbCr_Cb[i + 1][j] = (YCbCr_Cb[i][j] + YCbCr_Cb[i + 2][j]) / 2;// Cb component col upsampling
					YCbCr_Cr[i][j + 1] = (YCbCr_Cr[i][j] + YCbCr_Cr[i][j + 2]) / 2;// Cr component row upsampling
					YCbCr_Cr[i + 1][j] = (YCbCr_Cr[i][j] + YCbCr_Cr[i + 2][j]) / 2;// Cr component col upsampling
					//upsampling the middle 6x6 block
					YCbCr_Cb[i + 1][j + 1] = (YCbCr_Cb[i][j] + YCbCr_Cb[i][j + 2] + YCbCr_Cb[i + 2][j] + YCbCr_Cb[i + 2][j + 2]) / 4;
					YCbCr_Cr[i + 1][j + 1] = (YCbCr_Cr[i][j] + YCbCr_Cr[i][j + 2] + YCbCr_Cr[i + 2][j] + YCbCr_Cr[i + 2][j + 2]) / 4;
					break;
			default:
				break;
			}
		}
	}
	if (upsampling_type == 4){
		for (i = 0; i < Y - 2; i = i + 2){ //updating the (X - 1)th column
			YCbCr_Cb[i + 1][X - 2] = (YCbCr_Cb[i][X - 2] + YCbCr_Cb[i + 2][X - 2]) / 2;
			YCbCr_Cr[i + 1][X - 2] = (YCbCr_Cr[i][X - 2] + YCbCr_Cr[i + 2][X - 2]) / 2;
		}
		for (i = 0; i < X - 2; i = i + 2){ //updating the (Y - 1)th row
			YCbCr_Cb[Y - 2][i + 1] = (YCbCr_Cb[Y - 2][i] + YCbCr_Cb[Y - 2][i + 2]) / 2;
			YCbCr_Cr[Y - 2][i + 1] = (YCbCr_Cr[Y - 2][i] + YCbCr_Cr[Y - 2][i + 2]) / 2;
		}
	}
	if (X % 2 == 0) //Updating Last colum
		for (i = 0; i < Y; i = i + 1){ //updating the (X)th column
			YCbCr_Cb[i][X - 1] = YCbCr_Cb[i][X - 2];
			YCbCr_Cr[i][X - 1] = YCbCr_Cr[i][X - 2];
		}
	if (Y % 2 == 0) //Updating Last row
		for (i = 0; i < X; i = i + 1){ //updating the (Y)th row
			YCbCr_Cb[Y - 1][i] = YCbCr_Cb[Y - 2][i];
			YCbCr_Cr[Y - 1][i] = YCbCr_Cr[Y - 2][i];
		}
			
}


void upsampling_cbcr_integer(unsigned char comp){
	int upsample_val;
	for (row = 0; row < 8; row++){
		for (col = 0; col < 4; col++){
			upsample_val = integer_block[row][col];
			if (comp == 1)
				integer_block_upsampling_cb_1[row * 2][col * 2] = upsample_val;
			else
				integer_block_upsampling_cr_1[row * 2][col * 2] = upsample_val;
		}
		for (col = 0; col < 4; col++){
			upsample_val = integer_block[row][col + 4];
			if (comp == 1)
				integer_block_upsampling_cb_2[row * 2][col * 2] = upsample_val;
			else
				integer_block_upsampling_cr_2[row * 2][col * 2] = upsample_val;
		}
	}
	// Horizontal Upsampling
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			upsample_val = (integer_block[row][col] + integer_block[row][col + 1]) >> 1;
			if (comp == 1)
				integer_block_upsampling_cb_1[row * 2][col * 2 + 1] = upsample_val;
			else
				integer_block_upsampling_cr_1[row * 2][col * 2 + 1] = upsample_val;
		}
		for (col = 0; col < 3; col++){
			upsample_val = (integer_block[row][col + 4] + integer_block[row][col + 5]) >> 1;
			if (comp == 1)
				integer_block_upsampling_cb_2[row * 2][col * 2 + 1] = upsample_val;
			else
				integer_block_upsampling_cr_2[row * 2][col * 2 + 1] = upsample_val;
		}
		if (comp == 1)
			integer_block_upsampling_cb_2[row * 2][7] = integer_block[row][7];
		else
			integer_block_upsampling_cr_2[row * 2][7] = integer_block[row][7];
	}
	// Vertical Upsampling
	for (row = 1; row < 14; row=row+2){
		for (col = 0; col < 8; col++){
			if (comp == 1)
				integer_block_upsampling_cb_1[row][col] = (integer_block_upsampling_cb_1[row - 1][col] + integer_block_upsampling_cb_1[row + 1][col]) >> 1;
			else
				integer_block_upsampling_cr_1[row][col] = (integer_block_upsampling_cr_1[row - 1][col] + integer_block_upsampling_cr_1[row + 1][col]) >> 1;
		}
		for (col = 0; col < 8; col++){
			if (comp == 1)
				integer_block_upsampling_cb_2[row][col] = (integer_block_upsampling_cb_2[row - 1][col] + integer_block_upsampling_cb_2[row + 1][col]) >> 1;
			else
				integer_block_upsampling_cr_2[row][col] = (integer_block_upsampling_cr_2[row - 1][col] + integer_block_upsampling_cr_2[row + 1][col]) >> 1;
		}
	}
	for (col = 0; col < 8; col++){
		if (comp == 1){
			integer_block_upsampling_cb_1[15][col] = integer_block_upsampling_cb_1[14][col];
			integer_block_upsampling_cb_2[15][col] = integer_block_upsampling_cb_2[14][col];
		}
		else{
			integer_block_upsampling_cr_1[15][col] = integer_block_upsampling_cr_1[14][col];
			integer_block_upsampling_cr_2[15][col] = integer_block_upsampling_cr_2[14][col];
		}
	}
}

void color_transform_integer(int x, int y){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			integer_block_r[row][col] = integer_block_y[row][col] + ((((45941 * (integer_block_cr[row][col] - 128)) >> 14) + 1) >> 1);
			integer_block_g[row][col] = integer_block_y[row][col] - ((((11277 * (integer_block_cb[row][col] - 128)) >> 14) + 1) >> 1) - ((((23401 * (integer_block_cr[row][col] - 128)) >> 14) + 1) >> 1);
			integer_block_b[row][col] = integer_block_y[row][col] + ((((58065 * (integer_block_cb[row][col] - 128)) >> 14) + 1) >> 1);

			// R block Clamping
			if (integer_block_r[row][col] > 255)
				integer_block_r[row][col] = 255;
			else if (integer_block_r[row][col] < 0)
				integer_block_r[row][col] = 0;
			// G block Clamping
			if (integer_block_g[row][col] > 255)
				integer_block_g[row][col] = 255;
			else if (integer_block_g[row][col] < 0)
				integer_block_g[row][col] = 0;
			// B block Clamping
			if (integer_block_b[row][col] > 255)
				integer_block_b[row][col] = 255;
			else if (integer_block_b[row][col] < 0)
				integer_block_b[row][col] = 0;
		}
	}

	// Upload to the blocks to Image
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			integer_R[row + x * 8][col + y * 8] = integer_block_r[row][col];
			integer_G[row + x * 8][col + y * 8] = integer_block_g[row][col];
			integer_B[row + x * 8][col + y * 8] = integer_block_b[row][col];
		}
	}
}

void tone_mapping(int x, int y){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			integer_R[row + x * 8][col + y * 8] = tone_table[integer_block_r[row][col]] + resi_y[row + x * 8][col + y * 8];   //  tone_table[
			integer_G[row + x * 8][col + y * 8] = tone_table[integer_block_g[row][col]] + resi_cb[row + x * 8][col + y * 8];   //  tone_table[
			integer_B[row + x * 8][col + y * 8] = tone_table[integer_block_b[row][col]] + resi_cr[row + x * 8][col + y * 8];   //  tone_table[

			//fwrite(&integer_block_r[row][col], 1, 1, tone_map);
			//fwrite(&integer_block_g[row][col], 1, 1, tone_map);
			//fwrite(&integer_block_b[row][col], 1, 1, tone_map);
		}
	}
}

void resi_layer_transfer_element(int x, int y){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			resi_y[row + x * 8][col + y * 8] = integer_block_y[row][col];
			resi_cb[row + x * 8][col + y * 8] = integer_block_cb[row][col];
			resi_cr[row + x * 8][col + y * 8] = integer_block_cr[row][col];
		}
	}
}
