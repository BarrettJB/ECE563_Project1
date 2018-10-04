/*
 * CacheMath.h
 *
 *  Created on: Sep 11, 2018
 *      Author: bryso
 */

#ifndef CACHEMATH_H_
#define CACHEMATH_H_

class CacheMath {
public:
	static int log2(int i);
	static unsigned long getSet(unsigned long addr, int logBlock, int sets) {return (addr >> logBlock) & (sets-1);};
	static unsigned long getTag(unsigned long addr, int logBlock, int logSets) {return addr >> (logBlock + logSets);};
	static unsigned long getAddr(unsigned long tag, unsigned long set, int logBlock, int logSets) {return ((tag << (logBlock+logSets)) + (set << logBlock));};
};

#endif /* CACHEMATH_H_ */
