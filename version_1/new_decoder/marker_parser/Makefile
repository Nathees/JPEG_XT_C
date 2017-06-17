all							: 	marker_parser
				
marker_parser				: 	marker_parser.o quantization_table_gen.o huffman_table_gen.o \
								scan_frame_decoder.o tone_table_gen.o
				
marker_parser.o 			:	marker_parser.c
								g++ -c marker_parser.c
			
quantization_table_gen.o 	:	quantization_table_gen.c
								g++ -c quantization_table_gen.c

huffman_table_gen.o 		:	huffman_table_gen.c
								g++ -c huffman_table_gen.c

scan_frame_decoder.o 		:	scan_frame_decoder.c
								g++ -c scan_frame_decoder.c
			
tone_table_gen.o 			:	tone_table_gen.c
								g++ -c tone_table_gen.c
				
clean 						:							
								rm -rf *o marker_parser