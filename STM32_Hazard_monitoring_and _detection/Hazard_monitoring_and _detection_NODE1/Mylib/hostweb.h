#ifndef HOSTWEB_H
#define HOSTWEB_H
#include "stm32f1xx_hal.h"
#include "MPU6050.h"
#include "MQ2.h"

typedef struct 
{
	GPIO_TypeDef *GPIO_Led_ppm;
	uint16_t GPIO_Pin_Led_ppm;
	GPIO_TypeDef *GPIO_Led_pga;
	uint16_t GPIO_Pin_Led_pga;
	GPIO_TypeDef *GPIO_buzzer;
	uint16_t GPIO_Pin_buzzer;
	
	float pga_threshold;
	float ppm_threshold;
	uint8_t	buzzer_state;
}HostwebTypedef;

typedef enum
{
	BUZZER_AUTO_STATE,
	BUZZER_MANUAL_STATE
}Buzzer_State;

void hostweb_init(HostwebTypedef *hostweb_node1,
	GPIO_TypeDef *GPIO_Led_ppm, uint16_t GPIO_Pin_Led_ppm, 
	GPIO_TypeDef *GPIO_Led_pga, uint16_t GPIO_Pin_Led_pga,
	GPIO_TypeDef *GPIO_buzzer, uint16_t GPIO_Pin_buzzer);
void hostweb_handle(HostwebTypedef *hostweb_node1,
 MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2);
#endif