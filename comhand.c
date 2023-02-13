/*
  ----- comhand.c -----

  Description..: Comhand gives user option to select 3 function
  whether to get help, date, or time.
  Module R1
  Module R2
  Module R3
  Module R4
  Module R5
  Module R6
  THE COMMAND HANDLER WILL ALWAYS HAVE THE HIGHEST PRIORITY!!!
*/

#include "mpx_supt.h" 
#include "comhand.h"
#include "R1.h"
#include "R2.h"
#include "R3.h"
#include "R4.h" 
#include "R5.h" 
#include "R6.h"
#include "startup.h"
#include "QueueR2.h"
#include <core/serial.h>
#include <string.h>

extern int size_of_heap;


//Retrieves commands from the serial port (polling) and interprets them. (WIP)
/**
 * 
 * Procedure..: comhand
 * Description..: Includes the commands to set and get time and date as well as version and help.
 * 
 */
 
	 

void comhand()	{
	char *menu="\033[1;34mWelcome! What would you like to do?\033[0m";
   char *options="\n1. settime\n2. gettime\n3. setdate\n4. getdate\n5. help\n6. version\n7. pcboptions\n8. infinite\n9. createalarm\n10. checkalarm\n11. removealarm\n12. initheap\n13. allocatememory\n14. freememory\n15. isempty\n16. showfreememory\n17. showallocatedmemory \n18. quit\n";
   
	char cmdBuffer[100];    //Buffer to store characters for commands
	int bufferSize = 100;  	 //Size of buffer
	int quit = 0;		 //Flag for end of getting the command
	char buffer[99];
	char bytes[30];
	char addr[30];
	int tmp1, tmp2, tmp3;
	char name[20]; 														
	memset(name,'\0',20);
  char hours[20]; 															
	memset(hours,'\0',20);
	char minutes[20]; 															
	memset(minutes,'\0',20);
	char seconds[20]; 															
	memset(seconds,'\0',20);
	serial_println("Printing menus");
   sys_req(WRITE, COM1, menu, &bufferSize);
   sys_req(WRITE, COM1, options, &bufferSize);
   
   //serial_println("Comhand starting");
   //While there are commands
		while(!quit)	{
			//Fill the buffer with terminating characters
			memset(cmdBuffer,'\0',100);
			//Set the size of the buffer to 99 (Size - 1 (for terminating characters))
			bufferSize = 99;
			//Gets the commands, adds them character by character to the buffer
			sys_req(READ,DEFAULT_DEVICE,cmdBuffer,&bufferSize);
		

   		//Checks for allowed commands, if one is not recognized, return an error message
			if(strcmp(cmdBuffer, "gettime") == 0 || strcmp(cmdBuffer, "2") == 0 )	{
				bufferSize = 19;
				sys_req(WRITE, COM1, "Getting time...\n", &bufferSize);
				getTime();
				bufferSize = 4;
				sys_req(WRITE, COM1, "\n", &bufferSize);

			}
			else if(strcmp(cmdBuffer, "settime") == 0 || strcmp(cmdBuffer, "1") == 0 )	{
				bufferSize = 19;
				sys_req(WRITE, COM1, "Setting time...\n", &bufferSize);
				setTime();
				bufferSize = 51;
				sys_req(WRITE, COM1,"Time is set! type Get time to see the new date\n", &bufferSize);
			}
			else if(strcmp(cmdBuffer, "getdate") == 0 || strcmp(cmdBuffer, "4") == 0 )	{
				bufferSize = 19;
				sys_req(WRITE, COM1, "Getting date...\n", &bufferSize);
				getDate();
				bufferSize = 4;
			  sys_req(WRITE, COM1, "\n", &bufferSize);
			}
			else if(strcmp(cmdBuffer, "setdate") == 0 || strcmp(cmdBuffer, "3") == 0 )	{
				bufferSize = 19;
				sys_req(WRITE, COM1, "Setting date...\n", &bufferSize);
				setDate();
				bufferSize = 51;
				sys_req(WRITE, COM1,"Date is set! type Get date to see the new date\n", &bufferSize);
			} 
			else if(strcmp(cmdBuffer, "help") == 0 || strcmp(cmdBuffer, "5") == 0 )	{
				help();
			}
			else if(strcmp(cmdBuffer, "version") == 0 || strcmp(cmdBuffer, "6") == 0 )	{
				version();
			}
			else if(strcmp(cmdBuffer, "pcboptions") == 0 || strcmp(cmdBuffer, "7") == 0 )	{ 
				pcboptions();
			}
			else if(strcmp(cmdBuffer, "createalarm") == 0 || strcmp(cmdBuffer, "9") == 0 )	{
				bufferSize = 50;
				sys_req(WRITE, COM1, "Type alarm name:\n", &bufferSize);
				bufferSize = 51;
				sys_req(READ,COM1,name,&bufferSize);
				bufferSize = 62;
				sys_req(WRITE,COM1,"Enter alarm hours:\n",&bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,hours,&bufferSize);
				tmp1 = atoi(hours);
				if(tmp1  > 23 && tmp1 < 0){
					bufferSize = 50; 
					sys_req(WRITE,COM1,"Invalid input!\n",&bufferSize);
					break;
				}
				bufferSize = 50;
				sys_req(WRITE,COM1,"Enter alarm minutes:\n",&bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,minutes,&bufferSize);
				tmp2 = atoi(minutes);
				if(tmp2  > 59 && tmp1 < 0){
					bufferSize = 50; 
					sys_req(WRITE,COM1,"Invalid input!\n",&bufferSize);
					break;
				}
				bufferSize = 50;
				sys_req(WRITE,COM1,"Enter alarm seconds:\n",&bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,seconds,&bufferSize);
				tmp3 = atoi(seconds);
				int alarmTime[3] = {tmp1,tmp2,tmp3};
				if(tmp3  > 59 && tmp1 < 0){
					bufferSize = 50; 
					sys_req(WRITE,COM1,"Invalid input!\n",&bufferSize);
					break;
				}
				createAlarm(name, alarmTime);
				bufferSize = 50;
				sys_req(WRITE, COM1,"New alarm created\n", &bufferSize);
				memset(name,'\0',20);
				memset(hours,'\0',20);
				memset(minutes,'\0',20);
				memset(seconds,'\0',20);
			} 
			else if(strcmp(cmdBuffer, "checkalarm") == 0 || strcmp(cmdBuffer, "10") == 0 )	{
				checkAlarms();
			} 
			else if(strcmp(cmdBuffer, "initheap") == 0 || strcmp(cmdBuffer, "12") == 0 )	{
				bufferSize = 50;
				sys_req(WRITE, COM1, "Type heap size in bytes:\n", &bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,bytes,&bufferSize);
				tmp1 = atoi(bytes);
				init_heap(tmp1);
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				memset(bytes,'\0',30);
			} 
			else if(strcmp(cmdBuffer, "allocatememory") == 0 || strcmp(cmdBuffer, "13") == 0 )	{
				bufferSize = 50;
				sys_req(WRITE, COM1, "Type amount of bytes to be allocated:\n", &bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,bytes,&bufferSize);
				tmp1 = atoi(bytes);
				allocated_mem(tmp1);
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				memset(bytes,'\0',30);
			} 
			else if(strcmp(cmdBuffer, "freememory") == 0 || strcmp(cmdBuffer, "14") == 0 )	{
				bufferSize = 50;
				sys_req(WRITE, COM1, "Enter address:\n", &bufferSize);
				bufferSize = 20; 
				sys_req(READ,COM1,addr,&bufferSize);
				tmp2 = atoi(addr);
				free_mem(tmp2);
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				memset(addr,'\0',30);
			} 
			else if(strcmp(cmdBuffer, "isempty") == 0 || strcmp(cmdBuffer, "15") == 0 )	{
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				isEmpty();
			} 
			else if(strcmp(cmdBuffer, "showfreememory") == 0 || strcmp(cmdBuffer, "16") == 0 )	{
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				show_free_mem();
			} 
			else if(strcmp(cmdBuffer, "showallocatedmemory") == 0 || strcmp(cmdBuffer, "17") == 0 )	{
				bufferSize = 3;
				sys_req(WRITE, COM1, "\n", &bufferSize);
				show_allocated_mem();
			} 
			else if(strcmp(cmdBuffer, "clear") == 0)	{
				sys_req(WRITE, COM1, "\e[1;1H\e[2J", &bufferSize);

			}else if(strcmp(cmdBuffer, "infinite") == 0 || strcmp(cmdBuffer, "8") == 0)	{
				infinite();

			}
			  else if(strcmp(cmdBuffer, "quit") == 0 || strcmp(cmdBuffer, "18") == 0 )	{

				bufferSize = 52;
				sys_req(WRITE, COM1, "Are you sure you want to quit? y (Yes) or n (No)\n", &bufferSize);
				memset(buffer,'\0',20);
				bufferSize = 20;
				sys_req(READ,COM1,buffer,&bufferSize);

				if(strcmp(buffer, "y") == 0)	{
					quit = 1;
					//Empty ready queue and then sys_req(EXIT...);
					readyQueue = (queue*)sys_alloc_mem(sizeof(struct queue)); // Initial memory allocation 
					serial_println("Quitting");
					sys_req(EXIT,DEFAULT_DEVICE, NULL, NULL);

				} else if(strcmp(buffer, "n") == 0) {
					bufferSize = 30;
				  sys_req(WRITE, COM1, "Returning to the main menu...\n", &bufferSize);
				  bufferSize = 4;
					sys_req(WRITE, COM1, "\n", &bufferSize);
				  // comhand();

				} else {
					bufferSize = 30;
				  sys_req(WRITE, COM1, "Invalid input, returning to the main menu...\n", &bufferSize);
				  bufferSize = 4;
	        sys_req(WRITE, COM1, "\n", &bufferSize);
				  // comhand();
				}
				
			} else {
				sys_req(WRITE, COM1, "Type a valid command!\n", &bufferSize);
			}
			sys_req(IDLE,COM1,NULL,NULL);
		}

		
	
}


void pcboptions(){
				char *pcbOptions="suspend\nresume\nsetpriority\nshowpcb\nshowallprocesses\nshowreadyprocesses\nshowblockedprocesses\ncreatepcb\ndeletepcb\nblock\nunblock\nhelp\nyield\nloadr3\n\033[1;34mback\033[0m\n";


				int bufferSize;  	 //Size of buffer
				char buffer[21];
				bufferSize = 19;//Not sure if this will work
				sys_req(WRITE, COM1, "\e[1;1H\e[2J", &bufferSize);
				sys_req(WRITE, COM1, "\033[1;34mPlease choose an option:\033[0m\n", &bufferSize);
			  sys_req(WRITE, COM1, pcbOptions, &bufferSize);//steven
				memset(buffer,'\0',20);
				bufferSize = 20;
				sys_req(READ,COM1,buffer,&bufferSize);
				char name[20]; 															
				memset(name,'\0',20);
				char priority[20]; 															
				memset(priority,'\0',20);
				char class[20]; 															
				memset(class,'\0',20);
				int tmp;
				int tmp1;
				int quitq = 0;

				while(!quitq){
						
				if(strcmp(buffer, "suspend") == 0)	{
							bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20; 	
							sys_req(READ,COM1,name,&bufferSize);
							suspend(name);

				} else if(strcmp(buffer, "resume") == 0){
							bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20; 	
							sys_req(READ,COM1,name,&bufferSize);
						  resume(name);

				}
				  else if(strcmp(buffer, "setpriority") == 0){
				  		bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);															
							sys_req(READ,COM1,name,&bufferSize);
							if(findPCB(name) == NULL) {
								bufferSize = 30;
								sys_req(WRITE,COM1,"Error! process not found\n",&bufferSize);	
								break;
							}
							bufferSize = 30; 
							sys_req(WRITE,COM1,"Type process priority (0-9):\n",&bufferSize);
									bufferSize = 20; 
							sys_req(READ,COM1,priority,&bufferSize);
						  tmp1 = atoi(priority);

						  if(tmp1 <= 9 && tmp1 >= 0)	{
				  				set_priority(name, tmp1);
				  				bufferSize = 8;
									sys_req(WRITE,COM1,"Success, returning to the main menu..\n",&bufferSize);	
				  		}

				  		else{
				  			bufferSize = 58; 
								sys_req(WRITE,COM1,"Invalid priority!\n",&bufferSize);
								break;
				  		}	


				}
				  else if(strcmp(buffer, "showpcb") == 0){
				  		bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20;
							sys_req(READ,COM1,name,&bufferSize);
				  	  show_pcb(name);

				}
				  else if(strcmp(buffer, "showallprocesses") == 0){
				  		show_all_processes(); //no para

				}
				  else if(strcmp(buffer, "showreadyprocesses") == 0){
				  		show_ready_processes(); //no para

				}
				  else if(strcmp(buffer, "showblockedprocesses") == 0){
				  	show_blocked_processes(); //no para

				}
				  else if(strcmp(buffer, "createpcb") == 0){
				  		bufferSize = 50;
							sys_req(WRITE,COM1,"Type process name (At least 8 characters long):\n",&bufferSize);	
							bufferSize = 20;														
							sys_req(READ,COM1,name,&bufferSize);

							if(strlen(name) < 8) {
								bufferSize = 70;
								sys_req(WRITE,COM1,"Error! Invalid process name! Must be at least 8 characters long.\n",&bufferSize);	
								break;

							} else if(strlen(name) > 20) {
								bufferSize = 70;
								sys_req(WRITE,COM1,"Error! Invalid process name! Must be at most 20 characters long.\n",&bufferSize);	
								break;

							} else if(findPCB(name) != NULL) {
								bufferSize = 30;
								sys_req(WRITE,COM1,"Error! Process name is unavailable\n",&bufferSize);	
								break;
							}

							bufferSize = 62;
							sys_req(WRITE,COM1,"Type process class: (1) for application or (2) for system process\n",&bufferSize);
							bufferSize = 12; 
							sys_req(READ,COM1,class,&bufferSize);


							if(strcmp(class, "1") == 0)	{
								tmp = atoi(class);
								bufferSize = 8;
								//sys_req(WRITE,COM1,"Success\n",&bufferSize); //comment this out
								

							}else if(strcmp(class, "2") == 0)	{
								tmp = atoi(class);
								bufferSize = 8;
								//sys_req(WRITE,COM1,"Success\n",&bufferSize); //comment this out
								

							}else	{
								bufferSize = 65; 
								sys_req(WRITE,COM1,"Invalid class! Must be an application (1) or system process (2)\n",&bufferSize);
								break;
							}

							bufferSize = 30; 
							sys_req(WRITE,COM1,"Type process priority (0-9):\n",&bufferSize);
							bufferSize = 20; 
							sys_req(READ,COM1,priority,&bufferSize);
						  tmp1 = atoi(priority);

						  if(tmp1 <= 9 && tmp1 >= 0)	{
				  				createPCB(name, tmp, tmp1);
				  				bufferSize = 8;
									sys_req(WRITE,COM1,"Success, returning to the main menu..\n",&bufferSize);	//comment this out
				  		}

				  		else{
				  			bufferSize = 58; 
								sys_req(WRITE,COM1,"Invalid priority!\n",&bufferSize);
								break;
				  		}

				}
			    else if(strcmp(buffer, "deletepcb") == 0){
			    		bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20;
							sys_req(READ,COM1,name,&bufferSize);
			    		deletePCB(name);

				}
			    else if(strcmp(buffer, "block") == 0){
			    		bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20;
							sys_req(READ,COM1,name,&bufferSize);
			    		blockPCB(name);

				}
				  else if(strcmp(buffer, "help") == 0){
				  	help();

				}
				  else if(strcmp(buffer, "unblock") == 0){
				  		bufferSize = 20;
							sys_req(WRITE,COM1,"Type process name:\n",&bufferSize);
							bufferSize = 20;
							sys_req(READ,COM1,name,&bufferSize);
				  	unblockPCB(name);

				} else if(strcmp(buffer, "back") == 0){ 
						sys_req(WRITE, COM1, "\e[1;1H\e[2J", &bufferSize);
						quitq = 1;
				  	comhand();
				
				} 
				 else if(strcmp(buffer, "loadr3") == 0)	{
							loadr3();
							//idleproc();

				} 

				else {
						bufferSize = 56;
						sys_req(WRITE, COM1, "\e[1;1H\e[2J", &bufferSize);
						sys_req(WRITE, COM1, "Invalid command\nReturning to the main menu..\n", &bufferSize);


				}

				return;

	}
			
}









