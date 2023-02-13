/**
 * 
 * PCBs.h
 * MPX is a multiprocessing system so it needs a way
 * to keep track of the process.
 * PCBs will do the job
 * 
 * 
 */

#ifndef _PCBS_H 
#define _PCBS_H
#include <stdint.h>
#include <string.h>
#include <system.h>
#include <core/io.h>
#include <core/serial.h>
#include <core/tables.h>
#include <core/interrupts.h>
#include <mem/heap.h>
#include <mem/paging.h>
//#include "mpx_supt.h"
#include "comhand.h"


typedef struct PCB  {
    char processName[25];   /*Contains the name of the process. Cannot be more than 25 chars long*/
    int processClass;       /*1 for application, 2 for system process*/
    int priority;           /*0-9. Higher priority items are executed first*/
    int stateRead;          /*1 if running, 0 if ready, 2 if blocked*/
    int stateSus;           /*0 if not suspended, 1 if suspended*/
    unsigned char processStack[1024];   /*Process stack*/
    unsigned char* processTop;           /*Top of the process stack*/
    unsigned char* processBase;          /*Base element of the process stack*/   
    struct PCB *prev;                 /*Pointer to PCB before current*/
    struct PCB *next;                 /*Pointer to PCB after current*/
    struct PCB *cop;

}PCB;

PCB* AllocatePCB();

int FreePCB(struct PCB *ptr);

PCB* SetupPCB(char* newProcessName, int newProcessClass, int newPriority);

void createPCB(char* ProcessName, int ProcessClass, int ProcessPriority);

void deletePCB(char* name);

void blockPCB(char* name);

void unblockPCB(char* name);


#endif