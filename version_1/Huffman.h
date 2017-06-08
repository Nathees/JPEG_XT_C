#ifndef HUFFMAN_H_
#define HUFFMAN_H_

// Lookup table for Huffman Decoder
extern unsigned char huffman_table_DC_1[65536][2]; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
extern unsigned char huffman_table_AC_1[65536][2];
extern unsigned char huffman_table_DC_2[65536][2]; //column 0, 1 & 3 are huff_code, huff_code_len and huff_value 
extern unsigned char huffman_table_AC_2[65536][2];


// Variables for difference between quantized DC and prediction
extern int DIFF_Y;		//Y DC element
extern int DIFF_Cb;		//Cb DC element
extern int DIFF_Cr;		//Cr DC element

// Variables involed in Generation of table of Huffman codes
extern unsigned char K;
extern int CODE;
extern unsigned char SI;
extern unsigned char Li;				//Li - No of huffman code length
extern unsigned char row;
extern unsigned char col;

// Variable for handling bitstream
extern unsigned int bitstream;
extern unsigned char bitstream_state;


void generate_huffcode(unsigned char table_destination_id);
void State1(unsigned char table_id);
void State2(void);
void update_intermediate_space(unsigned char table_id);


void Reset_Block(void);
void initial_load_bitstream();
void shift(unsigned char shift);

void Huffman_decode(unsigned char comp, unsigned char DC_table_id, unsigned char AC_table_id, unsigned char Quantization_table_id);
void DC_Huffman_decode(unsigned char comp, unsigned char table_id, unsigned char Quantization_table_id);
void AC_Huffman_decode(unsigned char table_id, unsigned char Quantization_table_id);

void calculate_value(void);


#endif /* HUFFMAN_H_ */