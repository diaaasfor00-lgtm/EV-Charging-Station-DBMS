// common constants, structs, functions of this project
#include "common.h"

const char* const ChargingRateName[] = {"SLOW", "MID", "FAST"};

const char* const errMes[] = {
    "Success",
    "Failed to allocate memory",
    "Failed to read current time",
    "Failed to update files",
    "Unknown error(s) occurred"
};

void displayError(int errorCode) // display error code
{
	printf(errMes[errorCode]);
}

void toUpperCase(char* str) { // make a string upper
    if (NULL == str) {
        return; // Handle null pointer case
    }
    for (int i = 0; str[i] != '\0'; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

void trimString(char* str) { // trim a string (left & right)
    if (NULL == str) {
        return; // Handle null pointer case
    }
    size_t len = strlen(str);
    if (0 == len) {
        return; // empty string
    }
    size_t start = 0;
    size_t end = len-1;
    while ((start < len) && (str[start] <= ' ')) {
        start++;
    }
    while ((end >= 0) && (str[end] <= ' ')) {
        end--;
    }
    if (end < start) {
        str[0] = 0; // empty string after trimming
        return;
    }
    size_t i;
    for (i = start; i <= end; i++) {
        str[i - start] = str[i];
    }
    str[i - start] = 0;
}

enum ChargingRate getChargingRateFromString(char* strChargingRate) { // get charging rate from its name
    toUpperCase(strChargingRate);
    for (int i = SLOW; i <= FAST; i++) {
        if (strcmp(strChargingRate, ChargingRateName[i]) == 0) {
            return i;
        }
    }
    return -1; // Note: handle with care that -1 indicate error
}