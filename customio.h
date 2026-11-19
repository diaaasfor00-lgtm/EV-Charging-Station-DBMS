#pragma once
// custom input output functions
#include "common.h"

void keyboardClear(int isAfterScanf_s); // reset keyboard buffer
void pressAnykey(); // press anykey to continue
int readDouble(double* value, char* mesInfo, char* mesError); // read a double
int readCarLicense(char* license); // read car license
int readStationNameId(char* nameId, struct Station* stationTree, int stationMustExist); // read station name or id
int readChargingRate(enum ChargingRate* crate); // read charging rate 0=SLOW 1=MID 2=FAST
