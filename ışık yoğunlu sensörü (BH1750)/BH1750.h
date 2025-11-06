/*
 * BH1750.h
 *
 *  Created on: May 22, 2025
 *      Author: mehme
 */
#ifndef INC_BH1750_H_
#define INC_BH1750_H_

#include "stm32f1xx.h"


#define BH1750_ADDRESS 0x23 << 1  // BH1750'nin adresi
#define BH1750_POWER_ON 0x01
#define BH1750_RESET    0x07
#define BH1750_CONT_H_RES_MODE 0x10
#define MAX_JUMP 100 // filtre için
#define AVERAGE_COUNT 10 // filtre için

// Gerekli fonksiyonlar
void BH1750_Init(void);
uint16_t BH1750_ReadLight(void);

#endif /* INC_BH1750_H_ */
