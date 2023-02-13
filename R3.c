#include "R3.h"
#include "procsr3.h"


// struct context* saved_regs;

void yield(){//triggers the interrupt

	asm volatile ("int $60"); 

}


void* loadr3() { 

int bufferSize = 29;
struct PCB* newpcb = NULL;

if(findPCB("proc1") == NULL){
newpcb = SetupPCB("proc1" , 1, 0); 
newpcb->stateRead = 0; //ready
newpcb->stateSus = 1; //suspended 
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack); 
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)proc1; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
sys_req(WRITE, COM1, "Done 1\n", &bufferSize); //testing purposes

} else { sys_req(WRITE, COM1, "Error: process already exists\n", &bufferSize); } 

   

if(findPCB("proc2") == NULL){
newpcb = SetupPCB("proc2" , 1, 1);
//newpcb = findPCB("proc2"); 
newpcb->stateRead = 0; //ready
newpcb->stateSus = 1; //suspended 
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)proc2; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
sys_req(WRITE, COM1, "Done 2\n", &bufferSize);

} else { sys_req(WRITE, COM1, "Error: process already exists\n", &bufferSize); }

if(findPCB("proc3") == NULL){
newpcb = SetupPCB("proc3" , 1, 2); 
//newpcb = findPCB("proc3");
newpcb->stateRead = 0; //ready
newpcb->stateSus = 1; //suspended 
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)proc3; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
sys_req(WRITE, COM1, "Done 3\n", &bufferSize);

} else { sys_req(WRITE, COM1, "Error: process already exists\n", &bufferSize); }

if(findPCB("proc4") == NULL){
newpcb = SetupPCB("proc4" , 1, 3); 
//newpcb = findPCB("proc4");
newpcb->stateRead = 0; //ready
newpcb->stateSus = 1; //suspended 
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)proc4; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
sys_req(WRITE, COM1, "Done 4\n", &bufferSize);

} else { sys_req(WRITE, COM1, "Error: process already exists\n", &bufferSize); }

if(findPCB("proc5") == NULL){
newpcb = SetupPCB("proc5", 1, 4); 
//newpcb = findPCB("proc5");
newpcb->stateRead = 0; //ready
newpcb->stateSus = 1; //suspended 
struct context* cp = (struct context *)(newpcb->processTop); 
memset(cp , 0, sizeof(struct context)); 
cp -> fs = 0x10;
cp -> gs = 0x10;
cp -> ds = 0x10;
cp -> es = 0x10;
cp -> cs = 0x8;
cp -> ebp = (u32int)(newpcb -> processStack);
cp -> esp = (u32int)(newpcb -> processTop); 
cp -> eip = (u32int)proc5; // The function correlating to the process , ie. Proc1
cp -> eflags = 0x202;
insertPCB(newpcb);
sys_req(WRITE, COM1, "Done 5\n", &bufferSize);


} else { sys_req(WRITE, COM1, "Error: process already exists\n", &bufferSize); }
 
 return newpcb;

}