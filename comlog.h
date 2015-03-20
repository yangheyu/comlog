#ifndef COMLOG_H_INCLUDED
#define COMLOG_H_INCLUDED

#include <stdarg.h>

enum log_level {LEVEL_DEBUG, LEVEL_NOTICE, LEVEL_WARNING, LEVEL_FATAL};

extern int init_com_log(const char *log_dir, const char *log_name, size_t log_size = 0);

extern void com_log(const char *file_path, const char *function_name,  int line_num, log_level level, const char *format, ...);

#define COM_LOG_FATAL(format, ...) { \
	com_log(__FILE__, __FUNCTION__, __LINE__, LEVEL_FATAL, format, ##__VA_ARGS__); \
}

#define COM_LOG_WARNING(format, ...) { \
	com_log(__FILE__, __FUNCTION__, __LINE__, LEVEL_WARNING, format, ##__VA_ARGS__); \
}

#define COM_LOG_NOTICE(format, ...) { \
	com_log(__FILE__, __FUNCTION__, __LINE__, LEVEL_NOTICE, format, ##__VA_ARGS__); \
}

#define COM_LOG_DEBUG(format, ...) { \
	com_log(__FILE__, __FUNCTION__, __LINE__, LEVEL_DEBUG, format, ##__VA_ARGS__); \
}

#endif // COMLOG_H_INCLUDED
