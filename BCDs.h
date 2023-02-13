#define _BCDS_H

#define COM1 0x3f8
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8


/**
 * Procedure..: Int2BCD
 * Description..: converting integer values binary coversion decimal
 * @Params..: int integer value
 */
unsigned char Int2BCD(int Int);


/**
 * Procedure..: BCD2Int
 * Description..: converting BCD values into integer values.
 * @Params..: unsigined char BCD value
 */
int BCD2Int(unsigned char BCD);
