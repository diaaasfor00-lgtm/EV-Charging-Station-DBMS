#include "Port.h"
#include "Station.h"
#include "tCar.h"

void freePortList(struct Port ** portsList) { // free ports list
    if (NULL == (*portsList)) {
        return;
    }
	freePortList(&((*portsList)->next));
	free(*portsList);
    *portsList = NULL;
}

void appendPortToPortList(struct Port** portsList, struct Port* newPort) { // append port to ports list
    newPort->next = NULL;
    if (NULL == (*portsList)) {
        *portsList = newPort;
        return;
    }
    struct Port* traveller = *portsList;
    while (NULL != traveller->next) {
        traveller = traveller->next;
    }
    traveller->next = newPort;
}

struct Port* getFreePort(struct Port* ports, enum ChargingRate crate) { // search for a free port with provided charging rate
    if (NULL == ports) {
        return NULL;
    }
    for (; NULL != ports; ports = ports->next) {
        if (PORT_STATUS_FREE == ports->status) {
            if (crate == ports->portType) {
                return ports;
            }
        }
    }
    return NULL;
}

int getMaxPortNum(struct Port* ports) { // get max port num of a ports list
    if (NULL == ports) {
        return 0;
    }
    int maxNum = 0;
    for (; NULL != ports; ports = ports->next) {
        if (ports->num > maxNum) maxNum = ports->num;
    }
    return maxNum;
}

struct Port* createPortOfType(int portNum, enum ChargingRate crate) { // create a new port of type crate, num = portNum
    struct Port* pPort = (struct Port*)malloc(sizeof(struct Port));
    if (NULL == pPort) {
        displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
        exit(EXIT_FAILURE);
    }
    pPort->num = portNum;
    pPort->car2p = NULL;
    pPort->next = NULL;
    pPort->portType = crate;
    pPort->status = PORT_STATUS_FREE;
    resetDate(&(pPort->tin));
    return pPort;
}

void removePortFromPortsList(struct Port** portsList, struct Port* pPort) { // remove a port from ports list, please decrease counter outside
    if ((NULL == portsList) || (NULL == pPort)) {
        return;
    }
    struct Port* pPort1 = pPort1 = *portsList;
    struct Port* pPort2 = NULL;
    for (; NULL != pPort1; pPort2 = pPort1, pPort1 = pPort1->next) {
        if (pPort1 == pPort) {
            if (pPort2 == NULL) {
                *portsList = pPort->next;
            }
            else {
                pPort2->next = pPort->next;
            }
            if (pPort->car2p != NULL) {
                pPort->car2p->pPort = NULL;
            }
            free(pPort);
            return;
        }
    }
}
