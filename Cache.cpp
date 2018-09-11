/*
 * Cache.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */

#include "Cache.h"
#include "CacheMath.h"
#include<iostream>
#include<algorithm>

#define VERBOSE false

//TODO validate input options
Cache::Cache(int size, int assoc, int blocksize) {
	mBlocksize = blocksize;
	mAssoc = assoc;
	mSets = size/(assoc * blocksize);
	mLogSets = CacheMath::log2(mSets);
	mLogBlockSize = CacheMath::log2(blocksize);

	if(VERBOSE)
	std::cout << "New Cache Created!" << std::endl
			<< "   Blocksize: " << mBlocksize << " " << mLogBlockSize << std::endl
			<< "   Sets: " << mSets << " " << mLogSets << std::endl
			<< "   Association: " << mAssoc << std::endl <<std::endl;

	mTags = new unsigned long[mSets];
	mValid = new bool[mSets];
	mDirty = new bool[mSets];
	mLRU = new int[mSets];
	Cache::init_arrays();
    //TODO check that valid inits to false
}

//Returns true on hit and false on miss
bool Cache::read(unsigned long addr) {
	unsigned long index = CacheMath::getIndex(addr, mLogBlockSize, mSets);
	unsigned long tag = CacheMath::getTag(addr, mLogBlockSize, mLogSets);

	if(VERBOSE)
	std::cout << "Handling read request..." << std::endl
			<< "   Index: " << index << std::endl
			<< "   Tag: " << tag << std::endl
			<< "   Cache Tag: " << mTags[index] << std::endl
			<< "   Cache Valid: " << mValid[index] << std::endl;

	if (mTags[index] == tag && mValid[index])
	{
		//Cache hit
		if(VERBOSE)
		std::cout << "Cache hit!" << std::endl << std::endl;

		return true;
	} else
	{
		//Cache miss
		//TODO handle LRU replacement
		if(VERBOSE)
		std::cout << "Cache miss!" << std::endl << std::endl;

		if (mDirty[index]) {
			//TODO handle writeback
		}

		Cache::bump_LRU();
		mLRU[index] = 0;
		mTags[index] = tag;
		mValid[index] = true;
		mDirty[index] = false;
	    return false;
	}
}

bool Cache::write(unsigned long addr) {;
	unsigned long index = CacheMath::getIndex(addr, mLogBlockSize, mSets);
	unsigned long tag = CacheMath::getTag(addr, mLogBlockSize, mLogSets);

	if(VERBOSE)
	std::cout << "Handling write request..." << std::endl
			<< "   Index: " << index << std::endl
			<< "   Tag: " << tag << std::endl
			<< "   Cache Tag: " << mTags[index] << std::endl;

	if (mTags[index] == tag && mValid[index])
		{
			//Cache hit
			if(VERBOSE)
			std::cout << "Cache hit!" << std::endl << std::endl;
			mDirty[index] = true;
			return true;
		} else
		{
			//Cache miss
			//TODO handle LRU replacement
			if(VERBOSE)
			std::cout << "Cache miss!" << std::endl << std::endl;

			if (mDirty[index]) {
				//TODO handle writeback
			}

			Cache::bump_LRU();
			mLRU[index] = 0;
			mTags[index] = tag;
			mValid[index] = true;
			mDirty[index] = true;
		    return false;
		}
}

//Used to init valid and dirty arrays to false
void Cache::init_arrays() {
	for (int i = 0; i < mSets; i++) {
		mValid[i] = false;
		mDirty[i] = false;
		mLRU[i] = 0x7FFFFFFF;
	}
}

void Cache::bump_LRU() {
	for (int i = 0; i < mSets; i++) {
		mLRU[i] = std::min(0x7FFFFFFF, mLRU[i]+1);
	}
}

//fails if number is not power of two
//TODO fix this?
//TODO move to cacheMath class?

