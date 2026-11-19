#pragma once
// helper functions for managing database
#include "common.h"
#include "qCar.h"
#include "tCar.h"
#include "Station.h"
#include "customio.h"
#include "Port.h"
#include "Date.h"

struct Car* createNewCarForStation(struct tCar** carTree, char* license); // create a new Car in database, if license existed, return pointer to the existed car
void assignCar2port(struct Port* port, struct Car* car); // Assign a car to port
struct Port* stopChargingCar(struct Car* pCar); // stop charging a car
struct Car* moveSuitableCarFromQueue2Port(struct Station* stationNode, struct Port* pPort); // move a suitable car from queue to a free port
void displayAllStations(struct Station* stationNode); // display all stations in the sub-tree at stationNode'
void displayCarsAtStation(struct Station* stationNode); // display all cars at station
void reportStationStatistics(struct Station* stationNode); // report station statistics
void releasePortsForStation(struct Station* stationNode); // release ports for a specific station
int removeOutOfOrderPorts(struct Station* stationNode); // remove out of order ports
void prepareToCloseStation(struct Station* stationNode); // prepare to close a station
