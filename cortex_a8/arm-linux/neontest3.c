/************************************************************************************* 
*
* Test code for experimenting with NEON FPU on Cortex-A8
*
*  Build with:
*  arm-none-linux-gnueabi-gcc -O2 -g -mcpu=cortex-a8 -mfpu=neon -ftree-vectorize -mfloat-abi=softfp -ftree-vectorizer-verbose=5 -Wa,-ahls -Wl,-Map neontest3.map neontest3.c -o neontest3 >neontest3.lst
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

#define VLENGTH 100
#define LOOPCOUNT 1000000

/***************************************************************************************
*
* Multiplying a 4x4 matrix with a 4x1 vector.
* Giving ANY extra argument on the command line makes the program use NEON intrinsics
* Otherwise it uses compiler generated floating point, so we can time the different methods
* and check the results.
*
* Yes truly, some comments would be welcome and helpful. 
*
****************************************************************************************/

int main(int argc,char ** argv)
{
	float f[4];
	float32_t __attribute__ ((aligned (16)))  a[VLENGTH][4],b[VLENGTH][4];
	float32_t __attribute__ ((aligned (16)))  m[4][4]={{1,2,3,4},{1,-6,0.2,3},{1,0,1,2},{1,0,0,1}};

	int i,j;
	/* NEON registers */
	float32x4_t qm[4];
	
	printf("NEON [4,4] x [4 1] matrix vector multiplcation\n");
	/* Test data */
	for (i=0; i<VLENGTH; i++)
	{
		a[i][0] = 1;
		a[i][1] = 10;
		a[i][2] = 100;
		a[i][3] = 1000;
	}

	for (i=0; i<4; i++)
	{
		qm[i] = vld1q_f32(m[i]);
	}

	if (argc>1)
	{
		printf("NEON [4,4] x [4 1] matrix vector multiplcation - intrinsics\n");
		/* Unroll 4 times and use two sets of registers - this is 6 times faster than without any unrolling of the loop */
		for(j=0; j<LOOPCOUNT; j++)
		{
			int i;
			for (i=0; i<VLENGTH/4; i+=4)
			{
				/* NEON registers */
				float32x4_t qa, qr;
				float32x2_t qal, qah;
				float32x4_t qa2, qr2;
				float32x2_t qa2l, qa2h;

				qa = vld1q_f32(a[i]);
				qah = vget_high_f32(qa);
				qal = vget_low_f32(qa);

				qa2 = vld1q_f32(a[i+1]);
				qa2h = vget_high_f32(qa2);
				qa2l = vget_low_f32(qa2);
				
				qr = vmulq_lane_f32(qm[0], qal, 0); 		/* qal[0] is ai[0] */
				qr2 = vmulq_lane_f32(qm[0], qa2l, 0);
				qr = vmlaq_lane_f32(qr, qm[2], qah, 0); 	/* qah[0] is ai[2] */
				qr2 = vmlaq_lane_f32(qr2, qm[2], qa2h, 0);
				qr = vmlaq_lane_f32(qr, qm[1], qal, 1); 	/* qal[1] is ai[1] */
				qr2 = vmlaq_lane_f32(qr2, qm[1], qa2l, 1);
				qr = vmlaq_lane_f32(qr, qm[3], qah, 1); 	/* qah[1] is ai[3] */
				qr2 = vmlaq_lane_f32(qr2, qm[3], qa2h, 1);

				vst1q_f32(b[i], qr);
				vst1q_f32(b[i+1], qr2);

				qa = vld1q_f32(a[i+2]);
				qah = vget_high_f32(qa);
				qal = vget_low_f32(qa);

				qa2 = vld1q_f32(a[i+3]);
				qa2h = vget_high_f32(qa2);
				qa2l = vget_low_f32(qa2);
				
				qr = vmulq_lane_f32(qm[0], qal, 0);
				qr2 = vmulq_lane_f32(qm[0], qa2l, 0);
				qr = vmlaq_lane_f32(qr, qm[2], qah, 0);
				qr2 = vmlaq_lane_f32(qr2, qm[2], qa2h, 0);
				qr = vmlaq_lane_f32(qr, qm[1], qal, 1);
				qr2 = vmlaq_lane_f32(qr2, qm[1], qa2l, 1);
				qr = vmlaq_lane_f32(qr, qm[3], qah, 1);
				qr2 = vmlaq_lane_f32(qr2, qm[3], qa2h, 1);

				vst1q_f32(b[i+2], qr);
				vst1q_f32(b[i+3], qr2);
			}
		}
	}
	else
	{
		printf("NEON [4,4] x [4 1] matrix vector multiplcation - compiler optimization\n");
		for(j=0; j<LOOPCOUNT; j++)
		{
			int i;
			for (i=0; i<VLENGTH; i++)
			{				
				float32_t *ai = a[i];
				
				b[i][0] =  ai[0] * m[0][0] + ai[1] * m[1][0] + ai[2] * m[2][0] + ai[3] * m[3][0];
				b[i][1] =  ai[0] * m[0][1] + ai[1] * m[1][1] + ai[2] * m[2][1] + ai[3] * m[3][1];
				b[i][2] =  ai[0] * m[0][2] + ai[1] * m[1][2] + ai[2] * m[2][2] + ai[3] * m[3][2];
				b[i][3] =  ai[0] * m[0][3] + ai[1] * m[1][3] + ai[2] * m[2][3] + ai[3] * m[3][3];
			}
		}
	}

	for (i=0; i<4; i++)
	{
		f[i] = b[0][i];
	}
	/* Store and print the results */
	printf("r[0] = { %f, %f, %f, %f } \n",b[0][0],b[0][1],b[0][2],b[0][3]);

	return 0;
}

