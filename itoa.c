#include <string.h>
// #include "itoa.h"
#include "mpx_supt.h"

 /** 
  * Procedure..: itoa
  * Description..: A function that takes an integer input and converts that integer into a string. 
  */
  
 void itoa(int num, char* str){
    char rev[12];
    if (num == 0) {
        str[0] = '0';
        str[1] = '\0';
    } else {
        int counter = 0;
        int strCount = 0;
        while (num > 0) {
            int j = num % 10;
            j = (j >= 0) ? (j) : (j*-1);
            rev[counter] = j + 48;
            num = num / 10;
            counter++;
        }
        
        if (num < 0) {
            str[strCount] = '-';
            strCount++;
        }
        counter--;

        for (;counter >= 0; counter--) {
            str[strCount] = rev[counter];
            strCount++;
        }
        str[strCount] = '\0';
        
    }
    
}
