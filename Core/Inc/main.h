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
#include "stm32f0xx_hal.h"

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
#define AIN_S2_Pin GPIO_PIN_13
#define AIN_S2_GPIO_Port GPIOC
#define AIN_S3_Pin GPIO_PIN_14
#define AIN_S3_GPIO_Port GPIOC
#define AIN_S1_Pin GPIO_PIN_15
#define AIN_S1_GPIO_Port GPIOC
#define AIN_S0_Pin GPIO_PIN_0
#define AIN_S0_GPIO_Port GPIOF
#define ADC_IN_Pin GPIO_PIN_0
#define ADC_IN_GPIO_Port GPIOA
#define TEAM2_SWCLK_Pin GPIO_PIN_11
#define TEAM2_SWCLK_GPIO_Port GPIOB
#define TEAM2_LATCH_OUTPUT_Pin GPIO_PIN_12
#define TEAM2_LATCH_OUTPUT_GPIO_Port GPIOB
#define TEAM2_LED_EN_Pin GPIO_PIN_13
#define TEAM2_LED_EN_GPIO_Port GPIOB
#define TEAM2_SWD_Pin GPIO_PIN_14
#define TEAM2_SWD_GPIO_Port GPIOB
#define TEAM1_LED_EN_Pin GPIO_PIN_15
#define TEAM1_LED_EN_GPIO_Port GPIOB
#define ADD_SCORE_BUTTON_Pin GPIO_PIN_8
#define ADD_SCORE_BUTTON_GPIO_Port GPIOA
#define ADD_SCORE_BUTTON_EXTI_IRQn EXTI4_15_IRQn
#define TEAM1_SWD_Pin GPIO_PIN_9
#define TEAM1_SWD_GPIO_Port GPIOA
#define REMOVE_SCORE_BUTTON_Pin GPIO_PIN_10
#define REMOVE_SCORE_BUTTON_GPIO_Port GPIOA
#define REMOVE_SCORE_BUTTON_EXTI_IRQn EXTI4_15_IRQn
#define TEAM1_LATCH_OUTPUT_Pin GPIO_PIN_11
#define TEAM1_LATCH_OUTPUT_GPIO_Port GPIOA
#define SELECT_TEAM_BUTTON_Pin GPIO_PIN_12
#define SELECT_TEAM_BUTTON_GPIO_Port GPIOA
#define SELECT_TEAM_BUTTON_EXTI_IRQn EXTI4_15_IRQn
#define SWDIO_Pin GPIO_PIN_13
#define SWDIO_GPIO_Port GPIOA
#define TEAM1_SWCLK_Pin GPIO_PIN_6
#define TEAM1_SWCLK_GPIO_Port GPIOF
#define START_RESET_BUTTON_Pin GPIO_PIN_7
#define START_RESET_BUTTON_GPIO_Port GPIOF
#define START_RESET_BUTTON_EXTI_IRQn EXTI4_15_IRQn
#define SWCLK_Pin GPIO_PIN_14
#define SWCLK_GPIO_Port GPIOA
#define TEAM1_DIGIT1_EN_Pin GPIO_PIN_15
#define TEAM1_DIGIT1_EN_GPIO_Port GPIOA
#define TEAM2_DIGIT4_EN_Pin GPIO_PIN_3
#define TEAM2_DIGIT4_EN_GPIO_Port GPIOB
#define TEAM1_DIGIT2_EN_Pin GPIO_PIN_4
#define TEAM1_DIGIT2_EN_GPIO_Port GPIOB
#define TEAM2_DIGIT3_EN_Pin GPIO_PIN_5
#define TEAM2_DIGIT3_EN_GPIO_Port GPIOB
#define TEAM1_DIGIT3_EN_Pin GPIO_PIN_6
#define TEAM1_DIGIT3_EN_GPIO_Port GPIOB
#define TEAM2_DIGIT2_EN_Pin GPIO_PIN_7
#define TEAM2_DIGIT2_EN_GPIO_Port GPIOB
#define TEAM1_DIGIT4_EN_Pin GPIO_PIN_8
#define TEAM1_DIGIT4_EN_GPIO_Port GPIOB
#define TEAM2_DIGIT1_EN_Pin GPIO_PIN_9
#define TEAM2_DIGIT1_EN_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
