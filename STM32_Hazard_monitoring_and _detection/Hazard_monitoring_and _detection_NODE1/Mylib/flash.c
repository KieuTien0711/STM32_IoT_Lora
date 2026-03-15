#include "flash.h"
FLASH_EraseInitTypeDef eraseInit;

void flash_unlock()
{
	HAL_FLASH_Unlock();
}
void flash_lock()
{
	HAL_FLASH_Lock();
}
void flash_erase(uint32_t addr)
{
//	eraseInit.Banks=1; -> dong F4, pic16f887  to chuc theo bank
	eraseInit.PageAddress = addr;
	eraseInit.NbPages=1;
	eraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t PageError;
	HAL_FLASHEx_Erase(&eraseInit,&PageError);
}
//uint8_t a[] = {1,2,3,4,5,6,7,8,9};
//uint32_t a=123456543;
void flash_write_arr(uint32_t addr, uint8_t *data, uint16_t len)
{
	for(uint16_t i=0; i<len;i+=2)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD,addr + i, data[i]|(uint16_t)(data[i+1]<<8));
	} 																											// data[i] la byte thap; data[i+1] la byte cao; 
	//thap -> cao = trai -> phai doi voi kieu mang, nguoc lai doi voi kieu so
																													// ket qua: 01 02 03 04 05 06 07 08
}
uint16_t data_temp;
void flash_read_arr(uint32_t addr, uint8_t *data, uint16_t len)
{
	for(uint16_t i=0; i<len;i+=2)
	{
//		volatile uint32_t* p = (volatile uint32_t *)(addr + i);
//		uint16_t data = *p;
		data_temp = *(volatile uint32_t *)(addr + i);
		// Vi du: Doc cap so thu 1: 0x0201 (voi 0x02 cao, 0x01 thap)
		// data_temp co 16bit -> 0000 0010 0000 0001
		// data co kieu du lieu uint8_t -> 8bit, nen chi doc duoc du lieu 8bit thap cua data_temp
		//data[i] = data_temp; // byte thap
		//data[i+1] = data_temp>>8; // byte cao
		
		// cach viet theo con tro
		*(data+i) = data_temp; // byte thap: data[0] = 0000 0001->0x01
		*(data+i+1) = data_temp>>8; // byte cao: 0000 0010 0000 0001>>8 <-> data[1] = 0000 0010->0x02
	}
}