#include <stdio.h>
#include <stdlib.h>

#include "Cache.h"

typedef struct cache_params{
    unsigned long int block_size;
    unsigned long int l1_size;
    unsigned long int l1_assoc;
    unsigned long int vc_num_blocks;
    unsigned long int l2_size;
    unsigned long int l2_assoc;
};

/*  argc holds the number of command line arguments
    argv[] holds the commands themselves

    Example:-
    sim_cache 32 8192 4 7 262144 8 gcc_trace.txt
    argc = 8
    argv[0] = "sim_cache"
    argv[1] = "32"
    argv[2] = "8192"
    ... and so on
*/

void print_stats(cache_params params, Cache L1, Cache L2) {
	printf("===== L1 contents =====\n");
	L1.print_contents();
	printf("\n");

	if (params.l2_size != 0) {
		printf("===== L2 contents =====\n");
		L2.print_contents();
		printf("\n");
	}

	printf("===== Simulation results =====\n");
	printf("  a. number of L1 reads: %27d\n",L1.tracker.getReadCount());
	printf("  b. number of L1 read misses: %21d\n",L1.tracker.getReadMissCount());
	printf("  c. number of L1 writes: %26d\n",L1.tracker.getWriteCount());
	printf("  d. number of L1 write misses: %20d\n",L1.tracker.getWriteMissCount());
	//TODO: Track swap requests
	printf("  e. number of swap requests: %22d\n", L1.tracker.getSwapRequestCount());
	if (params.vc_num_blocks == 0)
		printf("  f. swap request rate: %28.4f\n", 0.0F);
	else
		printf("  f. swap request rate: %28.4f\n", 1.0f*L1.tracker.getSwapRequestCount()/(L1.tracker.getReadCount()+L1.tracker.getWriteCount()));
	printf("  g. number of swaps: %30d\n", L1.tracker.getSwapCount());
	if (params.vc_num_blocks == 0)
	{
		printf("  h. combined L1+VC miss rate: %21.4f\n",L1.tracker.getMissRate());
		printf("  i. number writebacks from L1/VC: %17d\n",L1.tracker.getWritebackCount());
	}
	else
	{
		float mr = 1.0f*(L1.tracker.getReadMissCount()+L1.tracker.getWriteMissCount()-L1.tracker.getSwapCount())/(L1.tracker.getReadCount()+L1.tracker.getWriteCount());
		printf("  h. combined L1+VC miss rate: %21.4f\n",mr);
		printf("  i. number writebacks from L1/VC: %17d\n",L1.mVictimCache->tracker.getWritebackCount());
	}
	printf("  j. number of L2 reads: %27d\n",L2.tracker.getReadCount());
	printf("  k. number of L2 read misses: %21d\n",L2.tracker.getReadMissCount());
	printf("  l. number of L2 writes: %26d\n",L2.tracker.getWriteCount());
	printf("  m. number of L2 write misses: %20d\n",L2.tracker.getWriteMissCount());
	printf("  n. L2 miss rate: %33.4f\n",L2.tracker.getReadMissRate());
	//TODO: this doesn't include write misses
	printf("  o. number of writebacks from L2: %17d\n",L2.tracker.getWritebackCount());
	//TODO: fix this calculation
	if (L2.exists)
	{
		printf("  p. total memory traffic: %25d\n", L2.tracker.getReadMissCount() + L2.tracker.getWriteMissCount()+L2.tracker.getWritebackCount());
	}
	else  {
		if (params.vc_num_blocks == 0)
			printf("  p. total memory traffic: %25d\n",L1.tracker.getReadMissCount()+L1.tracker.getWriteMissCount()+L1.tracker.getWritebackCount());
		else
			printf("  p. total memory traffic: %25d\n",L1.tracker.getReadMissCount()+L1.tracker.getWriteMissCount()+L1.mVictimCache->tracker.getWritebackCount()-L1.tracker.getSwapCount());
	}
}

// cache_project.exe 32
int main (int argc, char* argv[])
{
    FILE *FP;               // File handler
    char *trace_file;       // Variable that holds trace file name;
    cache_params params;    // look at sim_cache.h header file for the the definition of struct cache_params
    char rw;                // variable holds read/write type read from input file. The array size is 2 because it holds 'r' or 'w' and '\0'. Make sure to adapt in future projects
    unsigned long int addr; // Variable holds the address read from input file

    if(argc != 8)           // Checks if correct number of inputs have been given. Throw error and exit if wrong
    {
        printf("Error: Expected inputs:7 Given inputs:%d\n", argc-1);
        exit(EXIT_FAILURE);
    }

    // strtoul() converts char* to unsigned long. It is included in <stdlib.h>
    params.block_size       = strtoul(argv[1], NULL, 10);
    params.l1_size          = strtoul(argv[2], NULL, 10);
    params.l1_assoc         = strtoul(argv[3], NULL, 10);
    params.vc_num_blocks    = strtoul(argv[4], NULL, 10);
    params.l2_size          = strtoul(argv[5], NULL, 10);
    params.l2_assoc         = strtoul(argv[6], NULL, 10);
    trace_file              = argv[7];

    // Open trace_file in read mode
    FP = fopen(trace_file, "r");
    if(FP == NULL)
    {
        // Throw error and exit if fopen() failed
        printf("Error: Unable to open file %s\n", trace_file);
        exit(EXIT_FAILURE);
    }

    // Print params
    printf("===== Simulator configuration =====\n"
            "  BLOCKSIZE: %19lu\n"
            "  L1_SIZE: %21lu\n"
            "  L1_ASSOC: %20lu\n"
            "  VC_NUM_BLOCKS: %15lu\n"
            "  L2_SIZE: %21lu\n"
            "  L2_ASSOC: %20lu\n"
            "  trace_file: %18s\n\n", params.block_size, params.l1_size, params.l1_assoc, params.vc_num_blocks, params.l2_size, params.l2_assoc, "gcc_trace.txt");

    Cache L2(params.l2_size,params.l2_assoc,params.block_size);
    Cache L1(params.l1_size,params.l1_assoc,params.block_size,&L2,params.vc_num_blocks);


    char str[2];
    while(fscanf(FP, "%s %lx", str, &addr) != EOF)
    {

        rw = str[0];
        if (rw == 'r'){
            //printf("%s %lx\n", "read", addr);           // Print and test if file is read correctly
        	L1.read(addr);
        }
        else if (rw == 'w'){
            //printf("%s %lx\n", "write", addr);          // Print and test if file is read correctly
        	L1.write(addr);
        }
        /*************************************
                  Add cache code here
        **************************************/
    }
    print_stats(params,L1,L2);
    return 0;
}






