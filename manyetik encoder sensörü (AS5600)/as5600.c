/*
 * as5600.c
 *
 *  Created on: May 23, 2025
 *      Author: mehme
 */

#include "as5600.h"


// AS5600 Başlatma
HAL_StatusTypeDef AS5600_Init(I2C_HandleTypeDef *hi2c) {
    // AS5600'ün I2C adresini kontrol et
    if (HAL_I2C_IsDeviceReady(hi2c, AS5600_ADDR, 2, HAL_MAX_DELAY) != HAL_OK) {
        return HAL_ERROR;
    }
    // Varsayılan yapılandırma (isteğe bağlı, CONF register’ını ayarlayabilirsiniz)
    uint8_t conf_data[2] = {0x00, 0x00}; // Örnek: Normal mod, histerisiz kapalı
    return HAL_I2C_Mem_Write(hi2c, AS5600_ADDR, AS5600_REG_CONF_H, 1, conf_data, 2, HAL_MAX_DELAY);
}

// Ham açı okuma (12-bit, 0-4095)
uint16_t AS5600_ReadRawAngle(I2C_HandleTypeDef *hi2c) {
    uint8_t data[2];
    HAL_I2C_Mem_Read(hi2c, AS5600_ADDR, AS5600_REG_RAW_ANGLE_H, 1, data, 2, HAL_MAX_DELAY);
    return ((data[0] << 8) | data[1]);
}

// Derece cinsinden açı okuma
float AS5600_GetAngleDegrees(I2C_HandleTypeDef *hi2c) {
    uint16_t raw_angle = AS5600_ReadRawAngle(hi2c);
    return (raw_angle * 360.0f) / 4096.0f;
}

// Sıfır pozisyonu ayarlama
HAL_StatusTypeDef AS5600_SetZeroPosition(I2C_HandleTypeDef *hi2c, uint16_t zero_pos) {
    uint8_t data[2] = {(zero_pos >> 8) & 0xFF, zero_pos & 0xFF};
    return HAL_I2C_Mem_Write(hi2c, AS5600_ADDR, AS5600_REG_ZPOS_H, 1, data, 2, HAL_MAX_DELAY);
}

// Durum register’ını okuma (mıknatıs algılama durumu)
uint8_t AS5600_GetStatus(I2C_HandleTypeDef *hi2c) {
    uint8_t status;
    HAL_I2C_Mem_Read(hi2c, AS5600_ADDR, AS5600_REG_STATUS, 1, &status, 1, HAL_MAX_DELAY);
    return status;
}
