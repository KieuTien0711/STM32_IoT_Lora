#ifndef CPRINTF_H
#define CPRINTF_H
#include "stm32f1xx_hal.h"
#include "LiquidCrystal_I2C.h"
#include "dht22.h"
#include <stdio.h>
#include "hostweb.h"

void send_to_gpio_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, Dht22_Typedef *dht22);
void send_value_to_UART(UART_HandleTypeDef *huart,Dht22_Typedef *dht22);
void send_threshold_to_UART(UART_HandleTypeDef *huart,HostwebTypedef *hostweb_node2);
void lcd_display_temp_humid(LiquidCrystal_I2C *hlcd, Dht22_Typedef *dht22);
void lcd_display_threshold_temp_humid(LiquidCrystal_I2C *hlcd, HostwebTypedef *hostweb_node2);
#endif