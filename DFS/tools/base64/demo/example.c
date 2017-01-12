/*
 * example.c
 *
 *  Created on: 2017年1月12日
 *      Author: ubuntu
 */

#include <stdio.h>  /* fwrite */
#include "libbase64.h"

int main ()
{
    char src[] = "haidong.wang";
    char out[20];
    size_t srclen = sizeof(src) - 1;
    size_t outlen;

    base64_encode(src, srclen, out, &outlen, 0);

//    fwrite(out, outlen, 1, stdout);
    printf("out: %s\n", out);

    return 0;
}

