#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"
#include "tone_table_gen.h"

// control variables
int msb, lsb, tone_data;

void tone_table_gen(void){
	
	// Tracking Operation
	#if TRACKING_ENABLE
		printf("APP11 - TONE Table Gen Process\n");
	#endif

	index++;
	for (int x = 0; x < 256; x++){
		msb = buffer[index++];
		lsb = buffer[index++];
		tone_data = (msb << 8) + lsb;
		tone_table[x] = tone_data;
	}
}

void debug_tone_table(void){

}