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
#include <stdbool.h>

/**
 * @brief structure with semaphores and shared data
 * 
 * - `mutex`: semaphore for mutual exclusion
 * 
 * - `write`: semaphore for file writes
 * 
 * - `semVeh`: semaphores for vehicles (car/truck) at each port (0/1)
 * 
 * - `semFerry`: semaphore for ferry boarding and leaving
 * 
 * - `boardingDone`: semaphore for signalling successful boarding of all currently transferrable vehicles
 * 
 * - `boardingDone`: semaphore for signalling successful unloading of all previously loaded vehicles
 * 
 * - `actionVal`: number of current action, used for writes
 * 
 * - `waitVeh`: amount of waiting vehicles (car/truck) at each port (0/1)
 * 
 * - `vehLeft`: unfinished vehicle processes
 */
typedef struct {
    sem_t mutex;
    sem_t write;
    sem_t semVeh[2][2];   // car/truck   port0/1
    sem_t semFerry;
    sem_t boardingDone;
    sem_t unloadingDone;
    int actionVal;
    int waitVeh[2][2];
    int vehLeft;
} sharedData;

/**
 * @brief config data for quick access by all processes
 * 
 * - `maxCap`: maximal ferry capacity
 * 
 * - `vehTime`: maximal time vehicle takes to arrive to port
 * 
 * - `ferryTime`: maximal time ferry takes to move between ports
 */
typedef struct {
    int maxCap;
    int vehTime;
    int ferryTime;
} infoData;

enum vehType{ CAR, TRUCK };

/**
 * @brief prints program usage to error output
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
 */
int toInt(char strToInt[], int min, int max, char name[]);

/**
 * @brief allocates shared data and initializes semaphores
 */
void sharedSetup();

/**
 * @brief frees allocated shared data and destroys semaphores
 */
void sharedDelete();

/**
 * @brief frees not shared allocated data and closes file
 * 
 * (children copy allocated memory)
 */
void cleanupExtras();

/**
 * @brief saves a line to a file with action number and correct mutex usage
 *
 * @param line message to save
 * @param ... extra arguments in the style of printf
 */
void saveLine(const char line[], ...);

/**
 * @brief function for child process ferry
 *
 * process handles ferry behavior including saving ferry messages and semaphore management. circles ports until all vehicles are transferred, then frees allocated memory and ends
 */
void ferry();

/**
 * @brief function for child processes car and truck
 *
 * process handles vehicle behavior including saving vehicle messages and semaphore usage. picks a port, arrives, rides the ferry and leaves, freeing allocated memory and ending. car and truck logic is separated with car/truck enum
 * 
 * @param vehNum number of created vehicle of specified type, starting from 1
 * @param type CAR or TRUCK enum
 */
void vehicle(int vehNum, enum vehType type);

/**
 * @brief main body of the program
 * 
 * parses arguments, sets up file and shared variables, then creates child processes, waits for them to end and cleans up
 * 
 * @param argv 5 parameters: trucks, cars, ferry capacity, car travel time, ferry travel time
 * @return 0 on success, 1 on fail
 * 
 */
int main(int argc, char* argv[]);

#endif