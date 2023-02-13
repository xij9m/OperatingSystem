/**
 * 
 * 
 * 
 * 
 */

// #include "mpx_supt.h"
// #include "PCBs.h" 
#include "QueueR2.h"
#include "R4.h"
 



void AllocateQueue(){
    readyQueue = (queue*)sys_alloc_mem(sizeof(struct queue)); // Initial memory allocation 
    blockedQueue = (queue*)sys_alloc_mem(sizeof(struct queue)); // Initial memory allocation 
    alarmQ = (alarmQueue*)sys_alloc_mem(sizeof(struct alarmQueue)); // Initial memory allocation 

}



/**
 * Procedure..: findPCB
 * @param..: process name
 * returning PCB pointer
 */
PCB* findPCB(char* name)   {
    /*Temporary PCB variable for the current variable*/
    PCB* current = NULL;

    /*Set the current PCB to the first element of the ready queue*/
    current = readyQueue->head;
    /*While there are elements to be looked at within the ready queue, compare its name with the parameter name.*/ 
   while(current != NULL)  {
        if(strcmp((current->processName),name) == 0)  {
            return current;
        }else   {
            if(current->next != NULL)   {
                current = current->next;
            }
            else    {
                break;
            }
        }
    }

    /*Set the current PCB to the first element of the blocked queue*/
    current = blockedQueue->head;
    /*While there are elements to be looked at within the blocked queue, compare its name with the parameter name.*/ 
    while(current != NULL)  {
        if(strcmp(current->processName,name) == 0)  {
            return current;
        }else   {
            if(current->next != NULL)   {
                current = current->next;
            }
            else    {
                break;
            }
        }
    }

    return NULL;
}

/**
 * Procedure..: insertPCB
 * @param..: PCB pointer
 */

/*Still does not work? - SW*/
void insertPCB(PCB* process)    { 
    PCB* current = NULL;
    /*If ready, put it in the ready queue...*/
    if(process->stateRead == 0) {
        // serial_println("Adding to ready...");
        //ONLY THING IN QUEUE
        if(readyQueue->head == NULL)    {
            // serial_println("Adding new head");
            readyQueue->head = process;
            readyQueue->tail = process;
            // show_all_processes();
        }else   {
            current = readyQueue->head;

            /*For new head*/
            if(current->priority < process->priority)   {
                //serial_println("Adding head");
                //serial_println(process->processName);
                readyQueue->head->prev = process;
                process->next = readyQueue->head;
                readyQueue->head = process;
            }else   {
                /*Iterates through the linked list*/

                while((current->next != NULL) && current->priority >= process->priority)   {
                    current = current->next;
                }
                /*If reach the end of queue before placing process*/
                if(current->next == NULL && current->priority >= process->priority) {
                    process->prev = current;
                    current->next = process;
                    readyQueue->tail = process;
                }
                else{

                    process->prev = current->prev;
                    current->prev->next = process;
                    current->prev = process;
                    process->next = current;
                }
            }
        }
        
    

readyQueue->count++;

} else if(process->stateRead == 2)  {
        // serial_println("Adding to blocked queue...");
        //ONLY THING IN QUEUE
        if(blockedQueue->head == NULL)    {
            blockedQueue->head = process;
        }else   {
            current = blockedQueue->head;

            /*For new head*/
            if(current->priority < process->priority)   {
                blockedQueue->head->prev = process;
                process->next = blockedQueue->head;
                blockedQueue->head = process;
            }else   {
                /*Iterates through the linked list*/
                while((current->next != NULL) && current->priority >= process->priority)   {
                    current = current->next;
                }
                /*If reach the end of queue before placing process*/
                if(current == blockedQueue->tail) {
                    process->prev = current;
                    current->next = process;
                    blockedQueue->tail = process;
                /*Inserting in middle of queue*/
                }else   {

            //NEW HEAD
            if(current->priority < process->priority){
                blockedQueue->head->prev = process;
                process->next = blockedQueue->head;
                blockedQueue->head = process;
            }
            else{
                while((current->next != NULL) && (current->priority >= process->priority))   {

                    current = current->next;
                }
                if(current == blockedQueue->tail){
                    process->prev = current;
                    current->next = process;
                    blockedQueue->tail = process;

                }
                else{

                    process->prev = current->prev;
                    current->prev->next = process;
                    current->prev = process;
                    process->next = current;
                }
            }
        }
        
    }
}

blockedQueue->count++;

}


}



/**
 * Procedure..: removePCB
 * @param..: PCB pointer
 * returning success or failure message
 */
void removePCB(PCB* process)    {
    queue* q = NULL;

    // Is it in the ready queue
    if(process->stateRead == 0) {
        /*Set the current PCB to the first element of the ready queue*/
        q = readyQueue;

    // Is it in the blocked queue
    }else if(process->stateRead == 2)  {
        q = blockedQueue;
    }

    // Is the only thing in the queue
    if (q->count == 1){
        q->head = NULL;
        q->tail = NULL;
    }

    // remove from head
    else if(process == q->head) {
        q->head = process->next;
        process->next = NULL;
        q->head->prev = NULL;
    }

    // remove from tail
    else if(process == q->tail) {
        q->tail = process->prev;
        process->prev = NULL;
        q->tail->next = NULL;
    }

    // Remove from middle
    else {
      process->prev->next = process->next;
      process->next->prev = process->prev;

      process->next = NULL;
      process->prev = NULL;
    }
    return;
}

/**
 * Procedure..: show_all_processes
 * Displays PCB information for all PCBs in ready and blocked queues
 */
void show_all_processes()   {
    int bufferSize;
    //struct PCB* process = NULL;

    bufferSize = 17;
    sys_req(WRITE, COM1, "Ready queue PCBs:\n", &bufferSize);
    show_ready_processes();
    bufferSize = 19;
    sys_req(WRITE, COM1, "Blocked queue PCBs:\n", &bufferSize);
    show_blocked_processes();

}

/**
 * Procedure..: show_ready_processes
 * Displays PCB information for all PCBs in ready queues
 */
void show_ready_processes() {
    int bufferSize;
    struct PCB* process = NULL;

        /*Set the current PCB to the first element of the ready queue*/
    process = readyQueue->head;
    /*While there are elements to be looked at within the ready queue, compare its name with the parameter name.*/ 
   if(process == NULL){
       bufferSize = 20;
        sys_req(WRITE, COM1, "No process found!\n", &bufferSize);
        
    } 

    while(process != NULL){
        show_pcb(process->processName);
        process = process->next;
    }
    return;
}

/**
 * Procedure..: show_blocked_processes
 * Displays PCB information for all PCBs in blocked queues
 */
void show_blocked_processes()    {
    int bufferSize;
    struct PCB* process = NULL;

        /*Set the current PCB to the first element of the ready queue*/
    process = blockedQueue->head;
    /*While there are elements to be looked at within the ready queue, compare its name with the parameter name.*/ 
   if(process == NULL){
       bufferSize = 20;
        sys_req(WRITE, COM1, "No process found!\n", &bufferSize);
        
    } 

    while(process != NULL){
        show_pcb(process->processName);
        process = process->next;
    }
    return;
}

/*********************************************************************************************/
