#ifndef __IMU_H__
#define __IMU_H__

typedef struct _imu_packet
{
	char start_char;
	uint16_t sensor_val[6];
	char end_char;
}imu_packet;

#endif
