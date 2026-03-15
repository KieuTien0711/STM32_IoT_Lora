#ifndef HOSTWEB_H
#define HOSTWEB_H
#include "stm32f1xx_hal.h"
#include "LiquidCrystal_I2C.h"
#include "dht22.h"
#include "dfplayer.h"
#include <stdio.h>

typedef struct 
{
	GPIO_TypeDef *GPIO_Led_temp;
	uint16_t GPIO_Pin_Led_temp;
	GPIO_TypeDef *GPIO_Led_humid;
	uint16_t GPIO_Pin_Led_humid;
	GPIO_TypeDef *GPIO_buzzer;
	uint16_t GPIO_Pin_buzzer;
	
	float humid_threshold;
	float temperature_threshold;
	uint8_t	buzzer_state;
}HostwebTypedef;

typedef enum
{
	BUZZER_AUTO_STATE,
	BUZZER_MANUAL_STATE
}Buzzer_State;
void hostweb_init(HostwebTypedef *hostweb_node2,
	GPIO_TypeDef *GPIO_Led_temp, uint16_t GPIO_Pin_Led_temp, 
	GPIO_TypeDef *GPIO_Led_humid, uint16_t GPIO_Pin_Led_humid,
	GPIO_TypeDef *GPIO_buzzer, uint16_t GPIO_Pin_buzzer);
void hostweb_handle(HostwebTypedef *hostweb_node2,
	Dht22_Typedef *dht22,  DfplayerTypedef *dfplayer);
#endif