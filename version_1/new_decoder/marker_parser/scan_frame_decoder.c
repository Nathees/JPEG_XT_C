#include <stdio.h>

#include "define.h"
#include "Main.h"
#include "marker_parser.h"
#include "scan_frame_decoder.h"


char header_len;
char Nf_Ns;



void baseline_frame_decode(void){
	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0)
			printf("SOF0 Maker Process\n");
		else
			printf("APP11 - SOF0 Maker Process\n");
	#endif
	baseline_extended_frame_decode();
}
void extended_frame_decode(void){
	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0)
			printf("SOF1 Maker Process\n");
		else
			printf("APP11 - SOF1 Maker Process\n");
	#endif
	baseline_extended_frame_decode();
}
void progressive_frame_decode(void){
	printf("This decoder doesn't support Progressive Frame\n")
	exit(0);
}

void baseline_extended_frame_decode(void){

	//********************************** calculate frame header length (16 bit) **********************************************************
	header_len = buffer[index++]; 
	header_len = header_len << 8;
	header_len = header_len + buffer[index++] - 2;		// subtract 2 for remove the frame header length (16 bit)
		
	// this contains the sample precision value P (8 bits)
	index++;	 
	app11_processed_bytes = app11_processed_bytes + 3;

	//**************************************** Calulate Image Height (No of lines in a image)***********************************************
	img_height = buffer[index++]; 	app11_processed_bytes++;		// this byte_file contains the MSB of Y (8 bits)
	if (buffer[index - 1] == 0xFF){
		index++;	 
		app11_processed_bytes++;
	}
	img_height = img_height << 8;;
	img_height = img_height + buffer[index++]; app11_processed_bytes++;
	if (buffer[index - 1] == 0xFF){
		index++;	 
		app11_processed_bytes++;
	}

	// ******************************************Calulate Image Width (No of samples in a line)**********************************************
	img_width = buffer[index++];	app11_processed_bytes++;		// this byte_file contains the MSB of Y (8 bits)
	if (buffer[index - 1] == 0xFF){
		index++;	 
		app11_processed_bytes++;
	}
	img_width = img_width << 8;
	img_width = img_width + buffer[index++]; app11_processed_bytes++;

	if (buffer[index - 1] == 0xFF){
		index++;	 
		app11_processed_bytes++;
	}

	//*****************************************Calculate Nf (No of components in the image)***********************************************
	Nf_Ns = buffer[index++]; app11_processed_bytes++;
	if(Nf_Ns != 3){
		printf("This decoder doesn't support single plane\n")
		exit(0);
	}

	//********************************************Identify Component specification parameter*********************************************
	for (int i = 0; i < Nf; i++){
		index++; app11_processed_bytes++;	// identifier of ith component

		byte_file = buffer[index++];	app11_processed_bytes++;		// horizontal and vertical subsampling factor of ith component

		if(residual_layer_flag == 0){
			base_hori_samp_factor[i] = byte_file;
			base_hori_samp_factor[i] = base_hori_samp_factor[i] >> 4;
			base_vert_samp_factor[i] = byte_file;
			base_vert_samp_factor[i] = base_vert_samp_factor[i] & 0xF;

			base_dqt_id[i] = index++; app11_processed_bytes++;	// quantization table identifier of ith component
			if(base_dqt_id[i] > 1){
				printf("This decoder supports 2 DQT Tables only\n");
				exit(0);
			}

		}
		else{
			resi_hori_samp_factor[i] = byte_file;
			resi_hori_samp_factor[i] = resi_hori_samp_factor[i] >> 4;
			resi_vert_samp_factor[i] = byte_file;
			resi_vert_samp_factor[i] = resi_vert_samp_factor[i] & 0xF;

			resi_dqt_id[i] = index++; app11_processed_bytes++;	// quantization table identifier of ith component
			if(resi_dqt_id[i] > 1){
				printf("This decoder supports 2 DQT Tables only\n");
				exit(0);
			}
		}
	}
}

void scan_decoder(void){

	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0)
			printf("SOS Maker Process\n");
		else
			printf("APP11 - SOS Maker Process\n");
	#endif

	//************************************************** scan header length (16 bit)**************************************************
	header_len = buffer[index++]; app11_processed_bytes++;
	header_len = header_len << 8;
	header_len = header_len + buffer[index++] - 2;	app11_processed_bytes++;	// subtract 2 for remove the scan header length (16 bit)

	//********************************************calculate Ns [ no of components in the scan ]********************************************
	Nf_Ns = buffer[index++]; app11_processed_bytes++;
	if(Nf_Ns != 3){
		printf("This decoder doesn't support Non Interleaved\n")
		exit(0);
	}

	//*******************************************Reading scan component specification parameter *****************************************
	for(int i = 0; i < Ns; i++){
		index++; app11_processed_bytes++; // identifier of ith component

		byte_file = buffer[index++];	app11_processed_bytes++;		// DC and AC huffmantable identifier

		if(residual_layer_flag == 0){
			base_huff_dc_id[i] = byte_file;
			base_huff_dc_id[i] = base_huff_dc_id[i] >> 4;
			base_huff_ac_id[i] = byte_file;
			base_huff_ac_id[i] = base_huff_ac_id[i] & 0xF;
			if(base_huff_dc_id[i] > 1 || base_huff_ac_id[i] > 1){
				printf("This decoder supports 2 DHT Tables only\n");
				exit(0);
			}
		}
		else{
			resi_huff_dc_id[i] = byte_file;
			resi_huff_dc_id[i] = resi_huff_dc_id[i] >> 4;
			resi_huff_ac_id[i] = byte_file;
			resi_huff_ac_id[i] = resi_huff_ac_id[i] & 0xF;
			if(resi_huff_dc_id[i] > 1 || resi_huff_ac_id[i] > 1){
				printf("This decoder supports 2 DHT Tables only\n");
				exit(0);
			}
		}
	}

	//*************************************************Reading Ss, Se and Ah_Al parameters ****************************************************
	index = index + 3;
	app11_processed_bytes = app11_processed_bytes + 3;

	// Tracking Operation
	#if TRACKING_ENABLE
		if(residual_layer_flag == 0){
			printf("Image Width = %d\tImage Height = %d\n",img_width,img_height);
		}
	#endif
}