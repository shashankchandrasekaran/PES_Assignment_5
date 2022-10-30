/*
 * ISHAPMB.c
 * Author: Shashank Chandrasekaran
 * Created on: Oct 29, 2022
 * Description: .S file contains the function definition of ISHAProcessMessageBlock
 * which is written in assembly language
 */
.cpu cortex-m0
.syntax unified

.text
.globl ISHAProcessMessageBlock
.type ISHAProcessMessageBlock, %function

@ void ISHAProcessMessageBlock(ISHAContext *ctx):
ISHAProcessMessageBlock:
   push    {r4, r5, r6, r7, lr}
   mov     r6, r9
   mov     r5, r8
   mov     lr, r11
   mov     r7, r10
   @ A = ctx->MD[0];
   movs    r3, r0
   mov     r11, r0
   push    {r5, r6, r7, lr}
   @ D = ctx->MD[3];
   ldr     r2, [r3, #12]
   @E = ctx->MD[4];
   ldr     r3, [r3, #16]
   sub     sp, #36
   @E = ctx->MD[4];
   mov     r12, r3
   movs    r1, #88
   str     r3, [sp, #28]
   @temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
   movs    r3, #27
   mov     r6, r11
   @A = ctx->MD[0];
   ldr     r7, [r0, #0]
   @B = ctx->MD[1];
   ldr     r5, [r0, #4]
   @C = ctx->MD[2];
   ldr     r0, [r0, #8]
   @temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
   mov     r9, r3
   add     r1, r11
   @C = ISHACircularShift(30,B);
   subs    r3, #25
   mov     r8, r3
   mov     r4, r12
   @A = ctx->MD[0];
   str     r7, [sp, #12]
   @B = ctx->MD[1];
   str     r5, [sp, #16]
   @C = ctx->MD[2];
   str     r0, [sp, #20]
   @D = ctx->MD[3];
   str     r2, [sp, #24]
   @for(t = 0; t < 16; t++)
   adds    r6, #24
   str     r1, [sp, #8]
   b.n     JMP_HERE
JMP_HERE_AGAIN:   movs    r2, r0
   @temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
   movs    r7, r3
   @C = ISHACircularShift(30,B);
   mov     r0, r12
   @temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
JMP_HERE:ldmia   r6!, {r3}
   movs    r1, r7
   rev     r3, r3
   mov     r10, r3
   movs    r3, r0
   eors    r3, r2
   ands    r3, r5
   eors    r3, r2
   str     r3, [sp, #4]
   mov     r3, r9
   rors    r1, r3
   mov     r12, r1
   @C = ISHACircularShift(30,B);
   mov     r1, r8
   rors    r5, r1
   ldr     r3, [sp, #4]
   add     r12, r10
   @for(t = 0; t < 16; t++)
   ldr     r1, [sp, #8]
   add     r3, r12
   @temp = ISHACircularShift(5,A) + ((B & C) | ((~B) & D)) + E + __builtin_bswap32(*((uint32_t*)(ctx->MBlock+(t*4))));
   adds    r3, r3, r4
   @C = ISHACircularShift(30,B);
   mov     r12, r5
   @for(t = 0; t < 16; t++)
   movs    r4, r2
   movs    r5, r7
   cmp     r1, r6
   bne.n   JMP_HERE_AGAIN
   @ctx->MD[0] = (ctx->MD[0] + A);
   ldr     r4, [sp, #12]
   mov     r1, r12
   mov     r12, r4
   mov     r4, r11
   add     r3, r12
   str     r3, [r4, #0]
   @ctx->MD[1] = (ctx->MD[1] + B);
   ldr     r3, [sp, #16]
   mov     r12, r3
   @ctx->MD[2] = (ctx->MD[2] + C);
   ldr     r3, [sp, #20]
   @ctx->MD[1] = (ctx->MD[1] + B);
   add     r7, r12
   @ctx->MD[2] = (ctx->MD[2] + C);
   mov     r12, r3
   @ctx->MD[3] = (ctx->MD[2] + D);
   ldr     r3, [sp, #24]
   @ctx->MD[2] = (ctx->MD[2] + C);
   add     r1, r12
   mov     r12, r3
   @ctx->MD[4] = (ctx->MD[4] + E);
   ldr     r3, [sp, #28]
   @ctx->MD[3] = (ctx->MD[3] + D);
   add     r0, r12
   @ctx->MD[4] = (ctx->MD[4] + E);
   mov     r12, r3
   @ctx->MB_Idx = 0;
   movs    r3, #0
   @ctx->MD[4] = (ctx->MD[4] + E);
   add     r2, r12
   @ctx->MD[1] = (ctx->MD[1] + B);
   str     r7, [r4, #4]
   @ctx->MD[2] = (ctx->MD[2] + C);
   str     r1, [r4, #8]
   @ctx->MD[3] = (ctx->MD[3] + D);
   str     r0, [r4, #12]
   @ctx->MD[4] = (ctx->MD[4] + E);
   str     r2, [r4, #16]
   @ctx->MB_Idx = 0;
   str     r3, [r4, #88]
   add     sp, #36
   pop     {r4, r5, r6, r7}
   mov     r11, r7
   mov     r10, r6
   mov     r9, r5
   mov     r8, r4
   pop     {r4, r5, r6, r7, pc}
   nop

