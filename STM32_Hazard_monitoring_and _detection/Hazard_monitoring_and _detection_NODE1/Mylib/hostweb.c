/**
 * @file hostweb.c
 * @brief Handles pgaerature and ppmity threshold monitoring with alert mechanisms.
 * @author Kieu Tien
 * @date March 30, 2025
 */

#include "hostweb.h"
Buzzer_State buzzer_state;

void threshold_pga_handle(HostwebTypedef *hostweb_node1,
    MPU6050_Typedef *mpu6050)
{
	if (mpu6050->pga_value >= hostweb_node1->pga_threshold)
	{
		static uint32_t last_time = 0;
		if (HAL_GetTick() - last_time >= 100) // Toggle LED every 100ms
		{
				last_time = HAL_GetTick();
				HAL_GPIO_TogglePin(hostweb_node1->GPIO_Led_pga,
				hostweb_node1->GPIO_Pin_Led_pga);
		}
	}
	else 
	{
		
	}
}


void threshold_ppm_handle(HostwebTypedef *hostweb_node1,
    MQ2_Typedef *mq2)
{
	if (mq2->ppm_value >= hostweb_node1->ppm_threshold)
	{
		static uint32_t last_time = 0;
		if (HAL_GetTick() - last_time >= 100) // Toggle LED every 100ms
		{
				last_time = HAL_GetTick();
				HAL_GPIO_TogglePin(hostweb_node1->GPIO_Led_ppm,
				hostweb_node1->GPIO_Pin_Led_ppm); 
		}
	}
	else 
	{
		
	}
}

static void buzzer_change_state(HostwebTypedef *hostweb_node1,
 MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2)
{
	if ((mpu6050->pga_value >= hostweb_node1->pga_threshold) ||
			(mq2->ppm_value >= hostweb_node1->ppm_threshold))
	{
			buzzer_state = BUZZER_AUTO_STATE; // Activate buzzer automatically
	}
	else 
	{
			buzzer_state = BUZZER_MANUAL_STATE; // Buzzer controlled manually
	}
}

void buzzer_handle(HostwebTypedef *hostweb_node1,
 MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2)
{
  buzzer_change_state(hostweb_node1, mpu6050, mq2);
	switch (buzzer_state)
	{
		case BUZZER_AUTO_STATE:
			// Turn on buzzer automatically when threshold exceeded
			HAL_GPIO_WritePin(hostweb_node1->GPIO_buzzer,
												hostweb_node1->GPIO_Pin_buzzer, GPIO_PIN_SET);
			break;
		
		case BUZZER_MANUAL_STATE:
			// Buzzer controlled manually
			HAL_GPIO_WritePin(hostweb_node1->GPIO_buzzer,
												hostweb_node1->GPIO_Pin_buzzer, 
												hostweb_node1->buzzer_state);
			if(hostweb_node1->buzzer_state==1)
			{
				// Toggle warning LEDs every 100ms
				static uint32_t last_time = 0;
				if (HAL_GetTick() - last_time >= 100)
				{
						last_time = HAL_GetTick();
						HAL_GPIO_TogglePin(hostweb_node1->GPIO_Led_pga,
															 hostweb_node1->GPIO_Pin_Led_pga);
						HAL_GPIO_TogglePin(hostweb_node1->GPIO_Led_ppm,
															 hostweb_node1->GPIO_Pin_Led_ppm);
				}
			}
			else 
			{
				HAL_GPIO_WritePin(hostweb_node1->GPIO_Led_pga,
															 hostweb_node1->GPIO_Pin_Led_pga, 0);
				HAL_GPIO_WritePin(hostweb_node1->GPIO_Led_ppm,
															 hostweb_node1->GPIO_Pin_Led_ppm, 0);
			}
			break;
			default:
			break;
	}
}

void hostweb_handle(HostwebTypedef *hostweb_node1,
 MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2)
{
	// Handle pag threshold events
	threshold_pga_handle(hostweb_node1,mpu6050);
	
	// Handle ppm threshold events
	threshold_ppm_handle(hostweb_node1,mq2);
	
	// Handle buzzer control based on sensor readings
	buzzer_handle(hostweb_node1, mpu6050, mq2);

	if(hostweb_node1->buzzer_state!=0 || hostweb_node1->buzzer_state!=1)
	{
		if(mpu6050->pga_value < hostweb_node1->pga_threshold)
		{
			HAL_GPIO_WritePin(hostweb_node1->GPIO_Led_pga,
			hostweb_node1->GPIO_Pin_Led_pga, 0); // Turn off LED
		}
		if(mq2->ppm_value < hostweb_node1->ppm_threshold)
		{
			HAL_GPIO_WritePin(hostweb_node1->GPIO_Led_ppm,
			hostweb_node1->GPIO_Pin_Led_ppm, 0); // Turn off LED
		}
	}
	
}

/**
 * @brief Initializes the Hostweb configuration.
 * @param hostweb_node1: Pointer to Hostweb structure.
 * @param GPIO_Led_pga: GPIO port for pga warning LED.
 * @param GPIO_Pin_Led_pga: GPIO pin for pga warning LED.
 * @param GPIO_Led_ppm: GPIO port for ppm warning LED.
 * @param GPIO_Pin_Led_ppm: GPIO pin for ppm warning LED.
 * @param GPIO_buzzer: GPIO port for buzzer.
 * @param GPIO_Pin_buzzer: GPIO pin for buzzer.
 */
void hostweb_init(HostwebTypedef *hostweb_node1,
    GPIO_TypeDef *GPIO_Led_pga, uint16_t GPIO_Pin_Led_pga, 
    GPIO_TypeDef *GPIO_Led_ppm, uint16_t GPIO_Pin_Led_ppm,
    GPIO_TypeDef *GPIO_buzzer, uint16_t GPIO_Pin_buzzer)
{
    hostweb_node1->GPIO_Led_pga = GPIO_Led_pga;
    hostweb_node1->GPIO_Pin_Led_pga = GPIO_Pin_Led_pga;
    hostweb_node1->GPIO_Led_ppm = GPIO_Led_ppm;
    hostweb_node1->GPIO_Pin_Led_ppm = GPIO_Pin_Led_ppm;
    hostweb_node1->GPIO_buzzer = GPIO_buzzer;
    hostweb_node1->GPIO_Pin_buzzer = GPIO_Pin_buzzer;
}