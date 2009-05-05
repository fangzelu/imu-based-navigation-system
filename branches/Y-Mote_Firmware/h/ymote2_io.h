#ifndef		__YMOTE2_IO_H__
#define		__YMOTE2_IO_H__


///////////////////////////////////////////////////////////////////////
///////////// PORT BIT DEFINE /////////////////////////////////////////
#define P1_0			0x01
#define P1_1			0x02
#define P1_2			0x04
#define P1_3			0x08
#define P1_4			0x10
#define P1_5			0x20
#define P1_6			0x40
#define P1_7			0x80

#define P2_0			0x01
#define P2_1			0x02
#define P2_2			0x04
#define P2_3			0x08
#define P2_4			0x10
#define P2_5			0x20
#define P2_6			0x40
#define P2_7			0x80

#define P3_0			0x01
#define P3_1			0x02
#define P3_2			0x04
#define P3_3			0x08
#define P3_4			0x10
#define P3_5			0x20
#define P3_6			0x40
#define P3_7			0x80

#define P4_0			0x01
#define P4_1			0x02
#define P4_2			0x04
#define P4_3			0x08
#define P4_4			0x10
#define P4_5			0x20
#define P4_6			0x40
#define P4_7			0x80


#define P5_0			0x01
#define P5_1			0x02
#define P5_2			0x04
#define P5_3			0x08
#define P5_4			0x10
#define P5_5			0x20
#define P5_6			0x40
#define P5_7			0x80

#define P6_0			0x01
#define P6_1			0x02
#define P6_2			0x04
#define P6_3			0x08
#define P6_4			0x10
#define P6_5			0x20
#define P6_6			0x40
#define P6_7			0x80



#define RF_UCRESET			P1_0
#define BSL_TX				P1_1
#define USR_SW				P1_2
#define RF_TXRX				P1_3
#define LED4				P1_4
#define BTRST				P1_5
#define BTRTS				P1_6
#define BTCTS				P1_7

#define LED5				P2_0
#define DEBUG_EXT_SCI_OE	P2_1
#define	BSL_RX				P2_2
#define BT_GPS_SCI_OE		P2_3
#define COMPASS_LDO_CE		P2_4
#define MSP_ROSC			P2_5//don't know.
#define RF_SPISSN			P2_6
#define RF_UCIRQ			P2_7

#define RF_PONRESET			P3_0
#define I2C_SDA				P3_1
#define BT_LDO_CE			P3_2	
#define I2C_SCL				P3_3
#define UTXD0				P3_4
#define URXD0				P3_5
#define	NC0					P3_6//nc
#define NC1					P3_7//nc

#define GPS_LDO_CE			P4_0
#define GPS_EN				P4_1
#define RF_LDO_CE			P4_2
#define ACCEL_LDO_CE		P4_3
#define ACCEL_GSEL1			P4_4
#define ACCEL_GSEL2			P4_5
#define ACCEL_SEP			P4_6
#define GYRO_LDO_CE			P4_7

#define GYRO_AZ				P5_0
#define RF_SPIRXD			P5_1
#define RF_SPITXD			P5_2
#define RF_SPICLK			P5_3
#define LED1				P5_4
#define LED2				P5_5
#define LED3				P5_6
#define NC2					P5_7//nc



#endif





//EOF
