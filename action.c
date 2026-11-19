#include "action.h"

struct Car* createNewCarForStation(struct tCar** carTree, char* license) { // create a new Car in database, if license existed, return pointer to the existed car
    struct Car* pCar = searchCar(*carTree, license);
    if (NULL != pCar) {
        return pCar; // existed, note: in this case, car may not suitable for stationNode
    }
    printf(CMES_LICENSE_OF_NEW_CAR);
    enum ChargingRate crate;
    if (!readChargingRate(&crate)) {
        return NULL;
    }
    pCar = (struct Car*)malloc(sizeof(struct Car));
    struct tCar* pTCar = (struct tCar*)malloc(sizeof(struct tCar));
    if ((NULL == pCar) || (NULL == pTCar)) {
        displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
        exit(EXIT_FAILURE);
    }
    strcpy_s(pCar->nLicense, 10, license);
    pCar->portType = crate;
    pCar->totalPayed = 0.0;
    pCar->pPort = NULL;
    pCar->inqueue = CAR_STATUS_NOTINQUEUE;
    pTCar->pCar = pCar;
    pTCar->pLTCar = NULL;
    pTCar->pRTCar = NULL;
    insertTCar(&(*carTree), pTCar);
    return pCar;
}

void assignCar2port(struct Port* port, struct Car* pCar) { // Assign a car to port
    if ((NULL == port) || (NULL == pCar)) {
        return;
    }
    if ((NULL != port->car2p) || (NULL != pCar->pPort) || (port->portType != pCar->portType) || (PORT_STATUS_FREE != port->status)) {
        return;
    }
    pCar->inqueue = CAR_STATUS_NOTINQUEUE;
    pCar->pPort = port;
    port->car2p = pCar;
    port->status = PORT_STATUS_OCCUPIED;
    initDate(&(port->tin));
}

struct Port* stopChargingCar(struct Car* pCar) { // stop charging a car
    if (NULL == pCar) {
        return NULL;
    }
    struct Port* pPort = pCar->pPort;
    if (NULL == pPort) {
        return NULL;
    }
    pCar->pPort = NULL;
    pCar->inqueue = CAR_STATUS_NOTINQUEUE;
    pPort->car2p = NULL;
    pPort->status = PORT_STATUS_FREE;
    return pPort;
}

struct Car* moveSuitableCarFromQueue2Port(struct Station* stationNode, struct Port* pPort) { // move a suitable car from queue to a free port
    if ((NULL == stationNode) || (NULL == pPort)) {
        return NULL;
    }
    if (PORT_STATUS_FREE != pPort->status) {
        return NULL;
    }
    struct carNode* acarNode = NULL;
    struct Car* pCar = NULL;
    for (acarNode = stationNode->carQueue.pFrontNode; NULL != acarNode; acarNode = acarNode->pNextCarNode) {
        if (acarNode->pCar->portType == pPort->portType) { // suitable
            pCar = acarNode->pCar;
            removeCarFromStationQueue(stationNode, pCar);
            pCar->inqueue = CAR_STATUS_NOTINQUEUE;
            pCar->pPort = pPort;
            pPort->car2p = pCar;
            pPort->status = PORT_STATUS_OCCUPIED;
            initDate(&(pPort->tin));
            return pCar;
        }
    }
    return NULL; // no suitable car
}

void displayAllStations(struct Station* stationNode) { // display all stations in the sub-tree at stationNode'
    if (NULL == stationNode) {
        return;
    }
    int occupiedCnt = 0;
    int crSlowCnt = 0;
    int crMidCnt = 0;
    int crFastCnt = 0;
    int faultyCnt = 0;
    for (struct Port* pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
        if (PORT_STATUS_OUTOFORDER == pPort->status) {
            faultyCnt++;
        }
        else {
            if (FAST == pPort->portType) {
                crFastCnt++;
            }
            else if (pPort->portType == MID) {
                crMidCnt++;
            }
            else {
                crSlowCnt++;
            }
            if (PORT_STATUS_OCCUPIED == pPort->status) {
                occupiedCnt++;
            }
        }
    }
    printf("Station:                %s (ID %d)\n", stationNode->name, stationNode->id);
    printf("     Ports:             %d\n", crFastCnt+ crMidCnt+crSlowCnt);
    printf("          FAST:         %d\n          MID:          %d\n          SLOW:         %d\n", crFastCnt, crMidCnt, crSlowCnt);
    printf("          Faulty:       %d\n          Occupied:     %d\n          Free:         %d\n", faultyCnt, occupiedCnt, crFastCnt+crMidCnt+crSlowCnt- faultyCnt- occupiedCnt);
    printf("     Charging cars:     %d\n", occupiedCnt);
    printf("     Waiting cars:      %d\n", stationNode->nCars);
    displayAllStations(stationNode->pLeftStation); // travel left
    displayAllStations(stationNode->pRightStation); // travel right
}

void displayCarsAtStation(struct Station* stationNode) { // display all cars at station
    if (NULL == stationNode) {
        return;
    }
    printf("Station:                      %s (ID %d)\n", stationNode->name, stationNode->id);
    struct Port* pPort;
    int count = 0;
    for (pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) if (NULL != pPort->car2p) count++;
    printf("Charging cars:                %d\n", count);
    for (pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
        if (NULL != pPort->car2p) {
            printf("     Car license:             %s\n          Type:               %s\n          Minutes:            %ld\n",
                pPort->car2p->nLicense,
                ChargingRateName[pPort->portType],
                getMinuteDif(pPort->tin));
        }
    }
    count = 0;
    struct carNode* acarNode;
    int chargeTypeCount[3] = {0};
    for (acarNode = stationNode->carQueue.pFrontNode; NULL != acarNode; acarNode = acarNode->pNextCarNode) count++;
    printf("Queuing cars:                 %d\n", count);
    for (acarNode = stationNode->carQueue.pFrontNode; NULL != acarNode; acarNode = acarNode->pNextCarNode) {
        chargeTypeCount[acarNode->pCar->portType]++;
        printf("     Car license:             %s\n          Type:               %s\n          Queue position:     %d\n",
            acarNode->pCar->nLicense,
            ChargingRateName[acarNode->pCar->portType],
            chargeTypeCount[acarNode->pCar->portType]);
    }
}

void reportStationStatistics(struct Station* stationNode) { // report station statistics
    if (NULL == stationNode) {
        return;
    }
    int totalCnt = 0;
    int occupiedCnt = 0;
    int faultyCnt = 0;
    for (struct Port* pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
        totalCnt++;
        if (PORT_STATUS_OUTOFORDER == pPort->status) {
            faultyCnt++;
        }
        else if (PORT_STATUS_OCCUPIED == pPort->status) {
            occupiedCnt++;
        }
    }
    if (0 == totalCnt) {
        printf("Station: %s (ID %d)\n", stationNode->name, stationNode->id);
        printf("This station has no ports, statistics are skipped.\n");
    }
    else {
        printf("Station:                    %s (ID %d)\n", stationNode->name, stationNode->id);
        printf("Port-utilization rate:      %.2lf %%\n", (double)occupiedCnt * 100.0 / (double)totalCnt);
        printf("Faulty-port percentage:     %.2lf %%\n", (double)faultyCnt * 100.0 / (double)totalCnt);
        printf("Load level:                 ");
        if (0 == stationNode->nCars) {
            printf("No load");
        }
        else {
            double ratio = (double)occupiedCnt / (double)(stationNode->nCars);
            if (fabs(ratio-1.0)<0.01) {
                printf("Balanced");
            }
            else if (ratio < 0.2) {
                printf("Very heavy load");
            }
            else if (ratio > 1) {
                printf("No load");
            }
            else {
                printf("Busy");
            }
        }
        printf("\n");
    }
}

void releasePortsForStation(struct Station* stationNode) { // release ports for a specific station
    int found = FALSE;
    struct Port* pPort;
    for (pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
        if (PORT_STATUS_OCCUPIED == pPort->status) {
            if (getMinuteDif(pPort->tin) > 600) { // 10 hours'
                struct Car* pCar = pPort->car2p;
                stopChargingCar(pCar);
                resetDate(&(pPort->tin));
                printf(CMES_CAR_DISCONNECTED, pCar->nLicense, pPort->num);
                pCar = moveSuitableCarFromQueue2Port(stationNode, pPort);
                if (NULL == pCar) {
                    printf(CMES_NO_SUITABLE_CAR, pPort->num);
                }
                else {
                    assignCar2port(pPort, pCar);
                    printf(CMES_CAR_ADVANCED, pCar->nLicense, pPort->num);
                }
                found = TRUE;
            }
        }
    }
    if (!found) {
        printf("No port meets release conditions\n");
    }
}

int removeOutOfOrderPorts(struct Station* stationNode) { // remove out of order ports
    int done = TRUE;
    struct Port* pPort = NULL;
    printf("List of Out-of-order ports: ");
    for (pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
        if (PORT_STATUS_OUTOFORDER == pPort->status) {
            done = FALSE;
            printf("%d ", pPort->num);
        }
    }
    if (done) {
        printf("None\n");
        pressAnykey();
        return FALSE;
    }
    printf("\n");
    int portNum;
    while (!done) {
        printf("Port number to remove = ");
        if (0 == scanf_s("%d", &portNum)) portNum = 0;
        keyboardClear(TRUE);
        for (pPort = stationNode->portsList; NULL != pPort; pPort = pPort->next) {
            if (pPort->num == portNum) {
                if (PORT_STATUS_OUTOFORDER == pPort->status) {
                    removePortFromPortsList(&(stationNode->portsList), pPort);
                    stationNode->nPorts--;
                    done = 1;
                    break;
                }
            }
        }
        if (!done) {
            printf("No out-of-order port has number = %d\n", portNum);
            printf(CMES_R_TO_RETRY);
            int ch = _getch();
            keyboardClear(FALSE);
            if (('r' != ch) && ('R' != ch)) {
                return FALSE;
            }
            printf("\n");
        }
    }
    printf("Out-of-order port (num %d) successfully removed\n", portNum);
    pressAnykey();
    return TRUE;
}

void prepareToCloseStation(struct Station* stationNode) { // prepare to close a station
    for (; NULL != stationNode->carQueue.pFrontNode;) {
        struct Car* pCar = stationNode->carQueue.pFrontNode->pCar;
        removeCarFromStationQueue(stationNode, pCar);
    }
    for (; NULL != stationNode->portsList;) {
        removePortFromPortsList(&(stationNode->portsList), stationNode->portsList);
    }
}
