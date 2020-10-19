/*
 * Copyright (C) 2011-2020 Intel Corporation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Intel Corporation nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <assert.h>
#include <errno.h>
#include <time.h>

#include <sys/file.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>

#include "sgx_tprotected_fs_u.h"
#include <uprotected_fs.h>

#define PROFILING_UNTRUSTED_LEVEL_BASE 2000
#define PROFILING_UNTRUSTED_LEVEL_FILE_API PROFILING_UNTRUSTED_LEVEL_BASE + 1

#ifdef DEBUG
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "[sgx_uprotected_fs.h:%d] " fmt, __LINE__, ##args)
#else
#define DEBUG_PRINT(...)
#endif

#ifdef BENCHMARK_PROFILING

#define START_PROFILING(field, level) \
	if (level == profiling_current_level){ \
		start_tracking_time(&currently_tracked_time.field); \
	}
#define STOP_PROFILING(field, level) \
	if (level == profiling_current_level){ \
		stop_tracking_time(&currently_tracked_time.field, &sum_of_tracked_time.field); \
		sum_of_tracked_time.count_##field++; \
	}

typedef struct {
    clock_t exclusive_file_open;
    long int count_exclusive_file_open;
    clock_t check_if_file_exists;
    long int count_check_if_file_exists;
    clock_t fread_node;
    long int count_fread_node;
    clock_t fwrite_node;
    long int count_fwrite_node;
    clock_t fclose;
    long int count_fclose;
    clock_t fflush;
    long int count_fflush;
    clock_t remove;
    long int count_remove;
    clock_t recovery_file_open;
    long int count_recovery_file_open;
    clock_t fwrite_recovery_node;
    long int count_fwrite_recovery_node;
    clock_t do_file_recovery;
    long int count_do_file_recovery;
} untrusted_tracking_time_t;

static int profiling_current_level = 0;

static untrusted_tracking_time_t currently_tracked_time = {};
static untrusted_tracking_time_t sum_of_tracked_time = {};

inline void start_tracking_time(clock_t* tracking_field)
{
    if (*tracking_field != 0)
    {
        fprintf(stderr, "[benchmark_uprotected_fs]: error, the tracking time is already started.\n");
        return;
    }

    *tracking_field = clock();
}

inline void stop_tracking_time(clock_t* tracking_field, clock_t* sum_field)
{
    clock_t current_time = clock();

    if (*tracking_field == 0)
    {
        fprintf(stderr, "[benchmark_uprotected_fs]: tracking file writing is not started.\n");
        return;
    }

    *sum_field += current_time - *tracking_field;
    *tracking_field = 0;
}

void* u_sgxprotectedfs_get_sum_of_tracked_time()
{
    return &sum_of_tracked_time;
}

void u_sgxprotectedfs_reset_sum_of_tracked_time()
{
	memset(&currently_tracked_time, 0, sizeof(untrusted_tracking_time_t));
	memset(&sum_of_tracked_time, 0, sizeof(untrusted_tracking_time_t));
}

void u_sgxprotectedfs_profiling_set_level(int level)
{
	profiling_current_level = level;
}

void u_sgxprotectedfs_clock(clock_t* time)
{
	*time = clock();
}

void u_sgxprotectedfs_print_stderr(const char* message)
{
	fprintf(stderr, "%s", message);
}

#else

#define START_PROFILING(field, level)
#define STOP_PROFILING(field, level)


void* u_sgxprotectedfs_get_sum_of_tracked_time()
{
    return NULL;
}

void u_sgxprotectedfs_reset_sum_of_tracked_time()
{
}

void u_sgxprotectedfs_profiling_set_level(int)
{
}

void u_sgxprotectedfs_clock(clock_t*)
{
}

void u_sgxprotectedfs_print_stderr(const char*)
{
}

#endif

void* u_sgxprotectedfs_exclusive_file_open(const char* filename, uint8_t read_only, int64_t* file_size, int32_t* error_code)
{
	START_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* f = NULL;
	int result = 0;
	int fd = -1;
	mode_t mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
	struct stat stat_st;
	
	memset(&stat_st, 0, sizeof(struct stat));

	if (filename == NULL || strnlen(filename, 1) == 0)
	{
		DEBUG_PRINT("filename is NULL or empty\n");
		*error_code = EINVAL;
		STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}

	// open the file with OS API so we can 'lock' the file and get exclusive access to it
	fd = open(filename,	O_CREAT | (read_only ? O_RDONLY : O_RDWR) | O_LARGEFILE, mode); // create the file if it doesn't exists, read-only/read-write
	if (fd == -1)
	{
		DEBUG_PRINT("open returned %d, errno %d\n", result, errno);
		*error_code = errno;
		STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}

	// this lock is advisory only and programs with high priviliges can ignore it
	// it is set to help the user avoid mistakes, but it won't prevent intensional DOS attack from priviliged user
	result = flock(fd, (read_only ? LOCK_SH : LOCK_EX) | LOCK_NB); // NB - non blocking
	if (result != 0)
	{
		DEBUG_PRINT("flock returned %d, errno %d\n", result, errno);
		*error_code = errno;
		result = close(fd);
		assert(result == 0);
		STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}

	result = fstat(fd, &stat_st);
	if (result != 0)
	{
		DEBUG_PRINT("fstat returned %d, errno %d\n", result, errno);
		*error_code = errno;
		flock(fd, LOCK_UN);
		result = close(fd);
		assert(result == 0);
		STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}
	
	// convert the file handle to standard 'C' API file pointer
	f = fdopen(fd, read_only ? "rb" : "r+b");
	if (f == NULL)
	{
		DEBUG_PRINT("fdopen returned NULL\n");
		*error_code = errno;
		flock(fd, LOCK_UN);
		result = close(fd);
		assert(result == 0);
		STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}

	if (file_size != NULL)
		*file_size = stat_st.st_size;

	STOP_PROFILING(exclusive_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return f;
}


uint8_t u_sgxprotectedfs_check_if_file_exists(const char* filename)
{
	START_PROFILING(check_if_file_exists, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	struct stat stat_st;
	
	memset(&stat_st, 0, sizeof(struct stat));

	if (filename == NULL || strnlen(filename, 1) == 0)
	{
		DEBUG_PRINT("filename is NULL or empty\n");
		STOP_PROFILING(check_if_file_exists, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return 1;
	}
	
	uint8_t res = (stat(filename, &stat_st) == 0);
	STOP_PROFILING(check_if_file_exists, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return res; 
}


int32_t u_sgxprotectedfs_fread_node(void* f, uint64_t node_number, uint8_t* buffer, uint32_t node_size)
{
	START_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* file = (FILE*)f;
	uint64_t offset = node_number * node_size;
	int result = 0;
	size_t size = 0;

	if (file == NULL)
	{
		DEBUG_PRINT("file is NULL\n");
		STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return -1;
	}

	if ((result = fseeko(file, offset, SEEK_SET)) != 0)
	{
		DEBUG_PRINT("fseeko returned %d\n", result);
		if (errno != 0)
		{
			int err = errno;
			STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else
		{
			STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return -1;
		}
	}

	if ((size = fread(buffer, node_size, 1, file)) != 1)
	{
		int err = ferror(file);
		if (err != 0)
		{
			DEBUG_PRINT("fread returned %ld [!= 1], ferror: %d\n", size, err);
			STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else if (errno != 0)
		{
			err = errno;
			DEBUG_PRINT("fread returned %ld [!= 1], errno: %d\n", size, err);
			STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else
		{
			DEBUG_PRINT("fread returned %ld [!= 1], no error code\n", size);
			STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return -1;
		}
	}

	STOP_PROFILING(fread_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}


int32_t u_sgxprotectedfs_fwrite_node(void* f, uint64_t node_number, uint8_t* buffer, uint32_t node_size)
{
	START_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* file = (FILE*)f;
	uint64_t offset = node_number * node_size;
	int result = 0;
	size_t size = 0;

	if (file == NULL)
	{
		DEBUG_PRINT("file is NULL\n");
		STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return -1;
	}

	if ((result = fseeko(file, offset, SEEK_SET)) != 0)
	{
		DEBUG_PRINT("fseeko returned %d\n", result);
		if (errno != 0)
		{
			int err = errno;
			STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else
		{
			STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return -1;
		}
	}

	if ((size = fwrite(buffer, node_size, 1, file)) != 1)
	{
		DEBUG_PRINT("fwrite returned %ld [!= 1]\n", size);
		int err = ferror(file);
		if (err != 0)
		{
			STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else if (errno != 0)
		{
			err = errno;
			STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		else
		{
			STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return -1;
		}
	}

	STOP_PROFILING(fwrite_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}


int32_t u_sgxprotectedfs_fclose(void* f)
{
	START_PROFILING(fclose, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* file = (FILE*)f;
	int result = 0;
	int fd = 0;

	if (file == NULL)
	{
		DEBUG_PRINT("file is NULL\n");
		STOP_PROFILING(fclose, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return -1;
	}

	// closing the file handle should also remove the lock, but we try to remove it explicitly
	fd = fileno(file);
	if (fd == -1)
		DEBUG_PRINT("fileno returned -1\n");
	else
		flock(fd, LOCK_UN);
	
	if ((result = fclose(file)) != 0)
	{
		if (errno != 0)
		{
			int err = errno;
			DEBUG_PRINT("fclose returned %d, errno: %d\n", result, err);
			STOP_PROFILING(fclose, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return err;
		}
		DEBUG_PRINT("fclose returned %d\n", result);
		STOP_PROFILING(fclose, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return -1;
	}

	STOP_PROFILING(fclose, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}


uint8_t u_sgxprotectedfs_fflush(void* f)
{
	START_PROFILING(fflush, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* file = (FILE*)f;
	int result;

	if (file == NULL)
	{
		DEBUG_PRINT("file is NULL\n");
		STOP_PROFILING(fflush, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return 1;
	}
	
	if ((result = fflush(file)) != 0)
	{
		DEBUG_PRINT("fflush returned %d\n", result);
		STOP_PROFILING(fflush, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return 1;
	}

	STOP_PROFILING(fflush, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}


int32_t u_sgxprotectedfs_remove(const char* filename)
{
	START_PROFILING(remove, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	int result;

	if (filename == NULL || strnlen(filename, 1) == 0)
	{
		DEBUG_PRINT("filename is NULL or empty\n");
		STOP_PROFILING(remove, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return -1;
	}

	if ((result = remove(filename)) != 0)
	{// this function is called from the destructor which is called when calling fclose, if there were no writes, there is no recovery file...we don't want endless prints...
		//DEBUG_PRINT("remove returned %d\n", result);
		STOP_PROFILING(remove, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		if (errno != 0)
			return errno;
		return -1;
	}
	
	STOP_PROFILING(remove, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}

#define MILISECONDS_SLEEP_FOPEN 10
#define MAX_FOPEN_RETRIES       10
void* u_sgxprotectedfs_recovery_file_open(const char* filename)
{
	START_PROFILING(recovery_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* f = NULL;

	if (filename == NULL || strnlen(filename, 1) == 0)
	{
		DEBUG_PRINT("recovery filename is NULL or empty\n");
		STOP_PROFILING(recovery_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}
	
	for (int i = 0; i < MAX_FOPEN_RETRIES; i++)
	{
		f = fopen(filename, "wb");
		if (f != NULL)
			break;
		usleep(MILISECONDS_SLEEP_FOPEN);
	}
	if (f == NULL)
	{
		DEBUG_PRINT("fopen (%s) returned NULL\n", filename);
		STOP_PROFILING(recovery_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return NULL;
	}
	
	STOP_PROFILING(recovery_file_open, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return f;
}


uint8_t u_sgxprotectedfs_fwrite_recovery_node(void* f, uint8_t* data, uint32_t data_length)
{
	START_PROFILING(fwrite_recovery_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* file = (FILE*)f;

	if (file == NULL)
	{
		DEBUG_PRINT("file is NULL\n");
		STOP_PROFILING(fwrite_recovery_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return 1;
	}
		
	// recovery nodes are written sequentially
	size_t count = fwrite(data, 1, data_length, file);
	if (count != data_length)
	{
		DEBUG_PRINT("fwrite returned %ld instead of %d\n", count, data_length);
		STOP_PROFILING(fwrite_recovery_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
		return 1;
	}

	STOP_PROFILING(fwrite_recovery_node, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return 0;
}


int32_t u_sgxprotectedfs_do_file_recovery(const char* filename, const char* recovery_filename, uint32_t node_size)
{
	START_PROFILING(do_file_recovery, PROFILING_UNTRUSTED_LEVEL_FILE_API);

	FILE* recovery_file = NULL;
	FILE* source_file = NULL;
	int32_t ret = -1;
	uint32_t nodes_count = 0;
	uint32_t recovery_node_size = (uint32_t)(sizeof(uint64_t)) + node_size; // node offset + data
	uint64_t file_size = 0;
	int err = 0;
	int result = 0;
	size_t count = 0;
	uint8_t* recovery_node = NULL;
	uint32_t i = 0;

	do 
	{
		if (filename == NULL || strnlen(filename, 1) == 0)
		{
			DEBUG_PRINT("filename is NULL or empty\n");
			STOP_PROFILING(do_file_recovery, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return (int32_t)NULL;
		}

		if (recovery_filename == NULL || strnlen(recovery_filename, 1) == 0)
		{
			DEBUG_PRINT("recovery filename is NULL or empty\n");
			STOP_PROFILING(do_file_recovery, PROFILING_UNTRUSTED_LEVEL_FILE_API);
			return (int32_t)NULL;
		}
	
		recovery_file = fopen(recovery_filename, "rb");
		if (recovery_file == NULL)
		{
			DEBUG_PRINT("fopen of recovery file returned NULL - no recovery file exists\n");
			ret = -1;
			break;
		}

		if ((result = fseeko(recovery_file, 0, SEEK_END)) != 0)
		{
			DEBUG_PRINT("fseeko returned %d\n", result);
			if (errno != 0)
				ret = errno;
			break;
		}

		file_size = ftello(recovery_file);
	
		if ((result = fseeko(recovery_file, 0, SEEK_SET)) != 0)
		{
			DEBUG_PRINT("fseeko returned %d\n", result);
			if (errno != 0)
				ret = errno;
			break;
		}

		if (file_size % recovery_node_size != 0)
		{
			// corrupted recovery file
			DEBUG_PRINT("recovery file size is not the right size [%lu]\n", file_size);
			ret = ENOTSUP;
			break;
		}

		nodes_count = (uint32_t)(file_size / recovery_node_size);

		recovery_node = (uint8_t*)malloc(recovery_node_size);
		if (recovery_node == NULL)
		{
			DEBUG_PRINT("malloc failed\n");
			ret = ENOMEM;
			break;
		}

		source_file = fopen(filename, "r+b");
		if (source_file == NULL)
		{
			DEBUG_PRINT("fopen returned NULL\n");
			ret = -1;
			break;
		}

		for (i = 0 ; i < nodes_count ; i++)
		{
			if ((count = fread(recovery_node, recovery_node_size, 1, recovery_file)) != 1)
			{
				DEBUG_PRINT("fread returned %ld [!= 1]\n", count);
				err = ferror(recovery_file);
				if (err != 0)
					ret = err;
				else if (errno != 0) 
					ret = errno;
				break;
			}

			// seek the regular file to the required offset
			if ((result = fseeko(source_file, (*((uint64_t*)recovery_node)) * node_size, SEEK_SET)) != 0)
			{
				DEBUG_PRINT("fseeko returned %d\n", result);
				if (errno != 0)
					ret = errno;
				break;
			}

			// write down the original data from the recovery file
			if ((count = fwrite(&recovery_node[sizeof(uint64_t)], node_size, 1, source_file)) != 1)
			{
				DEBUG_PRINT("fwrite returned %ld [!= 1]\n", count);
				err = ferror(source_file);
				if (err != 0)
					ret = err;
				else if (errno != 0) 
					ret = errno;
				break;
			}
		}

		if (i != nodes_count) // the 'for' loop exited with error
			break;

		if ((result = fflush(source_file)) != 0)
		{
			DEBUG_PRINT("fflush returned %d\n", result);
			ret = result;
			break;
		}

		ret = 0;

	} while(0);

	if (recovery_node != NULL)
		free(recovery_node);

	if (source_file != NULL)
	{
		result = fclose(source_file);
		assert(result == 0);
	}

	if (recovery_file != NULL)
	{
		result = fclose(recovery_file);
		assert(result == 0);
	}

	if (ret == 0)
		remove(recovery_filename);
	
	STOP_PROFILING(do_file_recovery, PROFILING_UNTRUSTED_LEVEL_FILE_API);
	return ret;
}
