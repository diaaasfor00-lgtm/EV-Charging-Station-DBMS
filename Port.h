#pragma once
#include "common.h"
#include "Date.h"

struct Car; // forward declaration for Car

struct Port {
	int num; // port number
	enum ChargingRate portType; // charging rate of the port
	int status; // 1: occupied, 2: free, 3: out of order
	struct Car* car2p; // the Car being charged at the port, NULL if no car connected
	struct Date tin; // start time of charging
	struct Port* next; // next charging port in the list
};

void freePortList(struct Port** portsList); // free port linked list
void appendPortToPortList(struct Port** portsList, struct Port* newPort); // append port to ports list
struct Port* getFreePort(struct Port* ports, enum ChargingRate crate); // search for a free port with provided charging rate
int getMaxPortNum(struct Port* ports); // get max port num of a ports list
struct Port* createPortOfType(int portNum, enum ChargingRate crate); // create a new port of type crate, num = portNum
void removePortFromPortsList(struct Port** portsList, struct Port* pPort); // remove a port from ports list, please decrease counter outside
