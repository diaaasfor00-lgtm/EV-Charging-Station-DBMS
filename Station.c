#include "common.h"
#include "Station.h"
#include "Port.h"
#include "tCar.h"

// array of function pointers for searching station tree
#define NUM_OF_STATION_SEARCH_MODES 5
struct Station* searchStationById(struct Station* stationTree, void* key); // search by Station ID
struct Station* searchStationByName(struct Station* stationTree, void* key); // search by Station name
struct Station* searchStationByDistance(struct Station* stationTree, void* key); // search by Station distance
struct Station* searchStationByNameId(struct Station* stationTree, void* key); // search for station using both name and ID
struct Station* searchStationByCarLicense(struct Station* stationTree, void* key); // search for station that the Car is in queue/charging
StationSearchFunctionPtr stationSearchFunctions[] = {
   searchStationById,
   searchStationByName,
   searchStationByDistance,
   searchStationByNameId,
   searchStationByCarLicense
};

void freeStations(struct Station** stationTree) { // free memory of station tree
    if (NULL == (*stationTree)) {
        return;
    }
    freeStations(&((*stationTree)->pLeftStation));
    freeStations(&((*stationTree)->pRightStation));
    if (NULL != (*stationTree)->portsList) {
        freePortList(&((*stationTree)->portsList));
    }
    if (NULL != (*stationTree)->name) {
        free((*stationTree)->name);
    }
    freeQCar(&((*stationTree)->carQueue));
    free(*stationTree);
    *stationTree = NULL;
}

void initNewStation(struct Station *newStation) { // initialize new station
    newStation->portsList = NULL;
    newStation->nCars = 0;
    initQCar(&(newStation->carQueue));
    newStation->pLeftStation = NULL;
    newStation->pRightStation = NULL;

}

void insertNewStation(struct Station** stationTree, struct Station* newStation) { // insert new station to Station tree
    if (NULL == *stationTree) {
        newStation->pLeftStation = NULL;
        newStation->pRightStation = NULL;
        *stationTree = newStation;
        return;
    }
    if ((*stationTree)->id == newStation->id) {
        return; // duplicate id
    }
    if ((*stationTree)->id > newStation->id) {
        insertNewStation(&((*stationTree)->pLeftStation), newStation);
        return;
    }
    insertNewStation(&((*stationTree)->pRightStation), newStation);
}

struct Station* searchStationById(struct Station* stationTree, void* key) { // search by station Id
    if (NULL == stationTree) {
        return NULL;
    }
    int stationId = *(int*)key;
    if (stationId < stationTree->id) { // find in left branch
        return searchStationById(stationTree->pLeftStation, key);
    }
    else if (stationId > stationTree->id) { // find in right branch
        return searchStationById(stationTree->pRightStation, key);
    }
    else {
        return stationTree; // found
    }
}

struct Station* searchStationByName(struct Station* stationTree, void* key) { // search by station name
    if (NULL == stationTree) {
        return NULL;
    }
    char* stationName = (char*)key;
    if (0 == _stricmp(stationTree->name, stationName)) {
        return stationTree; // this node matched
    }
    struct Station* stationNode = searchStationByName(stationTree->pLeftStation, stationName); // travel left
    if (NULL != stationNode) {
        return stationNode; // a node on the left matched
    }
    return searchStationByName(stationTree->pRightStation, stationName); // travel right
}

struct Station* searchStationByDistance(struct Station* stationTree, void* key) { // search by station distance
    if (NULL == stationTree) {
        return NULL;
    }
    struct DistanceSearchData* searchData = (struct DistanceSearchData*)key;
    struct Station* stationNewBest = stationTree;
    struct Station* stationTemp;
    double dx = stationTree->coord.x - searchData->coord.x;
    double dy = stationTree->coord.y - searchData->coord.y;
    double newBest = sqrt(dx * dx + dy * dy);
    searchData->shortest = DBL_MAX;
    stationTemp = searchStationByDistance(stationTree->pLeftStation, key);
    if (searchData->shortest < newBest) {
        newBest = searchData->shortest;
        stationNewBest = stationTemp;
    }
    searchData->shortest = DBL_MAX;
    stationTemp = searchStationByDistance(stationTree->pRightStation, key);
    if (searchData->shortest < newBest) {
        newBest = searchData->shortest;
        stationNewBest = stationTemp;
    }
    searchData->shortest = newBest;
    return stationNewBest;
}

struct Station* searchStationByNameId(struct Station* stationTree, void* key) { // search station by both name and ID
    int stationId = atoi((char*)key);
    if (stationId > 0) {
        struct Station* stationNode = searchStationById(stationTree, &stationId);
        if (NULL != stationNode) {
            return stationNode;
        }
    }
    return searchStationByName(stationTree, key);
}

struct Station* searchStationByCarLicense(struct Station* stationTree, void* key) { // search for station that the Car is in queue/charging
    if (NULL == stationTree) {
        return NULL;
    }
    struct Port* portNode = getCarChargingPort(stationTree, (char *)key);
    if (NULL != portNode) {
        return stationTree;
    }
    int index = getCarIndexInQueue(stationTree, (char *)key, SLOW); // MID & FAST are also fine
    if (index >= 0) {
        return stationTree;
    }
    struct Station* stationNode = searchStationByCarLicense(stationTree->pLeftStation, key); // travel left
    if (NULL != stationNode) {
        return stationNode;
    }
    return searchStationByCarLicense(stationTree->pRightStation, key); // travel right
}

struct Station* searchStation(struct Station* stationTree, void* key, enum StationSearchMode searchMode) { // function to search station tree
    if (searchMode >= 0 && searchMode < NUM_OF_STATION_SEARCH_MODES) {
        return stationSearchFunctions[searchMode](stationTree, key);
    }
    return NULL;
}

void appendCar2StationQueue(struct Station* stationNode, struct Car* newCar) { // add a car to station queue
    appendCar2QCar(&(stationNode->carQueue), newCar);
    stationNode->nCars++;
}

struct Car* removeCarFromStationQueue(struct Station* stationNode, struct Car* pCar) { // remove a car from station queue
    if ((NULL == stationNode) || (NULL == pCar)) {
        return NULL;
    }
    if (CAR_STATUS_INQUEUE != pCar->inqueue) {
        return NULL;
    }
    struct carNode* carNode1 = NULL;
    struct carNode* carNode2 = NULL;
    for (carNode1 = stationNode->carQueue.pFrontNode; NULL != carNode1; carNode2 = carNode1, carNode1 = carNode1->pNextCarNode) {
        if (carNode1->pCar == pCar) {
            stationNode->nCars--; // remove this car from queue
            if (carNode1 == stationNode->carQueue.pFrontNode) {
                stationNode->carQueue.pFrontNode = carNode1->pNextCarNode;
                if (NULL == stationNode->carQueue.pFrontNode) { // last car of the queue
                    stationNode->carQueue.pRearNode = NULL;
                }
            }
            else { // case: carNode1 is not queue head, carNode2 is never NULL
                carNode2->pNextCarNode = carNode1->pNextCarNode;
                if (carNode1 == stationNode->carQueue.pRearNode) {
                    stationNode->carQueue.pRearNode = carNode2;
                }
            }
            free(carNode1);
            pCar->inqueue = CAR_STATUS_NOTINQUEUE;
            return pCar;
        }
    }
    return NULL; // car not in station queue
}

struct Port* getCarChargingPort(struct Station* stationNode, char* license) { // get pointer to the port of station if car is charging there
    if (NULL == stationNode) {
        return NULL;
    }
    for (struct Port* portNode = stationNode->portsList; NULL != portNode; portNode = portNode->next) {
        if (NULL != portNode->car2p) {
            if (0 == strcmp(portNode->car2p->nLicense, license)) {
                return portNode;
            }
        }
    }
    return NULL;
}

int getCarIndexInQueue(struct Station* stationNode, char* license, enum ChargingRate chargingRate) { // find index of car in station queue, if not found return -1
    if (NULL == stationNode) {
        return -1;
    }
    int index = 0;
    for (struct carNode* carnode = stationNode->carQueue.pFrontNode; NULL != carnode; carnode = carnode->pNextCarNode) {
        if (chargingRate == carnode->pCar->portType) {
            index++;
        }
        if (0 == strcmp(carnode->pCar->nLicense, license)) {
            return index;
        }
    }
    return -1;
}

int countActivePortByChargingRate(struct Station* stationNode, enum ChargingRate crate) { // count active ports of a station by charging rate
    if (NULL == stationNode) {
        return 0;
    }
    int count = 0;
    for (struct Port* portNode = stationNode->portsList; NULL != portNode; portNode = portNode->next) {
        if ((portNode->portType == crate) && (PORT_STATUS_OUTOFORDER != portNode->status)) {
            count++;
        }
    }
    return count;
}

void freeStationToClose(struct Station* stationNode) { // free memory to close station, queue & port list must be empty
    if (NULL == stationNode) {
        return;
    }
    if (NULL != stationNode->portsList) {
        return;
    }
    if (NULL != stationNode->name) {
        free(stationNode->name);
    }
    freeQCar(&(stationNode->carQueue));
    free(stationNode);
}
 
void swapStationsData(struct Station* stationNode1, struct Station* stationNode2) { // swap data between two stations
    struct Station swapBuffer = *stationNode1;
    stationNode1->carQueue = stationNode2->carQueue;
    stationNode1->coord = stationNode2->coord;
    stationNode1->id = stationNode2->id;
    stationNode1->name = stationNode2->name;
    stationNode1->nCars = stationNode2->nCars;
    stationNode1->nPorts = stationNode2->nPorts;
    stationNode1->portsList = stationNode2->portsList;
    stationNode2->carQueue = swapBuffer.carQueue;
    stationNode2->coord = swapBuffer.coord;
    stationNode2->id = swapBuffer.id;
    stationNode2->name = swapBuffer.name;
    stationNode2->nCars = swapBuffer.nCars;
    stationNode2->nPorts = swapBuffer.nPorts;
    stationNode2->portsList = swapBuffer.portsList;
}

// note: station id must be EXISTED in the tree
struct Station* deleteStationById(struct Station* stationNode, int id) { // delete a station from tree by searching for its id
    if (NULL == stationNode) {
        return NULL;
    }
    struct Station* swapNode = NULL;
    if (id == stationNode->id) { // delete this station
        if (NULL == stationNode->pLeftStation) { // no left child
            swapNode = stationNode->pRightStation;
            freeStationToClose(stationNode);
            return swapNode;
        }
        if (NULL == stationNode->pRightStation) { // no right child
            swapNode = stationNode->pRightStation;
            freeStationToClose(stationNode);
            return swapNode;
        }
        // has both left child & right child
        // swap destination with the left most node of right branch
        // delete destination after swapping
        swapNode = stationNode->pRightStation;
        while ((NULL != swapNode) && (NULL != swapNode->pLeftStation)) {
            swapNode = swapNode->pLeftStation;
        }
        swapStationsData(stationNode, swapNode);
        stationNode->pRightStation = deleteStationById(stationNode->pRightStation, id);
    }
    else if (id < stationNode->id) {
        stationNode->pLeftStation = deleteStationById(stationNode->pLeftStation, id); // travel left
    }
    else {
        stationNode->pRightStation = deleteStationById(stationNode->pRightStation, id); // travel right
    }
    return stationNode;
}
