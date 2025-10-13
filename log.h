#ifndef LOG_H
#define LOG_H

void info(const char *message);
void error(const char *message);
void warn(const char *message);
void debug(const char *message);
void _print_message(const char *level, const char *message);
void _print_date_time(void);

#endif // LOG_H