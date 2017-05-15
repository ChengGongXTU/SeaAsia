#pragma once

#include <stdlib.h>  

static unsigned long long seed = 1;

double drand48(void)
{
	seed = (0x5DEECE66DLL * seed + 0xB16) & 0xFFFFFFFFFFFFLL;
	unsigned int x = seed >> 16;
	return  ((double)x / (double)0x100000000LL);

}

void srand48(unsigned int i)
{
	seed = (((long long int)i) << 16) | rand();
}

