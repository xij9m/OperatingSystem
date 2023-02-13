#include "R5.h"
#include "mpx_supt.h"
#include "itoa.h"

allocatedblocks* allocatedblock;
freeblocks* freeblock;

int size_of_heap = 50000;

int init_heap(int size) {

	//allocating a certain amount of bytes
	size_of_heap = kmalloc(size + sizeof(allocatedblock) + sizeof(freeblock) + sizeof(cmcb));


	//Initializes free and allocated lists
	allocatedblock = (allocatedblocks*)size_of_heap;
	allocatedblock->head = NULL;
	freeblock = (freeblocks*)allocatedblock + sizeof(allocatedblock);
	freeblock->head = NULL;
	//freeblock->tail = lmcb_bottom; 


	// This commented section is not working. Needs to add a CMCB to top and LMCB to bottom, both free.
	//free block of memory
	cmcb* cmcb_head = (cmcb*)freeblock + sizeof(freeblock);
	//lmcb* lmcb_bottom = (lmcb*)size_of_heap + size + sizeof(cmcb);

	cmcb_head->type = FREE;
	cmcb_head->next = NULL;
	cmcb_head->previous = NULL;

	//May or may not be necessary? Ask Sam.
	//This simply adds CMCB_Head to free_list
	cmcb_head->size = size;
	
	cmcb_head->addr = (u32int)cmcb_head + sizeof(cmcb);
	insertCMCBFree(cmcb_head);
	//lmcb_bottom->type = FREE;

	// u32int (*alloc_mem_ptr)(u32int) = &allocated_mem;
	// void (*free_mem_ptr)(void *) = &free_mem;
	// sys_set_malloc(alloc_mem_ptr);
 //    sys_set_free(free_mem_ptr);


	
	return 0;

}

/**
 * 
 * Function..: allocated_mem
 * Using First-Fit Memory Allocation
 * low-ordered to high-ordered memory
 * first job claims the first available memory with space more
 * than or equal to it's size.
 * OS DOES NOT search for appropriate partition 
 * but just allocate the job to the nearest mem partition with sufficent size
 * 
 * 
 */
u32int allocated_mem(u32int bytes_of_mem)
{
	//find block to hold param size + sizeof(struct cmbc) + sizeof(struct lmcb)
	//if a block is free then store mem in that block
	u32int size_required = bytes_of_mem + sizeof(struct cmcb);

	u32int current_size;
	//temp CMCB variable for current variable
	cmcb* current = freeblock->head;
	//Traverse free list until a large enough space is found
	//First fit
	while (current != NULL)  {

		if (current->size >= size_required) {
			current_size = current->size;
			break;

		}

		current = current->next;

	}


	//not sure if this condition is correct
	//If the size of the current CMB minus the size required is less than the size of a CMCB, insert into allocated list.
	if ((current->size - size_required) < sizeof(struct cmcb)){
		current->type = ALLOCATED;
		//Remove from free list
		removeFreeCMCB(current);
		//Insert into allocated list
		insertCMCBAlloc(current);
		
	}else	{

		//SPLITS BLOCK OF MEMORY, SOME ALLOCATED SOME FREE

		//current = allocatedblock->head; //Current will be the allocated cmcb
		//current = 
		current->size = bytes_of_mem; //Change current size
		current->type = ALLOCATED; //Change current type

		//insert to alloclist and remove from free list
		removeFreeCMCB(current);
		insertCMCBAlloc(current);
		

		cmcb* newfreeblock = (cmcb*)((u32int)current + bytes_of_mem + sizeof(cmcb)); //create new cmcb 
		newfreeblock->type = FREE;
		newfreeblock->next = NULL;
		newfreeblock->previous = NULL;
		newfreeblock->addr = (u32int)newfreeblock + sizeof(cmcb);
		newfreeblock->size = current_size - (current->size + sizeof(cmcb));
		
		insertCMCBFree(newfreeblock); //insert to alloclist and remove it from freelist
		//newfreeblock->previous->next = newfreeblock->next;
		//newfreeblock->next->previous = newfreeblock->previous;



	}
	
	return current->addr;
}

int free_mem(u32int address)
{
	//Finds block whose memory address == param address
	cmcb* current = allocatedblock->head;
	while(current != NULL && current->addr != address)	{
		current = current->next;
	}
	//If memory block with that address is not found, return an error (-1)
	if(current == NULL)	{
		return -1;
	}

	//If memory block with that address IS FOUND, do the following...
		//Remove from allocated list
		removeAllocCMCB(current);

		//Need to check if these things exist
		if(current->previous != NULL)	{
			current->previous->next = current->next;
		}

		if(current->next != NULL)	{
			current->next->previous = current->previous;
		}

		//Inserts into free list
		insertCMCBFree(current);

		//Not confident in this
		//Merging adjacent free blocks of memory
		//Finds if there is an adjacent block before it

		//Before doing these, check existence of next and previous

		if(current->addr == (current->next->addr - current->size - (sizeof(cmcb))))	{
			current->size = current->size + current->next->size + sizeof(cmcb);	//Sets size = size of current block + size of block adjacent to it
			
			current->next = current->next->next;					//Set the next of the current block = to the value after the block to be merged.
			if(current->next != NULL)	{
				current->next->previous = current;
			}

		}
		
		if(current->previous->addr == (current->addr - current->previous->size - (sizeof(cmcb))))	{
			current->previous->size = current->previous->size + current->size + sizeof(cmcb);	//Sets size = size of current block + size of block adjacent to it

			current = current->next;					//Set the next of the current block = to the value after the block to be merged.
			if(current != NULL)	{
				//Basically a move backward.
				current->previous->previous = current->previous;
			}
		}

		
	
	return 0;
}

int isEmpty(){

 	int bufferSize = 16;

 	if(allocatedblock->head == NULL){
 		sys_req(WRITE, COM1, "Heap is empty\n", &bufferSize);
 		return 1;

 	} else {
 		sys_req(WRITE, COM1, "Heap is not empty\n", &bufferSize);
 		return 0;
 	}


}

//Prints size and address of all blocks in free memory
void show_free_mem()
{
	cmcb* current = freeblock->head;

	int addrBuffer = 30;
	char address[30];

	int sizeBuffer = 30;
	char size[30];


	while(current != NULL)	{
		//Integer to string Converts integer size to string to print it
		itoa(current->addr,address);
		sys_req(WRITE,COM1, "Address: ", &addrBuffer);
		sys_req(WRITE,COM1,address, &addrBuffer);
		sys_req(WRITE,COM1,"\n", &addrBuffer);

		itoa(current->size,size);
		sys_req(WRITE,COM1, "Size: ", &sizeBuffer);
		sys_req(WRITE,COM1, size, &sizeBuffer);
		sys_req(WRITE,COM1,"\n\n", &sizeBuffer);

		current = current->next;
	}

}

//Prints size and address of all blocks in allocated memory
void show_allocated_mem()
{
	cmcb* current = allocatedblock->head;

	int addrBuffer = 30;
	char address[30];

	int sizeBuffer = 30;
	char size[30];


	while(current != NULL)	{
		itoa(current->addr,address);
		sys_req(WRITE,COM1, "Address: ", &addrBuffer);
		sys_req(WRITE,COM1,address, &addrBuffer);
		sys_req(WRITE,COM1,"\n", &addrBuffer);

		itoa(current->size,size);
		sys_req(WRITE,COM1, "Size: ", &sizeBuffer);
		sys_req(WRITE,COM1, size, &sizeBuffer);
		sys_req(WRITE,COM1,"\n\n", &sizeBuffer);

		current = current->next;
	}
}

//function for inserting cmcbs into the allocated list
void insertCMCBAlloc(cmcb* allocated){
	cmcb* current;
	//insert at head if empty
	if (allocatedblock->head == NULL){
			allocatedblock->head = allocated;
	}
	//otherwise iterate until a space is found
	else {
		current = allocatedblock->head;
		while (current->next != NULL){
			if (allocated < current){
				allocated->previous = current->previous;
				allocated->previous->next = allocated;
				allocated->next = current;
				current->previous = allocated;
			}
			current = current->next;
		}
		if (allocated < current){
			allocated->previous = current->previous;
			allocated->previous->next = allocated;
			allocated->next = current;
			current->previous = allocated;
		}
		else {
			current->next = allocated;
			allocated->previous = current; 
		}
	}
}

//function for inserting cmcbs into the free list
void insertCMCBFree(cmcb* free){
	cmcb* current;
	//insert at head if empty
	if (freeblock->head == NULL){
			freeblock->head = free;
	}
	//otherwise iterate until a space is found
	else {
		//Sets current to head to free list
		current = freeblock->head;
		//Continues to iterate through freelist until it finds the end.
		while (current->next != NULL && current < free){
			// //If free is less than current (?)
			// if (free < current){
			// 	//Insert at that location
			// 	free->previous = current->previous;
			// 	free->previous->next = free;
			// 	free->next = current;
			// 	//Move item currently at that location back towards tail
			// 	current->previous = free;
			// }
			current = current->next;
		}

		//If free is less than current (?)
		if (free < current){
			if(freeblock->head == current)	{
				freeblock->head = free;
			}
			//Insert at that location
			free->previous = current->previous;
			free->next = current;
			if(current->previous != NULL)	{
				current->previous->next = free;
			}
			//Move item currently at that location towards the back 1 space
			current->previous = free;
		}
		//If reached end of free list, add item to end of list
		else {
			current->next = free;
			free->previous = current; 
		}
	}
}

void removeFreeCMCB(cmcb* current)	{

	if(freeblock->head == current)	{
		freeblock->head = current->next;
		if(current->next != NULL)	{
			current->next->previous = NULL;
		}
	}
	else	{
		current->previous->next = current->next;
		if(current->next != NULL)	{
			current->next->previous = NULL;
		}
	}

	current->next = NULL;
	current->previous = NULL;
}

void removeAllocCMCB(cmcb* current)	{

	if(allocatedblock->head == current)	{
		allocatedblock->head = current->next;
		if(current->next != NULL)	{
			current->next->previous = NULL;
		}
	}
	else	{
		current->previous->next = current->next;
		if(current->next != NULL)	{
			current->next->previous = NULL;
		}
	}

	current->next = NULL;
	current->previous = NULL;
}
