all					: 	jpeg_xt_decoder
					 
jpeg_xt_decoder		:	jpeg_xt_decoder.o
						(cd marker_parser; make all)
						(cd io_file; make all)
						(cd huffman_decoder; make all)
						(cd idct; make all)
						 g++ jpeg_xt_decoder.o marker_parser/*.o io_file/*.o huffman_decoder/*.o idct/*.o -o jpeg_xt_decoder

jpeg_xt_decoder.o	: 	jpeg_xt_decoder.c
						g++ -c jpeg_xt_decoder.c

clean 				:				
						rm -rf *o jpeg_xt_decoder
						(cd marker_parser; make clean)
						(cd io_file; make clean)
						(cd huffman_decoder; make clean)
						(cd idct; make clean)

