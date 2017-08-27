#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "upsample.h"

// Local variable decleration
unsigned char upsample_type;
unsigned char comp_layer; // y = 0, Cb = 1 & Cr = 2

void identify_upsample_type(void){
	if(ldr_hdr_img == 1){
		for(row = 0; row < 3; row++){
			if(base_hori_samp_factor[row] != resi_hori_samp_factor[row] || base_vert_samp_factor[row] != resi_vert_samp_factor[row]){
				printf("Base and Residual sampling factors are mismatch\n");
				exit(0);
			}
		}
	}
	/*			H1		V1		H2		V2		H3		V3
	type 1	:	1		1		1		1		1		1
	type 2	:	2		2		2		1		2		1
	type 3	:	2		2		1		2		1		2
	type 4	:	2		2		1		1		1		1
	*/
	if (base_hori_samp_factor[0] == 1){		
		upsample_type = 1; // type 1
	}
	else{											
		if (base_hori_samp_factor[1] == 1){			
			if (base_vert_samp_factor[1] == 1){		
				upsample_type = 4; // type 4
			}
			else{								
				upsample_type = 3; // type 3
			}
		}
		else{			
			upsample_type = 2; // type 2
		}
	}

	// Tracking Operation
	#if TRACKING_ENABLE
		printf("Upsampling Type - %d\n",upsample_type);
	#endif

	if(upsample_type != 1){
		printf("Upsample Type %d not implemented\n",upsample_type);
		exit(0);
	}
}

void upsample(unsigned char layer){
	comp_layer = layer;
	switch(upsample_type){
		case 1 :	upsample_1(); break;
		case 2 :	upsample_2(); break;
		case 3 :	upsample_3(); break;
		case 4 :	upsample_4(); break;
		default:	break;
	}
}

void upsample_1(void){
	for(row = 0; row < 8; row++){
		for(col = 0; col < 8; col++){
			if(base_resi_layer == 0){
				switch(comp_layer){
					case 	0 	:	base_upsample_y_block[row][col] = (unsigned char)base_int_block[row][col];
									break;
					case 	1 	:	base_upsample_cb_block[row][col] = (unsigned char)base_int_block[row][col];
									break;
					case 	2 	:	base_upsample_cr_block[row][col] = (unsigned char)base_int_block[row][col];
									break;
					default		:	break;
				}
			}
			else{
				switch(comp_layer){
					case 	0 	:	resi_upsample_y_block[row][col] = (unsigned char)resi_int_block[row][col];
									break;
					case 	1 	:	resi_upsample_cb_block[row][col] = (unsigned char)resi_int_block[row][col];
									break;
					case 	2 	:	resi_upsample_cr_block[row][col] = (unsigned char)resi_int_block[row][col];
									break;
					default		:	break;
				}			
			}
		}
	}
}

void upsample_2(void){
	printf("Upsample Type 2 not implemented\n");
	exit(0);
}

void upsample_3(void){
	printf("Upsample Type 3 not implemented\n");
	exit(0);
}

void upsample_4(void){
	printf("Upsample Type 4 not implemented\n");
	exit(0);
}