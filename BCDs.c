#include <system.h>
#include "BCDs.h"

/**
 * Procedure..: Int2BCD
 * @param..: integer value called Int
 * returning the binary coded decimal
 */
unsigned char Int2BCD(int Int) { 
        
	return (((Int / 100) << 8) | ((Int / 10) << 4) | (Int % 10)); // Converts integers to BCD 
}

/**
 * Procedure..: BCD2Int
 * @param..: unsigned character value called BCD
 * returning the integer value.
 */
int BCD2Int(unsigned char BCD) {

	return (((BCD >> 8) * 100) + ((BCD >> 4) * 10) + (BCD & 0xF)); // Converts BCD to integers 

}
