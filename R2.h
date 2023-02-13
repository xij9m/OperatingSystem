// #ifndef _R2_H
#define _R2_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

void suspend(char* processName);

void resume(char* processName);

void set_priority(char* processName, int newPriority);

void show_pcb(char* processName);

void createPCB(char* ProcessName, int ProcessClass, int ProcessPriority);

void deletePCB(char* name);

void blockPCB(char* name);

void unblockPCB(char* name);

void show_all_processes();

void show_ready_processes();

void show_blocked_processes();
