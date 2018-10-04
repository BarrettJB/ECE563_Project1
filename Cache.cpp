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

		int array_size = mSets*mAssoc;
		mTags = new unsigned long[array_size];
		mValid = new bool[array_size];
		mDirty = new bool[array_size];
		mLRU = new int[array_size];
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
				<< "   Set: " << set << " (" << index << ")" << std::endl
				<< "   Index: " << i  << std::endl
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
			update_LRU(set,index);
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

	update_LRU(set,LRUIndex);
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
			update_LRU(set,index);
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

	update_LRU(set,LRUIndex);
	mTags[LRUIndex] = tag;
	mValid[LRUIndex] = true;
	mDirty[LRUIndex] = true;
    return false;
}

void Cache::print_contents() {
	unsigned long Tags[mAssoc];
	bool Dirty[mAssoc];
	for(int i = 0; i < mSets; i++)
	{
		std::cout << " set\t" << i << ": ";
		//Get tags into LRU sorted order
		for(int j = 0; j < mAssoc; j++)
		{
			Tags[mLRU[mAssoc*i+j]] = mTags[mAssoc*i+j];
			Dirty[mLRU[mAssoc*i+j]] = mDirty[mAssoc*i+j];
		}

		for(int k = 0; k < mAssoc; k++)
		{
			std::cout << "  " << std::hex << Tags[k] << std::dec;
			if (Dirty[k])
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
		mLRU[i] = mAssoc-1;
	}
}

//REWRITE THIS
void Cache::update_LRU(unsigned long set, int way) {
	for (unsigned long i = set*mAssoc; i < (set+1)*mAssoc; i++) {
		if (mLRU[i] < mLRU[way])
		{
			mLRU[i]++;
		}
	}
	mLRU[way] = 0;
}

//fails if number is not power of two
//TODO fix this?
//TODO move to cacheMath class?

