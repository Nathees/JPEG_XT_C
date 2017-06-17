#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"

#include "marker_parser.h"
#include "tone_table_gen.h"

// control variables
int msb, lsb, tone_data;

void tone_table_gen(void){
	
	// Tracking Operation
	#if TRACKING_ENABLE
		printf("APP11 - TONE Table Gen Process\n");
	#endif

	buff_index++;
	for (int x = 0; x < 256; x++){
		msb = buffer[buff_index++];
		lsb = buffer[buff_index++];
		tone_data = (msb << 8) + lsb;
		tone_table[x] = tone_data;
	}
}

void debug_tone_table(void){

}