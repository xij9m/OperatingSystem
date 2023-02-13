/**
 * 
 * 
 * 
 * 
 */
#include "QueueR2.h"
//*********************************************************************/

// struct queue* readyQueue = sys_alloc_mem(sizeof(struct queue));
// struct queue* blockedQueue = sys_alloc_mem(sizeof(struct queue));

/**
 * Procedure..: AllocatePCB
 * returning PCB pointer
 */


PCB* AllocatePCB(){
    struct PCB* ptr = (struct PCB*)sys_alloc_mem(sizeof(struct PCB)); // Initial memory allocation 
   // ptr -> processBase = ptr -> processStack[0]; 
    ptr -> processTop = ptr -> processStack + 1024 - sizeof(struct context);
    return ptr; 
} 

/**
 * Procedure..: FreePCB
 * @param..: PCB pointer to pcb to be freed
 * returning success or failure message
 */
int FreePCB(struct PCB *ptr){
    int bufferSize = 10;      //Size of buffer
    int del = sys_free_mem(ptr);

    if(del == -1){
        sys_req(WRITE, COM1, "ERROR\n", &bufferSize);
        return -1;
    }

    else {
        sys_req(WRITE, COM1, "Success!\n", &bufferSize);
        return 0;
    }
    // return 0;
}

/**
 * Procedure..: SetupPCB
 * @param..: process name, process class, process priority
 * returning PCB pointer
 */
PCB* SetupPCB(char* newProcessName, int newProcessClass, int newPriority)    {
    int bufferSize = 20;
    PCB* newProcess = AllocatePCB();
    memset(newProcess->processName, '\0', 25);
    
    if(sizeof(newProcessName) <= 25)   {
        strcpy(newProcess->processName, newProcessName);
    } else {

        sys_req(WRITE, COM1, "Error setting name\n", &bufferSize);
    }
    if((newProcessClass == 1) || (newProcessClass == 2))    {
        newProcess->processClass = newProcessClass;
    } else {

        sys_req(WRITE, COM1, "Error setting class\n", &bufferSize);
    }
    if((newPriority >= 0) && (newPriority <= 9))    {
        newProcess->priority = newPriority;
    } else {

        sys_req(WRITE, COM1, "Error setting priority\n", &bufferSize);
    }

    newProcess->stateRead = 0;
    newProcess->stateSus = 0;

/*
   if((newProcess->processName != NULL) && (newProcess->processClass != NULL) && (newProcess->priority != NULL))   {
        return newProcess;
    }else   {
        return NULL;
    }
*/

    return newProcess;

}

/**
 * Procedure..: createPCB
 * @param..: process name, process class, process priority
 */
void createPCB(char* ProcessName, int ProcessClass, int ProcessPriority){
    int bufferSize = 30;
    struct PCB* tmp = SetupPCB(ProcessName,ProcessClass,ProcessPriority);
    insertPCB(tmp);
    bufferSize = 1;
    sys_req(WRITE, COM1, tmp->processName, &bufferSize);
}

/**
 * Procedure..: deletePCB
 * @param..: process name
 */
void deletePCB(char* name){
    struct PCB* process = findPCB(name);
    if(!process) {

        int bufferSize = 25;
        sys_req(WRITE, COM1, "Error: process not found\n", &bufferSize);
        
    } else {
    removePCB(process);
    FreePCB(process);
 }
}

/**
 * Procedure..: blockPCB
 * @param..: process name
 */
void blockPCB(char* name){
    int bufferSize;
    int state;
    struct PCB* tmp = findPCB(name);

    if(tmp != NULL){
       
        state = tmp->stateRead;

        if(state == 2) {
            bufferSize = 28;
            sys_req(WRITE, COM1, "Error: Process is already blocked\n", &bufferSize);

        } else if(state == 0) { 
            removePCB(tmp);
            tmp->stateRead = 2;
            insertPCB(tmp);
            bufferSize = 24;
            sys_req(WRITE, COM1, "Process is now blocked\n", &bufferSize);

        } else if(state == 1) {
            bufferSize = 18;
            sys_req(WRITE, COM1, "Error: Process is running\n", &bufferSize);

        }    
        

    } else {

        bufferSize = 22;
        sys_req(WRITE, COM1, "Process is not found\n", &bufferSize);
        
    }
}    

/**
 * Procedure..: unblockPCB
 * @param..: process name
 */
void unblockPCB(char* name){

    int bufferSize;
    int state;
    struct PCB* tmp = findPCB(name);

    if(tmp != NULL){
       
        state = tmp->stateRead;

        if(state == 2) {
           removePCB(tmp);
            tmp->stateRead = 0;
            insertPCB(tmp);
            bufferSize = 26;
            sys_req(WRITE, COM1, "Process is now unblocked\n", &bufferSize);

        } else if(state == 0) { 
            bufferSize = 18;
            sys_req(WRITE, COM1, "Error: Process is ready\n", &bufferSize);

        } else if(state == 1) {
            bufferSize = 18;
            sys_req(WRITE, COM1, "Error: Process is running\n", &bufferSize);

        }    
        

    } else {

        bufferSize = 22;
        sys_req(WRITE, COM1, "Process is not found\n", &bufferSize);
        
    }

}
