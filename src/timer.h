#ifndef _TIMER_
#define _TIMER_

#include <tr1/unordered_map>
#include <map>
#include <string>
#include <sys/timeb.h>
#include <ctime>
#include <sys/time.h>
#include <cstdio>
#include <sstream>

using namespace std;

#define unordered_map std::map

#define START_TIMER(ID, TIMER_DESCRIPTION)\
    struct timeval ID##_start; \
    std::stringstream ID##_ss; \
    ID##_ss << TIMER_DESCRIPTION; \
    std::string ID##_timer_desc = ID##_ss.str(); \
    gettimeofday(&ID##_start, NULL);

#define START_FUNCTION_TIMER START_TIMER(__FUNCTION__, __FUNCTION__)

#define FINISH_TIMER(ID) \
    struct timeval ID##_end; \
    gettimeofday(&ID##_end, NULL); \
    times[ID##_timer_desc] += (ID##_end.tv_sec - ID##_start.tv_sec) * 1000000 \
                              +ID##_end.tv_usec - ID##_start.tv_usec;

extern unordered_map<string, unsigned long long> times;

void PrintTimes(void);

#endif
