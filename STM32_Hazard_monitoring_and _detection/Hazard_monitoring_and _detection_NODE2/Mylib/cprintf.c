#include "cprintf.h"

/**
 * @brief Send data bit by bit using a custom protocol
 * @param GPIOx GPIO Port
 * @param GPIO_Pin GPIO Pin
 * @param u8Data Data to be sent
 */
static void send_data(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, uint8_t u8Data)
{
    for (uint8_t i = 0; i < 8; ++i)
    {
        if (u8Data & 0x80) // If MSB is 1
        {
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
            HAL_Delay(4);
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
            HAL_Delay(1);
        }
        else // If MSB is 0
        {
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
            HAL_Delay(1);
            HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
            HAL_Delay(4);
        }
        u8Data <<= 1; // Shift to the next bit
    }
}

/**
 * @brief Send data from STM32 to a specific GPIO Pin
 * @param GPIOx GPIO Port
 * @param GPIO_Pin GPIO Pin
 * @param dht22 Pointer to DHT22 sensor structure
 */
void send_to_gpio_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Dht22_Typedef *dht22)
{
    send_data(GPIOx, GPIO_Pin, dht22->Buff_rx[0]);
    HAL_Delay(10);
    send_data(GPIOx, GPIO_Pin, dht22->Buff_rx[1]);
    HAL_Delay(20);
    send_data(GPIOx, GPIO_Pin, dht22->Buff_rx[2]);
    HAL_Delay(10);
    send_data(GPIOx, GPIO_Pin, dht22->Buff_rx[3]);
}

/**
 * @brief Send temperature and humidity data from STM32 via UART
 * @param huart Pointer to UART handle
 * @param dht22 Pointer to DHT22 sensor structure
 */
void send_threshold_to_UART(UART_HandleTypeDef *huart,HostwebTypedef *hostweb_node2)
{
    char buff[50];
    int humid_threshold_length = sprintf(buff, "Humidity threshold: %.2f%% - ", hostweb_node2->humid_threshold);
    HAL_UART_Transmit(huart, (uint8_t*)buff, humid_threshold_length, HAL_MAX_DELAY);
    int temperature_threshold_length = sprintf(buff, "Temperature threshold: %.2f C - ",hostweb_node2->temperature_threshold);
    HAL_UART_Transmit(huart, (uint8_t*)buff, temperature_threshold_length, HAL_MAX_DELAY);
		int buzzer_length = sprintf(buff, "Buzzer_state_node2: %u \n", hostweb_node2->buzzer_state);
    HAL_UART_Transmit(huart, (uint8_t*)buff, buzzer_length, HAL_MAX_DELAY);
}

void send_value_to_UART(UART_HandleTypeDef *huart,Dht22_Typedef *dht22)
{
    char buff[50];
    int humid_length = sprintf(buff, "Humidity: %.1f%% - ", dht22->humid_value);
    HAL_UART_Transmit(huart, (uint8_t*)buff, humid_length, HAL_MAX_DELAY);
    int temperature_length = sprintf(buff, "Temperature: %.1f C - ", dht22->temperature_value);
    HAL_UART_Transmit(huart, (uint8_t*)buff, temperature_length, HAL_MAX_DELAY);
}
/**
 * @brief Display temperature and humidity values on LCD
 * @param hlcd Pointer to LCD handle
 * @param dht22 Pointer to DHT22 sensor structure
 */
void lcd_display_temp_humid(LiquidCrystal_I2C *hlcd, Dht22_Typedef *dht22)
{
    static uint8_t degree_created = 0;
    if (!degree_created)
    {
        uint8_t degree_symbol[8] = {0x0E, 0x0A, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00};
        lcd_create_char(hlcd, 0, degree_symbol);
        degree_created = 1;
    }

    // Check for sensor error
    if (dht22->temperature_value < -40.0 || dht22->temperature_value > 80.0 ||
        dht22->humid_value < 0.0 || dht22->humid_value > 100.0)
    {
        lcd_clear_display(hlcd);
        lcd_set_cursor(hlcd, 0, 0);
        lcd_printf(hlcd, "Sensor Error!");
        return;
    }

    char temp_str[10], humid_str[10];
    sprintf(temp_str, "%.1f", dht22->temperature_value);
    sprintf(humid_str, "%.1f", dht22->humid_value);

    lcd_set_cursor(hlcd, 0, 0);
    lcd_printf(hlcd, "Nhiet do: %s", temp_str);
    lcd_send_data(hlcd, 0);
    lcd_printf(hlcd, "C");

    lcd_set_cursor(hlcd, 1, 0);
    lcd_printf(hlcd, "Do am   : %s %%", humid_str);
}

void lcd_display_threshold_temp_humid(LiquidCrystal_I2C *hlcd, HostwebTypedef *hostweb_node2)
{
    static uint8_t degree_created = 0;
    if (!degree_created)
    {
        uint8_t degree_symbol[8] = {0x0E, 0x0A, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00};
        lcd_create_char(hlcd, 0, degree_symbol);
        degree_created = 1;
    }

//    // Check for sensor error
//    if (dht22->temperature_value < -40.0 || dht22->temperature_value > 80.0 ||
//        dht22->humid_value < 0.0 || dht22->humid_value > 100.0)
//    {
//        lcd_clear_display(hlcd);
//        lcd_set_cursor(hlcd, 0, 0);
//        lcd_printf(hlcd, "Sensor Error!");
//        return;
//    }

    char temp_str[10], humid_str[10];
    sprintf(temp_str, "%.1f", hostweb_node2->temperature_threshold);
    sprintf(humid_str, "%.1f", hostweb_node2->humid_threshold);

    lcd_set_cursor(hlcd, 0, 0);
    lcd_printf(hlcd, "N.Nhiet do:%s", temp_str);
//    lcd_send_data(hlcd, 0);
    lcd_printf(hlcd, "C");

    lcd_set_cursor(hlcd, 1, 0);
    lcd_printf(hlcd, "N.Do am   :%s%%", humid_str);
}