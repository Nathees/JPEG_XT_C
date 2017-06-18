#ifndef FAST_IDCT_H_
#define FAST_IDCT_H_

// Fast INT IDCT Function Declerations
void fast_int_idct(void);
void block_to_temp_data_transfer(void);
void temp_to_block_data_transfer(void);
void col_row_rotation(void);
void RTL_function(void);
void stage1(void);
void stage2(void);
void stage3(void);

#endif /* FAST_IDCT_H_ */