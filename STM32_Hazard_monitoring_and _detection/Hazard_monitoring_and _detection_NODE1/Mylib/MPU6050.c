/* ========================== MPU6050 LIBRARY ============================
 *  File: mpu6050.c
 *  Description: Functions to interface with MPU6050 and store calibration data in Flash.
 *  Author: Kieu Tien
 *  Date: 30/03/2025
 * ================================================================================ */

#include "MPU6050.h"
// Variables to store accelerometer and gyroscope offsets
float Accel_X_Offset, Accel_Y_Offset, Accel_Z_Offset;
float Gyro_X_Offset, Gyro_Y_Offset, Gyro_Z_Offset;

// Variables to check calibration data status
static uint8_t check_no_calib = 0;
static uint8_t check_calib = 0;

void MPU6050_Read_Gyro(MPU6050_Typedef *mpu6050);
void MPU6050_Read_Accel(MPU6050_Typedef *mpu6050);

/**
 * @brief Save MPU6050 sensor data to Flash memory for recovery after a reset.
 * @param mpu6050: Pointer to MPU6050 data structure.
 */
void flash_save_MPU6050(MPU6050_Typedef *mpu6050) {
    flash_unlock();                      // Unlock Flash memory
    flash_erase(FLASH_USER_START_ADDR);  // Erase Flash page before writing new data
    flash_write_arr(FLASH_USER_START_ADDR, (uint8_t*)mpu6050, sizeof(mpu6050));
    flash_lock();                        // Lock Flash to protect data
}

/**
 * @brief Read MPU6050 data from Flash memory.
 * @param mpu6050: Pointer to MPU6050 data structure.
 */
void flash_read_MPU6050(MPU6050_Typedef *mpu6050)
{
    flash_read_arr(FLASH_USER_START_ADDR, (uint8_t*)mpu6050, sizeof(MPU6050_Typedef));
}

/**
 * @brief Calibrate MPU6050 sensor offsets by averaging multiple measurements.
 * @param mpu6050: Pointer to MPU6050 data structure.
 * @param samples: Number of samples to average.
 */
void MPU6050_Calibrate(MPU6050_Typedef *mpu6050, int samples)
{
    int32_t Accel_X_Sum = 0, Accel_Y_Sum = 0, Accel_Z_Sum = 0;
    int32_t Gyro_X_Sum = 0, Gyro_Y_Sum = 0, Gyro_Z_Sum = 0;

    for (int i = 0; i < samples; i++)
    {
        uint8_t Rec_Data[6];
        int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
        int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;

        // Read accelerometer data from MPU6050
        HAL_I2C_Mem_Read(mpu6050->hi2c, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 6, 1000);
        Accel_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
        Accel_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
        Accel_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

        // Read gyroscope data from MPU6050
        HAL_I2C_Mem_Read(mpu6050->hi2c, MPU6050_ADDR, GYRO_XOUT_H, 1, Rec_Data, 6, 1000);
        Gyro_X_RAW = (int16_t)(Rec_Data[0] << 8 | Rec_Data[1]);
        Gyro_Y_RAW = (int16_t)(Rec_Data[2] << 8 | Rec_Data[3]);
        Gyro_Z_RAW = (int16_t)(Rec_Data[4] << 8 | Rec_Data[5]);

        // Sum up values for averaging
        Accel_X_Sum += Accel_X_RAW;
        Accel_Y_Sum += Accel_Y_RAW;
        Accel_Z_Sum += Accel_Z_RAW - 4096; // 1g ~ 4096

        Gyro_X_Sum += Gyro_X_RAW;
        Gyro_Y_Sum += Gyro_Y_RAW;
        Gyro_Z_Sum += Gyro_Z_RAW;
        HAL_Delay(1); // Short delay between samples
    }

    // Compute average offsets
    Accel_X_Offset = (float)Accel_X_Sum / samples;
    Accel_Y_Offset = (float)Accel_Y_Sum / samples;
    Accel_Z_Offset = (float)Accel_Z_Sum / samples;

    Gyro_X_Offset = (float)Gyro_X_Sum / samples;
    Gyro_Y_Offset = (float)Gyro_Y_Sum / samples;
    Gyro_Z_Offset = (float)Gyro_Z_Sum / samples;
}

/**
 * @brief Reset MPU6050 sensor reference coordinates.
 * @param mpu6050: Pointer to MPU6050 data structure.
 * @param set_again: 0 - Read from Flash memory, 1 - Perform new calibration.
 */
void calcGyroOffsets(MPU6050_Typedef *mpu6050, uint8_t set_again)
{
	flash_save_MPU6050(mpu6050);
	if(set_again == 0)
	{
			if(check_no_calib == 0)
			{
					check_no_calib = 1;
					flash_read_MPU6050(mpu6050);
			}
	}
	else if(set_again == 1)
	{
			if(check_calib == 0)
			{
					check_calib = 1;
					MPU6050_Calibrate(mpu6050, 100);
			}
	}
}

/**
 * @brief Read accelerometer data from MPU6050.
 * @param mpu6050: Pointer to MPU6050 data structure.
 */
void MPU6050_Read_Accel(MPU6050_Typedef *mpu6050)
{
    uint8_t Rec_Data[6];
    HAL_I2C_Mem_Read(mpu6050->hi2c, MPU6050_ADDR, ACCEL_XOUT_H, 1, Rec_Data, 6, 1000);
    mpu6050->Ax = ((int16_t)(Rec_Data[0] << 8 | Rec_Data[1])) / 4096.0;
    mpu6050->Ay = ((int16_t)(Rec_Data[2] << 8 | Rec_Data[3])) / 4096.0;
    mpu6050->Az = ((int16_t)(Rec_Data[4] << 8 | Rec_Data[5])) / 4096.0;
}

/**
 * @brief Reads the gyroscope data from the MPU6050 sensor.
 * @param mpu6050: Pointer to the MPU6050 structure that holds the sensor data.
 * @note This function reads 6 bytes of raw gyroscope data from the MPU6050 sensor and converts the values to degrees per second.
 */
void MPU6050_Read_Gyro(MPU6050_Typedef *mpu6050)
{
    uint8_t Rec_Data[6];  // Array to hold the received gyroscope data

    // Read 6 bytes of gyroscope data from the MPU6050 sensor over I2C
    HAL_I2C_Mem_Read(mpu6050->hi2c, MPU6050_ADDR, GYRO_XOUT_H, 1, Rec_Data, 6, 1000);

    // Combine the high and low byte of each axis to form a 16-bit value and scale to degrees per second
    mpu6050->Gx = ((int16_t)(Rec_Data[0] << 8 | Rec_Data[1])) / 65.5;
    mpu6050->Gy = ((int16_t)(Rec_Data[2] << 8 | Rec_Data[3])) / 65.5;
    mpu6050->Gz = ((int16_t)(Rec_Data[4] << 8 | Rec_Data[5])) / 65.5;
}


/**
 * @brief Process MPU6050 sensor data.
 * @param mpu6050: Pointer to MPU6050 data structure.
 */
void MPU6050_handle(MPU6050_Typedef *mpu6050)
{
    MPU6050_Read_Accel(mpu6050);
    MPU6050_Read_Gyro(mpu6050);
    
    // Tinh pga_value la do lech cua gia toc thuc te so voi trong luc chuan (0,0,1)
    float ax = mpu6050->Ax;
    float ay = mpu6050->Ay;
    float az = mpu6050->Az;

    // Lay do lon vector gia toc, tru 1 (tuong duong 1g trong luc)
    float diff = sqrt(ax * ax + ay * ay + az * az) - 1.0f;
    
    // Neu do lech gan 0 thi gan pga_value = 0, nguoc lai lay gia tri tuyet doi cua diff
    if (fabs(diff) < 0.05f)  // nguong 0.05g co the chinh theo thuc te
    {
        mpu6050->pga_value = 0.0f;
    }
    else
    {
        mpu6050->pga_value = fabs(diff);
    }
}

/**
 * @brief Initializes the MPU6050 sensor.
 * @param mpu6050: Pointer to the MPU6050 structure that holds the sensor data.
 * @param hi2c: Pointer to the I2C handler used for communication with the MPU6050 sensor.
 * @note This function checks the sensor's identity and initializes the MPU6050 sensor by configuring various registers.
 */
void MPU6050_init(MPU6050_Typedef *mpu6050, I2C_HandleTypeDef *hi2c)
{
	mpu6050->hi2c = hi2c;  // Assign the I2C handler to the MPU6050 structure
	uint8_t check, Data;

	// Read the WHO_AM_I register to check the identity of the MPU6050 sensor
	HAL_I2C_Mem_Read(hi2c, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 1000);

	// If the device is identified as MPU6050 (WHO_AM_I == 0x68), proceed with initialization
	if (check == 0x68) {
			// Set the power management register to wake up the sensor (set PWR_MGMT_1 to 0)
			Data = 0;
			HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, PWR_MGMT_1, 1, &Data, 1, 1000);
			
			// Set the sample rate divider register (SMPRT_DIV) to 0 to allow continuous data sampling
			Data = 0x00;
			HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, SMPRT_DIV, 1, &Data, 1, 1000);
			
			// Configure the accelerometer range (ACCEL_CONFIG_REG) to ±2g
			Data = 0x10;
			HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &Data, 1, 1000);
			
			// Configure the gyroscope range (GYRO_CONFIG_REG) to ±250 degrees per second
			Data = 0x08;
			HAL_I2C_Mem_Write(hi2c, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &Data, 1, 1000);
	}
}

