#include <stdio.h>
#include <stdlib.h>

#include "../define.h"
#include "../jpeg_xt_decoder.h"
#include "idct.h"

// Sub Header Files
#include "fast_idct.h"
#include "feig_idct.h"


void idct(void){
	//feig_idct();
	fast_int_idct();
}