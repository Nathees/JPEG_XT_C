#ifndef IDCT_H_
#define IDCT_H_

extern const int DCT_T[8][8];
extern float modified_quantization_table[64][2];
extern int integer_modified_quantization_table[64][2];

extern float inverse_IDCT_blk[8][8];

//***********************************************Kronector matrix generation***************************************************
void initial_kronector_matrix_feig(void);
void kronecker_matrix_mult(int num1, float num2, int num3, int num4, int num5, int row, int col);

//************************************* Modify the quantization table for the Feig implementation*******************************
void modify_quantization_table(unsigned char table_id);



//*************************************** Normal IDCT Method*******************************************************************
void normal_IDCT(void);

//*****************************************Hardcode Implementation of Feig IDCT*********************************************
void Hardcode_Feig_IDCT(void);
void B1_Operation(void);
void M_operation(void);
void A1_Operation(void);
void A2_Operation(void);
void A3_Operation(void);

//*****************************************Hardcode Implementation of Fast IDCT*********************************************
void Hardcode_Fast_IDCT(void);
void B1_Operation_row(unsigned char blk_row);
void B1_Operation_col(unsigned char blk_col);
void M_operation_row(void);
void A1_Operation_row();
void A2_Operation_row();
void A3_Operation_row(unsigned char blk_row);
void A3_Operation_col(unsigned char blk_col);

//****************************************** Integer Based IDCT **********************************************
void Integer_IDCT(unsigned char comp);
void RTL_function();
void stage1();
void stage2();
void stage3();



#endif /* IDCT_H_ */
