#include <stdio.h>
#include "math.h"
#include <stdlib.h>
#include <string.h>

#include "define.h"
#include "Main.h"
#include "Bitstream.h"
#include "scan_frame_decoder.h"


unsigned char header_len;


void Baseline_Frame(void){

	//********************************** calculate frame header length (16 bit) **********************************************************
	header_len = buffer[count++]; app11_len--;
	header_len = header_len << 8;
	header_len = header_len + buffer[count++] - 2;	app11_len--;	// subtract 2 for remove the frame header length (16 bit)
		
	count++;	app11_len--; // this contains the sample precision value P (8 bits)

	//**************************************** Calulate Y (No of lines in a image)*********************************************************
	Y = buffer[count++]; 	app11_len--;		// this byte_file contains the MSB of Y (8 bits)
	if (Y == 0xFF)
		remove_00_byte("Error in SOF marker segment");
	Y = Y << 8;;
	Y = Y + buffer[count++]; app11_len--;

	// ******************************************Calulate X (No of samples in a line)******************************************************
	X = buffer[count++];	app11_len--;		// this byte_file contains the MSB of Y (8 bits)
	X = X << 8;
	X = X + buffer[count++]; app11_len--;

	//*****************************************Calculate Nf (No of components in the image)***********************************************
	Nf = buffer[count++]; app11_len--;

	//********************************************Identify Component specification parameter*********************************************
	for (int i = 0; i < Nf; i++){
		comp_specific_param[i][0] = buffer[count++]; app11_len--;	// identifier of ith component
		byte_file = buffer[count++];	app11_len--;		// this byte_file contains the value of horizontal and vertical subsampling factor of ith component
		comp_specific_param[i][1] = byte_file;
		comp_specific_param[i][1] = comp_specific_param[i][1] >> 4;
		comp_specific_param[i][2] = byte_file;
		comp_specific_param[i][2] = comp_specific_param[i][1] & 0xF;
		comp_specific_param[i][3] = buffer[count++]; app11_len--;
	}
}

void Extended_Frame(void){
	Baseline_Frame();
}
void Progressive_Frame(void){
	printf("This decoder doesn't support Progressive Frame\n")
	exit(0);
}