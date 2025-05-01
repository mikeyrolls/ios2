/*
 * Author:  me
 * Date:    28.4.2025
 */

#ifndef PROJ2_H
#define PROJ2_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <time.h>
#include <stdarg.h>
#include <sys/wait.h>

typedef struct {
    sem_t mutex;
    sem_t write;
    sem_t semVeh[2][2];   // car/truck   port0/1
    sem_t semFerry;
    int currCap;
    int actionVal;
    int waitVeh[2][2];
    int vehLeft;
    int vehBoarded;
} sharedData;

typedef struct {
    int maxCap;
    int vehTime;
    int ferryTime;
} infoData;

enum vehType{ CAR, TRUCK };

/**
 * @brief prints program usage to error output
 * 
 */
void help();

/**
 * @brief converts a string to integer in a range from min to max
 * 
 * @param strToInt string to convert
 * @param min minimum value
 * @param max maximum value
 * @param name name of variable for error messages
 * @return converted value on success, -1 on fail
 * 
 */
int toInt(char strToInt[], int min, int max, char name[]);

void sharedSetup();

void ferry();

void vehicle(int vehNum, enum vehType type);

void truck();

/**
 * @brief main body of the program
 * 
 * @param argv 5 parameters: trucks, cars, ferry capacity, car travel time, ferry travel time
 * @return 0 on success, 1 on fail
 * 
 */
int main(int argc, char* argv[]);

#endif