#include "proj2.h"

sharedData *shared;
infoData *inf;
FILE *filePtr;

void help() {
    fprintf(stderr, "\nCorrect usage:\t./proj2 N O K TA TP\n");
    fprintf(stderr, "\tN:  Number of trucks, N < 10000\n");
    fprintf(stderr, "\tO:  Number of passenger cars, O < 10000\n");
    fprintf(stderr, "\tK:  Ferry capacity (maximum number of passenger cars that can be loaded), 3 <= K <= 100\n");
    fprintf(stderr, "\tTA: Maximum travel time for a car to reach the port (in microseconds), 0 <= TA <= 10000\n");
    fprintf(stderr, "\tTP: Maximum travel time for the ferry (in microseconds), 0 <= TP <= 1000\n\n");
}

int toInt(char strToInt[], int min, int max, char name[]) {
    char *strPtr;
    errno = 0;
    long num = strtol(strToInt, &strPtr, 10);   //10 decimal

    if (strPtr == strToInt || *strPtr != '\0') {
        fprintf(stderr, "Error: Invalid value \"%s\" for %s\n", strToInt, name);
        return -1;
    } 

    if (num < min || num > max) {
        fprintf(stderr, "Error: Value %s of %s out of range, use %d-%d\n", strToInt, name, min, max);
        return -1;
    }

    return (int)num;
}

void sharedSetup() {
    shared = mmap(NULL, sizeof(sharedData), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    if (shared == MAP_FAILED) {
        fprintf(stderr, "mmap failed\n");
        exit(1);
    }
    sem_init(&shared->mutex, 1, 1);
    sem_init(&shared->write, 1, 1);
    sem_init(&shared->semFerry, 1, 0);
    sem_init(&shared->boardingDone, 1, 0);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            sem_init(&shared->semVeh[i][j], 1, 0);
        }
    }
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            shared->waitVeh[i][j] = 0;
        }
    }
    shared->actionVal = 0;
    
}

void sharedDelete() {
    munmap(shared, sizeof(sharedData));
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->write);
    sem_destroy(&shared->semFerry);
    sem_destroy(&shared->boardingDone);
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            sem_destroy(&shared->semVeh[i][j]);
        }
    }
}

void cleanupExtras() {
    free(inf);
    fclose(filePtr);
}

void saveLine(const char line[], ...) {
    va_list args;
    va_start(args, line);

    //write into file
    sem_wait(&shared->write);

    shared->actionVal++;
    fprintf(filePtr, "%d: ", shared->actionVal);    // action number
    vfprintf(filePtr, line, args);                  // message
    fprintf(filePtr, "\n");                         // new line

    sem_post(&shared->write);

    va_end(args);
}   

void ferry() {  // handles capacity
    saveLine("P: started");
    int port = 0;
    srand(time(NULL) * getpid());

    int currCap = 0;
    int vehBoarded = 0;

    while(shared->vehLeft > 0) { // total vehicles or capacity
        usleep(rand() % (inf->ferryTime + 1));
        saveLine("P: arrived to %d", port);

        // let cars out
        for(int i = 0; i < vehBoarded; i++) {
            sem_post(&shared->semFerry);
        }
        vehBoarded = 0;
        currCap = inf->maxCap;

        // let cars in
        enum vehType boardingVeh = TRUCK;
        int vehSize;
        while (currCap > 0) {
            vehSize = (boardingVeh == CAR) ? 1 : 3;

            // leaving, car will wait if arriving too late
            if(shared->waitVeh[CAR][port] + shared->waitVeh[TRUCK][port] == 0)
                break;

            // boarding
            if (vehSize <= currCap && shared->waitVeh[boardingVeh][port]) {
                currCap -= vehSize;
                vehBoarded++;
                // saveLine("debug | attemtping to load type%d, assumed loaded: %d, storage %d/%d", boardingVeh, vehBoarded, inf->maxCap - currCap, inf->maxCap);

                sem_wait(&shared->mutex);
                shared->waitVeh[boardingVeh][port]--;
                sem_post(&shared->mutex);

                sem_post(&shared->semVeh[boardingVeh][port]);
            }

            // switch to other vehicle type if possible
            if (currCap >= 3)
                boardingVeh = (boardingVeh == CAR) ? TRUCK : CAR;
            else {
                boardingVeh = CAR;
                // truck won't fit, no cars waiting
                if (shared->waitVeh[CAR][port] == 0) {
                    break;
                }
            }
                
        }

        // saveLine("debug | waiting for %d", vehBoarded);
        // letting vehicles board safely
        for (int i = 0; i < vehBoarded; i++) {
            sem_wait(&shared->boardingDone);
        }

        saveLine("P: leaving %d", port);
        port = (port + 1) % 2;
    }

    usleep(rand() % (inf->ferryTime + 1));
    saveLine("P: finish");

    //freeing copied memory
    cleanupExtras();
    exit(0);
}

// ********************************************************************************************************

void vehicle(int vehNum, enum vehType type) {
    srand(time(NULL) * getpid());
    int port = rand() % 2;
    char vehName = (type == CAR) ? 'O' : 'N' ;

    // starting and arriving to a picked port
    saveLine("%c %d: started", vehName, vehNum);
    usleep(rand() % (inf->vehTime + 1));
    saveLine("%c %d: arrived to %d", vehName, vehNum, port);

    // arrived to port, waiting
    sem_wait(&shared->mutex);
    shared->waitVeh[type][port]++;
    sem_post(&shared->mutex);

    sem_wait(&shared->semVeh[type][port]);      // ferry post

    //boarding, leaving the port
    saveLine("%c %d: boarding", vehName, vehNum);
    sem_post(&shared->boardingDone);



    sem_wait(&shared->semFerry);                // ferry post

    saveLine("%c %d: leaving in %d", vehName, vehNum, (port + 1) % 2);

    sem_wait(&shared->mutex);
    shared->vehLeft--;
    sem_post(&shared->mutex);

    // freeing copied memory
    cleanupExtras();
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Error: Incorrect argument count %d\n", argc);
        help();
        return 1;
    }

    // helper structure
    inf = malloc(sizeof(infoData));
    if (inf == NULL) {
        fprintf(stderr, "Error: Malloc failed\n");
        return 1;
    }

    int trucks = toInt(argv[1], 0, 10000, "trucks");
    int cars = toInt(argv[2], 0, 10000, "cars");
    inf->maxCap = toInt(argv[3], 3, 100, "ferry capacity");
    inf->vehTime = toInt(argv[4], 0, 10000, "car travel time");
    inf->ferryTime = toInt(argv[5], 0, 1000, "ferry travel time");

    // checking for errors
    if (trucks == -1 || cars == -1 || inf->maxCap == -1 || inf->vehTime == -1 || inf->ferryTime == -1) {
        help();
        free(inf);
        return 1;
    }

    // semaphore setup
    sharedSetup();
    shared->vehLeft = trucks + cars;

    // file opening
    filePtr = fopen("proj2.out", "w");
    
    if (filePtr == NULL) {
        fprintf(stderr, "Error creating file!\n");
        free(inf);
        sharedDelete();
        return 1;
    }

    setbuf(filePtr, NULL);
    setbuf(stderr, NULL);

    // creating children
    pid_t pid = fork();
    if (pid == 0) {
        ferry();
    }
    for (int o = 0; o < cars; o++) {
        pid_t pid = fork();
        if (pid == 0) {
            vehicle(o+1, CAR);
        }
    }
    for (int n = 0; n < trucks; n++) {
        pid_t pid = fork();
        if (pid == 0) {
            vehicle(n+1, TRUCK);
        }
    }

    // waiting for children
    while(wait(NULL) > 0);

    // allocated files and shared mem freeing
    cleanupExtras();
    sharedDelete();

    return 0;
}

