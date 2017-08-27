#include <stdio.h>
#include "Bitstream.h"
#include "Main.h"
#include "Huffman.h"
#include "IDCT.h"

int huffcode_AC_identifier[16][2];
int huffcode_DC_identifier[16][2];

unsigned char huff_size[256];

unsigned char quantization_table[64][4];
unsigned char resi_quantization_table[64][4];

unsigned char no_of_huff_code_len_i[16];				//Array for store the number of huffman code length of each i

int tone_table[256];
unsigned char resi_tbox_flag = 0;

int X;
int Y;
unsigned char Nf;
unsigned char comp_specific_param[3][6];

int no_table;	// Number of tables in DQT and DHT


int app11_len = 0;


void Baseline_Frame(void){

	//********************************** calculate frame header length (16 bit) **********************************************************
	int frame_header_len;
	frame_header_len = buffer[count++]; app11_len--;
	frame_header_len = frame_header_len << 8;
	frame_header_len = frame_header_len + buffer[count++] - 2;	app11_len--;	// subtract 2 for remove the frame header length (16 bit)
		
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

}
void Progressive_Frame(void){

}

void Huffman(){

	//*************************************Calculate huffman table defination length (16 bit)*******************************************
	int huff_tab_len;
	huff_tab_len = buffer[count++];  app11_len--;
	huff_tab_len = huff_tab_len << 8;
	huff_tab_len = huff_tab_len + buffer[count++] - 2;	 app11_len--;	// subtract 2 for remove the quantization table defination length (16 bit)

	//*********************** identify Tc and Th and finally add one to no_table due to Tc and Th each 4 bits**************************
	unsigned char Tc_Th;			// Tc - Table class (1 - AC, 0 - DC)  Th - table identifier
	no_table = 0;
	while (no_table < huff_tab_len){
		Tc_Th = buffer[count++]; app11_len--;
		no_table = no_table + 1;

		int ii=0;
		//*****************************Identify Li and store the HUFFSIZE to huffman_table[K][1]**************************************
		Li = 0;
		K = 0;
		ii = 0;
		for (ii = 1; ii < 17; ii++){
			byte_file = buffer[count++]; app11_len--;
			Li = Li + byte_file;
			//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
			while (byte_file){
				huff_size[K] = ii;
				K = K + 1;
				byte_file = byte_file - 1;
			}
		}
		no_table = no_table + 16;				// add 16 for Li values each have 8 bits
		no_table = no_table + Li;
		generate_huffcode(Tc_Th);				//generate huff code for each Vi,j value and store it to Huffman table
	}

}
void Quantization(){
	
	//************************************* calculate quantization table defination length (16 bit)**************************************
	int quanti_tab_len;
	quanti_tab_len = buffer[count++]; app11_len--;
	quanti_tab_len = quanti_tab_len << 8;
	quanti_tab_len = quanti_tab_len + buffer[count++] - 2;	app11_len--;	// subtract 2 for remove the quantization table defination length (16 bit)
	
	//*********************** identify Pq and Tq and finally add one to no_table due to Pq and Tq each 4 bits*****************************
	unsigned char Pq,Tq;			// Pq - element precision  Tq - table identifier
	no_table = 0;
	while (no_table < quanti_tab_len){
		byte_file = buffer[count++]; app11_len--;
		Pq = byte_file;
		Pq = Pq >> 4;
		Tq = byte_file;
		Tq = Tq & 0xF;
		no_table = no_table + 1;

		if (byte_file == 0xFF)
			remove_00_byte("Error in DQT marker segment");

		//********************************************Adding elements to quantization table no_table*************************************** 
		/*int Qk = 0;			// quantization element
		for (Qk = 0; Qk < 64; Qk++){
			quantization_table[Qk][Tq] = buffer[count++];	app11_len--;		//Add elements to quantization table;
		}
		modify_quantization_table(Tq);	*/	//Modify the quantization table based on Feig method for inverse DCT

		int Qk = 0;			// quantization element
		for (Qk = 0; Qk < 64; Qk++){
			if(resi_tbox_flag == 0){
				quantization_table[Qk][Tq] = buffer[count++];	
			}
			else {
				resi_quantization_table[Qk][Tq] = buffer[count++];	
				app11_len--;		//Add elements to quantization table;
			}
		}

		if(resi_tbox_flag == 0)
			modify_quantization_table(Tq);		//Modify the quantization table based on Feig method for inverse DCT
		else
			modify_resi_quantization_table(Tq);

		no_table = no_table + 64;
	}
}

void Scan(){

	//************************************************** scan header length (16 bit)**************************************************
	int scan_header_len;
	scan_header_len = buffer[count++]; app11_len--;
	scan_header_len = scan_header_len << 8;
	scan_header_len = scan_header_len + buffer[count++] - 2;	app11_len--;	// subtract 2 for remove the scan header length (16 bit)

	//********************************************calculate Ns [ no of components in the scan ]********************************************
	unsigned char Ns;
	Ns = buffer[count++]; app11_len--;

	//*******************************************Reading scan component specification parameter *****************************************
	for(int i = 0; i < Ns; i++){
		count++; app11_len--;
		byte_file = buffer[count++];	app11_len--;		// this byte_file contains DC and AC huffmantable identifier
		comp_specific_param[i][4] = byte_file;
		comp_specific_param[i][4] = comp_specific_param[i][4] >> 4;
		comp_specific_param[i][5] = byte_file;
		comp_specific_param[i][5] = comp_specific_param[i][5] & 0xF;
	}

	//*************************************************Reading Ss, Se and Ah_Al parameters ****************************************************
	unsigned char Ss, Se, Ah_Al;
	Ss = buffer[count++]; app11_len--;
	Se = buffer[count++]; app11_len--;
	Ah_Al = buffer[count++]; app11_len--;
}

void APP11_process(){
	app11_len = buffer[count++];
	app11_len = app11_len << 8;
	app11_len = app11_len + buffer[count++] - 2;		// subtract 2 for remove the quantization table defination length (16 bit)
	
	// SKIP 12 bytes
	count = count + 15;
	app11_len = app11_len - 15;

	switch (buffer[count++]){
	case 0x45:	tone_mapping_process(); break;
	case 0x49:	if (resi_tbox_flag) {
		get_residual_layer_bitstream();
		//getchar();
	}
				else 
					resi_tbox_process(); 
				break;
	default:	count = count + app11_len - 1; break;
	}

}

void tone_mapping_process(){
	int msb, lsb, tone_data;
	count++;
	for (int x = 0; x < 256; x++){
		msb = buffer[count++];
		lsb = buffer[count++];
		tone_data = (msb << 8) + lsb;
		tone_table[x] = tone_data;
	}
}

void resi_tbox_process(){
	resi_tbox_flag = 1;

	// Skiping SOI Marker in Residual Layer
	count = count + 3;
	app11_len = app11_len - 3;
	while (buffer[count] != 0xDA){
		app11_len--;
		switch (buffer[count++]){
		case 0xC0:	Baseline_Frame(); printf("Basline frame procees\n"); break;
		case 0xC1:	Baseline_Frame();  /*Extended_Frame();*/ printf("Extended frame procees\n"); break;
		case 0xC2:	Progressive_Frame(); printf("Progressive frame procees\n");  break;

		case 0xC4:	Huffman(); printf("Huffman syntax procees\n");  break;
		case 0xDB:	Quantization(); printf("Quantization Syntax procees\n");  break;
		default:	skip_byte(); break;
		}
		count++;
		app11_len--;
	}
	count++; app11_len--;
	Scan();

	DIFF_Y = 0;
	DIFF_Cb = 0;
	DIFF_Cr = 0;

	printf("X = %d,  Y = %d\n", X, Y);
	if (X % 8)
		MCU_X = X / 8 + 1;
	else
		MCU_X = X / 8;
	if (Y % 8)
		MCU_Y = Y / 8 + 1;
	else
		MCU_Y = Y / 8;
	// Calculatiing horizontal (MCU_X) and vertical (MCU_Y) MCUs for the image
	if (MCU_X % comp_specific_param[0][1])
		MCU_X = MCU_X / comp_specific_param[0][1] + 1;
	else
		MCU_X = MCU_X / comp_specific_param[0][1];
	if (MCU_Y % comp_specific_param[0][2])
		MCU_Y = MCU_Y / comp_specific_param[0][2] + 1;
	else
		MCU_Y = MCU_Y / comp_specific_param[0][2];

	// Generating Array For Residual Layer
	//create_dynamic_array_resi_layer();

	// loading residual layer information to residual_buffer
	count_resi = 0;
	get_residual_layer_bitstream();
}


/*void create_dynamic_array_resi_layer(){
	resi_y = (int **)malloc(sizeof(int *) * Y);
	resi_cb = (int **)malloc(sizeof(int *) * Y);
	resi_cr = (int **)malloc(sizeof(int *) * Y);
	for (int i = 0; i < Y; i++){
		resi_y[i] = (int *)malloc(X * sizeof(int));
		resi_cb[i] = (int *)malloc(X * sizeof(int));
		resi_cr[i] = (int *)malloc(X * sizeof(int));
	}
}*/

void get_residual_layer_bitstream(){
	while (app11_len > 1){
		buffer_resi[count_resi++] = buffer[count++];
		app11_len--;
	}
}

void remove_00_byte(char *error){		// Remove 0x00 byte if exist after the 0xFF byte 
	unsigned char temp;
	if (resi_tbox_flag){
		temp = buffer_resi[count_resi++];	// Read next byte after the 0xFF byte.
	}
	else{
		temp = buffer[count++];	// Read next byte after the 0xFF byte.
	}
	
	if (temp != 0x00){
		printf("%s\n", error);
	}
}