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
    sem_init(&shared->semPort0Car, 1, 0);
    sem_init(&shared->semPort0Truck, 1, 0);
    sem_init(&shared->semPort1Car, 1, 0);
    sem_init(&shared->semPort1Truck, 1, 0);
    shared->currCap = 0;
}

void sharedDelete() {
    munmap(shared, sizeof(sharedData));
    sem_destroy(&shared->mutex);
    sem_destroy(&shared->semPort0Car);
    sem_destroy(&shared->semPort0Truck);
    sem_destroy(&shared->semPort1Car);
    sem_destroy(&shared->semPort1Truck);
}

void cleanupExtras() {
    free(inf);
    fclose(filePtr);
}

void saveLine(const char line[], ...) {
    va_list args;
    va_start(args, line);

    //write into file
    sem_wait(&shared->mutex);
    shared->actionVal++;
    // fprintf(filePtr, "%d: ", shared->actionVal);
    vfprintf(filePtr, line, args);
    sem_post(&shared->mutex);

    va_end(args);
}   

void ferry() {
    saveLine("ferry meow\n");
    cleanupExtras();
    exit(0);
}

void car() {
    srand(time(NULL) * getpid());
    int port = rand() % 2;
    saveLine("car meow at %d\n", port);
    cleanupExtras();
    exit(0);
}

void truck() {
    printf("truck imploded catastrophically\n");
    cleanupExtras();
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Error: Incorrect argument count %d\n", argc);
        help();
        return 1;
    }

    inf = malloc(sizeof(infoData));
    if (inf == NULL) {
        fprintf(stderr, "Error: Malloc failed\n");
        return 1;
    }

    int trucks = toInt(argv[1], 0, 10000, "trucks");
    int cars = toInt(argv[2], 0, 10000, "cars");
    inf->capacity = toInt(argv[3], 3, 100, "ferry capacity");
    inf->carTime = toInt(argv[4], 0, 10000, "car travel time");
    inf->boatTime = toInt(argv[5], 0, 1000, "ferry travel time");

    if (trucks == -1 || cars == -1 || inf->capacity == -1 || inf->carTime == -1 || inf->boatTime == -1) {
        help();
        free(inf);
        return 1;
    }

    sharedSetup();

    filePtr = fopen("proj2.out", "w");
    
    if (filePtr == NULL) {
        fprintf(stderr, "Error creating file!\n");
        free(inf);
        sharedDelete();
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        ferry();
    }
    for (int o = 0; o < cars; o++) {
        pid_t pid = fork();
        if (pid == 0) {
            car();
        }
    }
    for (int n = 0; n < trucks; n++) {
        pid_t pid = fork();
        if (pid == 0) {
            truck();
        }
    }

    while(wait(NULL) > 0);

    cleanupExtras();
    sharedDelete();

    return 0;
}

