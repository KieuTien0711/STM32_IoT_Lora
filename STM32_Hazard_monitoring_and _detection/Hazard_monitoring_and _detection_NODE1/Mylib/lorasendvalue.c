#include "lorasendvalue.h"


void lora_send_sensor_value_esp32(UART_HandleTypeDef *huart,
	MQ2_Typedef *mq2, MPU6050_Typedef *mpu6050)
{
//	dht22_handle(dht22);
	char buff[100]; // Tang kich thuoc bo dem de du chua chuoi
	int length = sprintf(buff, "id:node1;ppmvalue:%.2f;pgavalue:%.2f;\n",
		mq2->ppm_value, mpu6050->pga_value);
	// Gui du lieu qua UART
	HAL_UART_Transmit(huart, (uint8_t*)buff, length, HAL_MAX_DELAY);
}
