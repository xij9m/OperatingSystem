
#include "BCDs.h"
#include "itoa.h"
#include "R1.h"
#include "../include/core/io.h"
#include "../include/system.h"
#include "../include/core/serial.h"
#include "mpx_supt.h"
#include <string.h>
#include "comhand.h"
#include "QueueR2.h"
#include "itoa.h"

/**
 * Procedure..: suspend
 * @param..: process name
 */
void suspend(char* processName)	{
	int bufferSize;
	struct PCB* process = findPCB(processName);

	if(process == NULL)	{
		bufferSize = 19;
		sys_req(WRITE, COM1, "\nInvalid Process\n", &bufferSize);

	} else if(process->stateSus == 1){
		bufferSize = 26;
		sys_req(WRITE, COM1, "\nProcess is suspended\n", &bufferSize);

	} else if(process->stateSus == 0){
		removePCB(process);
		process->stateSus = 1;
		insertPCB(process);

	} else {
		bufferSize = 28;
		sys_req(WRITE, COM1, "\nCould not find process\n", &bufferSize);

	}
}

/**
 * Procedure..: resume
 * @param..: process name
 */
void resume(char* processName)	{
	struct PCB* process = findPCB(processName);
	int bufferSize;

	if(process == NULL)	{
		bufferSize = 19;
		sys_req(WRITE, COM1, "\nInvalid Process\n", &bufferSize);

	} else if(process->stateSus == 0){
		bufferSize = 30;
		sys_req(WRITE, COM1, "\nProcess is not suspended\n", &bufferSize);

	} else if(process->stateSus == 1){
		removePCB(process);
		process->stateSus = 0;
		insertPCB(process);

	} else {
		bufferSize = 28;
		sys_req(WRITE, COM1, "\nCould not find process\n", &bufferSize);

	}
		
}

/**
 * Procedure..: set_priority
 * @param..: process name, new priority
 */
void set_priority(char* processName, int newPriority)	{
	struct PCB* process = findPCB(processName);
	if(process != NULL)	{
		if(newPriority >= 0 && newPriority <= 9)	{
			process->priority = newPriority;
		}
	}
}


/**
 * Procedure..: show_pcb
 * @param..: process name
 */
void show_pcb(char* processName)	{
	struct PCB* process = findPCB(processName);
	int bufferSize = 50;
	if(!process){
		bufferSize = 15;
		sys_req(WRITE, COM1, "Error: process not found\n", &bufferSize);
		}

	else {
		/*Prints the name*/
		sys_req(WRITE, COM1, "Process Name: ", &bufferSize);
		sys_req(WRITE, COM1, process->processName, &bufferSize);
		sys_req(WRITE, COM1, "\n", &bufferSize);
		/*Prints the class*/
		sys_req(WRITE, COM1, "Process Class: ", &bufferSize);
		if (process->processClass == 1)	{ 
			sys_req(WRITE, COM1, "Application", &bufferSize);
			sys_req(WRITE, COM1, "\n", &bufferSize);
		}else if(process->processClass == 2)	{
			sys_req(WRITE, COM1, "System Process", &bufferSize);
			sys_req(WRITE, COM1, "\n", &bufferSize);
		}
		/*Prints the priority*/
		char* priorityChar = "\0";
		
		sys_req(WRITE, COM1, "Process Priority: ", &bufferSize);
		itoa(process->priority, priorityChar);
		sys_req(WRITE, COM1, priorityChar, &bufferSize);
		sys_req(WRITE, COM1, "\n", &bufferSize);
		/*Prints the state*/
		sys_req(WRITE, COM1, "Process State: ", &bufferSize);
		if(process->stateRead == 2)	{
			sys_req(WRITE, COM1, "Blocked", &bufferSize);
		}else if(process->stateRead == 0)	{
			sys_req(WRITE, COM1, "Ready", &bufferSize);
		}else if(process->stateRead == 1)	{
			sys_req(WRITE, COM1, "Running", &bufferSize);
		}
		sys_req(WRITE, COM1, "\n", &bufferSize);
		/*Prints the suspended state*/
		sys_req(WRITE, COM1, "Process Suspended State: ", &bufferSize);
		if(process->stateSus == 0)	{
			sys_req(WRITE, COM1, "Not suspended\n", &bufferSize);
		}else if(process->stateSus == 1)	{
			sys_req(WRITE, COM1, "Suspended\n", &bufferSize);
		}
		sys_req(WRITE, COM1, "\n", &bufferSize);
	}
}

