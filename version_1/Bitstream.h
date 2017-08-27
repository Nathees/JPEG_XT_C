#ifndef BITSTREAM_H_
#define BITSTREAM_H_



extern int huffcode_AC_identifier[16][2];	//this array contains the index of first element in each size of the above AC 1 & 2
extern int huffcode_DC_identifier[16][2];	//this array contains the index of first element in each size of the above DC 1 & 2

extern unsigned char huff_size[256];

extern unsigned char quantization_table[64][4];
extern unsigned char resi_quantization_table[64][4];

//unsigned char no_of_huff_code_len_i[16];				//Array for store the number of huffman code length of each i

extern int tone_table[256];

extern int X;				// No of samples in a line of image
extern int Y;				// No of lines in a image
extern unsigned char Nf;	// No of components in the image

extern int app11_len;	// Handling APP11 Length 

extern unsigned char comp_specific_param[3][6];	//Component specification parameter 
//[0]-component Identifier, [1]-H, [2]-V,[3]-Quantization tble identifier, [4]-DC identifier, [5]-AC identifier  


void Baseline_Frame(void);
void Extended_Frame(void);
void Progressive_Frame(void);

void Huffman();
void Quantization();

void Scan();

extern unsigned char resi_tbox_flag;

void APP11_process();
void tone_mapping_process();
void resi_tbox_process();
void create_dynamic_array_resi_layer();
void get_residual_layer_bitstream();

void remove_00_byte(char *error);

#endif /* BITSTREAM_H_ */