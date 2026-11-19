#include "Date.h"

void resetDate(struct Date* date) { // reset date to {0}
    date->year = 0;
    date->month = 0;
    date->day = 0;
    date->hour = 0;
    date->min = 0;
}

void initDate(struct Date* date) { // initialize date = current time
    time_t seconds;
    struct tm tmInfo;
    time(&seconds);
    if (0 != localtime_s(&tmInfo, &seconds)) {
        displayError(ERROR_FAILED_TO_READ_CURRENT_TIME);
        exit(EXIT_FAILURE);
    }
    date->year = tmInfo.tm_year + 1900;
    date->month = tmInfo.tm_mon + 1;
    date->day = tmInfo.tm_mday;
    date->hour = tmInfo.tm_hour;
    date->min = tmInfo.tm_min;
}

long getMinuteDif(struct Date date) { // minutes from date to now
    struct tm tmTemp = { 0 };
    tmTemp.tm_year = date.year - 1900;
    tmTemp.tm_mon = date.month - 1;
    tmTemp.tm_mday = date.day;
    tmTemp.tm_hour = date.hour;
    tmTemp.tm_min = date.min;
    tmTemp.tm_sec = 0;
    tmTemp.tm_isdst = -1;
    time_t seconds = mktime(&tmTemp);
    if (seconds < (time_t)0) {
        return 0;
    }
    return (long)((time(NULL) - seconds) / 60);
}
