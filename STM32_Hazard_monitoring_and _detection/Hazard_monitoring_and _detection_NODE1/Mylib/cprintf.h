#ifndef CPRINTF_H
#define CPRINTF_H
#include "stm32f1xx_hal.h"
#include "MPU6050.h"
#include "MQ2.h"
#include "hostweb.h"
#include "LiquidCrystal_I2C.h"
#include <stdio.h>
#include <string.h>

void send_pga_ppm_to_UART(UART_HandleTypeDef *huart,
				MPU6050_Typedef *mpu6050,MQ2_Typedef *mq2);
void lcd_display_pga_ppm(LiquidCrystal_I2C *hlcd,
			MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2);
void send_threshold_to_UART(UART_HandleTypeDef *huart,
	HostwebTypedef *hostweb_node2);
void lcd_display_threshold_pga_ppm(LiquidCrystal_I2C *hlcd,
			HostwebTypedef *hostweb_node2);
#endif