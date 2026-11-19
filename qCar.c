#include "qCar.h"

void freeQCar(struct qCar *aQCar) { // free a qCar
	struct carNode* traveller;
	struct carNode* next;
	traveller = aQCar->pFrontNode;
	while (NULL != traveller) {
		next = traveller->pNextCarNode;
		free(traveller);
		traveller = next;
	}
	aQCar->pFrontNode = NULL;
	aQCar->pRearNode = NULL;
}

void initQCar(struct qCar* aQCar) { // initialize a qCar
	aQCar->pFrontNode = NULL;
	aQCar->pRearNode = NULL;
}

void appendCar2QCar(struct qCar* aQCar, struct Car* newCar) { // append a car to a qCar queue
    if ((NULL == aQCar) || (NULL == newCar)) {
        return;
    }
    struct carNode* newCarNode = (struct carNode*)malloc(sizeof(struct carNode));
    if (NULL == newCarNode) {
        displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
        exit(EXIT_FAILURE);
    }
    newCarNode->pCar = newCar;
    newCarNode->pNextCarNode = NULL;
    if (NULL == aQCar->pFrontNode) {
        aQCar->pFrontNode = newCarNode;
    }
    else {
        aQCar->pRearNode->pNextCarNode = newCarNode;
    }
    aQCar->pRearNode = newCarNode;
}
