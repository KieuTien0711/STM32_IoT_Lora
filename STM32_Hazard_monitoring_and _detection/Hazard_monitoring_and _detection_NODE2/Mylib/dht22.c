/* ========================== DHT22 SENSOR LIBRARY ============================
 *  File: dht22.c
 *  Description: Functions to interface with the DHT22 temperature & humidity sensor
 *  Author: Kieu Tien
 *  Date: 	30/03/2025
 * ========================================================================== */


#include "dht22.h"

uint16_t u16Tim; // Variable to store pulse width timing
#define FILTER_WINDOW 5  // Number of samples for moving average filter

float temp_values[FILTER_WINDOW] = {0};
float humid_values[FILTER_WINDOW] = {0};
uint8_t filter_index = 0;

// ============================== DELAY FUNCTION ==============================
// Function: delay_ms
// Description: Creates an accurate delay using a hardware timer
// Parameters:
//   - dht22: Pointer to the DHT22 structure
//   - ms: Delay time in milliseconds
// ============================================================================
void delay_ms(Dht22_Typedef *dht22, uint16_t ms)
{
	while (ms--)
	{
		__HAL_TIM_SET_COUNTER(dht22->htim, 0);
		HAL_TIM_Base_Start(dht22->htim);
		while (__HAL_TIM_GET_COUNTER(dht22->htim) < 1000);
		HAL_TIM_Base_Stop(dht22->htim);
	}
}

// ========================== APPLY MOVING AVERAGE FILTER ============================
// Function: filter_sensor_value
// Description: Applies a moving average filter to smooth sensor readings.
// Parameters:
//   - values: Pointer to the array storing past sensor values.
//   - new_value: The latest sensor reading.
// Returns:
//   - Filtered sensor value.
// ==================================================================================
static float filter_sensor_value(float *values, float new_value) 
{
	values[filter_index] = new_value;
	float sum = 0;
	for (uint8_t i = 0; i < FILTER_WINDOW; i++) 
	{
			sum += values[i];
	}
	return sum / FILTER_WINDOW;
}

// ============================ UPDATE SENSOR VALUES ===============================
// Function: update_sensor_values
// Description: Updates sensor values with filtering and anomaly rejection.
// Parameters:
//   - dht22: Pointer to the DHT22 sensor structure.
// ==================================================================================
static void update_sensor_values(Dht22_Typedef *dht22)
{
    static float last_temp = 0, last_humid = 0;
    float new_temp = dht22->temperature_value;
    float new_humid = dht22->humid_value;

    // Reject sudden spikes in sensor readings
    if (fabs(new_temp - last_temp) > 5) new_temp = last_temp;
    if (fabs(new_humid - last_humid) > 10) new_humid = last_humid;

    // Apply moving average filter
    dht22->temperature_value = filter_sensor_value(temp_values, new_temp);
    dht22->humid_value = filter_sensor_value(humid_values, new_humid);

    // Update last known values
    last_temp = new_temp;
    last_humid = new_humid;
    
    // Increment filter index cyclically
    filter_index = (filter_index + 1) % FILTER_WINDOW;
}

// ======================== WAIT FOR DHT22 PIN HIGH ===========================
// Function: wait_dht22pin_up
// Description: Waits for the DHT22 data pin to go HIGH
// Parameters:
//   - dht22: Pointer to the DHT22 structure
//   - timeout: Maximum waiting time (us)
// ============================================================================
static void wait_dht22pin_up(Dht22_Typedef *dht22, uint16_t timeout)
{
    __HAL_TIM_SET_COUNTER(dht22->htim, 0);
    HAL_TIM_Base_Start(dht22->htim);
    while (__HAL_TIM_GET_COUNTER(dht22->htim) < timeout)
    {
        if (HAL_GPIO_ReadPin(dht22->GPIOx, dht22->GPIO_Pin))
        {
            HAL_TIM_Base_Stop(dht22->htim);
            break;
        }
    }
    u16Tim = __HAL_TIM_GET_COUNTER(dht22->htim);
}

// ======================== WAIT FOR DHT22 PIN LOW ============================
// Function: wait_dht22pin_down
// Description: Waits for the DHT22 data pin to go LOW
// Parameters:
//   - dht22: Pointer to the DHT22 structure
//   - timeout: Maximum waiting time (us)
// ============================================================================
static void wait_dht22pin_down(Dht22_Typedef *dht22, uint16_t timeout)
{
    __HAL_TIM_SET_COUNTER(dht22->htim, 0);
    HAL_TIM_Base_Start(dht22->htim);
    while (__HAL_TIM_GET_COUNTER(dht22->htim) < timeout)
    {
        if (!HAL_GPIO_ReadPin(dht22->GPIOx, dht22->GPIO_Pin))
        {
            HAL_TIM_Base_Stop(dht22->htim);
            break;
        }
    }
    u16Tim = __HAL_TIM_GET_COUNTER(dht22->htim);
}

// ======================== RECEIVE DATA FROM DHT22 ===========================
// Function: receive_dht22_data
// Description: Reads 40-bit data from the DHT22 sensor
// Parameters:
//   - dht22: Pointer to the DHT22 structure
// Returns:
//   - 1: Success
//   - 0: Checksum error
// ============================================================================
static uint8_t receive_dht22_data(Dht22_Typedef *dht22)
{
    uint8_t checksum = 0;
    memset(dht22->Buff_rx, 0, sizeof(dht22->Buff_rx));

    for (uint8_t i = 0; i < 5; i++)
    {
        for (uint8_t j = 0; j < 8; j++)
        {
            wait_dht22pin_up(dht22, 65);
            wait_dht22pin_down(dht22, 80);

            dht22->Buff_rx[i] <<= 1;
            if (u16Tim > 45) // HIGH pulse > 45us indicates bit '1'
            {
                dht22->Buff_rx[i] |= 1;
            }
        }
        if (i < 4)
        {
            checksum += dht22->Buff_rx[i];
        }
    }
    return (checksum == dht22->Buff_rx[4]) ? 1 : 0; // Verify checksum
}

// ======================== HANDLE DHT22 DATA ================================
// Function: dht22_handle
// Description: Handles data acquisition and conversion for DHT22
// Parameters:
//   - dht22: Pointer to the DHT22 structure
// Returns:
//   - 1: Success
//   - 0: Failure
// ============================================================================
uint8_t dht22_handle(Dht22_Typedef *dht22)
{
	HAL_GPIO_WritePin(dht22->GPIOx, dht22->GPIO_Pin, 0);
	delay_ms(dht22, 20);
	HAL_GPIO_WritePin(dht22->GPIOx, dht22->GPIO_Pin, 1);
	wait_dht22pin_up(dht22, 10);
	wait_dht22pin_down(dht22, 45);
	wait_dht22pin_up(dht22, 90);
	wait_dht22pin_down(dht22, 95);

	if (!receive_dht22_data(dht22))
	{
			return 0; // Checksum error
	}

	uint16_t humid_raw = (dht22->Buff_rx[0] << 8) | dht22->Buff_rx[1];
	uint16_t temp_raw = (dht22->Buff_rx[2] << 8) | dht22->Buff_rx[3];

	dht22->humid_value = humid_raw / 10.0;
	dht22->temperature_value = temp_raw / 10.0;
	return 1; // Success
}

// =========================== INITIALIZE DHT22 ===============================
// Function: dht22_init
// Description: Initializes the DHT22 sensor
// Parameters:
//   - dht22: Pointer to the DHT22 structure
//   - htim: Timer handle for precise timing
//   - GPIOx: GPIO port for the data pin
//   - GPIO_Pin: GPIO pin number
// ============================================================================
void dht22_init(Dht22_Typedef *dht22, TIM_HandleTypeDef *htim, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    dht22->htim = htim;
    dht22->GPIOx = GPIOx;
    dht22->GPIO_Pin = GPIO_Pin;
//    memset(dht22->Buff_rx, 0, sizeof(dht22->Buff_rx));
}

// =============================================================================
//                            END OF DHT22 LIBRARY
// =============================================================================
