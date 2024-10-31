#ifndef TIME_HELPER_H
#define TIME_HELPER_H

#ifdef _WIN32

#include <windows.h>

#define BILLION                             (1E9)

#ifndef _TIMESPEC_DEFINED
    typedef struct timespec
    {
        LARGE_INTEGER tv_sec;
        LARGE_INTEGER tv_nsec;
    } timespec;
#endif

static BOOL g_first_time = 1;
static LARGE_INTEGER g_counts_per_sec;

int clock_gettime(int dummy, timespec* ct)
{
    LARGE_INTEGER count;

    if (g_first_time)
    {
        g_first_time = 0;

        if (0 == QueryPerformanceFrequency(&g_counts_per_sec))
        {
            g_counts_per_sec.QuadPart = 0;
        }
    }

    if (( NULL == ct ) || ( g_counts_per_sec.QuadPart <= 0 ) ||
        ( 0 == QueryPerformanceCounter(&count) ))
    {
        return -1;
    }

    ct->tv_sec = count.QuadPart / g_counts_per_sec.QuadPart;
    ct->tv_nsec = ( ( count.QuadPart % g_counts_per_sec.QuadPart ) * BILLION ) / g_counts_per_sec.QuadPart;

    return 0;
}

#endif // _WIN32

#endif // TIME_HELPER_H