#ifndef DFPLAYER_H
#define DFPLAYER_H
#include "stm32f1xx.h"
#include <stdio.h>

#define Source      			0x02		// TF CARD
// -------- Frame Byte -------	
#define Start_Byte 				0x7E	
#define End_Byte   				0xEF	
#define Version    				0xFF	
#define Cmd_Len    				0x06	
#define Feedback   				0x00		//If need for Feedback: 0x01,  No Feedback: 0
// -------- Command ----------	
#define CMD_SetPlayer			0x3F	
#define CMD_SetVolume			0x06

#define CMD_FilePlay			0x03 		//file tu thu tu 0-2999
#define CMD_Next					0x01
#define CMD_Previous			0x02
#define CMD_Playback			0x0D
#define CMD_Pause					0x0E

#define CMD_IncVolume			0x04
#define CMD_DecVolume			0x05

typedef struct
{
	UART_HandleTypeDef *huart;
} DfplayerTypedef;

/**************************** MP3 FUNCTION *************************/
void dfplayer_playfile(DfplayerTypedef *dfplayer, uint16_t file);   
void dfplayer_playnext(DfplayerTypedef *dfplayer);                  
void dfplayer_playprevious(DfplayerTypedef *dfplayer);              
																																		
/**************************** VOLUME FUNCTION **********************/
void dfplayer_volumeincrease(DfplayerTypedef *dfplayer);            
void dfplayer_volumedecrease(DfplayerTypedef *dfplayer);            
																																		
/**************************** DFPLAYER INIT ************************/
void dfplayer_init(DfplayerTypedef *dfplayer, 
	UART_HandleTypeDef *huart, uint8_t volume);

#endif