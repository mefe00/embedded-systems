/*
 * BH1750.c
 *
 *  Created on: May 22, 2025
 *      Author: mehme
 */
#include "BH1750.h"


extern I2C_HandleTypeDef hi2c1;

// Sensörü başlatmak için gerekli olan fonksiyon
void BH1750_Init(void) {
    uint8_t cmd = BH1750_POWER_ON;
    HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDRESS, &cmd, 1, HAL_MAX_DELAY);

    cmd = BH1750_CONT_H_RES_MODE;
    HAL_I2C_Master_Transmit(&hi2c1, BH1750_ADDRESS, &cmd, 1, HAL_MAX_DELAY);
}

// Sensörün lüx verisini ölçtüğü fonksiyon
uint16_t BH1750_ReadLight(void) {
    uint8_t buf[2];
    HAL_I2C_Master_Receive(&hi2c1, BH1750_ADDRESS, buf, 2, HAL_MAX_DELAY);

    uint16_t lux = (buf[0] << 8) | buf[1];
    // 1.2 katsayısı datasheet'e göre gerçek lux değeri içindir
    return lux / 1.2;
}


// Sensörden aldığımız verileri filtrelemek için gerekli birkaç filtre algoritması

//Basit ortalama filtresi
/*
uint16_t lux_buffer[AVERAGE_COUNT];
uint8_t index = 0;

uint16_t MovingAverage(uint16_t new_value) {
    lux_buffer[index++] = new_value;
    if (index >= AVERAGE_COUNT) index = 0;

    uint32_t sum = 0;
    for (int i = 0; i < AVERAGE_COUNT; i++) {
        sum += lux_buffer[i];
    }
    return sum / AVERAGE_COUNT;
}
*/

// Basit low-pass filtresi
/*
float filtered_lux = 0;
float alpha = 0.2;  // 0 ile 1 arasında (küçükse daha yavaş ama stabil)

uint16_t BH1750_FilteredLux(uint16_t new_lux) {
    filtered_lux = alpha * new_lux + (1 - alpha) * filtered_lux;
    return (uint16_t)filtered_lux;
}

*/

// çıkıntı giderici filtre
/*
uint16_t last_lux = 0;

uint16_t SpikeRejectFilter(uint16_t current_lux) {
    if (abs(current_lux - last_lux) > MAX_JUMP) {
        return last_lux;  // ani sıçrama varsa eskiyi kullan
    } else {
        last_lux = current_lux;
        return current_lux;
    }
}

*/

