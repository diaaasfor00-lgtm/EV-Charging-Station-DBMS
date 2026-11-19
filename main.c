#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "database.h"
#include "customio.h"
#include "action.h"

void showMenu();
void loadFiles(struct Station** stationTree, struct tCar** carTree);
void updateFiles(struct Station** stationTree, struct tCar** carTree);
void locNearSt(struct Station** stationTree); // Locate nearest station
int chargeCar(struct Station** stationTree, struct tCar** carTree); // Charge car
void checkCarStatus(struct tCar** carTree, struct Station** stationTree); // Check car status
int stopCharge(struct tCar** carTree, struct Station** stationTree); // Stop charge
void dispAllSt(struct Station** stationTree); // Display all stations
void dispCarsAtSt(struct Station** stationTree); // Display cars at station
void reportStStat(struct Station** stationTree); // Report stations statistics
void dispTopCustomers(struct tCar** carTree); // Display top customers
int addNewPort(struct Station** stationTree); // Add new port
int releasePorts(struct Station** stationTree); // Release charging ports
int remOutOrderPort(struct Station** stationTree); // Remove out of order port
int remCustomer(struct tCar** carTree); // Remove customer
int closeSt(struct Station** stationTree); // Close station

int main() {
    struct Station* stationTree = NULL;
    struct tCar* carTree = NULL;
    loadFiles(&stationTree, &carTree);
    int menuSelection;
    do {
        showMenu();
        scanf_s("%d", &menuSelection);
        char ch = getchar(); // clear enter
        int shouldUpdateFiles = 0;
        switch (menuSelection) {
        case 0:
            // do nothing, keep it!
            break;
        case 1:
            locNearSt(&stationTree);
            break;
        case 2:
            shouldUpdateFiles = chargeCar(&stationTree, &carTree);
            break;
        case 3:
            checkCarStatus(&carTree, &stationTree);
            break;
        case 4:
            shouldUpdateFiles = stopCharge(&carTree, &stationTree);
            break;
        case 5:
            dispAllSt(&stationTree);
            break;
        case 6:
            dispCarsAtSt(&stationTree);
            break;
        case 7:
            reportStStat(&stationTree);
            break;
        case 8:
            dispTopCustomers(&carTree);
            break;
        case 9:
            shouldUpdateFiles = addNewPort(&stationTree);
            break;
        case 10:
            shouldUpdateFiles = releasePorts(&stationTree);
            break;
        case 11:
            shouldUpdateFiles = remOutOrderPort(&stationTree);
            break;
        case 12:
            shouldUpdateFiles = remCustomer(&carTree);
            break;
        case 13:
            shouldUpdateFiles = closeSt(&stationTree);
            break;
        default:
            printf("Selection not supported!");
            break;
        }
        if (shouldUpdateFiles) {
            updateFiles(&stationTree, &carTree);
        }
    } while (0 != menuSelection);
    printf("Saving data to files then exit!\n");
    updateFiles(&stationTree, &carTree);
    freeStations(&stationTree);
    freeTCar(&carTree);
    return 0;
}

void showMenu() { // show menu to user
    printf("\n\n------------------------------\nMenu\n");
    printf("1. Locate nearest station\n");
    printf("2. Charge car\n");
    printf("3. Check car status\n");
    printf("4. Stop charge\n");
    printf("5. Display all stations\n");
    printf("6. Display cars at station\n");
    printf("7. Report stations statistics\n");
    printf("8. Display top customers\n");
    printf("9. Add new port\n");
    printf("10. Release charging ports\n");
    printf("11. Remove out of order port\n");
    printf("12. Remove customer\n");
    printf("13. Close station\n");
    printf("0. Exit system\n");
    printf("Please enter a number (0..13): ");
}

void loadFiles(struct Station** stationTree, struct tCar** carTree) { // load database
    loadStations(&(*stationTree));
    loadCars(&(*carTree));
    loadPorts(&(*stationTree), &(*carTree));
    loadLineOfCars(&(*stationTree), &(*carTree));
}

void updateFiles(struct Station** stationTree, struct tCar** carTree) { // save database
    saveStations(*stationTree);
    saveTCars(*carTree, *stationTree);
}

 // __________Locate_nearest_station__________

void locNearSt(struct Station** stationTree) // Locate nearest station
{
    if (NULL == (*stationTree)) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return;
    }
    double ux, uy;
    if (!readDouble(&ux, "x = ", "x must be a double\n")) {
        return;
    }
    if (!readDouble(&uy, "y = ", "y must be a double\n")) {
        return;
    }
    const struct Station* nearestStation = NULL;
    struct DistanceSearchData searchData;
    searchData.coord.x = ux;
    searchData.coord.y = uy;
    searchData.shortest = DBL_MAX;
    nearestStation = searchStation(*stationTree, (void*)(&searchData), STATION_SEARCH_DISTANCE);
    if (NULL == nearestStation) {
        printf(CMES_NO_RESULT_FOUND);
        return;
    }
    int active = 0;
    int slowCnt = 0, midCnt = 0, fastCnt = 0;
    int occupied = 0;
    for (struct Port* p = nearestStation->portsList; p; p = p->next) {
        if (PORT_STATUS_OUTOFORDER != p->status) {
            active++;
            if (p->portType == SLOW) {
                slowCnt++;
            }
            else if (p->portType == MID) {
                midCnt++;
            }
            else {
                fastCnt++;
            }
            if (PORT_STATUS_OCCUPIED != p->status) {
                occupied++;
            }
        }
    }
    printf("\nNearest station:  %s (ID %d)\n", nearestStation->name, nearestStation->id);
    printf("Distance to you:  %.2lf\n", searchData.shortest);
    printf("Active ports:     %d  (FAST %d | MID %d | SLOW %d)\n", active, fastCnt, midCnt, slowCnt);
    printf("Ports occupied:   %d\n", occupied);
    printf("Cars in queue:    %d\n\n", nearestStation->nCars);
    pressAnykey();
}

// __________Charge_car__________

int chargeCar(struct Station** stationTree, struct tCar** carTree) { // Charge car
    if (NULL == (*stationTree)) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return FALSE;
    }
    char license[KEYBOARD_BUFFER];
    char nameId[KEYBOARD_BUFFER];
    if (!readCarLicense(license)) {
        return FALSE; // can't get license
    }
    struct Station* stationNode = searchStation(*stationTree, license, STATION_SEARCH_CAR);
    if (NULL != stationNode) {
        if (0 <= getCarIndexInQueue(stationNode, license, SLOW)) {
            printf(CMES_CAR_ALREADY_IN_QUEUE, stationNode->id);
        }
        else {
            printf(CMES_CAR_ALREADY_CHARGING, stationNode->id);
        }
        pressAnykey();
        return FALSE; // car at another station
    }
    if (!readStationNameId(nameId, *stationTree, TRUE)) {
        return FALSE; // can't get station
    }
    struct Car* pCar = createNewCarForStation(carTree, license);
    if (NULL == pCar) {
        return FALSE; // can't create new car
    }
    stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID); // always found
    if (0 == countActivePortByChargingRate(stationNode, pCar->portType)) {
        printf("Unfortunately there is no active port supports this car's charging rate.\n");
        pressAnykey();
        return TRUE;
    }
    struct Port* pPort = getFreePort(stationNode->portsList, pCar->portType);
    if (NULL != pPort) {
        assignCar2port(pPort, pCar);
        printf(CMES_ASSIGNCAR2PORT, license, pPort->num);
    }
    else {
        appendCar2StationQueue(stationNode, pCar);
        pCar->inqueue = CAR_STATUS_INQUEUE;
        printf("No free port. Car (%s) has been appended to station's queue\n", license);
    }
    pressAnykey();
    return TRUE;
}

// __________Check_car_status__________

void checkCarStatus(struct tCar** carTree, struct Station** stationTree) { // Check car status
    char license[KEYBOARD_BUFFER];
    if (!readCarLicense(license)) {
        return;
    }
    struct Car* pCar = searchCar(*carTree, license);
    if (NULL == pCar) {
        printf(CMES_CAR_LICENSE_NOT_FOUND);
    }
    else {
        struct Station* stationNode = searchStation(*stationTree, license, STATION_SEARCH_CAR);
        if (NULL == stationNode) {
            printf("The car is not charging or queuing at any station.\n");
        }
        else {
            if (NULL == pCar->pPort) {
                printf("The car is queuing.\nStation:             %s (ID %d)\nIndex in queue:      %d (%s)\n", stationNode->name, stationNode->id, getCarIndexInQueue(stationNode, license, pCar->portType), ChargingRateName[pCar->portType]);
            }
            else {
                long minDif = getMinuteDif(pCar->pPort->tin);
                printf("The car is charging.\nStation:          %s (ID %d)\nPort number:      %d\nMinutes:          %ld\n", stationNode->name, stationNode->id,pCar->pPort->num, minDif);
            }
        }
    }
    pressAnykey();
}

// __________Stop_charge__________

int stopCharge(struct tCar** carTree, struct Station** stationTree) { // Stop charge
    char license[KEYBOARD_BUFFER];
    if (!readCarLicense(license)) {
        return FALSE;
    }
    struct Car* pCar = searchCar(*carTree, license);
    if (NULL == pCar) {
        printf(CMES_CAR_LICENSE_NOT_FOUND);
    }
    else {
        if (NULL == pCar->pPort) {
            printf(CMES_CAR_NOT_CHARGING);
        }
        else {
            struct Station* stationNode = searchStation(*stationTree, license, STATION_SEARCH_CAR); // never NULL
            struct Port* pPort = stopChargingCar(pCar);
            double paid = (double)getMinuteDif(pPort->tin) * 1.2;
            resetDate(&(pPort->tin));
            pCar->totalPayed += paid;
            printf("Car (%s) stopped charging\n", license);
            printf("     Station:              %s (ID %d)\n     Port:                 %d\n", stationNode->name, stationNode->id, pPort->num);
            printf("     Fee:                  %.2lf ILS\n     Total payed so far:   %.2lf ILS\n", paid, pCar->totalPayed);
            pCar = moveSuitableCarFromQueue2Port(stationNode, pPort);
            if (NULL != pCar) {
                assignCar2port(pPort, pCar);
                printf(CMES_CAR_ADVANCED, pCar->nLicense, pPort->num);
            }
            else {
                printf(CMES_NO_SUITABLE_CAR, pPort->num);
            }
            pressAnykey();
            return TRUE;
        }
    }
    pressAnykey();
    return FALSE;
}

// __________Display_all_stations__________

void dispAllSt(struct Station** stationTree) { // Display all stations
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
    }
    else {
        displayAllStations(*stationTree);
    }
    pressAnykey();
}

// __________Display_cars_at_station__________

void dispCarsAtSt(struct Station** stationTree) { // Display cars at station
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
    }
    else {
        char nameId[KEYBOARD_BUFFER];
        if (!readStationNameId(nameId, *stationTree, FALSE)) {
            return;
        }
        struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
        if (NULL == stationNode) {
            printf(CMES_STATION_NOT_FOUND);
        }
        else {
            displayCarsAtStation(stationNode);
        }
    }
    pressAnykey();
}

// __________Report_stations_statistics__________

void reportStStat(struct Station** stationTree) { // Report stations statistics
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
    }
    else {
        char nameId[KEYBOARD_BUFFER];
        if (!readStationNameId(nameId, *stationTree, FALSE)) {
            return;
        }
        struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
        if (NULL == stationNode) {
            printf(CMES_STATION_NOT_FOUND);
        }
        else {
            reportStationStatistics(stationNode);
        }
    }
    pressAnykey();
}

// __________Display_top_customers__________

void dispTopCustomers(struct tCar** carTree) { // Display top customers
    if (NULL == *carTree) {
        printf("There are no customers in the database.\n");
    }
    else {
        struct Car* topCustomers[TOP_CUSTOMERS_COUNT] = {NULL};
        findTopCustomers(*carTree, topCustomers);
        printf("***** Top Customers *****\n");
        for (int i = 0; i < TOP_CUSTOMERS_COUNT; i++) {
            if (NULL != topCustomers[i]) {
                printf("%d. Car (License %s)     Total Payed: %.2lf ILS\n", i+1, topCustomers[i]->nLicense, topCustomers[i]->totalPayed);
            }
        }
    }
    pressAnykey();
}

// __________Add_new_port__________

int addNewPort(struct Station** stationTree) { // Add new port
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return FALSE;
    }
    char nameId[KEYBOARD_BUFFER];
    if (!readStationNameId(nameId, *stationTree, FALSE)) {
        return FALSE;
    }
    struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
    if (NULL == stationNode) {
        printf(CMES_STATION_NOT_FOUND);
        pressAnykey();
        return FALSE;
    }
    enum ChargingRate crate;
    if (!readChargingRate(&crate)) {
        return FALSE;
    }
    printf("Station: %s (ID %d)\n", stationNode->name, stationNode->id);
    struct Port* pPort = createPortOfType(getMaxPortNum(stationNode->portsList)+1, crate);
    appendPortToPortList(&(stationNode->portsList), pPort);
    stationNode->nPorts++;
    printf("New port of type %s has been added with num = %d\n", ChargingRateName[crate], pPort->num);
    struct Car* pCar = moveSuitableCarFromQueue2Port(stationNode, pPort);
    if (NULL != pCar) {
        assignCar2port(pPort, pCar);
        printf(CMES_ASSIGNCAR2PORT, pCar->nLicense, pPort->num);
    }
    else {
        printf("No suitable car is queuing, the new port stays unoccupied.\n");
    }
    pressAnykey();
    return TRUE;
}

// __________Release_charging_ports__________

int releasePorts(struct Station** stationTree) { // Release charging ports
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return FALSE;
    }
    char nameId[KEYBOARD_BUFFER];
    if (!readStationNameId(nameId, *stationTree, FALSE)) {
        return FALSE;
    }
    struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
    if (NULL == stationNode) {
        printf(CMES_STATION_NOT_FOUND);
        pressAnykey();
        return FALSE;
    }
    printf("Station: %s (ID %d)\n", stationNode->name, stationNode->id);
    releasePortsForStation(stationNode);
    pressAnykey();
    return TRUE;
}

// __________Remove_out_of_order_port__________

int remOutOrderPort(struct Station** stationTree) { // Remove out of order port
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return FALSE;
    }
    char nameId[KEYBOARD_BUFFER];
    if (!readStationNameId(nameId, *stationTree, FALSE)) {
        return FALSE;
    }
    struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
    if (NULL == stationNode) {
        printf(CMES_STATION_NOT_FOUND);
        pressAnykey();
        return FALSE;
    }
    printf("Station: %s (ID %d)\n", stationNode->name, stationNode->id);
    return removeOutOfOrderPorts(stationNode);
}

// __________Remove_customer__________

int remCustomer(struct tCar** carTree) { // Remove customer
    char license[KEYBOARD_BUFFER];
    if (!readCarLicense(license)) {
        return FALSE;
    }
    struct Car* pCar = searchCar(*carTree, license);
    if (NULL == pCar) {
        printf(CMES_CAR_LICENSE_NOT_FOUND);
        pressAnykey();
        return FALSE;
    }
    else {
        if (NULL != pCar->pPort) {
            printf("Car (license %s) is charging. Can't remove it at the moment.\n", license);
        }
        else if (CAR_STATUS_INQUEUE == pCar->inqueue) {
            printf("Car (license %s) is queuing. Can't remove it at the moment.\n", license);
        }
        else {
            *carTree = deleteCarByLicense(*carTree, license);
            printf("Car (license %s) is successfully removed.\n", license);
        }
    }
    pressAnykey();
    return TRUE;
}

// __________Close_station__________

int closeSt(struct Station** stationTree) { // Close station
    if (NULL == *stationTree) {
        printf(CMES_NO_STATION_IN_DATABASE);
        pressAnykey();
        return FALSE;
    }
    char nameId[KEYBOARD_BUFFER];
    if (!readStationNameId(nameId, *stationTree, FALSE)) {
        return FALSE;
    }
    struct Station* stationNode = searchStation(*stationTree, nameId, STATION_SEARCH_NAMEID);
    if (NULL == stationNode) {
        printf(CMES_STATION_NOT_FOUND);
        pressAnykey();
        return FALSE;
    }
    printf("Station: %s (ID %d)\n", stationNode->name, stationNode->id);
    printf("Preparing to close station...\n");
    prepareToCloseStation(stationNode);
    printf("All cars in queue have been removed\n");
    printf("All ports have been closed\n");
    printf("Station %s (ID %d) has been closed\n", stationNode->name, stationNode->id);
    *stationTree = deleteStationById(*stationTree, stationNode->id);
    pressAnykey();
    return TRUE;
}
