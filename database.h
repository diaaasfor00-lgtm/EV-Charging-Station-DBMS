#pragma once
// functions for load and save database
#include "common.h"
#include "Car.h"
#include "carNode.h"
#include "Date.h"
#include "Port.h"
#include "qCar.h"
#include "Station.h"
#include "tCar.h"

void loadStations(struct Station** stationTree); // load data into a station tree
void loadCars(struct tCar** carTree); // load cars from file
void loadPorts(struct Station** stationTree, struct tCar** carTree); // load ports from file
void loadLineOfCars(struct Station** stationTree, struct tCar** carTree); // load line of cars to database
void saveStations(struct Station* stationTree); // save station tree to file
void saveTCars(struct tCar* carTree, struct Station* stationTree); // save car tree to file
