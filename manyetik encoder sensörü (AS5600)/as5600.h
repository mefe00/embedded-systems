/*
 * as5600.h
 *
 *  Created on: May 23, 2025
 *      Author: mehme
 */

#ifndef INC_AS5600_H_
#define INC_AS5600_H_

//#include "stm32f4xx_hal.h"
#include "stm32f1xx_hal.h"

// AS5600 I2C Adresi
#define AS5600_ADDR (0x36 << 1) // 7-bit adres sola kaydırılmış

// AS5600 Register Adresleri
#define AS5600_REG_ZMCO       0x00 // Kalıcı sıfır pozisyonu sayacı
#define AS5600_REG_ZPOS_H     0x01 // Sıfır pozisyonu (yüksek bayt)
#define AS5600_REG_ZPOS_L     0x02 // Sıfır pozisyonu (düşük bayt)
#define AS5600_REG_MPOS_H     0x03 // Maksimum pozisyon (yüksek bayt)
#define AS5600_REG_MPOS_L     0x04 // Maksimum pozisyon (düşük bayt)
#define AS5600_REG_MANG_H     0x05 // Maksimum açı (yüksek bayt)
#define AS5600_REG_MANG_L     0x06 // Maksimum açı (düşük bayt)
#define AS5600_REG_CONF_H     0x07 // Yapılandırma (yüksek bayt)
#define AS5600_REG_CONF_L     0x08 // Yapılandırma (düşük bayt)
#define AS5600_REG_RAW_ANGLE_H 0x0C // Ham açı (yüksek bayt)
#define AS5600_REG_RAW_ANGLE_L 0x0D // Ham açı (düşük bayt)
#define AS5600_REG_ANGLE_H    0x0E // İşlenmiş açı (yüksek bayt)
#define AS5600_REG_ANGLE_L    0x0F // İşlenmiş açı (düşük bayt)
#define AS5600_REG_STATUS     0x0B // Durum register’ı

// Fonksiyon Prototipleri
HAL_StatusTypeDef AS5600_Init(I2C_HandleTypeDef *hi2c);
uint16_t AS5600_ReadRawAngle(I2C_HandleTypeDef *hi2c);
float AS5600_GetAngleDegrees(I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AS5600_SetZeroPosition(I2C_HandleTypeDef *hi2c, uint16_t zero_pos);
uint8_t AS5600_GetStatus(I2C_HandleTypeDef *hi2c);

#endif /* INC_AS5600_H_ */
