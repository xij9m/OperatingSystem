#ifndef _R6_H
#define _R6_H
#include "PCBs.h"
#include "R2.h"
//#include "R3.h"
#include <string.h>
#include "procsr3.h" 
//#include "mpx_supt.h"
#include <system.h>


#define PIC_MASK 0x21



//enum alloc {ALLOC, FREE}; 
enum currentOp {IDLES,READING,WRITING};

typedef struct device_control_block DCB;
typedef struct IOCBQueue IOCBQueue;
typedef struct IO_Control_Block IOCB;
typedef struct bufferCir ringbuffer;

struct device_control_block* COM1_DCB;
struct IOCBQueue* COM1_IOCB;


#define RING_SIZE_BUFFER 80 //Value can be change

u32int global_interrupt_var; //this is created for the idt_set_gate

/**
 * 
 * DCB needs to keep track of four items of info
 * 1. current location in the buffer
 * 2. buffer size
 * 3. Number of characters already transferred
 * 4. Address of requestors count variable
 * 
 * */
struct device_control_block
{
	//Is device allocated in memory?
	//SOMEWHERE create com1 dcb 
	//enum is_OPEN;
	//bool open

	int flag; // 0 -> port is closed, 1 -> port is open

	//What is the device doing?
	//Current operation (read, write, etc)
	//enum currentOp ioop;
	//enum dcb_satus status
	int status;

	//Event flag
	//binary value--->Maybe this should be an int? Page 8 of documentation
	int* event_flag;

	//address of requestor's count variable
	//u32int requestor_Count_Addr;


	// Interrupt vector
	int vector;


	//only need 1 buffer
	// User buffers...
	//Input buffer with descriptors
	char* usersBufferInput;

	//buffersize
	int* uIBuffSize;

	//Number of character already tranferred
	int uI_BuffTransfer;

	//Current location in the Buffer
	//int user_Input_Buff_location;

	//Output buffer with descriptors
	char* usersBufferOutput;

	//buffer size
	int* uOBuffSize;

	//Number of character already tranferred
	int uO_BuffTransfer;

	//Current location in the Buffer
	//int user_Output_Buff_location;

	//Internal buffer with descriptors
	ringbuffer* internalBuffer;
	//need front and rear vars---ring buffer

	//another counter for characters within the ring buffer

	//buffer size
	int iBuffSize;

	//current location in the Buffer
	//int internal_Buff_location;
};

/**
 * IOCB
 * Holds permanent characteristics of the device
 * 1. Name (I'm not sure if thats the process)
 * 2. channel number (that is not declared)
 * 3. vectors associated with the device (not created)
 * 4. address of each procedure (not created)
 * 
 * */

struct IO_Control_Block
{
	//Process being carried out
	PCB* process;

	//Device in question
	DCB* device;

	//Current operation
	enum currentOp op;

	//Character buffer
	char* buffer;

	//Size of buffer
	//comes from sys req
	int* bufferSize;

	//Count of bites transferred
	int count;
	//dcb will do that

	//Next IOCB in IOCBQueue
	IOCB* next;

	//Notes
	//According to page 11 there should be an event flag but there is none within the structs
};

//One per DCB. contains all IOCB pertaining to that device.
struct IOCBQueue	
{
	IOCB* head;
	IOCB* tail;
};


/** Note: we have to decide on whether to use the use ring buffer
 * Ring Buffer
 * 
 * Below basic psuedo code for normal generic ring buffer
 * more details on page 10 of r6 documentation
 * */
struct bufferCir
{
	char ring_buffer_storage[80];
	int in;
	int out;
	int count_characters_inBuffer;
};

//write
//Is Buffer Full
//else
//Buffer[head]=data
//increment head pointer
// if(head == tail)
//isFull_flag=true
void write_to_ring_buffer(DCB* current,char data_element);

//read
//is buffer empty
// No element avaibale to read
//else
//data = Buffer[Tail]
//increment tail pointer
//IsFull_Flag = False
char read_from_buffer(DCB* current);

//is Buffer Full
//return isFull_Flag
int is_buffer_full(DCB* current);

//Is Buffer Empty
//create temp var
//if (head ==  tail) && (isFull_Flag ! = 1)
//set temp var to 1
//return temp
int is_buffer_empty(DCB* current);

//Opens port
//Straight from page 21 to initialize the serial port
int com_open(int* eflag_p, int baud_rate);

//Closes port
//Straight from page 22 will end of a session serial port use
int com_close(void);

//Reads from port
//Straight from page 23 obtains input characters and loads them into the requestor's buffer
int com_read(char* buf_p, int* count_p);

//Writes to port
//straigh from page 24 used to initate the transfer of a block of data to the serial port
int com_write(char* buf_p, int* count_p);

//int io_scheduler(); 

int MSB(int value);

int LSB(int value);

void first_lvl_int();

void second_lvl_input();

void second_lvl_output();

#endif