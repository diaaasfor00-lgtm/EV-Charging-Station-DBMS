// functions for load and save database
#include "database.h"

void loadStations(struct Station** stationTree) { // load stations from file
    freeStations(&(*stationTree));
    FILE* file;
    if (fopen_s(&file, FILE_INPUT_STATIONS, "r") == 0) {
        char line[FILE_READING_BUFFER_SIZE];
        fgets(line, sizeof(line), file); // discard first line of the file
        int id;
        char name[256];
        int nPorts;
        double coordX;
        double coordY;
        while (fgets(line, sizeof(line), file)) {
            int result = sscanf_s(line, "%d,%255[^,],%d,%lf,%lf", &id, name, (unsigned int)sizeof(name), &nPorts, &coordX, &coordY);
            if (result != 5) {
                continue; // this line is not valid to parse
            }
            name[255] = 0; // input error catching
            struct Station* newStation = (struct Station*)malloc(sizeof(struct Station));
            if (NULL == newStation) {
                displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
                exit(EXIT_FAILURE);
            }
            newStation->name = (char*)malloc((strlen(name) + 1) * sizeof(char));
            if (NULL == newStation->name) {
                displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
                exit(EXIT_FAILURE);
            }
            initNewStation(newStation);
            newStation->id = id;
            strcpy_s(newStation->name, strlen(name) + 1, name);
            newStation->nPorts = nPorts;
            newStation->coord.x = coordX;
            newStation->coord.y = coordY;
            insertNewStation(&(*stationTree), newStation);
        }
        fclose(file);
    }
}

void loadCars(struct tCar** carTree) { // load cars from file
    freeTCar(&(*carTree));
    FILE* file;
    if (fopen_s(&file, FILE_INPUT_CARS, "r") == 0) {
        char line[FILE_READING_BUFFER_SIZE];
        fgets(line, sizeof(line), file); // discard first line of the file
        char carLicense[10];
        char portType[5];
        double totalPayed;
        int stationId;
        int chargingRate;
        int portNumber;
        int inQueue;
        while (fgets(line, sizeof(line), file)) {
            int result = sscanf_s(line, "%9[^,],%4[^,],%lf,%d,%d,%d", carLicense, (unsigned int)sizeof(carLicense), portType, (unsigned int)sizeof(portType), &totalPayed, &stationId, &portNumber, &inQueue);
            if (result != 6) {
                continue; // this line is not valid to parse
            }
            carLicense[9] = 0; // input error catching
            portType[4] = 0; // input error catching
            trimString(carLicense);
            toUpperCase(portType);
            trimString(portType);
            chargingRate = getChargingRateFromString(portType);
            if ((strlen(carLicense) == 0) || (chargingRate < 0)) {
                continue; // this line is not valid to parse
            }
            struct Car* pCar = (struct Car*)malloc(sizeof(struct Car));
            struct tCar* pTCar = (struct tCar*)malloc(sizeof(struct tCar));
            if ((NULL == pCar) || (NULL == pTCar)) {
                displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
                exit(EXIT_FAILURE);
            }
            strcpy_s(pCar->nLicense, 10, carLicense);
            pCar->portType = chargingRate;
            pCar->totalPayed = totalPayed;
            pCar->pPort = NULL; // check & update later when reading Ports.txt
            pCar->inqueue = 0; // check & update later when reading LineOfCars.txt
            pTCar->pCar = pCar;
            pTCar->pLTCar = NULL;
            pTCar->pRTCar = NULL;
            insertTCar(&(*carTree), pTCar);
        }
        fclose(file);
    }
}

void loadPorts(struct Station** stationTree, struct tCar** carTree) { // load ports from file
    FILE* file;
    if (fopen_s(&file, FILE_INPUT_PORTS, "r") == 0) {
        char line[FILE_READING_BUFFER_SIZE];
        fgets(line, sizeof(line), file); // discard first line of the file
        char license[10];
        char portType[5];
        int stationId;
        int portNumber;
        int status;
        int year;
        int month;
        int day;
        int hour;
        int min;
        int chargingRate;
        while (fgets(line, sizeof(line), file)) {
            int result = sscanf_s(line, "%d,%d,%4[^,],%d,%d,%d,%d,%d,%d,%9[^,]", &stationId, &portNumber, portType, (unsigned int)sizeof(portType), &status, &year, &month, &day, &hour, &min, license, (unsigned int)sizeof(license));
            if (result != 10) {
                continue; // this line is not valid to parse
            }
            license[9] = 0; // input error catching
            portType[4] = 0; // input error catching
            trimString(license);
            trimString(portType);
            chargingRate = getChargingRateFromString(portType);
            if ((strlen(license) <= 0) || (chargingRate < 0)) {
                continue; // this line is not valid to parse
            }
            struct Station* pStation = searchStation(*stationTree, &stationId, STATION_SEARCH_ID);
            if (NULL == pStation) {
                continue;
            }
            struct Car* pCar = searchCar(*carTree, license);
            struct Port* newPort = (struct Port*)malloc(sizeof(struct Port));
            if (NULL == newPort) {
                displayError(ERROR_FAILED_TO_ALLOCATE_MEMORY);
                exit(EXIT_FAILURE);
            }
            newPort->num = portNumber;
            newPort->portType = chargingRate;
            newPort->status = status;
            newPort->tin.year = year;
            newPort->tin.month = month;
            newPort->tin.day = day;
            newPort->tin.hour = hour;
            newPort->tin.min = min;
            newPort->car2p = pCar;
            if (NULL != pCar) {
                pCar->pPort = newPort;
            }
            appendPortToPortList(&(pStation->portsList), newPort);
        }
        fclose(file);
    }
}

void loadLineOfCars(struct Station** stationTree, struct tCar** carTree) { // load line of cars from file
    FILE* file;
    if (fopen_s(&file, FILE_INPUT_LINEOFCARS, "r") == 0) {
        char line[FILE_READING_BUFFER_SIZE];
        fgets(line, sizeof(line), file); // discard first line of the file
        int stationId;
        char carLicense[10];
        while (fgets(line, sizeof(line), file)) {
            int result = sscanf_s(line, "%9[^,],%d", carLicense, (unsigned int)sizeof(carLicense), &stationId);
            if (result != 2) {
                continue; // this line is not valid to parse
            }
            carLicense[9] = 0; // input error catching
            struct Station* pStation = searchStation(*stationTree, &stationId, STATION_SEARCH_ID);
            if (NULL == pStation) {
                continue;
            }
            struct Car* pCar = searchCar(*carTree, carLicense);
            if (NULL == pCar) {
                continue;
            }
            pCar->inqueue = 1;
            appendCar2StationQueue(pStation, pCar);
        }
        fclose(file);
    }
}

void saveStation(struct Station* stationNode, FILE* fileStations, FILE* filePorts, FILE* fileLineOfCars) { // save station node to file
    if (NULL == stationNode) {
        return;
    }
    fprintf(fileStations, "%d,%s,%d,%.2lf,%.2lf\n", stationNode->id, stationNode->name, stationNode->nPorts, stationNode->coord.x, stationNode->coord.y);
    for (struct Port* portNode = stationNode->portsList; NULL != portNode; portNode = portNode->next) {
        fprintf(filePorts, "%d,%d,%s,%d,", stationNode->id, portNode->num, ChargingRateName[portNode->portType], portNode->status);
        fprintf(filePorts, "%d,%d,%d,%d,%d,", portNode->tin.year, portNode->tin.month, portNode->tin.day, portNode->tin.hour, portNode->tin.min);
        if (NULL != portNode->car2p) {
            fprintf(filePorts, "%s\n", portNode->car2p->nLicense);
        }
        else {
            fprintf(filePorts, "-1\n");
        }
    }
    for (struct carNode* carnode = stationNode->carQueue.pFrontNode; NULL != carnode; carnode = carnode->pNextCarNode) {
        fprintf(fileLineOfCars, "%s,%d\n", carnode->pCar->nLicense, stationNode->id);
    }
    saveStation(stationNode->pLeftStation, fileStations, filePorts, fileLineOfCars); // travel left
    saveStation(stationNode->pRightStation, fileStations, filePorts, fileLineOfCars); // travel right
}

void saveStations(struct Station* stationTree) { // save station tree to file
    FILE* fileStations;
    FILE* filePorts;
    FILE* fileLineOfCars;
    if ((0 == fopen_s(&fileStations, FILE_OUTPUT_STATIONS, "w")) && (0 == fopen_s(&filePorts, FILE_OUTPUT_PORTS, "w")) && (0 == fopen_s(&fileLineOfCars, FILE_OUTPUT_LINEOFCARS, "w"))) {
        fprintf(fileStations, "ID,StationName,NumOfPorts,CoordX,CoordY\n");
        fprintf(filePorts, "StationID,PortNumber,PortType,Status,Year,Month,Day,Hour,Min,CarLicense\n");
        fprintf(fileLineOfCars, "License,StationID\n");
        saveStation(stationTree, fileStations, filePorts, fileLineOfCars);
        fclose(fileStations);
        fclose(filePorts);
        fclose(fileLineOfCars);
    }
    else {
        displayError(ERROR_FAILED_TO_UPDATE_FILES);
        exit(EXIT_FAILURE);
    }
}

void saveTCar(struct tCar* tCarNode, struct Station* stationTree, FILE* fileCars) { // save tCar node to file
    if (NULL == tCarNode) {
        return;
    }
    struct Station* stationNode = searchStation(stationTree, tCarNode->pCar->nLicense, STATION_SEARCH_CAR);
    fprintf(fileCars, "%s,%s,%.2lf,", tCarNode->pCar->nLicense, ChargingRateName[tCarNode->pCar->portType], tCarNode->pCar->totalPayed);
    if (NULL != stationNode) {
        if (NULL != tCarNode->pCar->pPort) {
            fprintf(fileCars, "%d,%d,0\n", stationNode->id, tCarNode->pCar->pPort->num);
        }
        else {
            fprintf(fileCars, "%d,0,1\n", stationNode->id);
        }
    }
    else {
        fprintf(fileCars, "0,0,0\n");
    }
    saveTCar(tCarNode->pLTCar, stationTree, fileCars); // travel left
    saveTCar(tCarNode->pRTCar, stationTree, fileCars); // travel right
}

void saveTCars(struct tCar* carTree, struct Station* stationTree) { // save cars to file
    FILE* fileCars;
    if (0 == fopen_s(&fileCars, FILE_OUTPUT_CARS, "w")) {
        fprintf(fileCars, "License,PortType,TotalPayed,StationID,PortNumber,InQueue\n");
        saveTCar(carTree, stationTree, fileCars);
        fclose(fileCars);
    }
    else {
        displayError(ERROR_FAILED_TO_UPDATE_FILES);
        exit(EXIT_FAILURE);
    }
}
