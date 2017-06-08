#ifndef UPSAMPLING_H_
#define UPSAMPLING_H_

extern float **YCbCr_Y;
extern float **YCbCr_Cb;
extern float **YCbCr_Cr;

//Dynamic array for Y, Cb and Cr components
void create_dynamic_array_YCbCr();

// void identify upsampling type in the transfer_element function
unsigned char identify_upsampling_type(void);

// Transfer (8x8) block elements to TCbCr 2D arry
void transfer_element(unsigned char upsampling_type, int mcu_y, int mcu_x, unsigned char component_id, unsigned char vert_blk, unsigned char hori_blk);

void upsampling_CbCr(unsigned char upsampling_type);

void upsampling_cbcr_integer(unsigned char comp);
void color_transform_integer(int x, int y);

void tone_mapping(int x, int y);

void resi_layer_transfer_element(int x, int y);

#endif /* UPSAMPLING_H_ */