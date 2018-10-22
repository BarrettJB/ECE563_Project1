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
	Cache(int size, int assoc, int blocksize, Cache* next = NULL, int vcsize = 0);
	bool read(unsigned long addr);
	bool write(unsigned long addr);
	bool swap(unsigned long addr, unsigned long addr_vic, bool dirty_vic, unsigned long *tag_ret, bool *dirty_ret);
	void print_contents();
	//Need this because for some reason the formatting on main cache and vc are different????
	void print_vc_contents();

	CacheStatTracker tracker;

	Cache *mVictimCache;

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
