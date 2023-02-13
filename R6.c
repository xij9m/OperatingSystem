#include "R6.h"
#include "mpx_supt.h"
#include <core/interrupts.h>
#include "BCDs.h"
#include <core/tables.h>
#include <string.h>

u32int original_idt_entry;
int isFull_Flag = 1;
int baud_rate_div;
int level = 4;



int com_open(int* eflag, int baud_rate) {

	cli(); //disable interrupts

	DCB* COM1_DCB = (DCB*)sys_alloc_mem(sizeof(DCB)); // Initialize DCB;
	// IOCBQueue* COM1_IOCB = (IOCBQueue*)sys_alloc_mem(sizeof(IOCBQueue));
	ringbuffer* COM1_RING = (ringbuffer*)sys_alloc_mem(sizeof(ringbuffer));


	// Step 1
	if(eflag == NULL){
		return -101; //  Invalid Null event flag pointer
	}

	else if(baud_rate != 110 && baud_rate != 150 && baud_rate != 300 && baud_rate != 600 && baud_rate != 1200 && baud_rate != 2400 && baud_rate != 4800 && baud_rate != 9600 && baud_rate != 19200){
		return -102; // Invalid baud rate divisor
	}

	else if(COM1_DCB->flag == 1){ // Checks if the port is open or not, we need to check for the port's flag.
		return -103; // Port already open
	}

	else {


	// Step 2
	COM1_DCB->flag = 1; // Change the flag to open
	COM1_DCB->event_flag = eflag; // Save a copy of the event flag (port's -> event flag = eflag_p)
	COM1_DCB->status = IDLES;// Setting the initial device status to idle

	// The ring buffer params initialized (not sure about this step)
	COM1_RING->in = 0;
	COM1_RING->out = 0; 
	COM1_RING->count_characters_inBuffer = 0;
	// Step 3
	// Save the address of the current interrupt handler
	// Install the new handler in the interrupt vector

	original_idt_entry = idt_get_gate(0x24);
	idt_set_gate(0x24/**approved by Sam*/, (u32int)first_lvl_int/**approved by Sam*/, 0x08, 0x8e); //call idt_set_gate get old  interrupt handler
	
	// Step 4
	baud_rate_div = 115200 / (long)baud_rate;

	// Disable interrupts
	outb(COM1+1,0b00000000);

	// Set line statues reg
	outb(COM1+3,0b10000000);

	// Step 6
	// Store the high order and low order bytes into MSB and LSB
	//LSB brd
	outb(COM1+0, baud_rate_div);

	//MSB brd
	outb(COM1+1, baud_rate_div >> 8);

	//Lock divisor
	outb(COM1+3,0b00000011);

	// Enable fifo, clear, 14 byte
	outb(COM1+2,0b11000111);

	// Step 5
	// Store the value 0x08 in the Line Control Register
	outb(COM1+3, 0x08);
	
	// Step 7
	// Store the value 0x03 in the Line Control Register
	outb(COM1+3, 0x03);

	// Step 8
	// Enable the appropriate level in the PIC mask reg
	//mask = inb(PIC_MASK);
	//mask = mask & ~0x08;
	//outb(PIC_MASK, mask);

	outb(0x21, inb(0x21) && ~(1<<level));

	// Step 9
	// Store the value 0x08 in the Modem Control reg
	outb(COM1+4, 0x08);

	// Step 10
	// Store the value 0x01 in the Interrupt Enable reg
	//outb(COM1+1, 0x01); //page 19

	outb(COM1+1,0b00000001);

	(void) inb(COM1);



	}

	sti(); //enable interrupts
	return 0;
}


int com_close(void)	{

	if(COM1_DCB->flag == 0){

		return -201; // Serial port not open

	} 

	else if(COM1_DCB->flag == 1){ // Ensure that the port is open

		COM1_DCB->flag = 0; // Serial port is closed
		// Disable the appropriate level in the PIC mask reg
		outb(PIC_MASK, 0x20); //0x20 is the EOI (End of interrupt)
		// Disable all interrupts in the ACC by loading zero value to the Modem Status reg and Interrupt Enable reg 
		outb(COM1+6, 0); //loading zero value to the Modem Status reg
		outb(COM1+1, 0); //loading zero value to the Interrupt Enable reg
		// Restore the original saved interrupt vector
		idt_set_gate(0x24, (u32int)first_lvl_int, 0x08, 0x8e); //not sure if the orignal vector meaning the 1st level handler


	} 


	return 0;
}

int com_read(char* buf_p, int* count_p)	{

  //index
	// int index_counter = 0;
	if(COM1_DCB->flag == 0) {
		return -301; // Port's not open

	} 

	else if(buf_p == NULL){
		return -302; // Invalid buffer address
	}

	else if(count_p == NULL){
		return -303; // Invalid count address or count value
	}

	else if(COM1_DCB->status == WRITING || COM1_DCB->status == READING){
		return -304; // Device is busy
	}

	else if(COM1_DCB->flag == 1 && COM1_DCB->status == IDLES){

		COM1_DCB->usersBufferInput = buf_p;// Initialize the input buffer
		COM1_DCB->uIBuffSize = count_p;
		COM1_DCB->status = READING; // Set the status to READING
		COM1_DCB->event_flag = 0; // Clear the caller's event flag
		COM1_DCB->uI_BuffTransfer = 0;
		// Copy chars from the ring buffer to the requestor's buffer (output buffer?) until the ring buffer is emptied or requested count's been reached
		//byte to a byte
		while((COM1_DCB->internalBuffer->count_characters_inBuffer != 0) && (*COM1_DCB->usersBufferInput-1 != '\r'))	{
			*(COM1_DCB->usersBufferInput) = COM1_DCB->internalBuffer->ring_buffer_storage[COM1_DCB->internalBuffer->out];
			COM1_DCB->uI_BuffTransfer++;
			COM1_DCB->usersBufferInput++;
			COM1_DCB->internalBuffer->out = (COM1_DCB->internalBuffer->out+1) %RING_SIZE_BUFFER;
		}
	
		// Copied chars should be removed from the ring buffer
		//COM1_RING->ring_buffer_storage = 0;
		//sys_free_mem
		// If more chars are needed, return. If the block is completed proceed.
		COM1_DCB->status = IDLES; // Reset DCB status to IDLES
		// Set the event flag
		*COM1_DCB->event_flag = 1;
		// Return the actual count 
		return COM1_DCB->uI_BuffTransfer;
	} 

	return 0;
}

int com_write(char* buf_p, int* count_p)	{

	if(COM1_DCB->flag == 0) {
		return -401; // Port's not open

	} 

	else if(buf_p == NULL){
		return -402; // Invalid buffer address 
	}

	else if(count_p == NULL){
		return -403; // Invalid count address or count value
	}

	else if(COM1_DCB->status == WRITING || COM1_DCB->status == READING){
		return -404; // Device is busy
	}

	else if(COM1_DCB->flag == 1 && COM1_DCB->status == IDLES){
		COM1_DCB->usersBufferOutput = buf_p;// Initialize the output buffer
		COM1_DCB->uOBuffSize = count_p;// Install the buffer pointer and counters in the DCB
		COM1_DCB->uO_BuffTransfer = 0;
		COM1_DCB->status = WRITING; // Set the current status to WRITING
		COM1_DCB->event_flag = 0; // Clear the caller's event flag
		// Get the first char from the requestor's buffer and store in the output reg
		outb(COM1, COM1_IOCB->head->buffer[0]);
		COM1_DCB->usersBufferOutput++;
		COM1_DCB->uO_BuffTransfer++;
		// Enable write interrupts by setting bit 1 of the Interrupt Enable reg
		outb(COM1+1, 0x01);
	}

	return 0;
}

void write_to_ring_buffer(DCB* current,char data_element)
{
	int msgbufferSize = 80;
	if(is_buffer_full(current))
	{
		sys_req(WRITE, COM1, "Buffer is Full\n", &msgbufferSize);

		current->internalBuffer->out = (current->internalBuffer->out + 1) % 80;
	}
	else
	{
		(current->internalBuffer->ring_buffer_storage[current->internalBuffer->in]) = data_element;///NEEDS TO BE FIXED
		sys_req(WRITE, COM1, "Writing to buffer\n", &msgbufferSize);
		current->internalBuffer->in = (current->internalBuffer->in+1) % RING_SIZE_BUFFER;
		if(current->internalBuffer->in == current->internalBuffer->out)
		{
			isFull_Flag = 1;
		}

	}
}

char read_from_buffer(DCB* current)
{
	int msgbufferSize = 30;
	char data_element;

	if(is_buffer_empty(current))
	{
	 sys_req(WRITE, COM1, "Buffer is empty\n", &msgbufferSize);
	 data_element = '\0';
	}
	else
	{
		data_element = (current->internalBuffer->ring_buffer_storage[current->internalBuffer->out]); //NEEDS TO BE FIXED
		sys_req(WRITE, COM1, "Reading from buffer\n", &msgbufferSize);
		current->internalBuffer->out = (current->internalBuffer->out + 1) % RING_SIZE_BUFFER;
		isFull_Flag = 0;
	}

	return data_element;
}

int is_buffer_full(DCB* current)
{
	if(current->internalBuffer->out != '\0')	{
		return 1;
	}
	return 0;
}

int is_buffer_empty(DCB* current)
{
	if((current->internalBuffer->in == current->internalBuffer->out) && (isFull_Flag != 1))
	{
		return 1;
	}
	return 0;
}


void first_lvl_int() { //Top handler

	if(COM1_DCB->flag == 0) { // Port's not open

	//cli();

	//klogv("Interrupt is running");

	//char in = inb(COM1);

	//outb(COM1, in); // FOR TESTING PURPOSES

	//outb(PIC_MASK, 0x20); // Clears interrupt

	//sti();

	} else {

	cli();

		int type = inb(COM1+2);
		int bit1 = type>>1 & 1;
		int bit2 = type>>2 & 1;

		if(!bit1 && !bit2){ // Modem status reg
			inb(COM1+6);
		}	

		else if(bit1 && !bit2){ // Output Interrupt
			second_lvl_output();
		}

		else if(!bit1 && bit2){ // Input Interrupt
			//second_lvl_input();

			char in = inb(COM1);

			outb(COM1, in); // FOR TESTING PURPOSES

		}

		else if(bit1 && bit2){ // Line Status Interrupt
			inb(COM1+5);
		}

			klogv("Interrupt is running");

			//outb(PIC_MASK, 0x20); // Clears interrupt
			//outb(0x20, 0x20);

			sti();

		}


	outb(0x20, 0x20);

return;

}

void second_lvl_input(){

	char in = inb(COM1);// Read a char from input reg

	if(COM1_DCB->status != READING){ // if not reading status != READING
		
		// COM1_DCB->internalBuffer->ring_buffer_storage
		if(!is_buffer_full(COM1_DCB))	{
			// Store char in ring buffer
			write_to_ring_buffer(COM1_DCB, in);
		}else	{
			// if buffer is full discard char
			in = NULL;	
		}
		// if either go back to first lvl interrupt handler
		return;
		
	} 

	else if(COM1_DCB->status == READING){
		// Store char in the requestor's input buffer
		COM1_DCB->usersBufferInput[COM1_DCB->uI_BuffTransfer] = in; 
		COM1_DCB->uI_BuffTransfer++;

		if(in == '\r' || (COM1_DCB->uI_BuffTransfer < *COM1_DCB->uIBuffSize))	{
			if(in == '\r')	{
				COM1_DCB->usersBufferInput[COM1_DCB->uI_BuffTransfer-1] = '\0';
			}else	{
				COM1_DCB->usersBufferInput[COM1_DCB->uI_BuffTransfer] = '\0';
			}
		}
		COM1_DCB->uI_BuffTransfer = *COM1_DCB->uIBuffSize;
		// if count is not completed and char is not CR, return
		// if(count != 0 &&)
		// if the transfer is completed set status to IDLE
		COM1_DCB->status = IDLES;
		// Set the event flag and return requestor's count value
		*COM1_DCB->event_flag = 1;  
	} 

return;

}

void second_lvl_output(){

	if(COM1_DCB->status != WRITING){ // Ignore the interrupt and return
		return;
	}

	if(COM1_DCB->uOBuffSize < (int*)COM1_DCB->uO_BuffTransfer)	{
		outb(COM1,*COM1_DCB->usersBufferOutput);
		COM1_DCB->uOBuffSize++;
		COM1_DCB->uO_BuffTransfer++;
		return;

	}else	{



		// if count has not been exhausted, get the next char from the requestor's output buffer 
		// and store in the output reg, return without signaling for completion
		// Otherwise, all chars has been transferred 
		// Reset status to idle
		COM1_DCB->status = IDLES;
		// Set the event flag
		*COM1_DCB->event_flag = 1; 
		outb(0x21, inb(0x21) && ~(1<<level));
		// return the count value
		//return COM1_DCB->uI_BuffTransfer;
		// Disable write interrupt by clearing bit 1 in the interrupt enable reg	
		outb(COM1+1, 0x01);
	}	
	

return;

}
