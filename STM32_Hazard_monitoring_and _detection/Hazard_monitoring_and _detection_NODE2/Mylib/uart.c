#include "uart.h"

#define MAX_DATA_UART 100
char uart_buff[MAX_DATA_UART];
static uint8_t uart_len;
uint8_t uart_flag = 0;
void receive_data(uint8_t data_rx)
{
	if(data_rx=='\n') // da nhan xong
	{
		uart_buff[uart_len++] = '\0';
		uart_flag = 1;
	}
	else // van dang nhan
	{
		uart_buff[uart_len++]=data_rx;
	}
}
void uart_receive_handle(HostwebTypedef *hostweb_node2, Dht22_Typedef *dht22)
{
	if(uart_flag)
	{
//		dht22_handle(dht22);
		cmd_process_data(uart_buff, hostweb_node2);
//		HAL_Delay(100);
		uart_len=0;
		uart_flag=0;
	}
}
void uart_init()
{
	
}
