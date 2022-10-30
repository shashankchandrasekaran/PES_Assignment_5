/*
 * isha.c
 *
 * A completely insecure and bad hashing algorithm, based loosely on
 * SHA-1 (which is itself no longer considered a good hashing
 * algorithm)
 *
 * Based on code for sha1 processing from Paul E. Jones, available at
 * https://www.packetizer.com/security/sha1/
 *
 * Edited by: Shashank Chandrasekaran
 * Changes made: All functions were modified for optimization.
 * Optimized C function ISHAProcessMessageBlock and ISHACircularShift macro
 * were removed from this file as the equivalent assembly was written. This has
 * been documented in the readme
 */

#include "ISHAPMB.h" //SC: Added header for IshaProcessMessageBlock function
#include "isha.h"


/*  
 * The message must be padded to an even 512 bits.  The first padding
 * bit must be a '1'.  The last 64 bits represent the length of the
 * original message.  All bits in between should be 0. This function
 * will pad the message according to those rules by filling the MBlock
 * array accordingly. It will also call ISHAProcessMessageBlock()
 * appropriately. When it returns, it can be assumed that the message
 * digest has been computed.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
static void ISHAPadMessage(ISHAContext *ctx)
{
	/*
	 *  Check to see if the current message block is too small to hold
	 *  the initial padding bits and length.  If so, we will pad the
	 *  block, process it, and then continue padding into a second
	 *  block.
	 */
	if (ctx->MB_Idx > 55)
	{
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		memset(ctx->MBlock + ctx->MB_Idx, 00, 64 - ctx->MB_Idx); //SC: While loop replaced by memcpy
		ISHAProcessMessageBlock(ctx);
		memset(ctx->MBlock, 00, 60); //SC: Zero padding through while loop replaced by memset
	}
	else
	{
		ctx->MBlock[ctx->MB_Idx++] = 0x80;
		memset(ctx->MBlock + ctx->MB_Idx, 00, 60-ctx->MB_Idx); //SC: While loop replaced by memcpy
	}

	/*
	 *  Store the message length
	 */
	*((uint32_t *)(ctx->MBlock+60))=__builtin_bswap32(ctx->Length);//SC: 8 byte copy Replaced by a single 32 byte copy
	ISHAProcessMessageBlock(ctx);
}



void ISHAReset(ISHAContext *ctx)
{
	ctx->Length 	 = 0;  //SC: Reset Length instead of Length_Low and Length_High
	ctx->MB_Idx      = 0;

	ctx->MD[0]       = 0x67452301;
	ctx->MD[1]       = 0xEFCDAB89;
	ctx->MD[2]       = 0x98BADCFE;
	ctx->MD[3]       = 0x10325476;
	ctx->MD[4]       = 0xC3D2E1F0;

	ctx->Computed    = 0;
	ctx->Corrupted   = 0;
}


void ISHAResult(ISHAContext *ctx, uint8_t *digest_out)
{
	if (ctx->Corrupted)
	{
		return;
	}

	if (!ctx->Computed)
	{
		ISHAPadMessage(ctx);
		ctx->Computed = 1;
	}

	//SC: Loop unrolling, bswap32 and typcasting of digest_out to 32 bit
	*(uint32_t *)(digest_out)=__builtin_bswap32(ctx->MD[0]);
	*(uint32_t *)(digest_out+4)=__builtin_bswap32(ctx->MD[1]);
	*(uint32_t *)(digest_out+8)=__builtin_bswap32(ctx->MD[2]);
	*(uint32_t *)(digest_out+12)=__builtin_bswap32(ctx->MD[3]);
	*(uint32_t *)(digest_out+16)=__builtin_bswap32(ctx->MD[4]);

	return;
}

void ISHAInput(ISHAContext *ctx, const uint8_t *message_array, size_t length)
{
	//SC: Length, Computed and corrupted check eliminated
	ctx->Length += length*8;  //SC: Length calculation taken out of loop as it is invariant
	//SC: While loop replaced by for loop, and i declared as register int for better speed
	for(register int i=length;i>0;i--) //SC: Corruption check removed
	{
		ctx->MBlock[ctx->MB_Idx++] = *(message_array++); //SC: Message array incremented here itself
		//SC: All the Length low and length high checks removed as length_low would never overflow
		if (ctx->MB_Idx == 64)
		{
			ISHAProcessMessageBlock(ctx);
		}
	}
}


