#include "uart.h"

#define MAX_DATA_UART 100
char uart_buff[MAX_DATA_UART];       // bo dem luu du lieu UART
static uint8_t uart_len = 0;         // do dai du lieu da nhan
uint8_t uart_flag = 0;               // co bao da nhan xong

// Ham xu ly du lieu nhan tung byte
void receive_data(uint8_t data_rx)
{
    // Neu nhan qua dai thi reset lai
    if (uart_len >= MAX_DATA_UART - 1) {
        uart_len = 0;
        return;
    }

    // Neu la ky tu ket thuc '\n' thi ket thuc chuoi
    if (data_rx == '\n') {
        uart_buff[uart_len] = '\0';   // ket thuc chuoi C
        uart_flag = 1;                // bao da nhan xong
    }
    // Bo qua '\r' va ky tu khong in duoc
    else if (data_rx >= 32 && data_rx <= 126) {
        uart_buff[uart_len++] = data_rx;
    }
}

// Ham xu ly khi da nhan xong 1 chuoi hoan chinh
void uart_receive_handle(HostwebTypedef *hostweb_node1)
{
    if (uart_flag) {
        // Xu ly du lieu nhan duoc
        cmd_process_data(uart_buff, hostweb_node1);

        // Reset de san sang nhan chuoi moi
        uart_len = 0;
        uart_flag = 0;
    }
}

// Ham khoi tao UART (neu can)
void uart_init()
{
    uart_len = 0;
    uart_flag = 0;
}
