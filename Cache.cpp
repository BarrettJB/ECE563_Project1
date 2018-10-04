/*
 * Cache.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */

#include "Cache.h"
#include "CacheMath.h"
#include<algorithm>

#define VERBOSE false

//TODO validate input options
Cache::Cache(int size, int assoc, int blocksize, Cache* next) {
	if(size != 0)
	{
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
		if (next != NULL && next->exists)
			mNextLevel = next;
		else
			mNextLevel = NULL;
		Cache::init_arrays();
		exists = true;
	}
	else
	{
		exists = false;
	}
}

//Returns true on hit and false on miss
bool Cache::read(unsigned long addr) {
	unsigned long set = CacheMath::getSet(addr, mLogBlockSize, mSets);
	unsigned long tag = CacheMath::getTag(addr, mLogBlockSize, mLogSets);
	int LRUMax = -1;
	int LRUIndex = -1;

	tracker.addRead();

	for(int i = 0; i < mAssoc; i++) {

		//Gets index in tag array
		//TODO put into cachemath;
		unsigned long index = set*mAssoc + i;

		if(VERBOSE)
		std::cout << "Handling read request..." << std::endl
				<< "   Set: " << set << std::endl
				<< "   Index: " << i << std::endl
				<< "   Tag: " << tag << std::endl
				<< "   Cache LRU: " << mLRU[index] << std::endl
				<< "   Cache Tag: " << mTags[index] << std::endl
				<< "   Cache Valid: " << mValid[index] << std::endl;

		//Check for cache hits;
		if (mTags[index] == tag && mValid[index])
		{
			//Cache hit
			if(VERBOSE)
			std::cout << "Cache hit!" << std::endl << std::endl;

			Cache::bump_LRU();
		    mLRU[LRUIndex] = 0;
			return true;
		}

		//Track LRU values
		if (LRUMax < mLRU[index])
		{
			if(VERBOSE)
			std::cout << "  LRU index changed to: " << index << std::endl;
			LRUMax = mLRU[index];
			LRUIndex = index;
		}
	}


	//Cache Miss
	if(VERBOSE)
	std::cout << "Cache miss!" << std::endl << std::endl;
	tracker.addReadMiss();

	//Writeback if dirtybit is set
	if (mDirty[LRUIndex]) {
		tracker.addWriteback();
		if(mNextLevel != NULL){
			unsigned long wb_addr = CacheMath::getAddr(mTags[LRUIndex],set,mLogBlockSize, mLogSets);
			mNextLevel->write(wb_addr);
		}
	}

	//Check lower level for value
	if(mNextLevel != NULL) {
		mNextLevel->read(addr);
	}

	Cache::bump_LRU();
	mLRU[LRUIndex] = 0;
	mTags[LRUIndex] = tag;
	mValid[LRUIndex] = true;
	mDirty[LRUIndex] = false;
    return false;
}

bool Cache::write(unsigned long addr) {
	unsigned long set = CacheMath::getSet(addr, mLogBlockSize, mSets);
	unsigned long tag = CacheMath::getTag(addr, mLogBlockSize, mLogSets);
	int LRUMax = -1;
	int LRUIndex = -1;

	tracker.addWrite();

	for(int i = 0; i < mAssoc; i++) {

		//Gets index in tag array
		//TODO put into cachemath;
		unsigned long index = set*mAssoc + i;

		if(VERBOSE)
		std::cout << "Handling read request..." << std::endl
				<< "   Set: " << set << std::endl
				<< "   Index: " << i << std::endl
				<< "   Tag: " << tag << std::endl
				<< "   Cache Tag: " << mTags[index] << std::endl
				<< "   Cache Valid: " << mValid[index] << std::endl;

		//Check for cache hits;
		if (mTags[index] == tag && mValid[index])
		{
			//Cache hit
			if(VERBOSE)
			std::cout << "Cache hit!" << std::endl << std::endl;

			Cache::bump_LRU();
			mLRU[LRUIndex] = 0;
			mDirty[index] = true;
			return true;
		}

		//Track LRU values
		if (LRUMax < mLRU[index])
		{
			LRUMax = mLRU[index];
			LRUIndex = index;
		}
	}

	//Cache Miss
	if(VERBOSE)
	std::cout << "Cache miss!" << std::endl << std::endl;
	tracker.addWriteMiss();

	if (mDirty[LRUIndex]) {
		tracker.addWriteback();
		if(mNextLevel != NULL){
			unsigned long wb_addr = CacheMath::getAddr(mTags[LRUIndex],set,mLogBlockSize, mLogSets);
			mNextLevel->write(wb_addr);
		}
	}

	//Check lower level for value
	if(mNextLevel != NULL) {
		mNextLevel->read(addr);
	}

	Cache::bump_LRU();
	mLRU[LRUIndex] = 0;
	mTags[LRUIndex] = tag;
	mValid[LRUIndex] = true;
	mDirty[LRUIndex] = true;
    return false;
}

void Cache::print_contents() {
	for(int i = 0; i < mSets; i++)
	{
		std::cout << " set\t" << i << ": ";
		for(int j = 0; j < mAssoc; j++)
		{
			std::cout << "  " << std::hex << mTags[i*mAssoc + j] << std::dec;
			if (mDirty[i*mAssoc + j])
			{
				std::cout << " D";
			}else
			{
				std::cout << "  ";
			}
		}
		std::cout << std::endl;
	}
}

//Used to init valid and dirty arrays to false
//uses 0x7FFFFFFE as max to prevent overflows
void Cache::init_arrays() {
	for (int i = 0; i < mSets*mAssoc; i++) {
		mValid[i] = false;
		mDirty[i] = false;
		mLRU[i] = 0x7FFFFFFE;
	}
}

void Cache::bump_LRU() {
	for (int i = 0; i < mSets*mAssoc; i++) {
		//Overflows cause errors if this is 0x7FFFFFFF
		mLRU[i] = std::min(0x7FFFFFFE, mLRU[i]+1);
	}
}

//fails if number is not power of two
//TODO fix this?
//TODO move to cacheMath class?

