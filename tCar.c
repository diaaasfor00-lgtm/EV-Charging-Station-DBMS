#include "tCar.h"

void freeTCar(struct tCar** carTree) { // free tCar tree
    if (NULL == (*carTree)) {
        return;
    }
    freeTCar(&(*carTree)->pLTCar);
    freeTCar(&(*carTree)->pRTCar);
    if (NULL != (*carTree)->pCar) {
        free((*carTree)->pCar);
    }
    free(*carTree);
    *carTree = NULL;
}

void insertTCar(struct tCar** carTree, struct tCar* newTCar) { // insert a new tCar to tCar tree
    if (NULL == *carTree) {
        newTCar->pLTCar = NULL;
        newTCar->pRTCar = NULL;
        *carTree = newTCar;
        return;
    }
    int result;
    result = strcmp((*carTree)->pCar->nLicense, newTCar->pCar->nLicense);
    if (0 == result) {
        return; // duplicate license number
    }
    if (result > 0) {
        insertTCar(&((*carTree)->pLTCar), newTCar);
        return;
    }
    insertTCar(&((*carTree)->pRTCar), newTCar);
}

struct tCar* searchTCar(struct tCar* carTree, char *license) { // search car by license, return tCar
    if (NULL == carTree) {
        return NULL;
    }
    int res = strcmp(carTree->pCar->nLicense, license);
    if (0 == res) {
        return carTree;
    }
    else if (res > 0) {
        return searchTCar(carTree->pLTCar, license);
    }
    return searchTCar(carTree->pRTCar, license);
}

struct Car* searchCar(struct tCar* carTree, char* license) { // search car by license, return Car
    struct tCar* result;
    result = searchTCar(carTree, license);
    if (NULL == result) {
        return NULL;
    }
    return result->pCar;
}

void findTopCustomers(struct tCar* atCar, struct Car* topCustomers[]) { // find top customers
    if (NULL == atCar) {
        return;
    }
    int pos = TOP_CUSTOMERS_COUNT;
    while ((pos > 0) && ((NULL == topCustomers[pos - 1]) || (atCar->pCar->totalPayed > topCustomers[pos - 1]->totalPayed))) {
        pos--;
    }
    for (int i = TOP_CUSTOMERS_COUNT - 1; i > pos; i--) {
        topCustomers[i] = topCustomers[i - 1];
    }
    if (pos < TOP_CUSTOMERS_COUNT) {
        topCustomers[pos] = atCar->pCar;
    }
    findTopCustomers(atCar->pLTCar, topCustomers); // travel left
    findTopCustomers(atCar->pRTCar, topCustomers); // travel right
}

// note: car license must be EXISTED in the tree
struct tCar* deleteCarByLicense(struct tCar* atCar, char* license) { // delete a car from tree by searching for its license
    if (NULL == atCar) {
        return NULL;
    }
    struct tCar* swapNode = NULL;
    int compare = strcmp(atCar->pCar->nLicense, license);
    if (0 == compare) { // delete this tCar
        if (NULL == atCar->pLTCar) { // no left child
            swapNode = atCar->pRTCar;
            free(atCar->pCar);
            free(atCar);
            return swapNode;
        }
        if (NULL == atCar->pRTCar) { // no right child
            swapNode = atCar->pLTCar;
            free(atCar->pCar);
            free(atCar);
            return swapNode;
        }
        // has both left child & right child
        // swap destination with the left most node of right branch
        // delete destination after swapping
        swapNode = atCar->pRTCar;
        while ((NULL != swapNode) && (NULL != swapNode->pLTCar)) {
            swapNode = swapNode->pLTCar;
        }
        struct Car* pCar = atCar->pCar;
        atCar->pCar = swapNode->pCar;
        swapNode->pCar = pCar;
        atCar->pRTCar = deleteCarByLicense(atCar->pRTCar, license);
    } else if (compare > 0) {
        atCar->pLTCar = deleteCarByLicense(atCar->pLTCar, license); // travel left
    }
    else {
        atCar->pRTCar = deleteCarByLicense(atCar->pRTCar, license); // travel right
    }
    return atCar;
}
