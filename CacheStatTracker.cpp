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
}

