#ifndef JPEG_XT_DECODER_H_
#define JPEG_XT_DECODER_H_


//  ******************************** Extern Variables Decleration ********************************

// **************** Table Memory Decleration ****************
// Base Quantization Table
extern float base_float_quantization_table[64][2];
extern int 	base_int_quantization_table[64][2];
// Residual Quantization Table
extern float resi_float_quantization_table[64][2];
extern int 	resi_int_quantization_table[64][2];

// Base Huffman Table
extern unsigned char base_huffman_table_DC_1[65536][2]; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
extern unsigned char base_huffman_table_AC_1[65536][2];
extern unsigned char base_huffman_table_DC_2[65536][2]; 
extern unsigned char base_huffman_table_AC_2[65536][2];
// Residual Huffman Table
extern unsigned char resi_huffman_table_DC_1[65536][2]; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
extern unsigned char resi_huffman_table_AC_1[65536][2];
extern unsigned char resi_huffman_table_DC_2[65536][2]; 
extern unsigned char resi_huffman_table_AC_2[65536][2];

// TONE table
extern int tone_table[256];


// ************** Controlling Image Array Control Variables **************
extern unsigned char *buffer;		// Contains the all image
extern unsigned char *buffer_resi;  // Contains the Residual layer image
extern int buff_index;
extern int index_resi;
extern unsigned char byte_file;

// ************** Image Component Specific Parameters  **************
// Common Component Specific Parameters
extern unsigned char Nf_Ns;
// Base Layer Component Specific Parameters
extern unsigned char base_hori_samp_factor[3]; 	
extern unsigned char base_vert_samp_factor[3];
extern unsigned char base_dqt_id[3];
extern unsigned char base_huff_dc_id[3];
extern unsigned char base_huff_ac_id[3];
// Residual Layer Component Specific Parameters
extern unsigned char resi_hori_samp_factor[3];
extern unsigned char resi_vert_samp_factor[3];
extern unsigned char resi_dqt_id[3];
extern unsigned char resi_huff_dc_id[3];
extern unsigned char resi_huff_ac_id[3];

// ************** Image Size Control Variables  **************
extern int img_height;
extern int img_width;

// ************** Zig-Zag Control Variable ********************
extern const unsigned char Zig_Zag[64];

// **************  8 x 8 Block Decleraion    ******************
extern int base_int_block[8][8];
extern float base_float_block[8][8];
extern int resi_int_block[8][8];
extern float resi_float_block[8][8];

// ************** Common Control Variables ********************
extern unsigned char row, col;

#endif /* JPEG_XT_DECODER_H_ */