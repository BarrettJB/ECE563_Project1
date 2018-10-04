/*
 * Cache.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */

#ifndef CACHE_H_
#define CACHE_H_

#include<iostream>
#include "CacheStatTracker.h"

class Cache {
public:
	Cache(int size, int assoc, int blocksize, Cache* next = NULL);
	bool read(unsigned long addr);
	bool write(unsigned long addr);

	void print_contents();

	CacheStatTracker tracker;

	//Used for to work in L1 only cases
	bool exists;


private:
	//Tracking of dirty blocks
	//Least Recently Used Tracking
	//Some reference to next lower memory (cache or mem)
	int mSets;
	int mAssoc;
	int mBlocksize; //must be power of 2
	int mLogSets;
	int mLogBlockSize;

	unsigned long *mTags;
	int *mLRU;
	bool *mValid;
	bool *mDirty;

	Cache *mNextLevel;

	void init_arrays();
	void update_LRU(unsigned long set, int way);

};

#endif /* CACHE_H_ */
