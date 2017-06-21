#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "idct.h"
#include "feig_idct.h"


//Intermediate variable initialization for Feig implementation
float FF[64];
float temp_FF[64];

// Variables Used in B1 operation
float a[64];
float temp1[8];
// Variables Used in M operation
const float C2 = 1.8477;
const float C4 = 1.4142;
const float C6 = 0.7653;
const float C4xC6 = 1.082287;
const float Q1 = 1.0824;	//Q1 = C2 - C6
const float R1 = 2.613;		//R1 = C2 + C6
const float Q2 = 1.53073;	//Q2 = C2.C4 - C4.C6
const float R2 = 3.6953;	//R2 = C2.C4 + C4.C6
float b[64];
float temp2;
float l[4], m[4];
// Variables Used in A1 operation
float c[64];
float temp3[6];
// Variables Used in A2 operation
float d[64];
float temp4[5];
// Variables Used in A2 operation
float temp5[8];

void feig_idct(void){

	// Transfer block elements to FF array
	for (row = 0; row < 8; row++)
		for (col = 0; col < 8; col++){
			#if INTEGER_OPERATION
			#else
				if(base_resi_layer == 0)
					FF[row * 8 + col] = base_float_block[row][col];
				else
					FF[row * 8 + col] = resi_float_block[row][col];
			#endif

		}

	B1_Operation();	//B1 kronecker matrix multiplication with low multiplication and addition
	M_operation();	//M kronecker matrix multiplication with low multiplication and addition
	A1_Operation(); //A1 kronecker matrix multiplication with low multiplication and addition
	A2_Operation(); //A2 kronecker matrix multiplication with low multiplication and addition
	A3_Operation(); //A3 kronecker matrix multiplication with low multiplication and addition

	// Transfer FF array elements to block 
	for (row = 0; row < 8; row++)
		for (col = 0; col < 8; col++){
			#if INTEGER_OPERATION
			#else
				if(base_resi_layer == 0)
					base_float_block[row][col] = FF[row * 8 + col] + 128;		//128 for level shifting
				else
					resi_float_block[row][col] = FF[row * 8 + col] + 128;		//128 for level shifting
			#endif
		}

}

void B1_Operation(void){

	//******************************************************row 1**************************************************************
	// Initialize temperary variable
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = FF[1] + FF[7];
	temp1[3] = 0;
	temp1[4] = FF[3] + FF[5];
	temp1[5] = 0;
	temp1[6] = 0;
	temp1[7] = 0;
	// row 1 operation
	a[0] = +FF[0];
	a[1] = +FF[4];
	a[2] = +FF[2] - FF[6];
	a[3] = +FF[2] + FF[6];
	a[4] = -FF[3] + FF[5];
	a[5] = temp1[2] - temp1[4];
	a[6] = +FF[1] - FF[7];
	a[7] = temp1[2] + temp1[4];
	//******************************************************row 2**************************************************************
	// Initialize temperary variable
	temp1[0] = 0;
	temp1[1] = 0;
	temp1[2] = FF[33] + FF[39];
	temp1[3] = 0;
	temp1[4] = FF[35] + FF[37];
	temp1[5] = 0;
	temp1[5] = 0;
	temp1[6] = 0;
	temp1[7] = 0;
	// row 2 operation
	a[8] = +FF[32];
	a[9] = +FF[36];
	a[10] = +FF[34] - FF[38];
	a[11] = +FF[34] + FF[38];
	a[12] = -FF[35] + FF[37];
	a[13] = temp1[2] - temp1[4];
	a[14] = +FF[33] - FF[39];
	a[15] = temp1[2] + temp1[4];
	//******************************************************row 3**************************************************************
	// Initialize temperary variable
	temp1[0] = FF[18] - FF[50];
	temp1[1] = FF[22] - FF[54];
	temp1[2] = FF[17] - FF[49];
	temp1[3] = FF[23] - FF[55];
	temp1[4] = FF[19] - FF[51];
	temp1[5] = FF[21] - FF[53];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 3 operation
	a[16] = +FF[16] - FF[48];
	a[17] = +FF[20] - FF[52];
	a[18] = temp1[0] - temp1[1];
	a[19] = temp1[0] + temp1[1];
	a[20] = -temp1[4] + temp1[5];
	a[21] = temp1[6] - temp1[7];
	a[22] = temp1[2] - temp1[3];
	a[23] = temp1[6] + temp1[7];
	//******************************************************row 4**************************************************************
	// Initialize temperary variable
	temp1[0] = FF[18] + FF[50];
	temp1[1] = FF[22] + FF[54];
	temp1[2] = FF[17] + FF[49];
	temp1[3] = FF[23] + FF[55];
	temp1[4] = FF[19] + FF[51];
	temp1[5] = FF[21] + FF[53];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 4 operation
	a[24] = +FF[16] + FF[48];
	a[25] = +FF[20] + FF[52];
	a[26] = temp1[0] - temp1[1];
	a[27] = temp1[0] + temp1[1];
	a[28] = -temp1[4] + temp1[5];
	a[29] = temp1[6] - temp1[7];
	a[30] = temp1[2] - temp1[3];
	a[31] = temp1[6] + temp1[7];
	//******************************************************row 5**************************************************************
	// Initialize temperary variable
	temp1[0] = -FF[26] + FF[42];
	temp1[1] = FF[46] - FF[30];
	temp1[2] = -FF[25] + FF[41];
	temp1[3] = -FF[31] + FF[47];
	temp1[4] = FF[43] - FF[27];
	temp1[5] = FF[45] - FF[29];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 5 operation
	a[32] = -FF[24] + FF[40];
	a[33] = -FF[28] + FF[44];
	a[34] = temp1[0] - temp1[1];
	a[35] = temp1[0] + temp1[1];
	a[36] = -temp1[4] + temp1[5];
	a[37] = temp1[6] - temp1[7];
	a[38] = temp1[2] - temp1[3];
	a[39] = temp1[6] + temp1[7];
	//******************************************************row 6**************************************************************
	// Initialize temperary variable
	temp1[0] = FF[10] - FF[26] - FF[42] + FF[58];
	temp1[1] = FF[14] - FF[30] - FF[46] + FF[62];
	temp1[2] = FF[9] - FF[25] - FF[41] + FF[57];
	temp1[3] = FF[15] - FF[31] - FF[47] + FF[63];
	temp1[4] = FF[11] - FF[27] - FF[43] + FF[59];
	temp1[5] = FF[13] - FF[29] - FF[45] + FF[61];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 6 operation
	a[40] = +FF[8] - FF[24] - FF[40] + FF[56];
	a[41] = +FF[12] - FF[28] - FF[44] + FF[60];
	a[42] = temp1[0] - temp1[1];
	a[43] = temp1[0] + temp1[1];
	a[44] = -temp1[4] + temp1[5];
	a[45] = temp1[6] - temp1[7];
	a[46] = temp1[2] - temp1[3];
	a[47] = temp1[6] + temp1[7];
	//******************************************************row 7**************************************************************
	// Initialize temperary variable
	temp1[0] = FF[10] - FF[58];
	temp1[1] = FF[14] - FF[62];
	temp1[2] = FF[9] - FF[57];
	temp1[3] = FF[15] - FF[63];
	temp1[4] = FF[11] - FF[59];
	temp1[5] = FF[13] - FF[61];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 7 operation
	a[48] = +FF[8] - FF[56];
	a[49] = +FF[12] - FF[60];
	a[50] = temp1[0] - temp1[1];
	a[51] = temp1[0] + temp1[1];
	a[52] = -temp1[4] + temp1[5];
	a[53] = temp1[6] - temp1[7];
	a[54] = temp1[2] - temp1[3];
	a[55] = temp1[6] + temp1[7];
	//******************************************************row 8**************************************************************
	// Initialize temperary variable
	temp1[0] = FF[10] + FF[26] + FF[42] + FF[58];
	temp1[1] = FF[14] + FF[30] + FF[46] + FF[62];
	temp1[2] = FF[9] + FF[25] + FF[41] + FF[57];
	temp1[3] = FF[15] + FF[31] + FF[47] + FF[63];
	temp1[4] = FF[11] + FF[27] + FF[43] + FF[59];
	temp1[5] = FF[13] + FF[29] + FF[45] + FF[61];
	temp1[6] = temp1[2] + temp1[3];
	temp1[7] = temp1[4] + temp1[5];
	// row 8 operation
	a[56] = +FF[8] + FF[24] + FF[40] + FF[56];
	a[57] = +FF[12] + FF[28] + FF[44] + FF[60];
	a[58] = temp1[0] - temp1[1];
	a[59] = temp1[0] + temp1[1];
	a[60] = -temp1[4] + temp1[5];
	a[61] = temp1[6] - temp1[7];
	a[62] = temp1[2] - temp1[3];
	a[63] = temp1[6] + temp1[7];
}

void M_operation(void){
	//******************************************************row 1**************************************************************
	temp2 = C6 * (a[4] + a[6]);
	b[0] = +a[0];
	b[1] = +a[1];
	b[2] = +C4 * a[2];
	b[3] = +a[3];
	b[4] = -Q1 * a[4] - temp2;
	b[5] = +C4 * a[5];
	b[6] = +R1 * a[6] - temp2;
	b[7] = +a[7];
	//******************************************************row 2**************************************************************
	temp2 = C6 * (a[12] + a[14]);
	b[8] = +a[8];
	b[9] = +a[9];
	b[10] = +C4 * a[10];
	b[11] = +a[11];
	b[12] = -Q1 * a[12] - temp2;
	b[13] = +C4 * a[13];
	b[14] = +R1 * a[14] - temp2;
	b[15] = +a[15];
	//******************************************************row 3**************************************************************
	temp2 = C4xC6 * (a[20] + a[22]);
	b[16] = +C4* a[16];
	b[17] = +C4* a[17];
	b[18] = a[18] * 2;
	b[19] = +C4 * a[19];
	b[20] = -Q2 * a[20] - temp2;
	b[21] = a[21] * 2;
	b[22] = +R2 * a[22] - temp2;
	b[23] = +C4 * a[23];
	//******************************************************row 4**************************************************************
	temp2 = C6 * (a[28] + a[30]);
	b[24] = +a[24];
	b[25] = +a[25];
	b[26] = +C4 * a[26];
	b[27] = +a[27];
	b[28] = -Q1 * a[28] - temp2;
	b[29] = +C4 * a[29];
	b[30] = +R1 * a[30] - temp2;
	b[31] = +a[31];
	//******************************************************row 6**************************************************************
	temp2 = C4xC6 * (a[44] + a[46]);
	b[40] = +C4 * a[40];
	b[41] = +C4 * a[41];
	b[42] = a[42] * 2;
	b[43] = +C4 * a[43];
	b[44] = -Q2 * a[44] - temp2;
	b[45] = a[45] * 2;
	b[46] = +R2 * a[46] - temp2;
	b[47] = +C4 * a[47];
	//******************************************************row 8**************************************************************
	temp2 = C6 * (a[60] + a[62]);
	b[56] = +a[56];
	b[57] = +a[57];
	b[58] = +C4 * a[58];
	b[59] = +a[59];
	b[60] = -Q1 * a[60] - temp2;
	b[61] = +C4 * a[61];
	b[62] = +R1 * a[62] - temp2;
	b[63] = +a[63];
	//******************************************************rows 5 and 7**************************************************************
	temp2 = C6 * (a[32] + a[48]);
	b[32] = -Q1 * a[32] - temp2;
	b[48] = +R1 * a[48] - temp2;
	temp2 = C6 * (a[33] + a[49]);
	b[33] = -Q1 * a[33] - temp2;
	b[49] = +R1 * a[49] - temp2;
	temp2 = C4xC6 * (a[34] + a[50]);
	b[34] = -Q2 * a[34] - temp2;
	b[50] = +R2 * a[50] - temp2;
	temp2 = C6 * (a[35] + a[51]);
	b[35] = -Q1 * a[35] - temp2;
	b[51] = +R1 * a[51] - temp2;
	temp2 = C4xC6 * (a[37] + a[53]);
	b[37] = -Q2 * a[37] - temp2;
	b[53] = +R2 * a[53] - temp2;
	temp2 = C6 * (a[39] + a[55]);
	b[39] = -Q1 * a[39] - temp2;
	b[55] = +R1 * a[55] - temp2;

	l[0] = +a[36] - a[54];
	l[1] = +a[38] + a[52];
	l[2] = +a[36] + a[54];
	l[3] = -a[38] + a[52];
	m[0] = C4 * (l[0] + l[1]);
	m[1] = C4 * (l[0] - l[1]);
	m[2] = l[2] * 2;
	m[3] = l[3] * 2;
	b[36] = +m[0] + m[2];
	b[38] = +m[1] + m[3];
	b[52] = +m[1] - m[3];
	b[54] = -m[0] + m[2];
}

void A1_Operation(void){
	//******************************************************row 1**************************************************************
	// Initialize temperary variable
	temp3[0] = -b[46] + b[54] - b[62];
	temp3[1] = +b[47] - b[55] + b[63];
	temp3[2] = -b[40] + b[48] - b[56];
	temp3[3] = +b[41] - b[49] + b[57];
	temp3[4] = -b[43] + b[51] - b[59];
	temp3[5] = temp3[0] + temp3[1];
	// row 1 operation
	c[0] = temp3[5] + b[45] - b[53] + b[61];
	c[1] = temp3[2] + temp3[3];
	c[2] = -temp3[4] - b[42] + b[50] - b[58];
	c[3] = temp3[2] - temp3[3];
	c[4] = temp3[5];
	c[5] = -b[44] + b[52] - b[60];
	c[6] = temp3[4];
	c[7] = -temp3[1];
	//******************************************************row 2**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[6] - b[14];
	temp3[1] = -b[7] + b[15];
	temp3[2] = +b[0] - b[8];
	temp3[3] = -b[1] + b[9];
	temp3[4] = +b[3] - b[11];
	temp3[5] = temp3[0] + temp3[1];
	// row 2 operation
	c[8] = temp3[5] - b[5] + b[13];
	c[9] = temp3[2] + temp3[3];
	c[10] = -temp3[4] + b[2] - b[10];
	c[11] = temp3[2] - temp3[3];
	c[12] = temp3[5];
	c[13] = +b[4] - b[12];
	c[14] = temp3[4];
	c[15] = -temp3[1];
	//******************************************************row 3**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[22] - b[30];
	temp3[1] = -b[23] + b[31];
	temp3[2] = +b[16] - b[24];
	temp3[3] = -b[17] + b[25];
	temp3[4] = +b[19] - b[27];
	temp3[5] = temp3[0] + temp3[1];
	// row 3 operation
	c[16] = temp3[5] - b[21] + b[29];
	c[17] = temp3[2] + temp3[3];
	c[18] = -temp3[4] + b[18] - b[26];
	c[19] = temp3[2] - temp3[3];
	c[20] = temp3[5];
	c[21] = +b[20] - b[28];
	c[22] = temp3[4];
	c[23] = -temp3[1];
	//******************************************************row 4**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[6] + b[14];
	temp3[1] = -b[7] - b[15];
	temp3[2] = +b[0] + b[8];
	temp3[3] = -b[1] - b[9];
	temp3[4] = +b[3] + b[11];
	temp3[5] = temp3[0] + temp3[1];
	// row 4 operation
	c[24] = temp3[5] - b[5] - b[13];
	c[25] = temp3[2] + temp3[3];
	c[26] = -temp3[4] + b[2] + b[10];
	c[27] = temp3[2] - temp3[3];
	c[28] = temp3[5];
	c[29] = +b[4] + b[12];
	c[30] = temp3[4];
	c[31] = -temp3[1];
	//******************************************************row 5**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[54] - b[62];
	temp3[1] = -b[55] + b[63];
	temp3[2] = +b[48] - b[56];
	temp3[3] = -b[49] + b[57];
	temp3[4] = +b[51] - b[59];
	temp3[5] = temp3[0] + temp3[1];
	// row 5 operation
	c[32] = temp3[5] - b[53] + b[61];
	c[33] = +temp3[2] + temp3[3];
	c[34] = -temp3[4] + b[50] - b[58];
	c[35] = temp3[2] - temp3[3];
	c[36] = temp3[5];
	c[37] = +b[52] - b[60];
	c[38] = temp3[4];
	c[39] = -temp3[1];
	//******************************************************row 6**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[38] - b[39];
	/*temp3[1] = 0;
	temp3[2] = 0;
	temp3[3] = 0;
	temp3[4] = 0;
	temp3[5] = 0;*/
	// row 6 operation
	c[40] = temp3[0] - b[37];
	c[41] = +b[32] - b[33];
	c[42] = +b[34] - b[35];
	c[43] = +b[32] + b[33];
	c[44] = temp3[0];
	c[45] = +b[36];
	c[46] = +b[35];
	c[47] = +b[39];
	//******************************************************row 7**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[30] - b[31];
	/*temp3[1] = 0;
	temp3[2] = 0;
	temp3[3] = 0;
	temp3[4] = 0;
	temp3[5] = 0;*/
	// row 7 operation
	c[48] = temp3[0] - b[29];
	c[49] = +b[24] - b[25];
	c[50] = +b[26] - b[27];
	c[51] = +b[24] + b[25];
	c[52] = temp3[0];
	c[53] = +b[28];
	c[54] = +b[27];
	c[55] = +b[31];
	//******************************************************row 8**************************************************************
	// Initialize temperary variable
	temp3[0] = +b[62] - b[63];
	/*temp3[1] = 0;
	temp3[2] = 0;
	temp3[3] = 0;
	temp3[4] = 0;
	temp3[5] = 0;*/
	// row 8 operation
	c[56] = temp3[0] - b[61];
	c[57] = +b[56] - b[57];
	c[58] = +b[58] - b[59];
	c[59] = +b[56] + b[57];
	c[60] = temp3[0];
	c[61] = +b[60];
	c[62] = +b[59];
	c[63] = +b[63];
}

void A2_Operation(void){
	//******************************************************row 1**************************************************************
	d[0] = +c[63];
	d[1] = +c[56];
	d[2] = +c[60];
	d[3] = +c[57] + c[58];
	d[4] = +c[59] + c[62];
	d[5] = +c[57] - c[58];
	d[6] = +c[59] - c[62];
	d[7] = -c[56] + c[61];
	//******************************************************row 2**************************************************************
	d[8] = +c[7];
	d[9] = +c[0];
	d[10] = +c[4];
	d[11] = +c[1] + c[2];
	d[12] = +c[3] + c[6];
	d[13] = +c[1] - c[2];
	d[14] = +c[3] - c[6];
	d[15] = -c[0] + c[5];
	//******************************************************row 3**************************************************************
	d[16] = +c[39];
	d[17] = +c[32];
	d[18] = +c[36];
	d[19] = +c[33] + c[34];
	d[20] = +c[35] + c[38];
	d[21] = +c[33] - c[34];
	d[22] = +c[35] - c[38];
	d[23] = -c[32] + c[37];
	//******************************************************row 4**************************************************************
	// Initialize temperary variable
	temp4[0] = +c[9] + c[17];
	temp4[1] = +c[10] + c[18];
	temp4[2] = +c[11] + c[19];
	temp4[3] = -c[14] - c[22];
	temp4[4] = +c[8] + c[16];
	// row 4 operation
	d[24] = +c[15] + c[23];
	d[25] = temp4[4];
	d[26] = +c[12] + c[20];
	d[27] = temp4[0] + temp4[1];
	d[28] = temp4[2] - temp4[3];
	d[29] = temp4[0] - temp4[1];
	d[30] = temp4[2] + temp4[3];
	d[31] = -temp4[4] + c[13] + c[21];
	//******************************************************row 5**************************************************************
	// Initialize temperary variable
	temp4[0] = +c[25] + c[49];
	temp4[1] = +c[26] + c[50];
	temp4[2] = +c[27] + c[51];
	temp4[3] = +c[30] + c[54];
	temp4[4] = +c[24] + c[48];
	// row 5 operation
	d[32] = +c[31] + c[55];
	d[33] = temp4[4];
	d[34] = +c[28] + c[52];
	d[35] = temp4[0] + temp4[1];
	d[36] = temp4[2] + temp4[3];
	d[37] = temp4[0] - temp4[1];
	d[38] = temp4[2] - temp4[3];
	d[39] = -temp4[4] + c[29] + c[53];
	//******************************************************row 6**************************************************************
	// Initialize temperary variable
	temp4[0] = +c[9] - c[17];
	temp4[1] = +c[10] - c[18];
	temp4[2] = +c[11] - c[19];
	temp4[3] = +c[14] - c[22];
	temp4[4] = +c[8] - c[16];
	// row 6 operation
	d[40] = +c[15] - c[23];
	d[41] = temp4[4];
	d[42] = +c[12] - c[20];
	d[43] = temp4[0] + temp4[1];
	d[44] = temp4[2] + temp4[3];
	d[45] = temp4[0] - temp4[1];
	d[46] = temp4[2] - temp4[3];
	d[47] = -temp4[4] + c[13] - c[21];
	//******************************************************row 7**************************************************************
	// Initialize temperary variable.
	temp4[0] = +c[25] - c[49];
	temp4[1] = +c[26] - c[50];
	temp4[2] = +c[27] - c[51];
	temp4[3] = +c[30] - c[54];
	temp4[4] = +c[24] - c[48];
	// row 7 operation
	d[48] = +c[31] - c[55];
	d[49] = temp4[4];
	d[50] = +c[28] - c[52];
	d[51] = temp4[0] + temp4[1];
	d[52] = temp4[2] + temp4[3];
	d[53] = temp4[0] - temp4[1];
	d[54] = temp4[2] - temp4[3];
	d[55] = -temp4[4] + c[29] - c[53];
	//******************************************************row 8**************************************************************
	// Initialize temperary variable.
	temp4[0] = -c[1] + c[41];
	temp4[1] = -c[2] + c[42];
	temp4[2] = -c[3] + c[43];
	temp4[3] = -c[6] + c[46];
	temp4[4] = -c[0] + c[40];
	// row 8 operation
	d[56] = -c[7] + c[47];
	d[57] = temp4[4];
	d[58] = -c[4] + c[44];
	d[59] = temp4[0] + temp4[1];
	d[60] = temp4[2] + temp4[3];
	d[61] = temp4[0] - temp4[1];
	d[62] = temp4[2] - temp4[3];
	d[63] = -temp4[4] - c[5] + c[45];
}

void A3_Operation(void){
	//******************************************************row 1**************************************************************
	// Initialize temperary variable
	temp5[0] = +d[4] + d[36];
	temp5[1] = +d[0] + d[32];
	temp5[2] = +d[3] + d[35];
	temp5[3] = +d[2] + d[34];
	temp5[4] = +d[5] + d[37];
	temp5[5] = -d[1] - d[33];
	temp5[6] = +d[6] + d[38];
	temp5[7] = -d[7] - d[39];
	// row 1 operation
	FF[0] = temp5[0] + temp5[1];
	FF[1] = temp5[2] + temp5[3];
	FF[2] = temp5[4] + temp5[5];
	FF[3] = temp5[6] + temp5[7];
	FF[4] = temp5[6] - temp5[7];
	FF[5] = temp5[4] - temp5[5];
	FF[6] = temp5[2] - temp5[3];
	FF[7] = temp5[0] - temp5[1];
	//******************************************************row 2**************************************************************
	// Initialize temperary variable
	temp5[0] = +d[20] + d[28];
	temp5[1] = +d[16] + d[24];
	temp5[2] = +d[19] + d[27];
	temp5[3] = +d[18] + d[26];
	temp5[4] = +d[21] + d[29];
	temp5[5] = -d[17] - d[25];
	temp5[6] = +d[22] + d[30];
	temp5[7] = -d[23] - d[31];
	// row 2 operation
	FF[8] = temp5[0] + temp5[1];
	FF[9] = temp5[2] + temp5[3];
	FF[10] = temp5[4] + temp5[5];
	FF[11] = temp5[6] + temp5[7];
	FF[12] = temp5[6] - temp5[7];
	FF[13] = temp5[4] - temp5[5];
	FF[14] = temp5[2] - temp5[3];
	FF[15] = temp5[0] - temp5[1];
	//******************************************************row 3**************************************************************
	// Initialize temperary variable
	temp5[0] = -d[12] + d[44];
	temp5[1] = -d[8] + d[40];
	temp5[2] = -d[11] + d[43];
	temp5[3] = -d[10] + d[42];
	temp5[4] = -d[13] + d[45];
	temp5[5] = +d[9] - d[41];
	temp5[6] = -d[14] + d[46];
	temp5[7] = +d[15] - d[47];
	// row 3 operation
	FF[16] = temp5[0] + temp5[1];
	FF[17] = temp5[2] + temp5[3];
	FF[18] = temp5[4] + temp5[5];
	FF[19] = temp5[6] + temp5[7];
	FF[20] = temp5[6] - temp5[7];
	FF[21] = temp5[4] - temp5[5];
	FF[22] = temp5[2] - temp5[3];
	FF[23] = temp5[0] - temp5[1];
	//******************************************************row 4**************************************************************
	// Initialize temperary variable
	temp5[0] = +d[52] - d[60];
	temp5[1] = +d[48] - d[56];
	temp5[2] = +d[51] - d[59];
	temp5[3] = +d[50] - d[58];
	temp5[4] = +d[53] - d[61];
	temp5[5] = -d[49] + d[57];
	temp5[6] = +d[54] - d[62];
	temp5[7] = -d[55] + d[63];
	// row 4 operation
	FF[24] = temp5[0] + temp5[1];
	FF[25] = temp5[2] + temp5[3];
	FF[26] = temp5[4] + temp5[5];
	FF[27] = temp5[6] + temp5[7];
	FF[28] = temp5[6] - temp5[7];
	FF[29] = temp5[4] - temp5[5];
	FF[30] = temp5[2] - temp5[3];
	FF[31] = temp5[0] - temp5[1];
	//******************************************************row 5**************************************************************
	// Initialize temperary variable
	temp5[0] = +d[52] + d[60];
	temp5[1] = +d[48] + d[56];
	temp5[2] = +d[51] + d[59];
	temp5[3] = +d[50] + d[58];
	temp5[4] = +d[53] + d[61];
	temp5[5] = -d[49] - d[57];
	temp5[6] = +d[54] + d[62];
	temp5[7] = -d[55] - d[63];
	// row 5 operation
	FF[32] = temp5[0] + temp5[1];
	FF[33] = temp5[2] + temp5[3];
	FF[34] = temp5[4] + temp5[5];
	FF[35] = temp5[6] + temp5[7];
	FF[36] = temp5[6] - temp5[7];
	FF[37] = temp5[4] - temp5[5];
	FF[38] = temp5[2] - temp5[3];
	FF[39] = temp5[0] - temp5[1];
	//******************************************************row 6**************************************************************
	// Initialize temperary variable
	temp5[0] = +d[12] + d[44];
	temp5[1] = +d[8] + d[40];
	temp5[2] = +d[11] + d[43];
	temp5[3] = +d[10] + d[42];
	temp5[4] = +d[13] + d[45];
	temp5[5] = -d[9] - d[41];
	temp5[6] = +d[14] + d[46];
	temp5[7] = -d[15] - d[47];
	// row 6 operation
	FF[40] = temp5[0] + temp5[1];
	FF[41] = temp5[2] + temp5[3];
	FF[42] = temp5[4] + temp5[5];
	FF[43] = temp5[6] + temp5[7];
	FF[44] = temp5[6] - temp5[7];
	FF[45] = temp5[4] - temp5[5];
	FF[46] = temp5[2] - temp5[3];
	FF[47] = temp5[0] - temp5[1];
	//******************************************************row 7**************************************************************
	// Initialize temperary variable
	temp5[0] = -d[20] + d[28];
	temp5[1] = -d[16] + d[24];
	temp5[2] = -d[19] + d[27];
	temp5[3] = -d[18] + d[26];
	temp5[4] = -d[21] + d[29];
	temp5[5] = +d[17] - d[25];
	temp5[6] = -d[22] + d[30];
	temp5[7] = +d[23] - d[31];
	// row 7 operation
	FF[48] = temp5[0] + temp5[1];
	FF[49] = temp5[2] + temp5[3];
	FF[50] = temp5[4] + temp5[5];
	FF[51] = temp5[6] + temp5[7];
	FF[52] = temp5[6] - temp5[7];
	FF[53] = temp5[4] - temp5[5];
	FF[54] = temp5[2] - temp5[3];
	FF[55] = temp5[0] - temp5[1];
	//******************************************************row 8**************************************************************
	// Initialize temperary variable
	temp5[0] = -d[4] + d[36];
	temp5[1] = -d[0] + d[32];
	temp5[2] = -d[3] + d[35];
	temp5[3] = -d[2] + d[34];
	temp5[4] = -d[5] + d[37];
	temp5[5] = +d[1] - d[33];
	temp5[6] = -d[6] + d[38];
	temp5[7] = +d[7] - d[39];
	// row 8 operation
	FF[56] = temp5[0] + temp5[1];
	FF[57] = temp5[2] + temp5[3];
	FF[58] = temp5[4] + temp5[5];
	FF[59] = temp5[6] + temp5[7];
	FF[60] = temp5[6] - temp5[7];
	FF[61] = temp5[4] - temp5[5];
	FF[62] = temp5[2] - temp5[3];
	FF[63] = temp5[0] - temp5[1];
}