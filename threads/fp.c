#include "threads/fp.h"

fp fp_convert_from(int num)
{
    return num * FP_CONST;
}

int fp_toint_floor(fp f)
{
    return f / FP_CONST;
}

int fp_toint_round(fp f)
{
    if (f > 0)
    {
        return (f + FP_CONST / 2) / FP_CONST;
    }
    else
    {
        return (f - FP_CONST / 2) / FP_CONST;
    }
}

fp fp_add_fp(fp x, fp y)
{
    return x + y;
}

fp fp_add_int(fp x, int n)
{
    return x + n * FP_CONST;
}

fp fp_sub_fp(fp x, fp y)
{
    return x - y;
}

fp fp_sub_int(fp x, int n)
{
    return x - n * FP_CONST;
}

fp fp_mul_fp(fp x, fp y)
{
    return ((int64_t)x) * y / FP_CONST;
}

fp fp_mul_int(fp x, int n)
{
    return x * n;
}

fp fp_div_fp(fp x, fp y)
{
    return ((int64_t)x) * FP_CONST / y;
}

fp fp_div_int(fp x, int n)
{
    return x / n;
}
