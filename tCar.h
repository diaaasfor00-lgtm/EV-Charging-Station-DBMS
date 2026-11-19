#pragma once
#include "common.h"
#include "Car.h"

struct tCar { // struct for BST of cars database
	struct Car* pCar; // a car
	struct tCar* pRTCar; // right tCar in the tree
	struct tCar* pLTCar; // left tCar in the tree
};

void loadCars(struct tCar** carTree); // load data into a car tree
void freeTCar(struct tCar** carTree); // free memory of a car tree
struct Car* searchCar(struct tCar* carTree, char* license); // search car by license
struct tCar* deleteCarByLicense(struct tCar* atCar, char* license); // delete car by license
void insertTCar(struct tCar** carTree, struct tCar* newTCar); // insert a new car to car tree
void findTopCustomers(struct tCar* atCar, struct Car* topCustomers[]); // find top customers
