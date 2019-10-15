typedef long time_t;
typedef long suseconds_t;

struct timespec {
    time_t  tv_sec;
    long    tv_nsec;
};

struct timezone {
};

struct timeval {
    time_t      tv_sec;     // seconds
    suseconds_t tv_usec;    // microseconds
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
};

int gettimeofday(struct timeval *tv, void *tz);
void convert(time_t t, struct tm *datetime);