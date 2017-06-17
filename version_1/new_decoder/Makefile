all					: 	jpeg_xt_decoder
					 
jpeg_xt_decoder		:	jpeg_xt_decoder.o
						(cd marker_parser; make all)
						 g++ jpeg_xt_decoder.o marker_parser/*.o -o jpeg_xt_decoder

jpeg_xt_decoder.o	: 	jpeg_xt_decoder.c
						g++ -c jpeg_xt_decoder.c

clean 				:				
						rm -rf *o jpeg_xt_decoder
						(cd marker_parser; make clean)


# g++ jpeg_xt_decoder.o marker_parser/marker_parser.o marker_parser/quantization_table_gen.o 
# marker_parser/huffman_table_gen.o marker_parser/scan_frame_decoder.o  marker_parser/tone_table_gen.o -o jpeg_xt_decoder