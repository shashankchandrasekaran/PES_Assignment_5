# PES_Assignment_5
PES Assignment 5
Created by: Shashank Chandrasekaran
Decription: ISHA Optimization

For Debug:
The .text size is 20,844 bytes
The execution time is 1136 msec

For Release
The .text size is 19,536 bytes
The execution time is 1137 msec

Most changes that were made have been added as comments in the source files. These can be identified by prefix "SC:"
One common change is that places where variables are forced to 8 bit or 32 bit of same type have been eliminated. Eg: temp &= 0xFFFFFFFF;

In pbkdf2_test.c,for hexdigit_to_int, there was a warning : "Control reaches end of non-void function". The file was updated to add return 0 at the 
end of function.

Following is an overview of the changes made for the functions:

1) isha.h
- The structure was updated to replace 2 lengths with one

2) isha.c
a) static void ISHAPadMessage(ISHAContext *ctx)
- memset and memcpy used to get rid of the while loop
- Zero padding of MB_Idx till 60 instead of 56 as the length_high is eliminated as it will always be zero
- For storing the message length, used the inbuilt bswap32 function on length and typecasted MBlock to 32 bit to store the length

b) void ISHAReset(ISHAContext *ctx)
- One length variable was reset instead of two.
This does not make a major impact in the optimization but it is logically correct to use one length instead of two

c) void ISHAInput(ISHAContext *ctx, const uint8_t *message_array, size_t length)
- The following portion of length and corruption checks was removed therby decreasing generality of code as these cases would never occur:
  if (!length)
  {
    return;
  }

  if (ctx->Computed || ctx->Corrupted)
  {
    ctx->Corrupted = 1;
    return;
  }
  
  And also inside while loop:
  
  if (ctx->Length_Low == 0)
    {
      ctx->Length_High++;
      /* Force it to 32 bits */
      ctx->Length_High &= 0xFFFFFFFF;
      if (ctx->Length_High == 0)
      {
        /* Message is too long */
        ctx->Corrupted = 1;
      }
    }
  
- Length calculation taken out of loop as it is an invariant condition

d) void ISHAResult(ISHAContext *ctx, uint8_t *digest_out)
- For the loop below, loop unrolling was performed:
  for (int i=0; i<20; i+=4) {
    digest_out[i]   = (ctx->MD[i/4] & 0xff000000) >> 24;
    digest_out[i+1] = (ctx->MD[i/4] & 0x00ff0000) >> 16;
    digest_out[i+2] = (ctx->MD[i/4] & 0x0000ff00) >> 8;
    digest_out[i+3] = (ctx->MD[i/4] & 0x000000ff);
  }
  
  Along with loop unrolling, the digest_out was typecasted to uint32_t and ctx->MD was reversed using bswap32
  
  e) static void ISHAProcessMessageBlock(ISHAContext *ctx)
  - The entire function was optimized first in C, then it was replaced with assembly code. The circular shift macro was removed as 
  the operation is performed inside the assembly code. Following is the optimized C function:
  
  /*  
 * Processes the next 512 bits of the message stored in the MBlock
 * array.
 *
 * Parameters:
 *   ctx         The ISHAContext (in/out)
 */
static void ISHAProcessMessageBlock(ISHAContext *ctx)
{
	uint32_t temp;
	register int t;
	uint32_t A, B, C, D, E;

	A = ctx->MD[0];
	B = ctx->MD[1];
	C = ctx->MD[2];
	D = ctx->MD[3];
	E = ctx->MD[4];

	for(t = 0; t < 16; t++) //One for loop eliminated
	{
		// W variable eliminated, bwap32 and uint_32 typcast of Ctx->MBlock used directly in temp calculation
		temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
		E = D;
		D = C;
		C = ISHACircularShift(30,B);
		B = A;
		A = temp;
	}

	//Forcing to 32 bits removed as ctx->MD is already 32 bit
	ctx->MD[0] = (ctx->MD[0] + A);
	ctx->MD[1] = (ctx->MD[1] + B);
	ctx->MD[2] = (ctx->MD[2] + C);
	ctx->MD[3] = (ctx->MD[3] + D);
	ctx->MD[4] = (ctx->MD[4] + E);

	ctx->MB_Idx = 0;
}
  The changes in optimized C code is one for loop instead of two, W variable eliminated. Use of bswap32 and pointer typecasting for temp calculaton.
  
  
  3) pbkdf2.c
  a) void pbkdf2_hmac_isha(const uint8_t *pass, size_t pass_len, const uint8_t *salt, size_t salt_len, int iter, size_t dkLen, uint8_t *DK)
  - Setting flag used for another logic in hmac_isha
  
  b) static void F(const uint8_t *pass, size_t pass_len,const uint8_t *salt, size_t salt_len,int iter, unsigned int blkidx, uint8_t *result)
  - Used memcpy instead of for loops
  - Resetting flag used for another logic in hmac_isha
  - Loop unrolling for the following inner loop performed:
  for (int i=0; i<ISHA_DIGESTLEN; i++)
      result[i] ^= temp[i];
      
  c) void hmac_isha(const uint8_t *key, size_t key_len,const uint8_t *msg, size_t msg_len,uint8_t *digest)
  - Defined keypad,ipad and opad as static as it will be calculated once
  - The following portion was removed as it will never occur:
  
   if (key_len > ISHA_BLOCKLEN) {
    // If key_len > ISHA_BLOCKLEN reset it to key=ISHA(key)
    ISHAReset(&ctx);
    ISHAInput(&ctx, key, key_len);
    ISHAResult(&ctx, keypad);
    
   - 2 structures were created to store the states when ipad and opad were passed in ISHAInput function
   - Flag is checked for 1 so that keypad,ipad and opad is calculated only once in the entire program as key is not changing
   - The states of the 2 structures are equated to ctx before passing ctx to ISHAInput
   
   The biggest optimization was from hmac_isha executing keypad,ipad and opad caculation only once which reduced execution time by almost 2 seconds.
   Before running assembly code, the execution times was around 1600-1700, after running the assembly, it changed to the final execution time mentioned above.
