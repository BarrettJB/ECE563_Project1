/*
 * Cache.h
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */

#ifndef CACHE_H_
#define CACHE_H_

class Cache {
public:
	Cache(int size, int assoc, int blocksize);
	virtual ~Cache();
	//TODO read and write requests
	int read(int addr);
private:
	//Tracking of dirty blocks
	//Least Recently Used Tracking
	//Some reference to next lower memory (cache or mem)
	int mSize;
	int mAssoc;
	int mBlocksize; //must be power of 2
};

#endif /* CACHE_H_ */
