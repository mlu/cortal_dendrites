#define PHASE 100


int LEDstate(unsigned int brightness, int cnt)
{
	if(brightness & 0x100)
		return (brightness>=cnt)?1:0;
	else
		return (brightness>=cnt)?0:1;
}

int main ()
{
	unsigned int i;
	int cnt;
	
	unsigned int LEDbrightness;
	
	while(1)
	{
		//most of this code is just to do software PWM on the LEDs to make them brighten and dim
		LEDbrightness++;
		for(cnt=0;cnt<512;cnt++)
		{
			i=LEDstate((LEDbrightness/2 + 0 * PHASE) & 0x1ff, cnt);
			i|=LEDstate((LEDbrightness + 1 * PHASE) & 0x1ff, cnt) << 1;
			i|=LEDstate((LEDbrightness + 2 * PHASE) & 0x1ff, cnt) << 2;
			if (i & 1)
			{
				*(volatile unsigned int *)0x49056090 = 0x00200000;		
			}
			else
			{
				*(volatile unsigned int *)0x49056094 = 0x00200000;						
			}
			if ( LEDstate((LEDbrightness/4 + 2 * PHASE) & 0x1ff, cnt))
			{
				*(volatile unsigned int *)0x49056090 = 0x00400000;		
			}
			else
			{
				*(volatile unsigned int *)0x49056094 = 0x00400000;						
			}
		}	
		
	}


	return 0;
}

