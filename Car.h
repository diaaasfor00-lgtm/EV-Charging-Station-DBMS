#pragma once
#include "common.h"

struct Port; // forward declaration for Port

struct Car {
    char nLicense[10]; // unique license number (a 9-digit string that includes digits only)
    enum ChargingRate portType; // charging rate
    double totalPayed; // total payment
    struct Port* pPort; // pointer to a Port while charging, NULL for not charging
    int inqueue; // 0: not in queue, 1: in queue
};
