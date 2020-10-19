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

#include "sgx_tprotected_fs.h"
#include "sgx_tprotected_fs_t.h"
#include "protected_fs_file.h"
#include "benchmark_utils.h"

#include <errno.h>


static SGX_FILE* sgx_fopen_internal(const char* filename, const char* mode, const sgx_key_128bit_t *auto_key, const sgx_key_128bit_t *kdk_key)
{
	protected_fs_file* file = NULL;

	if (filename == NULL || mode == NULL)
	{
		errno = EINVAL;
		return NULL;
	}

	try {
		file = new protected_fs_file(filename, mode, auto_key, kdk_key);
	}
	catch (std::bad_alloc& e) {
		(void)e; // remove warning
		errno = ENOMEM;
		return NULL;
	}

	if (file->get_error() != SGX_FILE_STATUS_OK)
	{
		errno = file->get_error();
		delete file;
		file = NULL;
	}

	return (SGX_FILE*)file;
}


SGX_FILE* sgx_fopen_auto_key(const char* filename, const char* mode)
{
	BENCHMARK_START(fopen_auto_key, PROFILING_TRUSTED_LEVEL_FILE_API);
	SGX_FILE* r = sgx_fopen_internal(filename, mode, NULL, NULL);
	BENCHMARK_STOP(fopen_auto_key, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


SGX_FILE* sgx_fopen(const char* filename, const char* mode, const sgx_key_128bit_t *key)
{
	return sgx_fopen_internal(filename, mode, NULL, key);
}


size_t sgx_fwrite(const void* ptr, size_t size, size_t count, SGX_FILE* stream)
{
	BENCHMARK_START(fwrite, PROFILING_TRUSTED_LEVEL_FILE_API);

	if (ptr == NULL || stream == NULL || size == 0 || count == 0)
		return 0;

	protected_fs_file* file = (protected_fs_file*)stream;

	size_t r = file->write(ptr, size, count);

	BENCHMARK_STOP(fwrite, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


size_t sgx_fread(void* ptr, size_t size, size_t count, SGX_FILE* stream)
{
	BENCHMARK_START(fread, PROFILING_TRUSTED_LEVEL_FILE_API);

	if (ptr == NULL || stream == NULL || size == 0 || count == 0)
		return 0;

	protected_fs_file* file = (protected_fs_file*)stream;

	size_t r = file->read(ptr, size, count);

	BENCHMARK_STOP(fread, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


int64_t sgx_ftell(SGX_FILE* stream)
{
	BENCHMARK_START(ftell, PROFILING_TRUSTED_LEVEL_FILE_API);

	if (stream == NULL)
		return -1;

	protected_fs_file* file = (protected_fs_file*)stream;

	int64_t r = file->tell();

	BENCHMARK_STOP(ftell, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


int32_t sgx_fseek(SGX_FILE* stream, int64_t offset, int origin)
{
	BENCHMARK_START(fseek, PROFILING_TRUSTED_LEVEL_FILE_API);
	if (stream == NULL)
		return -1;

	protected_fs_file* file = (protected_fs_file*)stream;

	int32_t r = file->seek(offset, origin);

	BENCHMARK_STOP(fseek, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


int32_t sgx_fflush(SGX_FILE* stream)
{
	BENCHMARK_START(fflush, PROFILING_TRUSTED_LEVEL_FILE_API);
	if (stream == NULL)
		return EOPNOTSUPP; // TBD - currently we don't support NULL as fflush input parameter

	protected_fs_file* file = (protected_fs_file*)stream;

	int32_t r = file->flush(/*false*/) == true ? 0 : EOF;
	BENCHMARK_STOP(fflush, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


/* sgx_fflush_and_increment_mc
 *  Purpose: force actual write of all the cached data to the disk (see c++ fflush documentation for more details).
 *           in addition, in the first time this function is called, it adds a monotonic counter to the file
 *           in subsequent calls, the monotonic counter is incremented by one every time this function is called
 *           the monotonic counter is a limited resource, please read the SGX documentation for more details
 *
 *  Parameters:
 *      stream - [IN] the file handle (opened with sgx_fopen or sgx_fopen_auto_key)
 *
 *  Return value:
 *     int32_t  - result, 0 on success, 1 in case of an error - check sgx_ferror for error code
 *
int32_t sgx_fflush_and_increment_mc(SGX_FILE* stream)
{
	if (stream == NULL)
		return 1;

	protected_fs_file* file = (protected_fs_file*)stream;

	return file->flush(true) == true ? 0 : 1;
}
*/


int32_t sgx_ferror(SGX_FILE* stream)
{
	if (stream == NULL)
		return -1;

	protected_fs_file* file = (protected_fs_file*)stream;

	return file->get_error();
}


int32_t sgx_feof(SGX_FILE* stream)
{
	BENCHMARK_START(feof, PROFILING_TRUSTED_LEVEL_FILE_API);
	if (stream == NULL)
		return -1;
	
	protected_fs_file* file = (protected_fs_file*)stream;

	int32_t r = ((file->get_eof() == true) ? 1 : 0);

	BENCHMARK_STOP(feof, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


void sgx_clearerr(SGX_FILE* stream)
{
	if (stream == NULL)
		return;

	protected_fs_file* file = (protected_fs_file*)stream;

	file->clear_error();
}


static int32_t sgx_fclose_internal(SGX_FILE* stream, sgx_key_128bit_t *key, bool import)
{
	int32_t retval = 0;

	if (stream == NULL)
		return EOF;

	protected_fs_file* file = (protected_fs_file*)stream;

	if (file->pre_close(key, import) == false)
		retval = 1;

	delete file;

	return retval;
}


int32_t sgx_fclose(SGX_FILE* stream)
{
	BENCHMARK_START(fclose, PROFILING_TRUSTED_LEVEL_FILE_API);

	int32_t r = sgx_fclose_internal(stream, NULL, false);

	BENCHMARK_STOP(fclose, PROFILING_TRUSTED_LEVEL_FILE_API);

	return r;
}


int32_t sgx_remove(const char* filename)
{
	return protected_fs_file::remove(filename);
}


int32_t sgx_fexport_auto_key(const char* filename, sgx_key_128bit_t *key)
{
	SGX_FILE* stream = sgx_fopen_internal(filename, "r", NULL, NULL);
	if (stream == NULL)
		return 1;

	return sgx_fclose_internal(stream, key, false);
}


int32_t sgx_fimport_auto_key(const char* filename, const sgx_key_128bit_t *key)
{
	SGX_FILE* stream = sgx_fopen_internal(filename, "r+", key, NULL);
	if (stream == NULL)
		return 1;

	return sgx_fclose_internal(stream, NULL, true);
}


int32_t sgx_fclear_cache(SGX_FILE* stream)
{
	if (stream == NULL)
		return 1;

	protected_fs_file* file = (protected_fs_file*)stream;

	return file->clear_cache();
}
