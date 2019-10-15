#include <stdint.h>
#include <xen/xen.h>
#include "time.h"
#include "console.h"

#define NANOSECONDS(tsc) (tsc << shared_info->vcpu_info[0].time.tsc_shift) \
    * shared_info->vcpu_info[0].time.tsc_to_system_mul

#define RDTSC(x) __asm volatile ("rdtsc" : "=A"(tsc))

extern shared_info_t * HYPERVISOR_shared_info;

int gettimeofday(struct timeval *tp, void *tzp)
{
    uint64_t tsc;
    uint32_t version, wc_version;
    uint32_t seconds, nanoseconds, system_time;
    uint64_t old_tsc;
    shared_info_t *shared_info = HYPERVISOR_shared_info;

    do
    {
        do
        {
            wc_version = shared_info->wc_version;
            version = shared_info->vcpu_info[0].time.version;
        } while ( (version & 1) == 1 || (wc_version & 1) == 1);
        seconds = shared_info->wc_sec;
        nanoseconds = shared_info->wc_nsec;
        system_time = shared_info->vcpu_info[0].time.system_time;
        old_tsc = shared_info->vcpu_info[0].time.tsc_timestamp;
    } while ( version != shared_info->vcpu_info[0].time.version || wc_version != shared_info->wc_version );

    RDTSC(tsc);
    tsc -= old_tsc;
    system_time += NANOSECONDS(tsc);
    nanoseconds += system_time;
    seconds += nanoseconds / 1000000000;
    nanoseconds = nanoseconds % 1000000000;
    tp->tv_sec = seconds;
    tp->tv_usec = nanoseconds * 1000;

    return 0;
}

void convert(time_t t, struct tm *datetime)
{
    int a,b,c,d,e,f;
    datetime->tm_sec = t % 60;
    t /= 60;
    datetime->tm_min = t % 60;
    t /= 60;
    datetime->tm_hour = t % 24;
    t /= 24;
    //Convert Unix time to date
    a = (uint32_t) ((4 * t + 102032) / 146097 + 15);
    b = (uint32_t) (t + 2442113 + a - (a / 4));
    c = (20 * b - 2442) / 7305;
    d = b - 365 * c - (c / 4);
    e = d * 1000 / 30601;
    f = d - e * 30 - e * 601 / 1000;

    //January and February are counted as months 13 and 14 of the previous year
    if(e <= 13)
    {
        c -= 4716;
        e -= 1;
    }
    else
    {
        c -= 4715;
        e -= 13;
    }
    datetime->tm_year = c;
    datetime->tm_mon = e;
    datetime->tm_mday = f;
}