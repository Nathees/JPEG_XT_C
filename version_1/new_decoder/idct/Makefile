all							: 	idct
				
idct						: 	idct.o fast_idct.o feig_idct.o 
				
idct.o 						:	idct.c
								g++ -c idct.c

fast_idct.o 				:	fast_idct.c
								g++ -c fast_idct.c
								
feig_idct.o 				:	feig_idct.c
								g++ -c feig_idct.c
				
clean 						:							
								rm -rf *o idct