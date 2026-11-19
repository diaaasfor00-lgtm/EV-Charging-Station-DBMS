#pragma once
// common constants, structs, functions of this project
#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <math.h>    
#include <float.h>

// boolean constants
#define TRUE 1
#define FALSE 0

// port status constants
#define PORT_STATUS_OCCUPIED 1
#define PORT_STATUS_FREE 2
#define PORT_STATUS_OUTOFORDER 3

// car status constants
#define CAR_STATUS_NOTINQUEUE 0
#define CAR_STATUS_INQUEUE 1

// error constants
#define ERROR_FAILED_TO_ALLOCATE_MEMORY 1
#define ERROR_FAILED_TO_UPDATE_FILES 2
#define ERROR_FAILED_TO_READ_CURRENT_TIME 3

// contants for file handling
#define FILE_INPUT_STATIONS "Stations.txt"
#define FILE_INPUT_CARS "Cars.txt"
#define FILE_INPUT_PORTS "Ports.txt"
#define FILE_INPUT_LINEOFCARS "LineOfCars.txt"
#define FILE_OUTPUT_STATIONS "Stations.txt"
#define FILE_OUTPUT_CARS "Cars.txt"
#define FILE_OUTPUT_PORTS "Ports.txt"
#define FILE_OUTPUT_LINEOFCARS "LineOfCars.txt"
#define FILE_READING_BUFFER_SIZE 1024

// common messages
#define CMES_NO_STATION_IN_DATABASE "There are no stations in the database.\n"
#define CMES_NO_CAR_IN_DATABASE "There are no cars in the database."
#define CMES_NO_RESULT_FOUND "No result found."
#define CMES_R_TO_RETRY "Press R to reenter. Pressing another key will return to menu..."
#define CMES_CAR_ALREADY_IN_QUEUE "This car is queuing at station number %d.\n"
#define CMES_CAR_ALREADY_CHARGING "This car is charging at station number %d.\n"
#define CMES_STATION_NOT_FOUND "The specified station could not be found.\n"
#define CMES_LICENSE_OF_NEW_CAR "New car found. Please declare information for the car.\n"
#define CMES_CAR_LICENSE_NOT_FOUND "There is no car with this license in the database.\n"
#define CMES_CAR_NOT_CHARGING "This car is not charging at any station.\n"
#define CMES_NO_SUITABLE_CAR "No suitable car in queue, port (num %d) is free now.\n"
#define CMES_CAR_ADVANCED "Car (license %s) advanced from queue to port (num %d)\n"
#define CMES_ASSIGNCAR2PORT "Car (license %s) has connected to port (num %d)\n"
#define CMES_CAR_DISCONNECTED "Car (license %s) disconnected from port (num %d)\n"

// others
#define KEYBOARD_BUFFER 256
#define TOP_CUSTOMERS_COUNT 5

enum ChargingRate {SLOW, MID, FAST}; // charging rate of Cars and Ports
extern const char* const ChargingRateName[]; // name of charging rates

struct Coord { // struct to store position of station and car
	double x;
	double y;
};

void displayError(int errorCode); // display error code

enum ChargingRate getChargingRateFromString(char* strChargingRate); // get charging rate from its name

// String helper functions
void toUpperCase(char* str); // make a string upper
void trimString(char* str); // trim a string (left & right)
