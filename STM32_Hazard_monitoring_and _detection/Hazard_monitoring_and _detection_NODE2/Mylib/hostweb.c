/**
 * @file hostweb.c
 * @brief Handles temperature and humidity threshold monitoring with alert mechanisms.
 * @author Kieu Tien
 * @date March 30, 2025
 */

#include "hostweb.h"

uint8_t temp_warning_played = 0; 	//state of dfplayer for temperature_threshold
uint8_t humid_warning_played = 0;	//state of dfplayer for humid_threshold
Buzzer_State buzzer_state;
/**
 * @brief Handles temperature threshold exceeding event.
 *        - Plays a warning sound using DFPlayer Mini.
 *        - Toggles a warning LED every 100ms.
 * @param hostweb_node2: Pointer to Hostweb configuration.
 * @param dht22: Pointer to the DHT22 sensor structure.
 * @param dfplayer: Pointer to the DFPlayer structure.
 */

void threshold_temp_handle(HostwebTypedef *hostweb_node2,
    Dht22_Typedef *dht22,  DfplayerTypedef *dfplayer)
{
	if (dht22->temperature_value >= hostweb_node2->temperature_threshold)
	{
		static uint32_t last_time = 0;
		if (HAL_GetTick() - last_time >= 100) // Toggle LED every 100ms
		{
				last_time = HAL_GetTick();
				HAL_GPIO_TogglePin(hostweb_node2->GPIO_Led_temp,
				hostweb_node2->GPIO_Pin_Led_temp);
		}
		if (!temp_warning_played) 
		{
			dfplayer_playfile(dfplayer, 3); // Play temperature threshold warning
			temp_warning_played = 1;
//			HAL_Delay(2000);
		}
	}
	else 
	{
		temp_warning_played = 0;
	}
}

/**
 * @brief Handles humidity threshold exceeding event.
 *        - Plays a warning sound using DFPlayer Mini.
 *        - Toggles a warning LED every 100ms.
 * @param hostweb_node2: Pointer to Hostweb configuration.
 * @param dht22: Pointer to the DHT22 sensor structure.
 * @param dfplayer: Pointer to the DFPlayer structure.
 */
void threshold_humid_handle(HostwebTypedef *hostweb_node2,
    Dht22_Typedef *dht22,  DfplayerTypedef *dfplayer)
{
	if (dht22->humid_value >= hostweb_node2->humid_threshold)
	{
		static uint32_t last_time = 0;
		if (HAL_GetTick() - last_time >= 100) // Toggle LED every 100ms
		{
				last_time = HAL_GetTick();
				HAL_GPIO_TogglePin(hostweb_node2->GPIO_Led_humid,
				hostweb_node2->GPIO_Pin_Led_humid); 
		}
		if (!humid_warning_played)
		{
			dfplayer_playfile(dfplayer, 4); // Play humidity threshold warning
			humid_warning_played = 1;
//			HAL_Delay(2000);
		}
	}
	else 
	{
		humid_warning_played = 0;
	}
}

/**
 * @brief Handles both temperature and humidity threshold events.
 * @param hostweb_node2: Pointer to Hostweb configuration.
 * @param dht22: Pointer to the DHT22 sensor structure.
 * @param dfplayer: Pointer to the DFPlayer structure.
 */

// ========================= BUZZER STATE CHANGE =============================
// Function: buzzer_change_state
// Description: Determines the buzzer state based on temperature and humidity
//              thresholds.
// Parameters:
//   - hostweb_node2: Pointer to the Hostweb configuration structure
//   - dht22: Pointer to the DHT22 sensor structure
// ============================================================================
static void buzzer_change_state(HostwebTypedef *hostweb_node2,
                                Dht22_Typedef *dht22)
{
	if ((dht22->temperature_value >= hostweb_node2->temperature_threshold)
	|| (dht22->humid_value >= hostweb_node2->humid_threshold))
	{
			buzzer_state = BUZZER_AUTO_STATE; // Activate buzzer automatically
	}
	else 
	{
			buzzer_state = BUZZER_MANUAL_STATE; // Buzzer controlled manually
	}
}

// ========================= BUZZER CONTROL HANDLER ==========================
// Function: buzzer_handle
// Description: Handles the buzzer operation based on its state. It also
//              toggles warning LEDs every 100ms when in manual mode.
// Parameters:
//   - hostweb_node2: Pointer to the Hostweb configuration structure
//   - dht22: Pointer to the DHT22 sensor structure
// ============================================================================
void buzzer_handle(HostwebTypedef *hostweb_node2,
                   Dht22_Typedef *dht22)
{
  buzzer_change_state(hostweb_node2, dht22);
	switch (buzzer_state)
	{
		case BUZZER_AUTO_STATE:
			// Turn on buzzer automatically when threshold exceeded
			HAL_GPIO_WritePin(hostweb_node2->GPIO_buzzer,
												hostweb_node2->GPIO_Pin_buzzer, 1);
		break;
		
		case BUZZER_MANUAL_STATE:
			// Buzzer controlled manually
			HAL_GPIO_WritePin(hostweb_node2->GPIO_buzzer,
												hostweb_node2->GPIO_Pin_buzzer, 
												hostweb_node2->buzzer_state);
			if(hostweb_node2->buzzer_state!=0)
			{
				// Toggle warning LEDs every 100ms
				static uint32_t last_time = 0;
				if (HAL_GetTick() - last_time >= 100)
				{
						last_time = HAL_GetTick();
						HAL_GPIO_TogglePin(hostweb_node2->GPIO_Led_temp,
															 hostweb_node2->GPIO_Pin_Led_temp);
						HAL_GPIO_TogglePin(hostweb_node2->GPIO_Led_humid,
															 hostweb_node2->GPIO_Pin_Led_humid);
				}
			}
			else 
			{
				HAL_GPIO_WritePin(hostweb_node2->GPIO_Led_temp,
															 hostweb_node2->GPIO_Pin_Led_temp , 0);
				HAL_GPIO_WritePin(hostweb_node2->GPIO_Led_humid,
															 hostweb_node2->GPIO_Pin_Led_humid, 0);
			}
			break;
		default:
			break;
	}
}
// ======================== HOSTWEB EVENT HANDLER ============================
// Function: hostweb_handle
// Description: Handles temperature, humidity, and buzzer control events.
// Parameters:
//   - hostweb_node2: Pointer to the Hostweb configuration structure.
//   - dht22: Pointer to the DHT22 sensor structure.
//   - dfplayer: Pointer to the DFPlayer structure (for audio alerts).
// ============================================================================
void hostweb_handle(HostwebTypedef *hostweb_node2,
  Dht22_Typedef *dht22, DfplayerTypedef *dfplayer)
{
	// Handle temperature threshold events
	threshold_temp_handle(hostweb_node2, dht22, dfplayer);
	
	// Handle humidity threshold events
	threshold_humid_handle(hostweb_node2, dht22, dfplayer);
	
	// Handle buzzer control based on sensor readings
	buzzer_handle(hostweb_node2, dht22);
	
		if((dht22->temperature_value < hostweb_node2->temperature_threshold) && (hostweb_node2->buzzer_state!=0||hostweb_node2->buzzer_state!=1))
	{
		HAL_GPIO_WritePin(hostweb_node2->GPIO_Led_temp,
		hostweb_node2->GPIO_Pin_Led_temp, 0); // Turn off LED
	}
	if((dht22->humid_value < hostweb_node2->humid_threshold) &&  (hostweb_node2->buzzer_state!=0||hostweb_node2->buzzer_state!=1))
	{
		HAL_GPIO_WritePin(hostweb_node2->GPIO_Led_humid,
		hostweb_node2->GPIO_Pin_Led_humid, 0); // Turn off LED
	}
}

/**
 * @brief Initializes the Hostweb configuration.
 * @param hostweb_node2: Pointer to Hostweb structure.
 * @param GPIO_Led_temp: GPIO port for temperature warning LED.
 * @param GPIO_Pin_Led_temp: GPIO pin for temperature warning LED.
 * @param GPIO_Led_humid: GPIO port for humidity warning LED.
 * @param GPIO_Pin_Led_humid: GPIO pin for humidity warning LED.
 * @param GPIO_buzzer: GPIO port for buzzer.
 * @param GPIO_Pin_buzzer: GPIO pin for buzzer.
 */
void hostweb_init(HostwebTypedef *hostweb_node2,
    GPIO_TypeDef *GPIO_Led_temp, uint16_t GPIO_Pin_Led_temp, 
    GPIO_TypeDef *GPIO_Led_humid, uint16_t GPIO_Pin_Led_humid,
    GPIO_TypeDef *GPIO_buzzer, uint16_t GPIO_Pin_buzzer)
{
    hostweb_node2->GPIO_Led_temp = GPIO_Led_temp;
    hostweb_node2->GPIO_Pin_Led_temp = GPIO_Pin_Led_temp;
    hostweb_node2->GPIO_Led_humid = GPIO_Led_humid;
    hostweb_node2->GPIO_Pin_Led_humid = GPIO_Pin_Led_humid;
    hostweb_node2->GPIO_buzzer = GPIO_buzzer;
    hostweb_node2->GPIO_Pin_buzzer = GPIO_Pin_buzzer;
}