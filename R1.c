
#include "BCDs.h"
#include "itoa.h"
#include "R1.h"
#include "../include/core/io.h"
#include "../include/system.h"
#include "../include/core/serial.h"
#include "mpx_supt.h"
#include <string.h>
#include "comhand.h"



void getDate() { 					/** Gets date in M-D-YY format*/

  cli(); 										/** Disable interupts*/
	int bufferSize = 50;			/**  Sets buffer size*/
	char* seps[2] = {"-", '\0'};

	char* elementPtr = "\0";	/** Initializes elementPtr*/
	int IntByte;							/**  Integer representation of Byte*/
	char BCD_Byte;						/**  BCD version of Byte*/
	
	// Getting month
	outb(0x70,0x08); 							/**  Writing the value to the RTC index register */
	BCD_Byte = inb(0x71); 				/**  Reading the result from location 0x71 */
	IntByte = BCD2Int(BCD_Byte); 	/**  Convert the result from BCD to an integer*/

	itoa(IntByte,elementPtr);													/** integer -> char*/
	sys_req(WRITE, COM1, elementPtr, &bufferSize);		/** Prints elementPtr (month)*/
	sys_req(WRITE, COM1, "-", &bufferSize);		/** Prints elementPtr (month)*/
	bufferSize = 100;			/**  Sets buffer size*/
	elementPtr = "\0";	/**  Initializes elementPtr*/
	

	/** Getting day*/
	outb(0x70,0x07);							/**  Writing the value to the RTC index register */
	BCD_Byte = inb(0x71);					/**  Reading the result from location 0x71 */
	IntByte = BCD2Int(BCD_Byte);	/**  Convert the result from BCD to an integer*/

	itoa(IntByte,elementPtr);										/** integer -> char*/
	
	/** Prints separators and month*/
	sys_req(WRITE, COM1, elementPtr, &bufferSize);
	sys_req(WRITE, COM1, seps[0], &bufferSize);		/** Prints elementPtr (month)*/

	
	/** Getting year*/
	outb(0x70,0x09);
	BCD_Byte = inb(0x71);
	IntByte = BCD2Int(BCD_Byte);
	
	itoa(IntByte,elementPtr);
	
	/** Prints year*/
	sys_req(WRITE, COM1, elementPtr, &bufferSize);
	

	
	sti(); /**  Enable interrupts*/

	

}

/** HH:MM:SS format*/
char* getTime() { 

	cli(); 										/** Disable interupts*/
	
	char* elementPtr = "\0";	/**Used for printing each element*/
	int IntByte;							/**Contains integer representation of element*/
	char BCD_Byte;						/**Contains bcd representation of element*/
	int bufferSize = 8;				/**Size of buffer for reading and writing*/
	
	// Getting Hours
	outb(0x70,0x04); 												/** Writing the value to the RTC index register */
	BCD_Byte = inb(0x71); 									/** Reading the result from location 0x71 */
	IntByte = BCD2Int(BCD_Byte); 						/** Convert the result from BCD to an integer*/
	
	itoa(IntByte,elementPtr);										/**Converts integer to string*/
	if(IntByte < 10) { 													/**Checking if month is less than 10, add a 0 before it to match formatting  */
		sys_req(WRITE, COM1, "0", &bufferSize);
	}

	sys_req(WRITE, COM1, elementPtr, &bufferSize);			/**	Writes hour to buffer*/

	// Getting Minutes
	outb(0x70,0x02);												/** Writes minutes to RTC register*/
	BCD_Byte = inb(0x71);										/** Reading the result from location 0x71 */
	IntByte = BCD2Int(BCD_Byte);						/** Convert the result from BCD to an integer*/
	sys_req(WRITE, COM1, ":", &bufferSize);
	
	itoa(IntByte,elementPtr);										/**Integer -> String*/
	if(IntByte < 10) { 													/** Checking if minutes is less than 10  */		
		sys_req(WRITE, COM1, "0", &bufferSize);
	}

	sys_req(WRITE, COM1, elementPtr, &bufferSize);		/**Writes minutes to buffer*/

	
	/** Getting seconds*/
	outb(0x70,0x00);																/**Writes minutes to RTC register*/
	BCD_Byte = inb(0x71);														/** Reading the result from location 0x71*/
	IntByte = BCD2Int(BCD_Byte);										/** Convert the result from BCD to integer*/
	
	itoa(IntByte,elementPtr);												/** integer -> elementPtr*/
	sys_req(WRITE, COM1, ":", &bufferSize);					/**Prints separator*/
	if(IntByte < 10)	{															/**If seconds is less than 10, add 0 before printing it*/
		sys_req(WRITE, COM1, "0", &bufferSize);
	}
	sys_req(WRITE, COM1, elementPtr, &bufferSize);	/** Prints seconds*/
	

	sti(); 																					/** Enable interrupts*/

	return elementPtr;
	
}



void setDate() {

	cli(); /** Disable interupts*/


	/**Variables*/
	int bufferSize;		/** Size of buffer*/
	int tmpDay;				/** Integer storage of day*/
	int tmpMon;				/**Integer storage of month*/
	int tmpYr;				/** Integer storage of year*/
	int tmpYrLow;			/** Integer storage of last 2 digits of year (ex. 2021 -> 21)*/
	char tmpByte;			/** Character representation of Byte*/

	int valid = 0;
	
	
	/**Setting Month*/
	bufferSize = 35;	
	while(!valid)	{
	sys_req(WRITE, COM1,"Choose Month (from 1 to 12):\n", &bufferSize);					/** Prompts user to choose a month*/
	char month[5]; 																															/** Initialize the array where the input will be stored */
	memset(month,'\0',5); 																											/** Fills month with terminating characters, wiping it clean*/
	bufferSize = 5; 																														/** Setting buffer size*/
	sys_req(READ, COM1, month, &bufferSize); 																		/** Request to read the input*/
	tmpMon = atoi(month); 																											/** Converting the input from a string to integers*/

	 
	 if (tmpMon > 12 || tmpMon < 1) { 																					/** Checking if the input is valid (The range is between 1-12) */
	 bufferSize = 14;																														/** Size of string to be written*/
	 sys_req(WRITE, COM1,"Invalid input\n", &bufferSize);												/** Writes error message*/
	 bufferSize = 30;																														/** Sets buffer size*/
	 
	 
	 } else { 														/**If the input is valid then process.. */
		tmpByte = Int2BCD(tmpMon); 					/** Converting integers to BCD*/
		outb(0x70,0x08); 										/** Writing the value to the RTC index register  */
		outb(0x71,tmpByte); 								/** Writing the new value to the location */
		valid = 1;
	 } 
	}
	 
	valid = 0;
	//Setting Day */
	while(!valid)	{
		bufferSize = 27;																										/**Size of string to be printed*/
		sys_req(WRITE, COM1,"Choose Day (from 1 to 31):\n", &bufferSize);		/**Prompts user to choose a day*/
		
		char day[5]; 														/** Initialize the array where the input will be stored */
		memset(day,'\0',5);											/** Cleans day char array by filling it with terminating characters*/
		bufferSize = 5; 												/** Setting buffer size*/
		sys_req(READ, COM1, day, &bufferSize); 	/** Request to read the input*/
		tmpDay = atoi(day); 										/** Converting the input from a string to integers*/
		 
		if (tmpDay > 31 || tmpDay < 1) { 				/** Checking if the input is valid (The range is between 1-31) */
				bufferSize = 18;
				sys_req(WRITE, COM1,"Invalid input\n", &bufferSize);
		} else { 
		 tmpByte = Int2BCD(tmpDay); 		/** Converting integers to BCD8*/
		 outb(0x70,0x07); 							/** Writing the value to the RTC index register*/
		 outb(0x71,tmpByte); 						/** Writing the new value to the location*/
			valid = 1;
		} 
	 
	}

	valid = 0;
	/**Setting Year*/ 
	bufferSize = 11;
	while(!valid)	{
		sys_req(WRITE, COM1,"Enter the year:\n", &bufferSize);
		
		char year[10]; 																/**Initialize the array where the input will be stored*/
		memset(year,'\0',10);													/** Fills year array with terminating characters*/
		bufferSize = 10; 															/**Setting buffer size*/
		sys_req(READ, COM1, year, &bufferSize); 			/**Request to read the input*/
		char yearLow[3] = {year[2], year[3], '\0'};		/**Detects last 2 elements of year*/	
		tmpYrLow = atoi(yearLow); 										/**Converting the input from a string to integer*/
		tmpYr = atoi(year);														/**Converting the input from a string to integer*/
		 
		if (tmpYr > 2100 || tmpYr < 1700) { 					/**Checking if the input is valid */
			bufferSize = 50;
		  sys_req(WRITE, COM1,"Invalid input, make sure to input 4 digits\n", &bufferSize);
		} else {	 
		  tmpByte = Int2BCD(tmpYrLow); 								/** Converting integers to BCD*/
		  outb(0x70,0x09); 														/**Writing the value to the RTC index register*/
		  outb(0x71,tmpByte); 												/** Writing the new value to the location*/	 
		  valid = 1;
		}
	}

	
	sti(); /** Enable interrupts*/
}

void setTime() {
cli(); /** Disable interupts*/

	int bufferSize;
	int tmpHour;
	int tmpMin;
	int tmpSc;
	char tmpByte;
	int valid = 0;

	/**Setting Hours*/
	bufferSize = 38;
	while(!valid)	{
		sys_req(WRITE, COM1,"Enter Hours (from 1 to 24):\n", &bufferSize);

		char hour[5]; 													/** Initialize the array where the input will be stored */
		memset(hour,'\0',5); 
		bufferSize = 5; 												/** Setting buffer size*/
		sys_req(READ, COM1, hour, &bufferSize); /** Request to read the input*/

		tmpHour = atoi(hour); 									/** Converting the input from a string to integers*/

		if (tmpHour > 24 || tmpHour < 1) { 			/** Checking if the input is valid (The range is between 1-12)*/ 
		  bufferSize = 14;
		  sys_req(WRITE, COM1,"Invalid input\n", &bufferSize);
		} else { 																/** If the input is valid then process.. */
		  tmpByte = Int2BCD(tmpHour); 					/** Converting integers to BCD*/
		  outb(0x70,0x04); 											/** Writing the value to the RTC index register  */
		  outb(0x71,tmpByte); 									/** Writing the new value to the location*/
		  valid = 1;
		} 
	}

	valid = 0;
	/**Setting Minutes*/ 
	bufferSize = 30;
	while(!valid)	{
	sys_req(WRITE, COM1,"Enter Minutes (from 0 to 60):\n", &bufferSize);

	char min[5]; 														/**Initialize the array where the input will be stored */
	memset(min,'\0',5);
	bufferSize = 5; 												/**Setting buffer size*/
	sys_req(READ, COM1, min, &bufferSize); 	/**Request to read the input*/

	tmpMin = atoi(min); 										/** Converting the input from a string to integers*/

	 if (tmpMin > 60 || tmpMin < 0) { 			/**Checking if the input is valid (The range is between 1-31)*/
	 bufferSize = 18;
	 sys_req(WRITE, COM1,"Invalid input\n", &bufferSize);

	 } else {

		 tmpByte = Int2BCD(tmpMin); 						/** Converting integers to BCD*/
		 outb(0x70,0x02); 											/** Writing the value to the RTC index register*/
		 outb(0x71,tmpByte); 										/** Writing the new value to the location*/
	 	 valid = 1;

	 } 
	}
	valid = 0;

	/**Setting Seconds*/
	bufferSize = 35;
	while(!valid)	{
	sys_req(WRITE, COM1,"Enter Seconds (from 0 to 60):\n", &bufferSize);

	char sec[5]; 															/** Initialize the array where the input will be stored*/
	memset(sec,'\0',5);
	bufferSize = 5; 													/** Setting buffer size*/
	sys_req(READ, COM1, sec, &bufferSize); 		/** Request to read the input*/

	tmpSc = atoi(sec); 												/** Converting the input from a string to integers*/

	 if (tmpSc > 60 || tmpSc < 0) { 					/** Checking if the input is valid */
	 bufferSize = 18;
	 sys_req(WRITE, COM1,"Invalid input\n", &bufferSize);

	 } else {

	  tmpByte = Int2BCD(tmpSc); 	/** Converting integers to BCD*/
	  outb(0x70,0x00); 					/** Writing the value to the RTC index register*/
	  outb(0x71,tmpByte); 				/** Writing the new value to the location*/
	 	valid = 1;

	 }
	}

	sti(); /**Enable interrupts*/

}

/**Returns current version of the OS*/
void version() {

	char buffer[60];
	int bufferSize = 45;
	memset(buffer,'\0',60);
	strcpy(buffer,"Version 6.0, Completion date: 12/9/2021");
	sys_req(WRITE, COM1, buffer, &bufferSize);
	

}


/** Help screen for assistance with using functions*/

void help()	{

      int bufferSize;  	 //Size of buffer
			bufferSize = 1950;

			//sys_req(WRITE, COM1, "\e[1;1H\e[2J", &bufferSize);
			sys_req(WRITE, COM1, "\033[1;34mCommand: gettime\n", &bufferSize);
			sys_req(WRITE, COM1, "Retrieves the current time from Real Time Clock registers\n", &bufferSize);
		
			sys_req(WRITE, COM1, "\nCommand: settime\n", &bufferSize);		
			sys_req(WRITE, COM1, "Allows you to set the time within the Real Time Clock registers\n", &bufferSize);
		
			sys_req(WRITE, COM1, "\nCommand: getdate\n", &bufferSize);
			sys_req(WRITE, COM1, "Allows you to get the date within the Real Time Clock registers\n", &bufferSize);
		
			sys_req(WRITE, COM1, "\nCommand: getdate\n", &bufferSize);
			sys_req(WRITE, COM1, "Allows you to set the date within the Real Time Clock registers\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: version\n", &bufferSize);
			sys_req(WRITE, COM1, "Returns the currently installed version of this program and release date\n", &bufferSize);
			
			sys_req(WRITE, COM1, "\nCommand: shutdown\n", &bufferSize);
			sys_req(WRITE, COM1, "Shuts down the program after a y (Yes) or n (No) confirmation\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: suspend\n", &bufferSize);
			sys_req(WRITE, COM1, "Places a PCB in the suspended state and reinserts it into\n", &bufferSize);
			sys_req(WRITE, COM1, "the appropriate queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: resume\n", &bufferSize);
			sys_req(WRITE, COM1, "Places a PCB in the not suspended state and reinserts it\n", &bufferSize);
			sys_req(WRITE, COM1, "into the appropriate queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: setpriority\n", &bufferSize);
			sys_req(WRITE, COM1, "Sets a PCB's priority and reinserts the process into the correct place\n", &bufferSize);
			sys_req(WRITE, COM1, "in the correct queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: showpcb\n", &bufferSize);
			sys_req(WRITE, COM1, "Displays the following information for a PCB:\n", &bufferSize);
			sys_req(WRITE, COM1, "Process Name, Class, State, Suspended Status, Priority\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: showallprocesses\n", &bufferSize);
			sys_req(WRITE, COM1, "Displays the following information for each PCB in the ready and blocked queues:\n", &bufferSize);
			sys_req(WRITE, COM1, "Process Name, Class, State, Suspended Status, Priority\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: showreadyprocesses\n", &bufferSize);
			sys_req(WRITE, COM1, "Displays the following information for each PCB in the ready queue:\n", &bufferSize);
			sys_req(WRITE, COM1, "Process Name, Class, State, Suspended Status, Priority\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: showblockedprocesses\n", &bufferSize);
			sys_req(WRITE, COM1, "Displays the following information for each PCB in the blocked queue:\n", &bufferSize);
			sys_req(WRITE, COM1, "Process Name, Class, State, Suspended Status, Priority\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: createpcb\n", &bufferSize);
			sys_req(WRITE, COM1, "This command will setup a PCB and insert it\n", &bufferSize);
			sys_req(WRITE, COM1, "in the appropriate queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: deletepcb\n", &bufferSize);
			sys_req(WRITE, COM1, "This command will remove a PCB from the appropriate queue\n", &bufferSize);
			sys_req(WRITE, COM1, "and then free all associated memory\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: block\n", &bufferSize);
			sys_req(WRITE, COM1, "Finds a PCB sets it’s state to blocked and reinserts it\n", &bufferSize);
			sys_req(WRITE, COM1, "into the appropriate queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: unblock\n", &bufferSize);
			sys_req(WRITE, COM1, "Places a PCB in the unblocked state and reinserts it\n", &bufferSize);
			sys_req(WRITE, COM1, "into the appropriate queue\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: clear\n", &bufferSize);
			sys_req(WRITE, COM1, "Clears terminal\033[0m\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: yield\n", &bufferSize);
			sys_req(WRITE, COM1, "This command will cause comhand to yield to other processes\033[0m\n", &bufferSize);

				sys_req(WRITE, COM1, "\nCommand: loadr3\n", &bufferSize);
			sys_req(WRITE, COM1, "Load test processes into memory in a suspended ready state\033[0m\n", &bufferSize);
			
			sys_req(WRITE, COM1, "\nCommand: infinite\n", &bufferSize);
			sys_req(WRITE, COM1, "Calling sys_req(IDLE) for ever\033[0m\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: createalarm\n", &bufferSize);
			sys_req(WRITE, COM1, "Creates new alarm\033[0m\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: checkalarm\n", &bufferSize);
			sys_req(WRITE, COM1, "Finds and checks alarm\033[0m\n", &bufferSize);

			sys_req(WRITE, COM1, "\nCommand: removealarm\n", &bufferSize);
			sys_req(WRITE, COM1, "Remove alarm from the alarm's queue\033[0m\n", &bufferSize);


}
			








