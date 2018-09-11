/*
 * CacheMath.cpp
 *
 *  Created on: Sep 11, 2018
 *      Author: bryso
 */

#include "CacheMath.h"

int CacheMath::log2(int i) {
	int out = 0;
	int bit = i & 1;
	while(bit == 0)
	{
		out++;
		i = i >> 1;
		bit = i & 1;
	}
	return out;
}

