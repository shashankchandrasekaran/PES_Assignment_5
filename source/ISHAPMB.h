/*
 * ISHAPMB.h
 * Author: Shashank Chandrasekaran
 * Created on: Oct 29, 2022
 * Description: .H file contains the extern function declaration of ISHAProcessMessageBlock
 * which is written in assembly language
 */

#ifndef ISHAPMB_H_
#define ISHAPMB_H_
#include "isha.h"
/*
 * Processes the next 512 bits of the message stored in the MBlock
 * array.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
void ISHAProcessMessageBlock(ISHAContext *ctx);


#endif /* ISHAPMB_H_ */
