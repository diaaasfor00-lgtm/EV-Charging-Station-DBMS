#pragma once
#include "common.h"

struct Date {
    int year; // valid from 1900 up to the current year
    int month; // 1 to 12
    int day; // 1 to 31
    int hour; // 0 to 23
    int min; // 0 to 59
};

void resetDate(struct Date* date); // reset date to {0}
void initDate(struct Date* date); // initialize date = current time
long getMinuteDif(struct Date date); // minutes from date to now
