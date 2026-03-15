/* ========================== DFPLAYER MP3 MODULE LIBRARY ============================
 *  File: dfplayer.c
 *  Description: Functions to interface with the DFPlayer Mini MP3 module via UART.
 *  Author: Kieu Tien
 *  Date: 30/03/2025
 * ================================================================================ */

#include "dfplayer.h"

/********************************** GLOBAL VARIABLES **********************************/
int ispause = 0;   // Variable to track pause state
int isplaying = 1; // Variable to track playing state

/********************************** FUNCTION DEFINITIONS **********************************/

/**
 * @brief  Send command to DFPlayer module via UART.
 * @param  dfplayer: Pointer to DFPlayer structure
 * @param  cmd: Command byte
 * @param  Parameter1: First parameter
 * @param  Parameter2: Second parameter
 * @retval None
 */
void Send_cmd(DfplayerTypedef *dfplayer, uint8_t cmd, uint8_t Parameter1, uint8_t Parameter2)
{
    uint16_t Checksum = Version + Cmd_Len + cmd + Feedback + Parameter1 + Parameter2;
    Checksum = 0 - Checksum;

    uint8_t CmdSequence[10] = { Start_Byte, Version, Cmd_Len, cmd, Feedback,
                                Parameter1, Parameter2, (Checksum >> 8) & 0x00FF, (Checksum & 0x00FF), End_Byte };

    HAL_UART_Transmit(dfplayer->huart, CmdSequence, 10, HAL_MAX_DELAY);
    HAL_Delay(100);
}

/********************************** MP3 CONTROL FUNCTIONS **********************************/

/**
 * @brief  Play a specific file on DFPlayer
 * @param  dfplayer: Pointer to DFPlayer structure
 * @param  file: File number to play (1-999)
 * @retval None
 */
void dfplayer_playfile(DfplayerTypedef *dfplayer, uint16_t file)
{
    uint8_t high_byte = (file >> 8) & 0xFF; // Extract high byte
    uint8_t low_byte = file & 0xFF;         // Extract low byte
    Send_cmd(dfplayer, CMD_FilePlay, high_byte, low_byte);
}

/**
 * @brief  Play the next song
 * @param  dfplayer: Pointer to DFPlayer structure
 * @retval None
 */
void dfplayer_playnext(DfplayerTypedef *dfplayer)
{
    Send_cmd(dfplayer, CMD_Next, 0x00, 0x00);
}

/**
 * @brief  Play the previous song
 * @param  dfplayer: Pointer to DFPlayer structure
 * @retval None
 */
void dfplayer_playprevious(DfplayerTypedef *dfplayer)
{
    Send_cmd(dfplayer, CMD_Previous, 0x00, 0x00);
}

/********************************** VOLUME CONTROL FUNCTIONS **********************************/

/**
 * @brief  Increase volume by 1 level
 * @param  dfplayer: Pointer to DFPlayer structure
 * @retval None
 */
void dfplayer_volumeincrease(DfplayerTypedef *dfplayer)
{
    Send_cmd(dfplayer, CMD_IncVolume, 0x00, 0x00);
}

/**
 * @brief  Decrease volume by 1 level
 * @param  dfplayer: Pointer to DFPlayer structure
 * @retval None
 */
void dfplayer_volumedecrease(DfplayerTypedef *dfplayer)
{
    Send_cmd(dfplayer, CMD_DecVolume, 0x00, 0x00);
}

/********************************** DFPLAYER INITIALIZATION **********************************/

/**
 * @brief  Initialize the DFPlayer module
 * @param  dfplayer: Pointer to DFPlayer structure
 * @param  huart: Pointer to UART handle
 * @param  volume: Initial volume level (0 - 30)
 * @retval None
 */
void dfplayer_init(DfplayerTypedef *dfplayer, UART_HandleTypeDef *huart, uint8_t volume)
{
    dfplayer->huart = huart;
    
    // Initialize DFPlayer and select TF card as source
    Send_cmd(dfplayer, CMD_SetPlayer, 0x00, Source);
    
    // Set initial volume level
    Send_cmd(dfplayer, CMD_SetVolume, 0x00, volume);
}