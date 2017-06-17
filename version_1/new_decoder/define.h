#ifndef DEFINE_H_
#define DEFINE_H_


#define XX 1
#define YY 1

// define integer or float operation 
#define INTEGER_OPERATION 1   	// (1- Integer Operation & 0 - Float Operation)

// define IDCT Operation
#define IDCT_OPERATION 1		/* 
									1 - (INTEGER_OPERATION == 1) --> Fast Interger IDCT 
										(INTEGER_OPERATION == 0) --> Fast float IDCT
									2 - Feig float IDCT
									3 - Normal IDCT [Not Implemented]
								*/

// define Decoder Tracking Enable operation
#define TRACKING_ENABLE 1 	 	// (1 - Enable Tracking & 0 - Disable Tracking)	

#endif /* DEFINE_H_ */