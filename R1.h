#ifndef _R1_H
#define _R1_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8


/**
 * Procedure..:getDate
 * Description..: geting the Month-Day-Year
 * @param..: No params
 */
void getDate();

/**
 * Procedure..:getTime
 * Description..: getting the Hour-Minute-Second
 * @param.: No Params
 */
char* getTime();

/**
 * Procedure..:setTime
 * Description..: setting the appropriate current time
 * @param..: No Params
 */
void setTime();

/**
 * Procedure..:setDate
 * Description..: setting the appropriate current date
 * @param..: No Params
 */

void setDate();


/**
 * Procedure..:version
 * Description..: showing the current version
 * @param..: No Params
 */

void version();

/**
 * Procedure..: help
 * Description..: help provides instruction to the user in explaining each command and their function
 * @param..: None
 */
void help();

#endif
