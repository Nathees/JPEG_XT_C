#ifndef MARKER_PARSER_H_
#define MARKER_PARSER_H_

//  ******************************** Extern Variables Decleration ********************************

// **************** Table Memory Decleration ****************
// Base Quantization Table
extern float base_float_quantization_table[64][2];
extern int 	base_int_quantization_table[64][2];
// Residual Quantization Table
extern float resi_float_quantization_table[64][2];
extern int 	resi_int_quantization_table[64][2];

// Base Huffman Table
extern unsigned char base_huffman_table_DC_1[65536][2] = { { 0 } }; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
extern unsigned char base_huffman_table_AC_1[65536][2] = { { 0 } };
extern unsigned char base_huffman_table_DC_2[65536][2] = { { 0 } }; 
extern unsigned char base_huffman_table_AC_2[65536][2] = { { 0 } };
// Residual Huffman Table
extern unsigned char resi_huffman_table_DC_1[65536][2] = { { 0 } }; //column 0 - HUFF_SIZE, 1 - HUFF_VALUE 
extern unsigned char resi_huffman_table_AC_1[65536][2] = { { 0 } };
extern unsigned char resi_huffman_table_DC_2[65536][2] = { { 0 } }; 
extern unsigned char resi_huffman_table_AC_2[65536][2] = { { 0 } };

// TONE table
extern int tone_table[256];



// ******************************** Functions Decleration ********************************

// Main Marker Parser Operation
void marker_parser(void);

// APP11 Marker Decoder
void APP11_process(void);

// Residual Marker Parser Operation
void residual_marker_parser(void);

// Skip Marker Process
void skip_marker(void);

// Loading Residual Layer Scan bitstream to Residual Buffer
void get_residual_layer_bitstream(void);

#endif /* MARKER_PARSER_H_ */