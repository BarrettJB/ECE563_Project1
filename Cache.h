/*
 * Cache.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */

#ifndef CACHE_H_
#define CACHE_H_

//TODO define verbose?

class Cache {
public:
	Cache(int size, int assoc, int blocksize);
	//TODO read and write requests
	bool read(unsigned long addr);
	bool write(unsigned long addr);


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

	void init_arrays();
	void bump_LRU();

};

#endif /* CACHE_H_ */
