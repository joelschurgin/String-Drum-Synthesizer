/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Bit5_Pin GPIO_PIN_5
#define Bit5_GPIO_Port GPIOC
#define Bit0_Pin GPIO_PIN_1
#define Bit0_GPIO_Port GPIOB
#define Bit1_Pin GPIO_PIN_2
#define Bit1_GPIO_Port GPIOB
#define Select_C_Pin GPIO_PIN_10
#define Select_C_GPIO_Port GPIOB
#define Bit2_Pin GPIO_PIN_12
#define Bit2_GPIO_Port GPIOB
#define Bit6_Pin GPIO_PIN_6
#define Bit6_GPIO_Port GPIOC
#define Bit7_Pin GPIO_PIN_8
#define Bit7_GPIO_Port GPIOC
#define Select_A_Pin GPIO_PIN_8
#define Select_A_GPIO_Port GPIOA
#define Select_B_Pin GPIO_PIN_9
#define Select_B_GPIO_Port GPIOA
#define Bit3_Pin GPIO_PIN_11
#define Bit3_GPIO_Port GPIOA
#define Bit4_Pin GPIO_PIN_12
#define Bit4_GPIO_Port GPIOA

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
