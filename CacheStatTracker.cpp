/*
 * CacheStatTracker.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: Barrett Bryson
 */

#include "CacheStatTracker.h"

CacheStatTracker::CacheStatTracker() {
	mRead = 0;
	mReadMiss = 0;
	mWrite = 0;
	mWriteMiss = 0;
	mWriteback = 0;
	mSwap = 0;
	mSwapRequests = 0;
}

float CacheStatTracker::getMissRate() {
	if ((mRead + mWrite) > 0)
		return ((mReadMiss + mWriteMiss - mSwapRequests) * 1.0F)/(mRead + mWrite);
	else
		return 0.0F;
}


float CacheStatTracker::getReadMissRate() {
	if ((mRead + mWrite) > 0)
		return ((mReadMiss) * 1.0F)/(mRead);
	else
		return 0.0F;
}
