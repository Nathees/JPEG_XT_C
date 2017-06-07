#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>

extern unsigned char byte_file;
extern unsigned char marker_id;

extern unsigned char *buffer;		// Contains the all image
extern unsigned char *buffer_resi;

extern long count;			// control the buffer
extern long count_resi;
extern int marker_len;

extern int MCU_X, MCU_Y; //MCUs for X and Y


/*extern int RGB_R[8000][8000];
extern int RGB_G[8000][8000];
extern int RGB_B[8000][8000];*/

extern int integer_R[8000][8000];
extern int integer_G[8000][8000];
extern int integer_B[8000][8000];

extern int resi_y[8000][8000];
extern int resi_cb[8000][8000];
extern int resi_cr[8000][8000];

extern int integer_block_upsampling_cb_1[16][8];
extern int integer_block_upsampling_cb_2[16][8];
extern int integer_block_upsampling_cr_1[16][8];
extern int integer_block_upsampling_cr_2[16][8];

extern int integer_block_y[8][8];
extern int integer_block_cb[8][8];
extern int integer_block_cr[8][8];

extern int integer_block_r[8][8];
extern int integer_block_g[8][8];
extern int integer_block_b[8][8];

// Tempary files for debuging
//FILE *fp1;
//FILE *fp2;
//FILE *fp3;
//FILE *fp3;

											//0     1     2     3     4     5     6     7
extern const unsigned char Zig_Zag[64];/* = { 0x00, 0x10, 0x01, 0x02, 0x11, 0x20, 0x30, 0x21,   //0
											0x12, 0x03, 0x04, 0x13, 0x22, 0x31, 0x40, 0x50,   //1
											0x41, 0x32, 0x23, 0x14, 0x05, 0x06, 0x15, 0x24,	  //2
											0x33, 0x42, 0x51, 0x60, 0x70, 0x61, 0x52, 0x43,	  //3
											0x34, 0x25, 0x16, 0x07, 0x17, 0x26, 0x35, 0x44,	  //4
											0x53, 0x62, 0x71, 0x72, 0x63, 0x54, 0x45, 0x36,	  //5
											0x27, 0x37, 0x46, 0x55, 0x64, 0x73, 0x74, 0x65,	  //6
											0x56, 0x47, 0x57, 0x66, 0x75, 0x76, 0x67, 0x77 }; //7*/

// Declaring block variables
extern float block[8][8];
extern int integer_block[8][8];
extern float block_temp[8][8];

//unsigned char read_nxt_byte();

void create_dynamic_array_RGB(void);
void YCbCr_to_RGB(void);

// Residual Decoding Process
void residual_decode_process();
float sixteento32float(int x);

// Debuging method
void File_print(int mcu_y, int mcu_x, unsigned char comp, unsigned char v, unsigned char h);

//FILE *tone_map;

void skip_byte(void);
void copy_file_to_memory(void);

#endif /* MAIN_H_ */