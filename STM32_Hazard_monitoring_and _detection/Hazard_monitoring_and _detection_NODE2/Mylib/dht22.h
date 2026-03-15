#ifndef DHT22_H
#define DHT22_H
#include "stm32f1xx.h"
#include <string.h>
#include <math.h>
typedef struct
{
	TIM_HandleTypeDef *htim;
	GPIO_TypeDef *GPIOx;
	uint16_t GPIO_Pin;
	uint8_t Buff_rx[5];
	uint8_t buffer_temper[2];
	float humid_value;
	float temperature_value;
}Dht22_Typedef;

uint8_t dht22_handle(Dht22_Typedef *dht22);
void dht22_init(Dht22_Typedef *dht22,TIM_HandleTypeDef *htim,
	GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
#endif