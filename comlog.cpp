#include <new>
#include <cstdio>
#include <cstring>
#include <cstdlib>

#include <dlfcn.h>
#include <pthread.h>

#include "comlog.h"

static bool is_inited (false);

static size_t each_log_size (0);
static const size_t minimum_log_size (1 << 25);
static const size_t default_log_size (1 << 30);

static FILE *fw_fp    (stdout);
static FILE *other_fp (stderr);

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static const char *log_level_desc[] = {"DEBUG", "NOTICE", "WARNING", "FATAL"};

static void com_safe_fclose(FILE **fp)
{
	if (NULL != *fp)
    {
		fclose(*fp);
		*fp = NULL;
	}

	return ;
}

int init_com_log(const char *log_dir, const char *log_name, size_t log_size)
{
    if (is_inited)
    {
        return -2;
    }

    each_log_size = (0 == log_size) ? default_log_size : log_size;
    each_log_size = (each_log_size < minimum_log_size) ? minimum_log_size : each_log_size;

	size_t dir_len (strlen(log_dir));
	size_t name_len (strlen(log_name));
	size_t log_path_len (dir_len + name_len + 16);

	char *fw_log_path = new(std::nothrow) char[log_path_len];
	char *other_log_path = new(std::nothrow) char[log_path_len];

	bool func_flag (true);

	if ((NULL == fw_log_path) || (NULL == other_log_path))
    {
		func_flag = false;
	}

	if (func_flag)
    {
		memcpy(fw_log_path, log_dir, dir_len);

		if ('/' != log_dir[dir_len-1])
        {
            fw_log_path[dir_len] = '/';
            ++dir_len;
        }

		memcpy(fw_log_path + dir_len, log_name, name_len);
		fw_log_path[dir_len + name_len] = '\0';

		memcpy(other_log_path, fw_log_path, log_path_len);
		strcat(fw_log_path, ".log.fw");
		strcat(other_log_path, "log");

		fw_fp = fopen(fw_log_path, "a");
		other_fp = fopen(other_log_path, "a");

		if ((NULL == fw_fp) || (NULL == other_fp))
        {
			func_flag = false;
			com_safe_fclose(&fw_fp);
			com_safe_fclose(&other_fp);
		}
	}

	if (NULL != fw_log_path)
    {
		delete []fw_log_path;
	}

	if (NULL != other_log_path)
    {
		delete []other_log_path;
	}

	is_inited = func_flag ? true : false;

	return func_flag ? 0 : -1;
}

static const char *get_filename(const char *file_path)
{
	const char *file_name_pt (rindex(file_path, '/'));

	if (NULL == file_name_pt)
    {
		return file_path;
	}

	return ++file_name_pt;
}

void com_log(const char *file_path, const char *function_name,  int line_num, log_level level, const char *format, ...)
{

	FILE *output = level < LEVEL_WARNING ? other_fp : fw_fp;

	if (NULL == output)
    {
        fprintf(stderr, "Series Error! File pointer is NULL!\n");
        return ;
    }

    pthread_mutex_lock(&mutex);

	fprintf(output, "[%s: in file: %s    function: %s    line: %d    Time: %lld]", log_level_desc[level], get_filename(file_path), function_name, line_num, (long long)time(NULL));

	va_list argptr;
	va_start(argptr, format);
	vfprintf(output, format, argptr);
	va_end(argptr);

    pthread_mutex_unlock(&mutex);
}
