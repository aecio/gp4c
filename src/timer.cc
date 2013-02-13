#include "timer.h"

unordered_map<string, unsigned long long> times;

void PrintTimes(void) {
    unordered_map<string, unsigned long long>::iterator it = times.begin();
    for(; it != times.end(); ++it) {
        pair<const string, unsigned long long> &p = *it;
        printf("time: %s %f\n", p.first.c_str(), p.second/(double)1000000);
    }
}
