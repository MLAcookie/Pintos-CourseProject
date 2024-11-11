#ifndef THREADS_FP_H
#define THREADS_FP_H

#include <stdint.h>

#define FP_CONST 16384

typedef int fp;

// int -> fp
fp fp_convert_from(int num);

// fp -> int
int fp_toint_floor(fp f);
// fp -> int
int fp_toint_round(fp f);

// fp: x + fp: y
fp fp_add_fp(fp x, fp y);
// fp: x + int: n
fp fp_add_int(fp x, int n);
// fp: x - fp: y
fp fp_sub_fp(fp x, fp y);
// fp: x - int: n
fp fp_sub_int(fp x, int n);
// fp: x * fp: y
fp fp_mul_fp(fp x, fp y);
// fp: x * int: n
fp fp_mul_int(fp x, int n);
// fp: x / fp: y
fp fp_div_fp(fp x, fp y);
// fp: x / int: n
fp fp_div_int(fp x, int n);


#endif /* threads/fp.h */