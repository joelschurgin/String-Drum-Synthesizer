/*
 * EmotiveKnobs.c
 *
 *  Created on: Dec 3, 2024
 *      Author: joels
 */

#include "EmotiveKnobs.h"

uint32_t getADCValue(ADC_HandleTypeDef *hadc)
{
	HAL_ADC_Start(hadc);
	HAL_ADC_PollForConversion(hadc, 1000);
	uint32_t val = HAL_ADC_GetValue(hadc);
	HAL_ADC_Stop(hadc);
	return val;
}
