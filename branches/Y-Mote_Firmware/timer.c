#include <io.h>
#include <signal.h>
#include <sys/inttypes.h>
#include "common_def.h"
#include "ymote2_io.h"

#include "timer.h"
#include "led.h"

#define MAX_TIMER_TICK		1000//1ms
#define SW_DELAY			0x70
#define BT_CONSOLE_DELAY	0x200

uint16_t Tick1S = 0;
uint16_t SWCnt = 0;
uint16_t BTConsole = OFF;



void InitTimer(void)
{
	msdelay(1);
	_DINT();
	// Initialize TimerA
	TACTL = TASSEL_SMCLK + ID_DIV1 + MC_1; // 1MHz
	TACCTL0 = CCIE;
	TACCTL1 = 0;
	TACCTL2 = 0;

	TACCR0 = MAX_TIMER_TICK;

	_EINT();


}


interrupt(TIMERA0_VECTOR) TIMERA0(void)
{
	static uint16_t i = 0;
	static uint16_t Cnt = 0;


	
	if(Cnt++ > 1000)
	{
		Cnt = 0;
		Tick1S++;
		//led1_toggle();
	}
	

	
	if(!(P1IN & USR_SW))
	{
		i++;
		if(i > BT_CONSOLE_DELAY)
		{
			if(BTConsole == OFF)
				BTConsole = ON;
			else
				BTConsole = OFF;
			
			i = 0;
		}
		
	}
	else
	{
		if(i > SW_DELAY)
			SWCnt++;

		i = 0;
	}

}
