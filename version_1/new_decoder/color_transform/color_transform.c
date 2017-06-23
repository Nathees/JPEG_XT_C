#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "color_transform.h"


// Local Variable Decleraations
unsigned char comp_r[8][8];
unsigned char comp_g[8][8];
unsigned char comp_b[8][8];


void color_transform(int loact_x, int loact_y){
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			comp_r[row][col] = base_upsample_y_block[row][col] + ((((45941 * (base_upsample_cr_block[row][col] - 128)) >> 14) + 1) >> 1);
			comp_g[row][col] = base_upsample_y_block[row][col] - ((((11277 * (base_upsample_cb_block[row][col] - 128)) >> 14) + 1) >> 1) - 
										((((23401 * (base_upsample_cr_block[row][col] - 128)) >> 14) + 1) >> 1);
			comp_b[row][col] = base_upsample_y_block[row][col] + ((((58065 * (base_upsample_cb_block[row][col] - 128)) >> 14) + 1) >> 1);

			// R block Clamping
			if (comp_r[row][col] > 255)
				comp_r[row][col] = 255;
			else if (comp_r[row][col] < 0)
				comp_r[row][col] = 0;
			// G block Clamping
			if (comp_g[row][col] > 255)
				comp_g[row][col] = 255;
			else if (comp_g[row][col] < 0)
				comp_g[row][col] = 0;
			// B block Clamping
			if (comp_b[row][col] > 255)
				comp_b[row][col] = 255;
			else if (comp_b[row][col] < 0)
				comp_b[row][col] = 0;
		}
	}

	// Upload to the blocks to Image
	for (row = 0; row < 8; row++){
		for (col = 0; col < 8; col++){
			channel_r[row + loact_x * 8][col + loact_y * 8] = comp_r[row][col];
			channel_g[row + loact_x * 8][col + loact_y * 8] = comp_g[row][col];
			channel_b[row + loact_x * 8][col + loact_y * 8] = comp_b[row][col];
		}
	}
}