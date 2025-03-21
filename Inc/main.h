/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define B1_EXTI_IRQn EXTI15_10_IRQn
#define GREEN_Pin GPIO_PIN_2
#define GREEN_GPIO_Port GPIOC
#define RED_Pin GPIO_PIN_3
#define RED_GPIO_Port GPIOC
#define USART_TX_Pin GPIO_PIN_2
#define USART_TX_GPIO_Port GPIOA
#define USART_RX_Pin GPIO_PIN_3
#define USART_RX_GPIO_Port GPIOA
#define R3_Pin GPIO_PIN_5
#define R3_GPIO_Port GPIOC
#define C3_Pin GPIO_PIN_1
#define C3_GPIO_Port GPIOB
#define ECHO_Pin GPIO_PIN_2
#define ECHO_GPIO_Port GPIOB
#define VOID_TFT_RESET_Pin GPIO_PIN_10
#define VOID_TFT_RESET_GPIO_Port GPIOB
#define C2_Pin GPIO_PIN_12
#define C2_GPIO_Port GPIOB
#define C4_Pin GPIO_PIN_15
#define C4_GPIO_Port GPIOB
#define R2_Pin GPIO_PIN_6
#define R2_GPIO_Port GPIOC
#define TFT_DC_Pin GPIO_PIN_7
#define TFT_DC_GPIO_Port GPIOC
#define R1_Pin GPIO_PIN_8
#define R1_GPIO_Port GPIOC
#define IR_TX_Pin GPIO_PIN_9
#define IR_TX_GPIO_Port GPIOC
#define C1_Pin GPIO_PIN_11
#define C1_GPIO_Port GPIOA
#define R4_Pin GPIO_PIN_12
#define R4_GPIO_Port GPIOA
#define TMS_Pin GPIO_PIN_13
#define TMS_GPIO_Port GPIOA
#define TCK_Pin GPIO_PIN_14
#define TCK_GPIO_Port GPIOA
#define TRIG_Pin GPIO_PIN_15
#define TRIG_GPIO_Port GPIOA
#define DETECT_Pin GPIO_PIN_10
#define DETECT_GPIO_Port GPIOC
#define DETECT_EXTI_IRQn EXTI15_10_IRQn
#define SWO_Pin GPIO_PIN_3
#define SWO_GPIO_Port GPIOB
#define TFT_CS_Pin GPIO_PIN_6
#define TFT_CS_GPIO_Port GPIOB
#define BLUE_Pin GPIO_PIN_7
#define BLUE_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
