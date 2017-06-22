#ifndef SCAN_FRAME_DECODER_H_
#define SCAN_FRAME_DECODER_H_

// Frame Decoder Functions
void baseline_frame_decode(void);
void extended_frame_decode(void);
void progressive_frame_decode(void);
void baseline_extended_frame_decode(void);
void calculate_mcu(void);

// Scan Decoder Functions
void scan_decoder(void);

#endif /* SCAN_FRAME_DECODER_H_ */