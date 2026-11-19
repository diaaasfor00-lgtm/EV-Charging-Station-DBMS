#pragma once
#include "common.h"
#include "qCar.h"

struct Station {
	int id; // station id
	char* name; // station name
	int nPorts; // number of ports at the station
	struct Coord coord; // coordinates of the station location
	struct Port* portsList; // linked list of all ports at the station
	int nCars; // number of cars in the queue at the station
	struct qCar carQueue; // queue manager (qCar) for cars in the queue
	struct Station* pLeftStation; // left station
	struct Station* pRightStation; // right station
};

enum StationSearchMode {STATION_SEARCH_ID, STATION_SEARCH_NAME, STATION_SEARCH_DISTANCE, STATION_SEARCH_NAMEID, STATION_SEARCH_CAR}; // search mode while searching station tree
typedef struct Station* (*StationSearchFunctionPtr)(struct Station* stationTree, void* key); // function prototype for searching station tree
struct Station* searchStation(struct Station* stationTree, void* key, enum StationSearchMode searchMode); // function to search station tree

struct DistanceSearchData {
	struct Coord coord;
	double shortest;
};

void initNewStation(struct Station* newStation); // initialize new station
void freeStations(struct Station** stationTree); // free memory of a station tree
void insertNewStation(struct Station** stationTree, struct Station* newStation); // add new station to Station tree

void appendCar2StationQueue(struct Station* station, struct Car* newCar); // add a car to station queue
struct Car* removeCarFromStationQueue(struct Station* stationNode, struct Car* pCar); // remove a car from station queue
struct Port* getCarChargingPort(struct Station* stationNode, char* license); // get pointer to the port of station if car is charging there
int getCarIndexInQueue(struct Station* stationNode, char* license, enum ChargingRate chargingRate); // find index of car in station queue, if not found return -1
int countActivePortByChargingRate(struct Station* stationNode, enum ChargingRate crate); // count active ports of a station by charging rate
struct Station* deleteStationById(struct Station* stationNode, int id); // delete a station from tree by searching for its id
