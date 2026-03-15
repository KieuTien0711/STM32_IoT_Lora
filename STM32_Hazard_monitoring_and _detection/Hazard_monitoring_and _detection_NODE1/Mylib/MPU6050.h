#ifndef MPU6050_H
#define MPU6050_H
#include "stm32f1xx.h"
#include <math.h>
#include "flash.h"
#define MPU6050_ADDR  	 				0x68 << 1u
#define WHO_AM_I_REG  	 				0x75
#define PWR_MGMT_1    	 				0x6B
#define SMPRT_DIV    	 					0x19
#define ACCEL_XOUT_H  	 				0x3B
#define GYRO_XOUT_H 			 			0x43
#define ACCEL_CONFIG_REG 				0x1C
#define GYRO_CONFIG_REG  				0x1B

/* ================= Flash VAR DECLARATION ================= */
#define FLASH_USER_START_ADDR  0x801FC00 // Dia chi luu tru


typedef struct
{
/* VAR DECLARATION */
	I2C_HandleTypeDef *hi2c;
	float Ax, Ay, Az;					// gia toc 
	float Gx, Gy, Gz;					// goc quay
	float pga_value;			 					// do rung chan
} MPU6050_Typedef;
void calcGyroOffsets(MPU6050_Typedef *mpu6050, uint8_t set_again);
void MPU6050_handle(MPU6050_Typedef *mpu6050);
void MPU6050_init(MPU6050_Typedef *mpu6050, I2C_HandleTypeDef *hi2c);
#endif