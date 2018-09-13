/*
 * main.cpp
 *
 *  Created on: Sep 5, 2018
 *      Author: Barrett Bryson
 */
#include <iostream>
#include <assert.h>

#include "Cache.h"



//TODO add command line args?
int main() {
	//Run some tests
	std::cout <<"***********************************************"
			  "\n             Testing cache class \n" <<
			    "***********************************************\n" <<std::endl;
	std::cout << "*** Direct Mapped Cache ***" << std::endl;
	Cache myCache(256,1,16);

	std::cout << "Testing basic read miss...    " << std::flush;
	assert(!myCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing basic read hit...   " << std::flush;
	assert(myCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing spacial locality...   " << std::flush;
	assert(myCache.read(0xDEADBEE2));
	std::cout << "1   " << std::flush;
	assert(myCache.read(0xDEADBEE6));
	std::cout << "2   " << std::flush;
	assert(myCache.read(0xDEADBEEA));
	std::cout << "3   Success!" << std::endl;

	std::cout << "Testing initial valid bits...   " << std::flush;
	assert(!myCache.read(0x00000000));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing overwriting...   " << std::flush;
	myCache.read(0xCAB0BEEF);
	assert(!myCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;


    //Associative Cache tests
	std::cout << "\n\n*** Associative Cache *** " << std::endl;
	Cache assocCache(256,2,16);

	std::cout << "Testing basic read miss...    " << std::flush;
	assert(!assocCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing basic read hit...   " << std::flush;
	assert(assocCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing spacial locality...   " << std::flush;
	assert(assocCache.read(0xDEADBEE2));
	std::cout << "1   " << std::flush;
	assert(assocCache.read(0xDEADBEE6));
	std::cout << "2   " << std::flush;
	assert(assocCache.read(0xDEADBEEA));
	std::cout << "3   Success!" << std::endl;

	std::cout << "Testing Association...   " << std::flush;
	assocCache.read(0xCAB0BEEF);
	assert(assocCache.read(0xCAB0BEEF));
	assert(assocCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

	std::cout << "Testing Least Recently Used...   " << std::flush;
	assocCache.read(0x1234BEEF);
	assert(assocCache.read(0x1234BEEF));
	assert(assocCache.read(0xDEADBEEF));
	std::cout << "Success!" << std::endl;

}


