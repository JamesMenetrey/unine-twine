#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "instrumentation_callbacks.h"

/*
 * Measures the current (and peak) resident and virtual memory
 * usage of your linux C process, in bytes, accurate to nearest kB.
 * Returns a 0 if memory info access was successful, else prints
 * an error message and returns 1.
 * 
 * Source of the code: https://gist.github.com/TysonRayJones/15083e2ad3025451e7eee5f58589a543.
 */
static int getMemory(
	unsigned long *currRealMem, unsigned long *peakRealMem,
	unsigned long *currVirtMem, unsigned long *peakVirtMem)
{
	// Clear the buffers
	*currRealMem = 0;
	*peakRealMem = 0;
	*currVirtMem = 0;
	*peakVirtMem = 0;

	// stores each word in status file
	char buffer[1024] = "";
	
	// linux file contains this-process info
	FILE* file = NULL;
	file = fopen("/proc/self/status", "r");
	if (file == NULL)
    {
		fprintf(stderr, "Call to getMemory FAILED; "
			   "linux file proc/self/status not found!\n");
		return 1;
	}

	// read the entire file, recording mems in kB
	while (fscanf(file, " %1023s", buffer) == 1)
    {
		if (strcmp(buffer, "VmRSS:") == 0) {
			if (fscanf(file, " %lu", currRealMem) < 1)
            {
                fprintf(stderr, "Error while reading memory state.\n");
            }
		}
		if (strcmp(buffer, "VmHWM:") == 0) {
			if (fscanf(file, " %lu", peakRealMem) < 1)
            {
                fprintf(stderr, "Error while reading memory state.\n");
            }
		}
		if (strcmp(buffer, "VmSize:") == 0) {
			if (fscanf(file, " %lu", currVirtMem) < 1)
            {
                fprintf(stderr, "Error while reading memory state.\n");
            }
		}
		if (strcmp(buffer, "VmPeak:") == 0) {
			if (fscanf(file, " %lu", peakVirtMem) < 1)
            {
                fprintf(stderr, "Error while reading memory state.\n");
            }
		}
	}
    
	fclose(file);

	unsigned int factor = 1;
	*currRealMem *= factor;
	*peakRealMem *= factor;
	*currVirtMem *= factor;
	*peakVirtMem *= factor;
	
	return 0;
}

void on_initialization_finished() {}

void on_records_inserted(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
    unsigned long currRealMem, peakRealMem;
	unsigned long currVirtMem, peakVirtMem;

    int ret = getMemory(&currRealMem, &peakRealMem, &currVirtMem, &peakVirtMem);
    if (ret == 0)
    {
        printf("%lu\n", currRealMem);
    }
    else
    {
        fprintf(stderr, "Cannot retrieve the memory information for the benchmark.\n");
    }
}

void on_sequential_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}

void on_random_queries_finished(int database_type, int number_of_write,
                         int number_of_read, long int time)
{
}