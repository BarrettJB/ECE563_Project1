/*
 * CacheStatTracker.h
 *
 *  Created on: Sep 13, 2018
 *      Author: Barrett Bryson
 */

#ifndef CACHESTATTRACKER_H_
#define CACHESTATTRACKER_H_

//TODO add report method?
class CacheStatTracker {
public:
	CacheStatTracker();

	int getReadCount() {return mRead;};
	int getReadMissCount() {return mReadMiss;};
	int getWriteCount() {return mWrite;};
	int getWriteMissCount() {return mWriteMiss;};
	int getWritebackCount() {return mWriteback;};

	float getMissRate() {return (mReadMiss * 1.0F)/(mReadMiss + mRead);};

	void addRead() {mRead++;};
	void addReadMiss() {mReadMiss++;};
	void addWrite() {mWrite++;};
	void addWriteMiss() {mWriteMiss++;};
	void addWriteback() {mWriteback++;};

private:
	int mRead;
	int mReadMiss;
	int mWrite;
	int mWriteMiss;
	int mWriteback;
};

#endif /* CACHESTATTRACKER_H_ */
