#pragma once
#include "common.h"
#include "Car.h"

struct carNode { // struct for queue node at a station
	struct Car* pCar; // a Car
	struct carNode* pNextCarNode; // next node in the queue
};
