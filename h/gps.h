#ifndef __GPS_H__
#define __GPS_H__

extern char GPSRowData[256];

extern void InitGPSUart(void);
extern char Uart0GetByte(char *c);
extern int GPSGetData(void);
extern int GPS_isVaild(int len);
extern void GPS_parseGPGGA(int len);
extern void GPS_parseGPRMC(int len);
extern void GPS_parseGPVTG(int len);
extern void GPSTask(void);

#endif




