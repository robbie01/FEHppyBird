#include "FEHUtility.h"
#include <sys/time.h>
#include "FEHLCD.h"

long time_at_last_reset_msec = 0;
struct timeval current_time;

void Sleep(int msec)
{
    unsigned long t = TimeNowMSec(); 
    while (TimeNowMSec() - t < (unsigned long) msec) {
        LCD.Update();
    }
}

void Sleep(float sec)
{
    Sleep((int)(sec * 1000));
}

void Sleep(double sec)
{
    Sleep((int)(sec * 1000));
}

double TimeNow()
{
    // This is implicitly casted to a double and retains any decimals
    return TimeNowMSec() / 1000.0;
}

unsigned int TimeNowSec()
{
    // This is implicitly casted to an int, truncating any decimals
    return TimeNowMSec() / 1000.0;
}

unsigned long TimeNowMSec()
{
    gettimeofday(&current_time, NULL);

    // current_time.tv_sec is the number of seconds since Jan 1, 1970 since that is how time
    // is stored on computers. current_time.tv_usec is an offset in micro-seconds to show
    // how many micro-seconds have passed since the last whole second.

    // Return the time since last reset in milli-seconds
    return (current_time.tv_sec * 1000.0) + (current_time.tv_usec / 1000.0) - time_at_last_reset_msec;
}

void ResetTime()
{
    gettimeofday(&current_time, NULL);

    // Reset the "start" time for the various TimeNow functions
    time_at_last_reset_msec = (current_time.tv_sec * 1000.0) + (current_time.tv_usec / 1000.0);
}