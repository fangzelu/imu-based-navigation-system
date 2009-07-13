#include <io.h>
#include <signal.h>
#include <stdio.h>
#include "main.h"
#include "common_def.h"
#include "ymote2_io.h"
#include "dco.h"
#include "led.h"
#include "delay.h"
#include "util.h"
#include "usart.h"
#include "types.h"
#include "i2c.h"
#include "imu.h"
#include "bm.h"
#include "ina219.h"
#include "adc.h"
#include "compass.h"
#include "gps.h"
#include "timer.h"
#include "hwclock.h"
#include "nnspi.h"
#include "ntrxdil.h"
#include "ntrxiqpar.h"
#include "ntrxranging.h"
#include "ntrxtypes.h"

#define		RF_ENABLE		OFF
#define		BT_ENABLE		ON

#define		GYRO_ENABLE		ON
#define		ACCEL_ENABLE	ON
#define		COMPASS_ENABLE	ON
#define		GPS_ENABLE		OFF

//nanoloc//////
int	 is_pending = 0;
extern uint32_t RF_seq;
extern int16_t RF_frac, RF_number;
/////////////////////////////////////////
char TestFlag = 0;
uint8_t UartType = 0;

int	main()
{
	int32_t BMCurrent = 0;
	int16_t BMVolt = 0;

	uint16_t MSPCurrent = 0;
	uint16_t RadioCurrent = 0;
	uint16_t SensorCurrent = 0;

	uint16_t RadioVolt = 0;
	uint16_t MSPVolt = 0;
	uint16_t SensorVolt = 0;

	int32_t MSPShuntVolt = 0;
	int32_t RadioShuntVolt = 0;
	int32_t SensorShuntVolt = 0;

	uint16_t CompassX = 0;
	uint16_t CompassY = 0;
	uint16_t CompassZ = 0;

	uint8_t  DebugStartF = OFF;

	uint8_t i;

	uint16_t bt_status = BTConsole;

	imu_packet imu;
	imu.start_char = '*';
	imu.end_char = '%';

	_DINT();
	
	dev_leds_init();	

	msp430_timer_init();

	adc_sensor_init();

	InitMSPGPIOFuction();

	InitCurrentMonitor();

	InitComponentPower();

	InitTimer();

	led1_on();
#if COMPASS_ENABLE == ON
	InitCompass();
#endif
	
	_DINT();
	uart_init();
	_EINT();
	
	SCIDebugExtSelect(SCI_DEBUG);
	TxPrintf("\nYmote2.0 Test!!\n");


#if BT_ENABLE == ON

	HBM2X1M_init();
	led1_on();
	msdelay(1500);
	led1_off();
	SCIDebugExtSelect(SCI_BT);
	bt_send_cmd("AT+BTSCAN\r");
	msdelay(500);
#endif//BT_ENABLE


#if RF_ENABLE == ON
	//Initialize the ntrx driver
	NtrxInit();

	//Start the ntrx driver
	#if CONFIG_REG_MAP_NR != 501
	NTRXStart();
	#endif

	//initialize user application
	InitApplication();

#endif//RF_ENABLE

	_EINT();
	
	TestFlag = 0;	


	while(TRUE)
	{

		//led1_toggle();
		//led2_toggle();
		//led3_toggle();
		//led4_toggle();
		//led5_toggle();
		//led1_toggle();
	//	msdelay(50);

		
		_DINT();
		InitI2CModule();
		_EINT();
//======= Current Measure====================
/*
		BMCurrent = ReadDS2782CURRENTReg();
		BMVolt = ReadDS2782VOLTReg();

		MSPShuntVolt = ReadINA219ShuntVoltReg(I2C_INA219_MSP_ADD);
		MSPCurrent = ReadINA219CurrentReg(I2C_INA219_MSP_ADD);
		MSPVolt = ReadINA219BusVoltReg(I2C_INA219_MSP_ADD);

		SensorShuntVolt = ReadINA219ShuntVoltReg(I2C_INA219_SENSOR_ADD);
		SensorCurrent = ReadINA219CurrentReg(I2C_INA219_SENSOR_ADD);
		SensorVolt = ReadINA219BusVoltReg(I2C_INA219_SENSOR_ADD);
		
		RadioShuntVolt = ReadINA219ShuntVoltReg(I2C_INA219_RADIO_ADD);
		RadioCurrent = ReadINA219CurrentReg(I2C_INA219_RADIO_ADD);
		RadioVolt = ReadINA219BusVoltReg(I2C_INA219_RADIO_ADD);
*/
//===========================================

		if(BTConsole == OFF)
		{
			_DINT();
			uart_init();
			_EINT();
			SCIDebugExtSelect(SCI_DEBUG);
			led3_off();				
		}
		else
		{
			_DINT();
			InitBTUart();
			_EINT();
			SCIDebugExtSelect(SCI_BT);
			led3_on();
		}

		TxBinary(&(imu.start_char), (uint8_t)sizeof(char));

#if ACCEL_ENABLE == ON
		adc_get(ACC_X_CHANNEL);
		TxBinary(&(ADCResult[ACC_X_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		adc_get(ACC_Y_CHANNEL);
		TxBinary(&(ADCResult[ACC_Y_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		adc_get(ACC_Z_CHANNEL);
		TxBinary(&(ADCResult[ACC_Z_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		led1_toggle();
#endif

#if GYRO_ENABLE == ON
		adc_get(GYRO1_X_CHANNEL);
		TxBinary(&(ADCResult[GYRO1_X_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		adc_get(GYRO1_Y_CHANNEL);
		TxBinary(&(ADCResult[GYRO1_Y_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		//adc_get(GYRO2_X_CHANNEL);
		//WAIT_ADC();
		adc_get(GYRO2_Y_CHANNEL);
		TxBinary(&(ADCResult[GYRO2_Y_CHANNEL]), (uint8_t)sizeof(uint16_t));
		//WAIT_ADC();
		led2_toggle();
#endif


#if COMPASS_ENABLE == ON
		CompassX = ReadCompassXAxis();
		TxBinary(&CompassX, (uint8_t)sizeof(uint16_t));
		CompassY = ReadCompassYAxis();
		TxBinary(&CompassY, (uint8_t)sizeof(uint16_t));
		CompassZ = ReadCompassZAxis();
		TxBinary(&CompassZ, (uint8_t)sizeof(uint16_t));

		led4_toggle();
#endif

		TxBinary(&(imu.end_char), (uint8_t)sizeof(char));


#if RF_ENABLE == ON
		//call application periodically
		PollApplication();

		#ifdef USE_UCIRQ //USE Interrupt 

		CRITICAL_SECTION_START();
		if (is_pending)
		{
			NTRXUpdate();
			is_pending = 0;
			//CRITICAL_SECTION_END_NO_BRACE();
		}
		CRITICAL_SECTION_END();

		#else //Polling.

		 //Check for incomming packets 
		NTRXUpdate();
		#endif//USE_UCIRQ

#endif//RF_ENABLE		



		//imu.sensor_val[0] = ADCResult[ACC_X_CHANNEL];
		//imu.sensor_val[1] = ADCResult[ACC_Y_CHANNEL];
		//imu.sensor_val[2] = ADCResult[ACC_Z_CHANNEL];
		//imu.sensor_val[3] = ADCResult[GYRO1_X_CHANNEL];
		//imu.sensor_val[4] = ADCResult[GYRO1_Y_CHANNEL];
		//imu.sensor_val[5] = ADCResult[GYRO2_Y_CHANNEL];
		//imu.sensor_val[0] = 0;
		//imu.sensor_val[1] = 0;
		//imu.sensor_val[2] = 0;
		//imu.sensor_val[3] = 0;
		//imu.sensor_val[4] = 0;
		//imu.sensor_val[5] = 0;
		//TxPrintf("S,%04d,%04d,%04d,%04d,%04d,%04d,E,"
		//		, imu.sensor_val[0], imu.sensor_val[1], imu.sensor_val[2]
		//		, imu.sensor_val[3], imu.sensor_val[4], imu.sensor_val[5]);
		//TxBinary(&imu, (uint8_t)sizeof(imu_packet));

		/*
		switch(SWCnt)
		{
			case 0:
				TxPrintf("BM C: %06ld V: %04d MSP SV:%06ld C:%06d V:%04d Radio SV:%06ld C:%06d V:%04d Sensor SV:%06ld C:%06d V:%04d\n"
						, BMCurrent, BMVolt, MSPShuntVolt, MSPCurrent, MSPVolt, RadioShuntVolt, RadioCurrent, RadioVolt, SensorShuntVolt, SensorCurrent, SensorVolt);
				break;
			case 1:
				TxPrintf("ACC X : %04d, Y : %04d, Z:%04d\n"
						, ADCResult[ACC_X_CHANNEL], ADCResult[ACC_Y_CHANNEL], ADCResult[ACC_Z_CHANNEL]);
				break;
			case 2:
				TxPrintf("GYRO x1 : %04d, y1 : %04d, x2 : %04d, y2 : %04d\n"
						, ADCResult[GYRO1_X_CHANNEL], ADCResult[GYRO1_Y_CHANNEL], ADCResult[GYRO2_X_CHANNEL], ADCResult[GYRO2_Y_CHANNEL]);
				break;
			case 3:
				TxPrintf("COMPASS x:%04d, y:%04d, z:%04d \n", CompassX, CompassY, CompassZ);
				break;
			case 4:
				if(RF_number < 0)
					TxPrintf("Radio Dist Drop\n");
				else
					TxPrintf("Radio Dis %lu : %3u.%02u\n", RF_seq, RF_number, RF_frac);
				break;
			case 5:

			#if GPS_ENABLE == ON
				GPSTask();
				TxPrintf("GPS : %s\n",GPSRowData);
			#endif
				break;
			default :
				SWCnt = 0;
				break;
		}
		*/


		/*if(Tick1S >= 25 && DebugStartF == OFF)
		{
			TxPrintf("DebugStart\n");
			DebugStartF = ON;
		}

		if(Tick1S >= 625)
		{
			TxPrintf("DebugEnd\n");
			break;
		}*/

	}

	return 0;
}

void InitComponentPower(void)
{
/////////////////////////////////////////////////////
// component power set

	BT_POWER_OFF;
	GYRO_POWER_OFF;
	ACCEL_POWER_OFF;
	RF_POWER_OFF;
	COMPASS_POWER_OFF;
	GPS_POWER_OFF;

#if	RF_ENABLE == ON
	RF_POWER_ON;
#else
	RF_POWER_OFF;
#endif

#if BT_ENABLE == ON
	BT_POWER_ON;
#else
	BT_POWER_OFF;
#endif

#if GYRO_ENABLE == ON
	GYRO_POWER_ON;
#else
	GYRO_POWER_OFF;
#endif

#if ACCEL_ENABLE == ON
	ACCEL_POWER_ON;
	ACCEL_G_SEL15;
	ACCEL_ACTIVE;
#else
	ACCEL_POWER_OFF;
#endif

#if COMPASS_ENABLE == ON
	COMPASS_POWER_ON;
#else
	COMPASS_POWER_OFF;
#endif

#if GPS_ENABLE == ON
	GPS_POWER_ON;
	GPS_ACTIVE;
#else
	GPS_POWER_OFF;
#endif


}

void InitCurrentMonitor(void)
{
	_DINT();
	InitI2CModule();
	_EINT();


	//msp current monitor setup
	INA219ConfigreReg.bit.MODE = 0x07;
	INA219ConfigreReg.bit.SADC = 0x03;
	INA219ConfigreReg.bit.BADC = 0x03;
	INA219ConfigreReg.bit.PG = 0x00;//+-40mV // shunt resistor voltage scale select
	INA219ConfigreReg.bit.BRANG = 0;
	INA219ConfigreReg.bit.RST = 0;
	WriteINA219ConfigReg(I2C_INA219_MSP_ADD, INA219ConfigreReg.all);
	WriteINA219CalibrationReg(I2C_INA219_MSP_ADD, INA219_CAL(1.0));

	//nanoloc current monitor setup
	INA219ConfigreReg.bit.MODE = 0x07;
	INA219ConfigreReg.bit.SADC = 0x03;
	INA219ConfigreReg.bit.BADC = 0x03;
	INA219ConfigreReg.bit.PG = 0x01;//+- 40mV shunt resistor voltage scale select
	INA219ConfigreReg.bit.BRANG = 0;
	INA219ConfigreReg.bit.RST = 0;
	WriteINA219ConfigReg(I2C_INA219_RADIO_ADD, INA219ConfigreReg.all);
	WriteINA219CalibrationReg(I2C_INA219_RADIO_ADD, INA219_CAL(10.0));

	//sensor module current monitor setup
	INA219ConfigreReg.bit.MODE = 0x07;
	INA219ConfigreReg.bit.SADC = 0x03;
	INA219ConfigreReg.bit.BADC = 0x03;
	INA219ConfigreReg.bit.PG = 0x02;//+- 120mV // shunt resistor voltage scale select
	INA219ConfigreReg.bit.BRANG = 0;
	INA219ConfigreReg.bit.RST = 0;
	WriteINA219ConfigReg(I2C_INA219_SENSOR_ADD, INA219ConfigreReg.all);
	WriteINA219CalibrationReg(I2C_INA219_SENSOR_ADD, INA219_CAL(10.0));


}

void InitCompass(void)
{
	uint8_t CompassCREGA = 0;
	uint8_t CompassCREGB = 0;
	uint8_t CompassMODEREG = 0;
	uint8_t CompassSR = 0;
	uint8_t CompassIRA = 0;
	uint8_t CompassIRB = 0;
	uint8_t CompassIRC = 0;


	_DINT();
	InitI2CModule();
	_EINT();

	WriteCompassCREGA(0x18);
	WriteCompassCREGB(0x80);
	WriteCompassMODEREG(0x00);
	
	
	CompassSR = ReadCompassStatusReg();
	CompassIRA = ReadCompassIREGA();
	CompassIRB = ReadCompassIREGB();
	CompassIRC = ReadCompassIREGC();
	CompassCREGA = ReadCompassCREGA();
	CompassCREGB = ReadCompassCREGB();
	CompassMODEREG = ReadCompassMODEREG();

	_DINT();
	uart_init();
	_EINT();


	SCIDebugExtSelect(SCI_DEBUG);
	TxPrintf("Compass S Reg : %x IREGA : %x IREGB : %x IREGC : %x CREGA : %x CREGB : %x MODEREG :%x\n"
			, CompassSR, CompassIRA, CompassIRB, CompassIRC, CompassCREGA, CompassCREGB, CompassMODEREG);

	msdelay(1);
}

void InitMSPGPIOFuction(void)
{
	////////////////////////////////////////////////////
	// user switch
	BIT_CLEAR(P1DIR, USR_SW);
	BIT_CLEAR(P1SEL, USR_SW);
	/////////////////////////////////////////////////////	
	
}

void SCIDebugExtSelect(uint8_t Type)
{
	UartType = Type;

	BIT_SET(P2DIR, DEBUG_EXT_SCI_OE | BT_GPS_SCI_OE);
	BIT_CLEAR(P2SEL, DEBUG_EXT_SCI_OE | BT_GPS_SCI_OE);

	
	if(Type == SCI_DEBUG)
		BIT_SET(P2OUT, DEBUG_EXT_SCI_OE);

	else if(Type == SCI_EXT)
		BIT_CLEAR(P2OUT, DEBUG_EXT_SCI_OE);
	
	else if(Type == SCI_GPS)
	{
		BIT_CLEAR(P2OUT, DEBUG_EXT_SCI_OE);
		BIT_SET(P2OUT, BT_GPS_SCI_OE);
	}
	else if(Type == SCI_BT)
	{
		BIT_CLEAR(P2OUT, DEBUG_EXT_SCI_OE);
		BIT_CLEAR(P2OUT, BT_GPS_SCI_OE);
	}
	//else //error
	//	;	
}


//EOF
