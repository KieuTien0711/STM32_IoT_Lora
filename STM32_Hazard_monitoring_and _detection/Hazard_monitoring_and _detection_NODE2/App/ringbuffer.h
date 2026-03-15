#ifndef RING_BUFFER_H
#define RING_BUFFER_H
#include <stdint.h>

typedef struct{
	uint8_t *buffer;
	int head;
	int tail;
	int maxlen;
}RingBuff;

int8_t ring_buff_push(RingBuff *ring_buff, uint8_t data);
int8_t ring_buff_pop(RingBuff *ring_buff, uint8_t *data);
uint16_t ring_buff_available(RingBuff *ring_buff);
void ring_buff_init(RingBuff *ring_buff, uint8_t *buff, uint16_t len);
#endif