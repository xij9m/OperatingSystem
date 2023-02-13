/**
 * 
 * QueueR2
 * 
 * 
 */

#ifndef _QueueR2_H
#define _QueueR2_H
#include "PCBs.h"
#include "R2.h"
#include "mpx_supt.h"
#include <string.h>



typedef struct queue    {
    int count;
    PCB *head;
    PCB *tail;
}queue;

queue* readyQueue;
queue* blockedQueue;

void AllocateQueue();

PCB* findPCB(char* name);

void insertPCB(PCB* process);

void removePCB(PCB* process); 

void show_all_processes();

void show_ready_processes();

void show_blocked_processes();

#endif 