/************************************************************************************* 
*
* Test code for experimenting with NEON FPU on Cortex-A8
*
*  Build with:
*  arm-none-linux-gnueabi-gcc -O2 -g -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -ftree-vectorizer-verbose=5 -Wa,-ahls -Wl,-Map neontest.map neontest.c -o neontest >neontest.lst
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
* Learning to use intrinsics, load and stor to floating point registers.
*
* Testing some ways to do vectorized multiplications.
* Yes truly, some comments would be welcome and helpful. 
*
****************************************************************************************/

int vectormult(int *a,int *b, int*r, int count)
{
	int i;
	int32x4_t aq, bq, rq;
	
	for (i=0; i<count; i+=4)
	{
		aq = vld1q_s32(&a[i]);
		bq = vld1q_s32(&b[i]);
		rq = vmulq_s32(aq, bq);
		vst1q_s32 (&r[i], rq);  		
	}
}

int vectormult2(int *a,int *b, int*r, int count)
{
	int i;
	int32x4_t aq, bq, rq;
	
	for (i=0; i<count; i+=4)
	{
		aq = vld1q_s32(&a[i]);
		bq = vld1q_s32(&b[i]);
		rq = vqdmulhq_s32(aq, bq);
		vst1q_s32 (&r[i], rq);  		
	}
}

int vectormult3(int *a,int *b, int*r, int count)
{
	int i;
	int32x4_t aq, bq, rq;
	
	for (i=0; i<count; i+=4)
	{
		aq = vld1q_s32(&a[i]);
		bq = vld1q_s32(&b[i]);
		rq = vqrdmulhq_s32(aq, bq);
		vst1q_s32 (&r[i], rq);  		
	}
}

int main(int argc,char ** argv)
{
	int __attribute__ ((aligned (16)))  a[VLENGTH],b[4][VLENGTH];
	int __attribute__ ((aligned (16)))  c[VLENGTH];
	int __attribute__ ((aligned (16)))  ml[VLENGTH];
	int i,j;
	
	for (i=0; i<VLENGTH; i++)
	{
		a[i] = (1<< (2*i));
		for (j=0;j<4;j++)
			b[j][i] = (1<< (2*i))-j;
		c[i] = 0;
	}

	for (i=0; i<VLENGTH; i++)
	{
		int temp = 0;
		for (j=0;j<4;j++)
			temp += b[j][i];
		c[i] = c[i] + a[i] * temp;
	}

	for (i=0; i<VLENGTH; i++)
	{
		printf("%i %i = % i\n",a[i],b[0][i],c[i]);
	}

	printf("vmulq_s32\n");
	vectormult(a, b[1], c, VLENGTH);
	
	for (i=0; i<VLENGTH; i++)
	{
		printf("%i %i = % i\n",a[i],b[1][i],c[i]);
	}

	printf("vmulq_s32\n");
	vectormult2(a, b[1], c, VLENGTH);
	
	for (i=0; i<VLENGTH; i++)
	{
		printf("%i %i = % i\n",a[i],b[1][i],c[i]);
	}

	printf("vmulq_s32\n");
	vectormult3(a, b[1], c, VLENGTH);
	
	for (i=0; i<VLENGTH; i++)
	{
		printf("%i %i = % i\n",a[i],b[1][i],c[i]);
	}

	return 0;
}

