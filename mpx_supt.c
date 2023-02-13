/*************************************************************
*	This C file contains the MPX support functions 
*	which will be used through out the semester, many set
*	flags or methods that will allow us to modify
*	The behavior of MPX as it progresses throughout 
* 	the semester.
**************************************************************/
#include "mpx_supt.h"
#include <mem/heap.h>
#include <string.h>
#include <core/serial.h>
#include "QueueR2.h"
#include "PCBs.h"
#include "itoa.h"
//#include "R6.h"

// global variable containing parameter used when making 
// system calls via sys_req
param params;   
extern int eflag_p;
// global for the current module
int current_module = -1;  
static int io_module_active = 0;
static int mem_module_active = 0;

// If a student created heap manager is implemented this
// is a pointer to the student's "malloc" operation.
u32int (*student_malloc)(u32int);

// if a student created heap manager is implemented this
// is a pointer to the student's "free" operation.
int (*student_free)(void *);

PCB* cop; //currently running process
IOCB* current;
struct context* oldContext;



/* *********************************************
*	This function is use to issue system requests
*	for service.  
*
*	Parameters:  op_code:  Requested Operation, one of
*					READ, WRITE, IDLE, EXIT
*			  device_id:  For READ & WRITE this is the
*					  device to which the request is 
*					  sent.  One of DEFAULT_DEVICE or
*					   COM_PORT
*			   buffer_ptr:  pointer to a character buffer
*					to be used with READ & WRITE request
*			   count_ptr:  pointer to an integer variable
*					 containing the number of characters
*					 to be read or written
*
*************************************************/
int sys_req( 	int  op_code,
			int device_id,
			char *buffer_ptr,
			int *count_ptr )

{
	int return_code =0;

  if (op_code == IDLE || op_code == EXIT){
    // store the process's operation request
    // triger interrupt 60h to invoke
    params.op_code = op_code;
    asm volatile("int $60");
  }// idle or exit

  else if (op_code == READ || op_code == WRITE) {
    // validate buffer pointer and count pointer
    if (buffer_ptr == NULL)
      return_code = INVALID_BUFFER;
    else if (count_ptr == NULL || *count_ptr <= 0)
      return_code = INVALID_COUNT; 

    // if parameters are valid store in the params structure
    if ( return_code == 0){ 
      params.op_code = op_code;
      params.device_id = device_id;
      params.buffer_ptr = buffer_ptr;
      params.count_ptr = count_ptr;

      if (!io_module_active){
        // if default device
        if (op_code == READ)
          return_code = *(polling(buffer_ptr, count_ptr));
		  			
        else if(op_code == WRITE) {//must be WRITE
          return_code = serial_print(buffer_ptr);	
        }
	    
      } else {// I/O module is implemented
        serial_println("Interrupt in sys_req");
        asm volatile ("int $60");
      } // NOT IO_MODULE
    }
  } else return_code = INVALID_OPERATION;
  
  return return_code;
}// end of sys_req

int io_manager()  {
  //If it is not a read or write operation OR If the device is not com1 or default_device (May need to be just com1) OR If buffer and count are null, return an error
  if((params.op_code != READ && params.op_code != WRITE) || (params.device_id != 111 && params.device_id != 222) || (params.buffer_ptr == NULL || params.count_ptr == NULL)) {
    return -1;
  }
  cop->stateRead = 2;
  cop->processTop = (unsigned char*)oldContext;
  serial_println("pre-alloc");
  IOCB* new = (IOCB*)sys_alloc_mem(sizeof(IOCB));
  serial_println("post-alloc");
  new->process = cop;
  if(params.op_code == READ)  {
    new->op = READING;
  }else {
    new->op = WRITING;
  }
  new->buffer = params.buffer_ptr;
  new->bufferSize = params.count_ptr;
  serial_println("Before null");
  if(COM1_IOCB == NULL) {
    serial_println("After null check");
    COM1_IOCB->head = new;
    if(new->op == WRITING)  {
      serial_println("Pre write");
      com_write(new->buffer, (int*)new->count);
    }
    else if(new->op == READING) {
      com_read(new->buffer, (int*)new->count);
    }
  }else {
    current = COM1_IOCB->head;
    while(current != NULL)  {
      current = current->next;
    }
    current = new;
  }
  serial_println("Pre scheduler");
  eflag_p = 1;
  io_scheduler();
  return 0;
}

//Not entirely sure that this is what was drawn, but will clarify tomorrow with Sam
int io_scheduler()  {
  if(eflag_p == 1)  {
    //Remove current IOCB's process
    serial_println("before remove");
    removePCB(current->process);
    serial_println("post remove");
    current->process->stateRead = 0;
    serial_println("before insert");
    insertPCB(current->process);
    serial_println("after insert");
    IOCB* t = COM1_IOCB->head;
    COM1_IOCB->head = COM1_IOCB->head->next;
    serial_println("before free");
    sys_free_mem(t);
    serial_println("after free");

    serial_println("IOCB not NULL");
    if(params.op_code == READING)  {
      com_read(COM1_IOCB->head->buffer,(int*)COM1_IOCB->head->count);
    }
    if(params.op_code == WRITE)  {
      serial_println("In conditional");
      char* counta = ".";
      itoa(*COM1_IOCB->head->bufferSize, counta);
      com_write(COM1_IOCB->head->buffer,(int*)COM1_IOCB->head->count);
    }
  }
  return 0;
}

u32int* sys_call(context* registers){
  serial_println("In sys_call");
  struct PCB* ptr = NULL;
  ptr = readyQueue->head; //Moved from 120
  cop = (PCB*) registers;
  // cop->processTop = (unsigned char*)registers;
  if(cop == NULL) {
    serial_println("Test");
    oldContext = registers;//beginning before the switch
  }else {
    //How to check op_code?? Where is this located? 
    if(params.op_code == IDLE)  {
      cop->processTop = (unsigned char*)registers;
      cop->stateRead = 0;
      insertPCB(cop);
    }else if(params.op_code == EXIT)  {
      FreePCB(cop);
    }
    else if(params.op_code == READ || params.op_code == WRITE) {
      serial_println("WRITE op_code caught");
      io_manager();
      
      //If there is another process, make sure it is not currently using same device
      //To handle this, pass each request to IO Scheduler
      if(readyQueue->head != NULL)  {
        cop = readyQueue->head;
        removePCB(cop);
        cop->stateRead = 1;
        return (u32int*)cop->processTop;
      }
    }
  }

  if(readyQueue->head != NULL)  { //improper
    while(ptr != NULL){

      if(ptr->stateSus == 0){
      cop = ptr;
      removePCB(ptr);
      cop->stateRead = 1;
      return (u32int*)cop->processTop;

      } else {
     
      ptr = ptr->next; 
     
      }

    }  

  }
return (u32int*)oldContext;

}

/*
  Procedure..: mpx_init
  Description..: Initialize MPX support software, based
			on the current module.  The operation of 
			MPX will changed based on the module selected.
			THIS must be called as the first executable 
			statement inside your command handler.

  Params..: int cur_mod (symbolic constants MODULE_R1, MODULE_R2, 			etc.  These constants can be found inside
			mpx_supt.h
*/
void mpx_init(int cur_mod)
{
  
  current_module = cur_mod;
  if (cur_mod == MEM_MODULE)
		mem_module_active = TRUE;

  if (cur_mod == IO_MODULE)
		io_module_active = TRUE;
}



/*
  Procedure..: sys_set_malloc
  Description..: Sets the memory allocation function for sys_alloc_mem
  Params..: Function pointer
*/
void sys_set_malloc(u32int (*func)(u32int))
{
  student_malloc = func;
}

/*
  Procedure..: sys_set_free
  Description..: Sets the memory free function for sys_free_mem
  Params..: s1-destination, s2-source
*/
void sys_set_free(int (*func)(void *))
{
  student_free = func;
}

/*
  Procedure..: sys_alloc_mem
  Description..: Allocates a block of memory (similar to malloc)
  Params..: Number of bytes to allocate
*/
void *sys_alloc_mem(u32int size)
{
  if (!mem_module_active)
    return (void *) kmalloc(size);
  else
    return (void *) (*student_malloc)(size);
}


/*
  Procedure..: sys_free_mem
  Description..: Frees memory
  Params..: Pointer to block of memory to free
*/
int sys_free_mem(void *ptr)
{
  if (mem_module_active)
    return (*student_free)(ptr);
  // otherwise we don't free anything
  return -1;
}

/*
  Procedure..: idle
  Description..: The idle process, used in dispatching
			it will only be dispatched if NO other
			processes are available to execute.
  Params..: None
*/
void idle()
{
  /*
  char msg[30];
  int count=0;
	
	memset( msg, '\0', sizeof(msg));
	strcpy(msg, "IDLE PROCESS EXECUTING.\n");
	count = strlen(msg);
  */
  
  while(1){
	//sys_req( WRITE, DEFAULT_DEVICE, msg, &count);
    sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
  }
}
