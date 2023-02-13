#include "R4.h"
#include "BCDs.h"
#include "R1.h"


void createAlarm(char alarmName[60], int alarmTime[3])	{
	struct alarm* newAlarm = sys_alloc_mem(sizeof(alarm));
	strcpy(newAlarm->message,alarmName);
	newAlarm->date[0] = alarmTime[0];
	newAlarm->date[1] = alarmTime[1];
	newAlarm->date[2] = alarmTime[2];
	insertAlarm(newAlarm);
	
}

void checkAlarms()	{
	while(1)	{
	int bufferSize = 60;
	struct alarmQueue* ptr = alarmQ;
 	struct alarm* current = ptr->head;
 	while(current != NULL)	{
 		//If alarm hour is less than current hour...
		//If alarm hour and current hour are the same BUT current minutes is larger than alarm minutes...
		//If alarm hour and current hour are the same, alarm minutes and current minutes are the same, BUT alarm seconds is less than or equal to 
		if(current->date[0] > BCD2Int(inb(0x04)) || 
			(current->date[0] == BCD2Int(inb(0x04)) && current->date[1] > BCD2Int(inb(0x02))) ||
			(current->date[0] == BCD2Int(inb(0x04)) && current->date[1] == BCD2Int(inb(0x02)) && current->date[2] <= BCD2Int(inb(0x00))))	{

			sys_req(WRITE, COM1, current->message, &bufferSize);
			removeAlarm(current);
			sys_req(EXIT, COM1, NULL, NULL);

		}

		current = current->next;
 	}

 	sys_req(IDLE, COM1, NULL, NULL);
 }

}

 void removeAlarm(alarm* current)	{
 	struct alarmQueue* ptr = alarmQ;

    // Is the only thing in the queue
    if (ptr->num == 1){
        ptr->head = NULL;
        ptr->tail = NULL;
    }

    // remove from head
    else if(current == ptr->head) {
        ptr->head = current->next;
        current->next = NULL;
        ptr->head->prev = NULL;
    }

    // remove from tail
    else if(current == ptr->tail) {
        ptr->tail = current->prev;
        ptr->tail->next = NULL;
        current->prev = NULL;
    }

    // Remove from middle
    else {
      current->prev->next = current->next;
      current->next->prev = current->prev;
      current->next = NULL;
      current->prev = NULL;
    }
    ptr->num = ptr->num - 1;
    return;
}

void insertAlarm(alarm* alarmName)    { 
	alarmQueue* ptr = alarmQ;
	if(ptr->num == 1)    {
		ptr->head = alarmName;
	} else{
		ptr->head->prev = alarmName;
		alarmName->next = ptr->head;
		ptr->head = alarmName;
	}



}

//char* messageBuffer;

void alarmproc() {
// int bufferSize = 29;
struct PCB* newpcb = NULL;
char* alarmcheck = "checkAlarms"; 

if(findPCB(alarmcheck) == NULL){
newpcb = SetupPCB(alarmcheck , 2, 9);
newpcb->stateSus = 0; //not suspended
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)checkAlarms; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);

}
} 
  
void getinfinite() {

// int bufferSize = 29;
struct PCB* newpcb = NULL;
char* infinte = "infinite"; 

if(findPCB(infinte) == NULL){
newpcb = SetupPCB(infinte , 2, 0);
newpcb->stateSus = 0; //not suspended
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)infinite; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
// sys_req(WRITE, COM1, "Done infinite\n", &bufferSize); //testing purposes

}
} 

void infinite() {

	char msg[30];
	int count=0;
	
	memset( msg, '\0', sizeof(msg));
	strcpy(msg, "INFINITE PROCESS EXECUTING.\n");
	count = strlen(msg);
  
	while(1){
		sys_req( WRITE, DEFAULT_DEVICE, msg, &count);
		sys_req(IDLE, DEFAULT_DEVICE, NULL, NULL);
	}
}
