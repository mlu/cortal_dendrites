/************************************************************************************* 
*
* Test code for experimenting with NEON FPU on Cortex-A8
*
*  Build with:
*  arm-none-linux-gnueabi-gcc -O2 -g -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -ftree-vectorizer-verbose=5 -Wa,-ahls -Wl,-Map neontest2.map neontest2.c -o neontest2 >neontest2.lst
*
*  Copyright 2009, Magnus Lundin.
*  lundin@mlu.mine.nu
*
*  This code may be freely copied, modified and distributed with no restrictions and no guarantees of any kind. 
*
**************************************************************************************/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/* For intrinsics we need this */
/* http://gcc.gnu.org/onlinedocs/gcc/ARM-NEON-Intrinsics.html */
#include <arm_neon.h>

#define VLENGTH 16

/***************************************************************************************
*
* Learning to restructure data in floating point registers.
*
* Testing some ways to add  pairs of values stored in the same register.
* Yes truly, some comments would be welcome and helpful. 
*
****************************************************************************************/

int main(int argc,char ** argv)
{
	int __attribute__ ((aligned (16)))  a[VLENGTH],b[VLENGTH];
	int __attribute__ ((aligned (16)))  r[VLENGTH];

	int k = 100;
	int i,j;
	
	printf("NEON element shift test\n");
	/* Test data */
	for (i=0; i<VLENGTH; i++)
	{
		a[i] = i+1;
		r[i] = 0;
	}

	/* NEON registers */
	int32x4_t qa, qb;
	int32x2_t qal, qah;
	int32x4x2_t qc2;
	

	qa = vld1q_s32 (a) ;
	qa = vextq_s32 (qa, qa, 1);
	qa = vld1q_lane_s32 ( &k, qa, 3) ;

	/* Store and print the results */
	vst1q_s32(r, qa) ;
	printf("r[] = { %i, %i, %i, %i }\n",r[0],r[1],r[2],r[3]);


	printf("NEON pairvise add test\n");
	/* Test data */
	for (i=0; i<VLENGTH; i++)
	{
		a[i] = (1 << i);
		b[i] = (1 << (16+i));
	}

	qa =  vld1q_s32(a);
	qah = vget_high_s32(qa);
	qal = vget_low_s32(qa);
	qal = vpadd_s32(qal,qah);
	qal = vpadd_s32(qal,qah);

	/* Store and print the results */
	vst1_s32(r, qal);
	printf("r[] = { %x, %x, %x, %x } \n",r[0],r[1],r[2],r[3]);

	printf("NEON unzip test\n");
	qa = vld1q_s32(a);
	qb = vld1q_s32(b);
	qc2 = vuzpq_s32(qa, qb);

	/* Store and print the results */
	vst1q_s32(r, qc2.val[0]);
	printf("r[0][] = { %x, %x, %x, %x }\n",r[0],r[1],r[2],r[3]);
	vst1q_s32(r, qc2.val[1]);
	printf("r[1][] = { %x, %x, %x, %x } \n",r[0],r[1],r[2],r[3]);



	return 0;
}

