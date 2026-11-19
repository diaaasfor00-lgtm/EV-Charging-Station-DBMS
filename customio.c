// custom input output functions
#include "customio.h"
#include "Station.h"

void keyboardClear(int isAfterScanf_s) { // reset keyboard buffer
    if (isAfterScanf_s) {
        for (; '\n' != getchar(););
    } else {
        for (; _kbhit();) {
            int ch = _getch();
        }
    }
}

void pressAnykey() { // press anykey to continue
    printf("Press anykey to continue...");
    int ch = _getch();
    keyboardClear(0);
}

int readDouble(double* value, char* mesInfo, char *mesError) { // read a double
    while (TRUE) {
        printf(mesInfo);
        if (1 == scanf_s("%lf", value)) {
            keyboardClear(TRUE);
            return TRUE;
        }
        keyboardClear(TRUE);
        printf(mesError);
        printf(CMES_R_TO_RETRY);
        int ch = _getch();
        keyboardClear(FALSE);
        if (('r' != ch) && ('R' != ch)) {
            return FALSE;
        }
        printf("\n");
    }
}

int readCarLicense(char* license) { // read car license
    while (TRUE) {
        printf("Car license = ");
        if (0 == scanf_s("%[0-9]", license, 10)) {
            license[0] = 0;
        }
        keyboardClear(TRUE);
        if (strlen(license) < 8) {
            printf("Car license must be a 9-digit string.\n");
            printf(CMES_R_TO_RETRY);
            int ch = _getch();
            keyboardClear(FALSE);
            if (('r' != ch) && ('R' != ch)) {
                return FALSE;
            }
            printf("\n");
        }
        else {
            return TRUE;
        }
    }
}

int readStationNameId(char* nameId, struct Station* stationTree, int stationMustExist) { // read station name or id
    while (TRUE) {
        printf("Station name (or ID) = ");
        if (0 == scanf_s("%[^\n]", nameId, KEYBOARD_BUFFER)) {
            nameId[0] = 0;
        }
        keyboardClear(TRUE);
        trimString(nameId);
        if (0 == strlen(nameId)) {
            printf("Name (or ID) cannot be blank.\n");
            printf(CMES_R_TO_RETRY);
            int ch = _getch();
            keyboardClear(FALSE);
            if (('r' != ch) && ('R' != ch)) {
                return FALSE;
            }
            printf("\n");
        }
        else {
            if (FALSE == stationMustExist) {
                return TRUE;
            }
            if (NULL == searchStation(stationTree, nameId, STATION_SEARCH_NAMEID)) {
                printf(CMES_STATION_NOT_FOUND);
                printf(CMES_R_TO_RETRY);
                int ch = _getch();
                keyboardClear(FALSE);
                if (('r' != ch) && ('R' != ch)) {
                    return FALSE;
                }
                printf("\n");
            }
            else {
                return TRUE;
            }
        }
    }
}

int readChargingRate(enum ChargingRate* crate) { // read charging rate 0=SLOW 1=MID 2=FAST
    while (TRUE) {
        printf("Charging rate (0=SLOW 1=MID 2=FAST): ");
        if (0 == scanf_s("%d", crate)) {
            keyboardClear(TRUE);
            *crate = 1000;
        }
        if ((SLOW <= *crate) && (*crate <= FAST)) {
            return TRUE;
        }
        printf("Charging rate only accepts values 0 1 or 2\n");
        printf(CMES_R_TO_RETRY);
        int ch = _getch();
        keyboardClear(FALSE);
        if (('r' != ch) && ('R' != ch)) {
            return FALSE;
        }
        printf("\n");
    }
}



