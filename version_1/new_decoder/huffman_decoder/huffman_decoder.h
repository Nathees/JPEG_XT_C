#ifndef HUFFMAN_DECODER_H_
#define HUFFMAN_DECODER_H_

// Huffman Decoder Control Functions
// Gloabal Gunctions
void initial_load_bitstream(void);
void huffman_decoder(unsigned char comp, unsigned char dht_dc_id, unsigned char dht_ac_id, unsigned char dqt_id, unsigned char layer);
// Local Functions
void reset_block(void);
void dc_huffman_decode(void);
void ac_huffman_decode(void);
void shift(unsigned char shift);
void calculate_value(void);

#endif /* HUFFMAN_DECODER_H_ */