#include "common.h"

String timeToString(time_t t)
{
    char time[21];
    tm* currentTime(localtime(&t));
    sprintf(time, "%u.%u.%u %02u:%02u:%02u\n",
        currentTime->tm_mday, currentTime->tm_mon + 1, currentTime->tm_year + 1900,
        currentTime->tm_hour, currentTime->tm_min, currentTime->tm_sec);
    return time;
}
