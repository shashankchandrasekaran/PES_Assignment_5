/*
 * pbkdf2.c
 *
 * A perfectly legitimate implementation of HMAC and PBKDF2, but based
 * on the "ISHA" insecure and bad hashing algorithm.
 * 
 * Author: Howdy Pierce, howdy.pierce@colorado.edu
 *
 * Edited by: Shashank Chandrasekaran
 * Changes made: All functions were modified for optimization
 */

#include <assert.h>

#include "pbkdf2.h"

uint32_t flag=1;//SC: Used for executing keypad,ipad and opad only once

/*
 * See function description in pbkdf2.h
 */
void hmac_isha(const uint8_t *key, size_t key_len,
		const uint8_t *msg, size_t msg_len,
		uint8_t *digest)
{
	static uint8_t ipad[ISHA_BLOCKLEN]; //SC: Declared static
	static uint8_t opad[ISHA_BLOCKLEN]; //SC: Declared static
	static uint8_t keypad[ISHA_BLOCKLEN]; //SC: Declared static
	uint8_t inner_digest[ISHA_DIGESTLEN];
	ISHAContext ctx;
	static ISHAContext Ipad_ctx,Opad_ctx; /*SC: Created 2 structures to store state when ISHAInput
                                        is called with ipad and opad*/

	if(flag==1) //SC: Execute the calculation of keypad,ipad and opad once
	{
		//SC:If condition of key_len > ISHA_BLOCKLEN removed as it will always be greater.
		//SC: Else condition removed
		memcpy(keypad,key,key_len); //SC: Copying of key to keypad in for loop replaced by memcpy
		memset(keypad+key_len,00,ISHA_BLOCKLEN-key_len); //SC: Zero padding through memset instead of loop

		// XOR key into ipad and opad
		for (int i=0; i<key_len; i++) //SC: XOR key only till key_len as rest of elements has zero
		{
			ipad[i] = keypad[i] ^ 0x36;
			opad[i] = keypad[i] ^ 0x5c;
		}
		//SC: For remaining elements till ISHA_BLOCKLEN, use memset
		memset(ipad+key_len,0x36,ISHA_BLOCKLEN-key_len);
		memset(opad+key_len,0x5c,ISHA_BLOCKLEN-key_len);

		//SC: Stores states when ipad and opad are send as message
		ISHAReset(&Ipad_ctx);
		ISHAInput(&Ipad_ctx, ipad, ISHA_BLOCKLEN);
		ISHAReset(&Opad_ctx);
		ISHAInput(&Opad_ctx, opad, ISHA_BLOCKLEN);
	}

	// Perform inner ISHA
	ctx=Ipad_ctx; //SC: Equate the states in Ipad_Ctx to ctx
	ISHAInput(&ctx, msg, msg_len);
	ISHAResult(&ctx, inner_digest);

	// perform outer ISHA
	ctx=Opad_ctx; //SC: Equate the states in Opad_Ctx to ctx
	ISHAInput(&ctx, inner_digest, ISHA_DIGESTLEN);
	ISHAResult(&ctx, digest);
}


/*
 * Implements the F function as defined in RFC 8018 section 5.2
 *
 * Parameters:
 *   pass      The password
 *   pass_len  length of pass
 *   salt      The salt
 *   salt_len  length of salt
 *   iter      The iteration count ("c" in RFC 8018)
 *   blkidx    the block index ("i" in RFC 8018)
 *   result    The result, which is ISHA_DIGESTLEN bytes long
 * 
 * Returns:
 *   The result of computing the F function, in result
 */
static void F(const uint8_t *pass, size_t pass_len,
		const uint8_t *salt, size_t salt_len,
		int iter, unsigned int blkidx, uint8_t *result)
{
	uint8_t temp[ISHA_DIGESTLEN];
	uint8_t saltplus[2048];
	assert(salt_len + 4 <= sizeof(saltplus));
	memcpy(saltplus,salt,salt_len); //SC: For loop replaced by memcpy to store salt in saltplus
	blkidx=__builtin_bswap32(blkidx); //SC: Reverse blkidx
	memcpy(saltplus+salt_len,&blkidx,4); //SC: Store blkidx in saltplus instead of copying byte by byte

	hmac_isha(pass, pass_len, saltplus, salt_len+4, temp);
	flag=0; //SC: Reset the flag after hmac_isha is called once
	memcpy(result,temp,ISHA_DIGESTLEN); //SC: For loop replaced by memcpy to store temp in result

	for(int j=1; j<iter; j++)
	{
		hmac_isha(pass, pass_len, temp, ISHA_DIGESTLEN, temp);
		//SC: Loop unrolling to store XOR temp and result in result
		result[0] ^= temp[0];
		result[1] ^= temp[1];
		result[2] ^= temp[2];
		result[3] ^= temp[3];
		result[4] ^= temp[4];
		result[5] ^= temp[5];
		result[6] ^= temp[6];
		result[7] ^= temp[7];
		result[8] ^= temp[8];
		result[9] ^= temp[9];
		result[10] ^= temp[10];
		result[11] ^= temp[11];
		result[12] ^= temp[12];
		result[13] ^= temp[13];
		result[14] ^= temp[14];
		result[15] ^= temp[15];
		result[16] ^= temp[16];
		result[17] ^= temp[17];
		result[18] ^= temp[18];
		result[19] ^= temp[19];
	}
}

/*
 * See function description in pbkdf2.h
 */
void pbkdf2_hmac_isha(const uint8_t *pass, size_t pass_len,
		const uint8_t *salt, size_t salt_len, int iter, size_t dkLen, uint8_t *DK)
{
	uint8_t accumulator[2560];
	assert(dkLen < sizeof(accumulator));
	flag=1; //SC: Set flag to 1 before F is called
	int l = dkLen / ISHA_DIGESTLEN + 1;
	for (int i=0; i<l; i++) {
		F(pass, pass_len, salt, salt_len, iter, i+1, accumulator + i*ISHA_DIGESTLEN);
	}

	for (size_t i=0; i<dkLen; i++) {
		DK[i] = accumulator[i];
	}
}



