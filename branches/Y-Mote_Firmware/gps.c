#include <sys/inttypes.h>
#include <io.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gps.h"
#include "common_def.h"
//#include "ymote2_io.h"
#include "util.h"
#include "led.h"
#include "delay.h"
#include "usart.h"
#include "main.h"
#include "timer.h"
#include "i2c.h"

#define RX_BUF_SIZE		2048
#define RX_BUF_MASK		( RX_BUF_SIZE - 1 )

char   	gpsSATUse = 0;
float   gpsHDOP = 0;
char	GPSRowData[256];

static volatile int		RX_HeadPtr;
static volatile int		RX_TailPtr;
static volatile char	RX_Buffer[ RX_BUF_SIZE ];
static char				BufGPSLine[256];
static char				gpsAvailable=0;
//static double			gpsLatitude=0;
//static double			gpsLongitude=0;
//static float			gpsAltitude=0;
//static float			gpsHeading=0;
//static float			gpsSpeed=0;
static int			gpsUTCTime=0;
static char			gpsKORTimeY=0;//year
static char			gpsKORTimeM=0;//month
static char			gpsKORTimeD=0;//day
static char			gpsKORTimeHH=0;//hour
static char			gpsKORTimeMM=0;//minute
static char			gpsKORTimeSS=0;//second


void InitGPSUart(void)
{
	msdelay(1);
	//P3.1,P3.3  I2C pin not select
	//BIT_CLEAR(P3SEL, I2C_SDA + I2C_SCL);
	SET_PORT_IO_I2C_SDA_PIN();
	SET_PORT_IO_I2C_SCL_PIN();

	//UART0 Pin Setting
	//P3SEL |= (1<<4) | (1<<5); //P3.4/UTXD0, P3.5/URXD0 Module Func 
	SET_PORT_MOD_UTX0_PIN();
	SET_PORT_MOD_URX0_PIN();

	//msp430 uart0 init. 57600bps 8n1

	U0CTL = 0;//R-Configuring the USART Module for UART(uart <= i2c or spi)
	
	//msp430 UART0 init. 9600 bps 8n1
	U0CTL = CHAR + SWRST; // 8-bit data, S/W Reset
	ME1 = UTXE0 + URXE0; //UTXE0, URXE0 module enable
	U0TCTL = SSEL_ACLK; //9600 bps / ACLK (32 kHz)
	U0BR1 = 0x00;
	U0BR0 = 0x03;
	U0MCTL = 0x4A;
	U0CTL &= ~SWRST; // Start USART

	BIT_CLEAR(IFG1, UTXIFG0);
	BIT_CLEAR(IFG1, URXIFG0);

	IFG1 &= ~URXIFG0; IE1 |= URXIE0; //UART0RX interrupt ENABLE
	//IFG1 &= ~UTXIFG0;	IE1 |= UTXIE0; //UART0TX interrupt ENABLE
	
	//IFG1 &= ~URXIFG0; IE1 &= ~URXIE0; //UART0RX interrupt DISABLE
	//IFG1 &= ~UTXIFG0; IE1 &= ~UTXIE0; //UART0TX interrupt DISABLE

}


interrupt(USART0RX_VECTOR) Uart0Rx(void)
{
	char c = U0RXBUF;

	int tmp_head;

	if(UartType == SCI_GPS)
	{
		tmp_head = (RX_HeadPtr + 1) & RX_BUF_MASK;

	/* Check for free buffer */
		if( tmp_head == RX_TailPtr )
			return;

		RX_Buffer[ RX_HeadPtr = tmp_head ] = c;
	
		led2_toggle();
	}

	else if(UartType == SCI_BT)
	{
		//led1_toggle();
	}

}

char Uart0GetByte(char *c)
{
	char ReturnValue = TRUE;
	int tmp_tail;
	
	CRITICAL_SECTION_START();

	if(RX_HeadPtr == RX_TailPtr)
		ReturnValue = FALSE;

	if(ReturnValue == TRUE)
	{
		RX_TailPtr = tmp_tail = ( RX_TailPtr + 1 ) & RX_BUF_MASK;
		*c = RX_Buffer[tmp_tail];
	}
	
	CRITICAL_SECTION_END();

	return ReturnValue;
	
}


int GPSGetData(void)
{
	char c;
	unsigned int i=0;
	//int sleepCount=0;

	while(TRUE)
	{
		if(Uart0GetByte(&c))
		{
			if(!c) continue;// c ==> NULL 

			if(!i) // index 0 and first char ==> $
				if(c!='$') continue;

			BufGPSLine[i++] = c;

			if(c=='\n')
			{
				if(i >= sizeof(BufGPSLine))
					i = sizeof(BufGPSLine) - 1;
				//else
				//	;
				
				BufGPSLine[i] = 0;//null terminate

				break;
			}

			if(i>=sizeof(BufGPSLine))
				i=0;

		}
		else
		{
			/*
			if(sleepCount>100)
			{
				sleepCount = 0;
				//led1_off();
			}
			else if(sleepCount>60)
			{
				//led1_on();
			}

			sleepCount++;

			led1_on();
			msdelay(10);//sleep 10ms
			led1_off();
			*/
		}
		
	}
	
	return i;
}

int GPS_isVaild(int len)
{
	int i=1;
	unsigned char  c1;
	unsigned char  c2;
	unsigned char  checksum=0;
	while(i<len-2)
	{
		if(BufGPSLine[i]=='*')
			break;

		checksum^=BufGPSLine[i++];
	}
	
	c1 = BufGPSLine[i+1];
	c2 = BufGPSLine[i+2];

	if(c1 >='A')
		c1 = c1 -'A' + 10;
	else
		c1 = c1 -'0';

	if(c2 >='A')
		c2 = c2 -'A' + 10;
	else
		c2 = c2 -'0';
	c1 = c1<<4 | c2;

	checksum ^= c1;

	return checksum;
}


void GPS_parseGPGGA(int len)
{
	int i=7;//index of right next of first comma;$GPGGA,1231231.12312
	int lat0,lat1, lon0, lon1;//, alt;
	//double lat, lon, alt;
	//double deg, frac;
	
	if(BufGPSLine[i]==',' &&BufGPSLine[i+1]==',')// attempt to reject emplty packets right away
		return;
	// skipping UTC time
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// latitude ddmm.mmmm
	//lat = strtod(&BufGPSLine[i], &endptr);
	lat0 =  ((int)(BufGPSLine[i]- 0x30) * 1000) + \
			((int)(BufGPSLine[i+1] - 0x30) * 100) + \
			((int)(BufGPSLine[i+2] - 0x30) * 10) + \
			((int)(BufGPSLine[i+3] - 0x30)); 
	
	lat1 =  ((int)(BufGPSLine[i+5]- 0x30) * 1000) + \
			((int)(BufGPSLine[i+6] - 0x30) * 100) + \
			((int)(BufGPSLine[i+7] - 0x30) * 10) + \
			((int)(BufGPSLine[i+8] - 0x30)); 
	
	
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// N/S indicator
	if(BufGPSLine[i] == 'S')
		lat0 = -lat0;
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// longitude dddmm.mmmm
	//lon = strtod(&BufGPSLine[i], &endptr);

	lon0 =  ((int)(BufGPSLine[i]- 0x30) * 10000) + \
			((int)(BufGPSLine[i+1]- 0x30) * 1000) + \
			((int)(BufGPSLine[i+2] - 0x30) * 100) + \
			((int)(BufGPSLine[i+3] - 0x30) * 10) + \
			((int)(BufGPSLine[i+4] - 0x30)); 
	
	lon1 =  ((int)(BufGPSLine[i+6]- 0x30) * 1000) + \
			((int)(BufGPSLine[i+7] - 0x30) * 100) + \
			((int)(BufGPSLine[i+8] - 0x30) * 10) + \
			((int)(BufGPSLine[i+9] - 0x30)); 


	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// E/W indicator
	if(BufGPSLine[i] == 'W')
		lon0 = -lon0;

	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// position fix status
	if((BufGPSLine[i] != '0') && (BufGPSLine[i] != ','))
		gpsAvailable = 1;
	else
		gpsAvailable = 0;
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// saterllites in use
	gpsSATUse = atoi(&BufGPSLine[i]);
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// HDOP
	//gpsHDOP = strtod(&BufGPSLine[i], &endptr);
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// altitude
	//alt = strtod(&BufGPSLine[i], &endptr);
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	
	// else... (not interesting)
	//gpsLatitude = lat;
	//gpsLongitude= lon;
	//gpsAltitude = alt;	

	//TxPrintf("lat : %d.%d, lon %d.%d\n", lat0, lat1, lon0, lon1);

}


void GPS_parseGPRMC(int len)
{
	int i=7;//index of right next of first comma;$GPRMC,125920.000,......
	char temp[3];
	char over=0;

	if(BufGPSLine[i]==',' &&BufGPSLine[i+1]==',')// attempt to reject emplty packets right away
		return;
	// UTC time
	//gpsUTCTime = (int)strtod(&BufGPSLine[i], &endptr);
	gpsKORTimeHH = gpsUTCTime/10000;//hour
	gpsKORTimeHH += 9;
	if(gpsKORTimeHH>=24){
		over = 1;
		gpsKORTimeHH -=24;
	}
	gpsKORTimeMM = (gpsUTCTime%10000)/100;//min
	gpsKORTimeSS = (gpsUTCTime%100);//sec
	
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// data vaild 'A' or 'B'
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	//latitude	
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// N/S indicator
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	//longitude	
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// E/W indicator
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// Speed over ground
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// course over ground
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// date
//	temp = atoi(&BufGPSLine[i]);
	temp[2] = 0;
	temp[0] = BufGPSLine[i++];
	temp[1] = BufGPSLine[i++];
	gpsKORTimeD = atoi(temp);
	gpsKORTimeD += over;
	temp[0] = BufGPSLine[i++];
	temp[1] = BufGPSLine[i++];
	gpsKORTimeM = atoi(temp);
	temp[0] = BufGPSLine[i++];
	temp[1] = BufGPSLine[i];
	gpsKORTimeY = atoi(temp);
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// else... (not interesting)
	
	//MSG_LOW("GPRMC : %d %4d,%2d,%2d %2d:%2d:%2d",gpsUTCTime, gpsKORTimeY+2000, gpsKORTimeM, gpsKORTimeD, gpsKORTimeHH, gpsKORTimeMM, gpsKORTimeSS);
}

void GPS_parseGPVTG(int len)
{
	int i=7;//index of right next of first comma;$GPRMC,125920.000,......

	if(BufGPSLine[i]==',' &&BufGPSLine[i+1]==',')// attempt to reject emplty packets right away
		return;
	// heading
	//gpsHeading = strtod(&BufGPSLine[i], &endptr);
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// reference
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}			
	// heading(magnetic)
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// reference
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// Speed(knots)
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	// Speed(km/h)
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	//gpsSpeed = strtod(&BufGPSLine[i], &endptr)*1000.0/3600.0;//m
	while(BufGPSLine[i++] != ','){if(i>=len)	return;}
	//else....(not interesting)
	//MSG_LOW("GPVTG : %lf %lf",gpsHeading, gpsSpeed);
//	STDPrintf("%s\n",BufGPSLine);
}

void GPSTask(void)
{
	int i;
	//int skipCounter=0;
	//int skipFlag=0;


	//_DINT();
	//uart_init();
	//_EINT();
	//SCIDebugExtSelect(SCI_DEBUG);
	//TxPrintf("GPSTask started..\n");

	
	_DINT();
	InitGPSUart();
	_EINT();

	SCIDebugExtSelect(SCI_GPS);

	while(1)
	{
		//led3_toggle();
		i = GPSGetData();

		//TxPrintf("gps : %s\n", BufGPSLine);
		if(GPS_isVaild(i))//return value is 0 if valid.(checksum check)
			continue;

		if(!strncmp("$GPGGA", BufGPSLine, 6))
		{
			GPS_parseGPGGA(i);

			memcpy(GPSRowData, BufGPSLine, 256);
			//_DINT();
			//uart_init();
			//_EINT();
			
			//SCIDebugExtSelect(SCI_DEBUG);
			//TxPrintf("%s", BufGPSLine);
			//TxPrintf("%s", GPSRowData);
			
			//_DINT();
			//InitGPSUart();
			//_EINT();

			//SCIDebugExtSelect(SCI_GPS);
			
			if(BTConsole == OFF)
			{
				_DINT();
				uart_init();
				_EINT();
				SCIDebugExtSelect(SCI_DEBUG);

			}
			else
			{
				_DINT();
				InitBTUart();
				_EINT();
				SCIDebugExtSelect(SCI_BT);

			}

			break;

		}

		//if(!strncmp("$GPGSA", BufGPSLine, 6))
		//	;
		
		//if(!strncmp("$GPGSV", BufGPSLine, 6))
		//	;
		
		//if(!strncmp("$GPRMC", BufGPSLine, 6))
		//	GPS_parseGPRMC(i);
		
	}


}



