all: 				jpeg_xt_decoder

jpeg_xt_decoder: 	Main.o Bitstream.o IDCT.o Huffman.o Upsampling.o
					g++ Main.o Bitstream.o IDCT.o Huffman.o Upsampling.o -o jpeg_xt_decoder

Main.o : 			Main.c
					g++ -c Main.c

Bitstream.o : 		Bitstream.c
					g++ -c Bitstream.c

IDCT.o : 			IDCT.c
					g++ -c IDCT.c

Huffman.o : 		Huffman.c
					g++ -c Huffman.c

Upsampling.o : 		Upsampling.c
					g++ -c Upsampling.c

clean :				
					rm -rf *o jpeg_xt_decoder
