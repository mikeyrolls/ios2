#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>

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

void ferry() {
    printf("ferry meow\n");
    exit(0);
}

void car() {
    printf("car meow\n");
    exit(0);
}

void truck() {
    printf("truck meow\n");
    exit(0);
}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        fprintf(stderr, "Error: Incorrect argument count %d\n", argc);
        help();
        return 1;
    }

    int trucks = toInt(argv[1], 0, 10000, "trucks");
    int cars = toInt(argv[2], 0, 10000, "cars");
    int capacity = toInt(argv[3], 3, 100, "ferry capacity");
    int carTime = toInt(argv[4], 0, 10000, "car travel time");
    int boatTime = toInt(argv[5], 0, 1000, "ferry travel time");

    if (trucks == -1 || cars == -1 || capacity == -1 || carTime == -1 || boatTime == -1) {
        help();
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


    //check which process

    //main wait for end

    return 0;
}