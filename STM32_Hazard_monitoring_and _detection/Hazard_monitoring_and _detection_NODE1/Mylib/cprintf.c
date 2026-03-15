/* ============================ UART COMMUNICATION LIBRARY ============================
 *  File: uart.c
 *  Description: Functions to send data over UART interface using STM32 HAL.
 *  Author: Kieu Tien
 *  Date: 30/03/2025
 * ============================================================================ */

#include "cprintf.h"

/**
 * @brief Sends a string message via UART.
 * @param huart: UART handler pointer.
 * @param msg: Pointer to the message string to be transmitted.
 * @note This function uses HAL_UART_Transmit to send data over UART.
 */
void UART_Print(UART_HandleTypeDef *huart, char *msg) 
{
    HAL_UART_Transmit(huart, (uint8_t *)msg, strlen(msg), HAL_MAX_DELAY);
}

/**
 * @brief Sends sensor data (MPU6050 and MQ2 sensor values) via UART.
 * @param huart: UART handler pointer.
 * @param mpu6050: Pointer to the MPU6050 structure holding sensor data.
 * @param mq2: Pointer to the MQ2 structure holding sensor data.
 * @note This function formats sensor data into human-readable strings and transmits them over UART.
 */
void send_pga_ppm_to_UART(UART_HandleTypeDef *huart, MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2)
{
    char buf[50];  // Buffer to hold formatted output string for UART transmission

    // Send MPU6050 accelerometer data
    sprintf(buf, "Ax=%.2fg\t Ay=%.2fg\t Az=%.2fg\r\n", mpu6050->Ax, mpu6050->Ay, mpu6050->Az);
    UART_Print(huart, buf);
    
    // Send MPU6050 gyroscope data
    sprintf(buf, "Gx=%.2f\t Gy=%.2f\t Gz=%.2f\r\n", mpu6050->Gx, mpu6050->Gy, mpu6050->Gz);
    UART_Print(huart, buf);
    
    // Send PGA (Programmable Gain Amplifier) value
    sprintf(buf, "PGA               :%.2fg\r\n", mpu6050->pga_value);
    UART_Print(huart, buf);
    
    // Send MQ2 gas sensor data
    sprintf(buf, "ADC Value (MQ2)   : %hu\n", mq2->adc_value);
    UART_Print(huart, buf);
    
    // Send MQ2 gas concentration in PPM
    sprintf(buf, "Gas Concentration : %.2f ppm\n", mq2->ppm_value);
    UART_Print(huart, buf);
}


// Send threshold to Uart
void send_threshold_to_UART(UART_HandleTypeDef *huart,HostwebTypedef *hostweb_node2)
{
    char buff[50];
    int ppm_threshold_length = sprintf(buff, "Ppm threshold: %.2f - ", hostweb_node2->ppm_threshold);
    HAL_UART_Transmit(huart, (uint8_t*)buff, ppm_threshold_length, HAL_MAX_DELAY);
    int pga_threshold_length = sprintf(buff, "Pga threshold: %.2f - ",hostweb_node2->pga_threshold);
    HAL_UART_Transmit(huart, (uint8_t*)buff, pga_threshold_length, HAL_MAX_DELAY);
		int buzzer_length = sprintf(buff, "Buzzer_state_node1: %u \n", hostweb_node2->buzzer_state);
    HAL_UART_Transmit(huart, (uint8_t*)buff, buzzer_length, HAL_MAX_DELAY);
}

// Send to Lcd
void lcd_display_pga_ppm(LiquidCrystal_I2C *hlcd,
			MPU6050_Typedef *mpu6050, MQ2_Typedef *mq2)
{
    char pga_str[10], ppm_str[10];
    
    // Convert PGA (maximum acceleration) and PPM (air quality) values to strings
    // Ensuring that the format does not include extra characters
    sprintf(pga_str, "%5.2f", mpu6050->pga_value); // PGA value with 5 characters, 2 decimal places (maximum acceleration)
    sprintf(ppm_str, "%5.2f", mq2->ppm_value);     // PPM value with 5 characters, 1 decimal place (air quality)

    // Display the PGA (maximum acceleration) value on the first line of the LCD
    // Align the text to the left and ensure there's space after the ":"
    lcd_set_cursor(hlcd, 0, 0);
    lcd_printf(hlcd, "PGA: %-6sg", pga_str); // Left-aligned, with a space after the colon

    // Display the PPM (air quality) value on the second line of the LCD
    // Align the text to the left and ensure there's space after the ":"
    lcd_set_cursor(hlcd, 1, 0);
    lcd_printf(hlcd, "PPM: %-6sppm", ppm_str); // Left-aligned, with a space after the colon

    // Add a small delay to prevent flickering on the LCD when updating frequently
}

void lcd_display_threshold_pga_ppm(LiquidCrystal_I2C *hlcd,
			HostwebTypedef *hostweb_node2)
{
    char pga_str[10], ppm_str[10];
    
    // Convert PGA (maximum acceleration) and PPM (air quality) values to strings
    // Ensuring that the format does not include extra characters
    sprintf(pga_str, "%5.2f", hostweb_node2->pga_threshold); // PGA value with 5 characters, 2 decimal places (maximum acceleration)
    sprintf(ppm_str, "%5.2f", hostweb_node2->ppm_threshold);     // PPM value with 5 characters, 1 decimal place (air quality)

    // Display the PGA (maximum acceleration) value on the first line of the LCD
    // Align the text to the left and ensure there's space after the ":"
    lcd_set_cursor(hlcd, 0, 0);
    lcd_printf(hlcd, "NPGA: %-5sg", pga_str); // Left-aligned, with a space after the colon

    // Display the PPM (air quality) value on the second line of the LCD
    // Align the text to the left and ensure there's space after the ":"
    lcd_set_cursor(hlcd, 1, 0);
    lcd_printf(hlcd, "NPPM: %-5sppm", ppm_str); // Left-aligned, with a space after the colon

    // Add a small delay to prevent flickering on the LCD when updating frequently
}