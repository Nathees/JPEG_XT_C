##all: 				
##					(cd sources; make all)
##
##clean :				
##					(cd sources; make clean)##

all: 				system
					 
					

system:				main.o
					(cd sources; make all)
					g++ main.o -o main

main.o: 			main.c
					g++ -c main.c

clean :				
					rm -rf *o system
					(cd sources; make clean)
