#define _ITOA_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

/**
 * Procedure..: itoa
 * Description..: converting integer values to ascii code
 * Params..: int i value and char array
*/
 void itoa(int num, char str[]);


 
/**
 * Procedure..: reverse
 * Description..: converting integer values to ascii code (reverse)
 * Params..: char array
*/
 void reverse(char str[]);
