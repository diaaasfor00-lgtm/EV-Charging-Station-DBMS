#pragma once
#include "common.h"

#include "carNode.h"

struct qCar { // queue of cars
	struct carNode* pFrontNode; // front of the queue
	struct carNode* pRearNode; // rear of the queue
};

void freeQCar(struct qCar* aQCar); // free a qCar
void initQCar(struct qCar* aQCar); // initialize a qCar
void appendCar2QCar(struct qCar* aQCar, struct Car* newCar); // append a car to a qCar queue
