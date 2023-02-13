#ifndef _R5_H
#define _R5_H
#include "PCBs.h"
#include "R2.h"
#include "R3.h"
#include <string.h>
#include "procsr3.h" 
#include "mpx_supt.h"
#include <system.h>

enum memtype {FREE,ALLOCATED};

typedef struct freeblocks
{
	struct cmcb* head;
	struct lmcb* tail;

}freeblocks;

typedef struct allocatedblocks
{
	struct cmcb* head;
	struct lmcb* tail;

}allocatedblocks;


typedef struct cmcb
{
	u32int addr;
	u32int size;
	enum memtype type;
	struct cmcb* next;
	struct cmcb* previous;

}cmcb;


typedef struct lmcb
{
	u32int size;
	enum memtype type;

}lmcb;


int init_heap(int size);

u32int allocated_mem(u32int bytes_of_mem);

int free_mem(u32int address);

int isEmpty();

void show_free_mem();

void show_allocated_mem();

//void AllocateLists();

void insertCMCBAlloc(cmcb* allocated);

void insertCMCBFree(cmcb* free);

void removeFreeCMCB(cmcb* current);

void removeAllocCMCB(cmcb* current);

#endif