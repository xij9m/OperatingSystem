#ifndef _R4_H
#define _R4_H
#include "PCBs.h"
#include "R2.h"
#include "R3.h"
#include <string.h>
#include "procsr3.h" 
//#include "mpx_supt.h"

typedef struct alarm
{
	char message[60];
	int date[3];
	struct alarm* next;
	struct alarm* prev;
	
}alarm;

typedef struct alarmQueue	{
	int num;
    alarm *head;
    alarm *tail;

}alarmQueue;

alarmQueue* alarmQ;


void createAlarm(char alarmName[60], int alarmTime[3]);

void checkAlarms();

void removeAlarm(alarm* current);

void insertAlarm(alarm* alarmName);

void alarmproc();

void getinfinite();

void infinite();

void idleproc(); 

#endif
