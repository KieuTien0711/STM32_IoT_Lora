#ifndef LORA_SEND_VALUE
#define LORA_SEND_VALUE
#include "stm32f1xx_hal.h"
#include "hostweb.h"
#include <stdio.h>


void lora_send_sensor_value_esp32(UART_HandleTypeDef *huart,
	MQ2_Typedef *mq2, MPU6050_Typedef *mpu6050);
#endif