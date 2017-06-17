#ifndef MARKER_PARSER_H_
#define MARKER_PARSER_H_

//  ******************************** Extern Variables Decleration ********************************
extern int app11_processed_bytes;
extern char residual_layer_flag;


// ******************************** Functions Decleration ********************************

// Main Marker Parser Operation
void marker_parser(void);

// APP11 Marker Decoder
void APP11_process(void);

// Residual Marker Parser Operation
void residual_marker_parser(void);

// Skip Marker Process
void skip_marker(void);

// Loading Residual Layer Scan bitstream to Residual Buffer
void get_residual_layer_bitstream(void);

#endif /* MARKER_PARSER_H_ */