#include "lorasendvalue.h"


void lora_send_sensor_value_esp32(UART_HandleTypeDef *huart, Dht22_Typedef *dht22)
{
//	dht22_handle(dht22);
	char buff[100]; // Tang kich thuoc bo dem de du chua chuoi
	int length = sprintf(buff, "id:node2;tempvalue:%.2f;humidvalue:%.2f;\n", dht22->temperature_value, dht22->humid_value);
	// Gui du lieu qua UART
	HAL_UART_Transmit(huart, (uint8_t*)buff, length, HAL_MAX_DELAY);
}
