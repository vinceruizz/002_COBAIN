/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
enum StartButtonRole
{
  RESTART,
  BEGIN,
  NONE
};
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void updateLEDs();
int getWeight(int);
void displayScore();
bool bounceFree(GPIO_TypeDef*, uint16_t);
void beginGame();
void restartGame();
void blinkScore();
void initializeBaseWeights();
int digitToHexDisplay(int);
void flashDigit(GPIO_TypeDef*, uint16_t, int);
int getRawWeight(int);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
// Variables to help button logics
enum StartButtonRole startRole = BEGIN;
bool isSettingUp = true;

// Variables to help game logics
uint32_t adcValue = 0;
int holeScores[9] = { 50, 150, 75, 300, 500, 200, 25, 100, 25 };
int baseWeights[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int weightPerBag[9] = { 0, 0, 0, 0, 0, 0, 0, 0, 0 };
int currentTeam = 1;
int team1Score = 123;
int team1TargetScore = 0;
int team1SetTargetScore = 0;
int team2Score = 0;
int team2TargetScore = 0;
int team2SetTargetScore = 0;

// See if buttons were pressed or held
bool startButtonPressed = false;
bool startButtonHeld = false;
bool selectTeamButtonPressed = false;
bool addScoreButtonPressed = false;
bool removeScoreButtonPressed = false;
bool addAndRemoveScoreButtonsHeld = false;
bool switchingTeam = false;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC_Init();
  MX_TIM1_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_ADCEx_Calibration_Start(&hadc);
  HAL_ADC_Start_DMA(&hadc, &adcValue, 1);
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Stop_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim3);
  HAL_TIM_Base_Stop_IT(&htim3);
  initializeBaseWeights();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // ========== Game Logic ==========
    if (startRole == NONE)
    {
      // Calculate the total score from all holes
      int totalScore = 0;
      for (int i = 0; i < 9; i++)
      {
        if (i != 0) continue;
        totalScore += round(getWeight(i) / weightPerBag[i]) * holeScores[i];
        //totalScore += getWeight(i) * holeScores[i];
      }

      if (!switchingTeam)
      {
        if (currentTeam == 1)
        {
          // Game ends
          if (totalScore == team1TargetScore)
          {
            team1Score = 0;
            blinkScore();
          }
          else if (totalScore < team1TargetScore)
          {
            team1Score = team1TargetScore - totalScore;
          }
          else
          {
            if (currentTeam != 1) { continue; }
            team1Score = team1TargetScore;
            updateLEDs();
            blinkScore();
          }
        }
        else
        {
          // Game ends
          if (totalScore == team2TargetScore)
          {
            team2Score = 0;
            blinkScore();
          }
          else if (totalScore < team2TargetScore)
          {
            team2Score = team2TargetScore - totalScore;
          }
          else
          {
            if (currentTeam != 2) { continue; }
            team2Score = team2TargetScore;
            updateLEDs();
            blinkScore();
          }
        }
      }
      else
      {
        switchingTeam = totalScore != 0;
      }
    }

    // ========== Update the LEDs ==========
//    team1SetTargetScore = getRawWeight(0);
//    team2SetTargetScore = getRawWeight(2);
    updateLEDs();
    displayScore();

    // ========== Button logics ==========
    // Reset
    if (startButtonHeld)
    {
      restartGame();
      startButtonHeld = false;
      __HAL_TIM_SET_COUNTER(&htim1, 0);
      while (HAL_GPIO_ReadPin(START_RESET_BUTTON_GPIO_Port, START_RESET_BUTTON_Pin) == GPIO_PIN_RESET);
      startButtonPressed = false;
    }
    // Start
    else if (startButtonPressed && bounceFree(START_RESET_BUTTON_GPIO_Port, START_RESET_BUTTON_Pin))
    {
      switch (startRole)
      {
        case RESTART:
          restartGame();
          break;
        case BEGIN:
          beginGame();
          break;
        default: break;
      }

      startButtonPressed = false;
    }
    // Calibrate
    else if (addAndRemoveScoreButtonsHeld && startRole == BEGIN)
    {
      isSettingUp = false;
      currentTeam = 1;
      for (int i = 0; i < 9; i++)
      {
        if (i != 0) continue;
        weightPerBag[i] = getWeight(i);
      }
      team1Score = weightPerBag[0];
      addScoreButtonPressed = false;
    }
    // Select team
    else if (selectTeamButtonPressed && bounceFree(SELECT_TEAM_BUTTON_GPIO_Port, SELECT_TEAM_BUTTON_Pin))
    {
      if (currentTeam == 1)
      {
        currentTeam = 2;
        if (startRole == NONE)
        {
          team1TargetScore = team1Score;
          if (team1Score == 0) { startRole = RESTART; }
        }
      }
      else
      {
        currentTeam = 1;
        if (startRole == NONE)
        {
          team2TargetScore = team2Score;
          if (team2Score == 0) { startRole = RESTART; }
        }
      }

      switchingTeam = true;
      selectTeamButtonPressed = false;
    }
    // Add score
    else if(addScoreButtonPressed && bounceFree(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin))
    {
      if (isSettingUp)
      {
        if (currentTeam == 1) { if (team1SetTargetScore < 9000) { team1SetTargetScore += 100; } }
        else if (team2SetTargetScore < 9000) { team2SetTargetScore += 100; }
      }
      // Manually adjust the score
      else
      {
        if (currentTeam == 1) { if (team1TargetScore < 9975) { team1TargetScore += 25; } }
        else if (team2TargetScore < 9975) { team2TargetScore += 25; }
      }
      addScoreButtonPressed = false;
    }
    // Remove score
    else if (removeScoreButtonPressed && bounceFree(REMOVE_SCORE_BUTTON_GPIO_Port, REMOVE_SCORE_BUTTON_Pin))
    {
      if (isSettingUp)
      {
        if (currentTeam == 1) { if (team1SetTargetScore >= 100) { team1SetTargetScore -= 100; } }
        else if (team2SetTargetScore >= 100) { team2SetTargetScore -= 100; }
      }
      // Manually adjust the score
      else
      {
        if (currentTeam == 1) { if (team1TargetScore >= 25) { team1TargetScore -= 25; } }
        else if (team2TargetScore >= 25) { team2TargetScore -= 25; }
      }
      removeScoreButtonPressed = false;
    }

    // Stop the "held for 3s" timers if the button is released early
    if (HAL_GPIO_ReadPin(START_RESET_BUTTON_GPIO_Port, START_RESET_BUTTON_Pin) == GPIO_PIN_SET)
    {
      __HAL_TIM_SET_COUNTER(&htim1, 0);
    }
    if (HAL_GPIO_ReadPin(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin) == GPIO_PIN_SET)
//    if (HAL_GPIO_ReadPin(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin) == GPIO_PIN_SET ||
//        HAL_GPIO_ReadPin(REMOVE_SCORE_BUTTON_GPIO_Port, REMOVE_SCORE_BUTTON_Pin) == GPIO_PIN_SET)
    {
      __HAL_TIM_SET_COUNTER(&htim3, 0);
    }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_HSI14;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSI14State = RCC_HSI14_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.HSI14CalibrationValue = 16;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void updateLEDs()
{
  HAL_GPIO_WritePin(TEAM1_LED_EN_GPIO_Port, TEAM1_LED_EN_Pin, (GPIO_PinState) (currentTeam == 1));
  HAL_GPIO_WritePin(TEAM2_LED_EN_GPIO_Port, TEAM2_LED_EN_Pin, (GPIO_PinState) (currentTeam == 2));
}

// Reset the game
void restartGame()
{
  startRole = BEGIN;
  isSettingUp = true;
  currentTeam = 1;
  startButtonPressed = false;
  startButtonHeld = false;
  selectTeamButtonPressed = false;
  addScoreButtonPressed = false;
  removeScoreButtonPressed = false;
  addAndRemoveScoreButtonsHeld = false;
  switchingTeam = false;
  initializeBaseWeights();

  // Stop the timers and reset their counters
  HAL_TIM_Base_Stop_IT(&htim1);
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  HAL_TIM_Base_Stop_IT(&htim3);
  __HAL_TIM_SET_COUNTER(&htim3, 0);
}

// Get the total weight on the FSR, including the base
int getRawWeight(int fsrNum)
{
  // Set the MUX to get value from the interested FSR
  HAL_GPIO_WritePin(AIN_S0_GPIO_Port, AIN_S0_Pin, (GPIO_PinState) ((fsrNum >> 0) & 1));
  HAL_GPIO_WritePin(AIN_S1_GPIO_Port, AIN_S1_Pin, (GPIO_PinState) ((fsrNum >> 1) & 1));
  HAL_GPIO_WritePin(AIN_S2_GPIO_Port, AIN_S2_Pin, (GPIO_PinState) ((fsrNum >> 2) & 1));
  HAL_GPIO_WritePin(AIN_S3_GPIO_Port, AIN_S3_Pin, (GPIO_PinState) ((fsrNum >> 3) & 1));
  HAL_Delay(2);

  adcValue = HAL_ADC_GetValue(&hadc);
  //return exp((adcValue - 1384.04) / 307.17) + 19.98;
  return pow(2.75, 0.00151 * adcValue);
  //return adcValue;
}

// Get the bean bag weight, excluding the base
int getWeight(int fsrNum)
{
  int totalWeight = getRawWeight(fsrNum);

  if (totalWeight < baseWeights[fsrNum]) { return 0; }
  return totalWeight - baseWeights[fsrNum];

//  if (totalWeight < 1800) { return 0; }
//  if (totalWeight < 2150) { return 1; }
//  if (totalWeight < 2450) { return 2; }
//  if (totalWeight < 2550) { return 3; }
//  if (totalWeight < 2700) { return 4; }
//
//  return 0;
}

void initializeBaseWeights()
{
  for (int i = 0; i < 9; i++)
  {
    baseWeights[i] = getRawWeight(i);
  }
}

// Display an error message - The meaning of each error is documented in the manual
void displayError(int errorNumber)
{
  int currentTick = HAL_GetTick();
  while (HAL_GetTick() - currentTick < 2000)
  {
    flashDigit(TEAM1_DIGIT1_EN_GPIO_Port, TEAM1_DIGIT1_EN_Pin, 0b1001111); // E
    flashDigit(TEAM1_DIGIT2_EN_GPIO_Port, TEAM1_DIGIT2_EN_Pin, 0b0000101); // r
    flashDigit(TEAM1_DIGIT3_EN_GPIO_Port, TEAM1_DIGIT3_EN_Pin, 0b0000101); // r
    flashDigit(TEAM1_DIGIT4_EN_GPIO_Port, TEAM1_DIGIT4_EN_Pin, digitToHexDisplay(errorNumber));
  }
}

void beginGame()
{
  if (team1SetTargetScore > 0 && team2SetTargetScore > 0 && weightPerBag[0] > 0)
  {
    startRole = NONE;
    isSettingUp = false;
    currentTeam = 1;
    team1TargetScore = team1SetTargetScore;
    team1Score = team1TargetScore;
    team2TargetScore = team2SetTargetScore;
    team2Score = team2TargetScore;
    addAndRemoveScoreButtonsHeld = false;
    __HAL_TIM_SET_COUNTER(&htim3, 0);
  }
  else
  {
    // Display the error on team 1 score board
    int oldCurrentTeam = currentTeam;
    currentTeam = 1;
    if (team1SetTargetScore <= 0)
    {
      displayError(1);
    }
    else if (team2SetTargetScore <= 0)
    {
      displayError(2);
    }
    else if (weightPerBag[0] <= 0)
    {
      displayError(3);
    }
    currentTeam = oldCurrentTeam;
  }
}

int digitToHexDisplay(int digit) {
  switch (digit) {
      case 0: return 0b1111110;
      case 1: return 0b0110000;
      case 2: return 0b1101101;
      case 3: return 0b1111001;
      case 4: return 0b0110011;
      case 5: return 0b1011011;
      case 6: return 0b1011111;
      case 7: return 0b1110000;
      case 8: return 0b1111111;
      case 9: return 0b1111011;
      // If digit is out of range, turn off all segments
      default: return 0;
  }
}

void loadLatch(int data)
{
  if (currentTeam == 1)
  {
    for (int i = 0; i < 7; i++)
    {
      HAL_GPIO_WritePin(TEAM1_SWD_GPIO_Port, TEAM1_SWD_Pin, (GPIO_PinState) ((data >> i) & 1));
      HAL_GPIO_WritePin(TEAM1_SWCLK_GPIO_Port, TEAM1_SWCLK_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(TEAM1_SWCLK_GPIO_Port, TEAM1_SWCLK_Pin, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(TEAM1_LATCH_OUTPUT_GPIO_Port, TEAM1_LATCH_OUTPUT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TEAM1_LATCH_OUTPUT_GPIO_Port, TEAM1_LATCH_OUTPUT_Pin, GPIO_PIN_RESET);
  }
  else
  {
    for (int i = 0; i < 7; i++)
    {
      HAL_GPIO_WritePin(TEAM2_SWD_GPIO_Port, TEAM2_SWD_Pin, (GPIO_PinState) ((data >> i) & 1));
      HAL_GPIO_WritePin(TEAM2_SWCLK_GPIO_Port, TEAM2_SWCLK_Pin, GPIO_PIN_SET);
      HAL_GPIO_WritePin(TEAM2_SWCLK_GPIO_Port, TEAM2_SWCLK_Pin, GPIO_PIN_RESET);
    }
    HAL_GPIO_WritePin(TEAM2_LATCH_OUTPUT_GPIO_Port, TEAM2_LATCH_OUTPUT_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(TEAM2_LATCH_OUTPUT_GPIO_Port, TEAM2_LATCH_OUTPUT_Pin, GPIO_PIN_RESET);
  }
}

void flashDigit(GPIO_TypeDef* port, uint16_t pin, int data)
{
  loadLatch(data);
  HAL_GPIO_WritePin(port,pin, GPIO_PIN_SET);
  HAL_Delay(2);
  HAL_GPIO_WritePin(port,pin, GPIO_PIN_RESET);
}

void displayScore()
{
  if (currentTeam == 1)
  {
    int dig1 = (isSettingUp ? team1SetTargetScore : team1Score) / 1000;
    int dig2 = ((isSettingUp ? team1SetTargetScore : team1Score) / 100) % 10;
    int dig3 = ((isSettingUp ? team1SetTargetScore : team1Score) / 10) % 10;
    int dig4 = (isSettingUp ? team1SetTargetScore : team1Score) % 10;

    flashDigit(TEAM1_DIGIT1_EN_GPIO_Port, TEAM1_DIGIT1_EN_Pin, digitToHexDisplay(dig1));
    flashDigit(TEAM1_DIGIT2_EN_GPIO_Port, TEAM1_DIGIT2_EN_Pin, digitToHexDisplay(dig2));
    flashDigit(TEAM1_DIGIT3_EN_GPIO_Port, TEAM1_DIGIT3_EN_Pin, digitToHexDisplay(dig3));
    flashDigit(TEAM1_DIGIT4_EN_GPIO_Port, TEAM1_DIGIT4_EN_Pin, digitToHexDisplay(dig4));
  }
  else
  {
  int dig1 = (isSettingUp ? team2SetTargetScore : team2Score) / 1000;
  int dig2 = ((isSettingUp ? team2SetTargetScore : team2Score) / 100) % 10;
  int dig3 = ((isSettingUp ? team2SetTargetScore : team2Score) / 10) % 10;
  int dig4 = (isSettingUp ? team2SetTargetScore : team2Score) % 10;

    flashDigit(TEAM2_DIGIT1_EN_GPIO_Port, TEAM2_DIGIT1_EN_Pin, digitToHexDisplay(dig1));
    flashDigit(TEAM2_DIGIT2_EN_GPIO_Port, TEAM2_DIGIT2_EN_Pin, digitToHexDisplay(dig2));
    flashDigit(TEAM2_DIGIT3_EN_GPIO_Port, TEAM2_DIGIT3_EN_Pin, digitToHexDisplay(dig3));
    flashDigit(TEAM2_DIGIT4_EN_GPIO_Port, TEAM2_DIGIT4_EN_Pin, digitToHexDisplay(dig4));
  }
}

void blinkScore()
{
  if (currentTeam == 1)
  {
    HAL_GPIO_WritePin(TEAM1_DIGIT1_EN_GPIO_Port, TEAM1_DIGIT1_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM1_DIGIT2_EN_GPIO_Port, TEAM1_DIGIT2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM1_DIGIT3_EN_GPIO_Port, TEAM1_DIGIT3_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM1_DIGIT4_EN_GPIO_Port, TEAM1_DIGIT4_EN_Pin, GPIO_PIN_RESET);
  }
  else
  {
    HAL_GPIO_WritePin(TEAM2_DIGIT1_EN_GPIO_Port, TEAM2_DIGIT1_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM2_DIGIT2_EN_GPIO_Port, TEAM2_DIGIT2_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM2_DIGIT3_EN_GPIO_Port, TEAM2_DIGIT3_EN_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(TEAM2_DIGIT4_EN_GPIO_Port, TEAM2_DIGIT4_EN_Pin, GPIO_PIN_RESET);
  }

  int blinkTime = 500;
  HAL_Delay(blinkTime);
  int currentTick = HAL_GetTick();
  while (HAL_GetTick() - currentTick < blinkTime)
  {
    displayScore();
  }
}

// De-bounce the button
bool bounceFree(GPIO_TypeDef* port, uint16_t pin)
{
  uint32_t tickStart = HAL_GetTick();
  while (HAL_GPIO_ReadPin(port, pin) == GPIO_PIN_SET)
  {
    // If the button is not jumping for 50ms straight, it's good
    if ((HAL_GetTick() - tickStart) > 250)
    {
      return true;
    }
  }

  return false;
}

// GPIO Interrupt handler
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  switch (GPIO_Pin)
  {
    case START_RESET_BUTTON_Pin:
    {
      startButtonPressed = true;
      HAL_TIM_Base_Start_IT(&htim1);
      break;
    }
    case SELECT_TEAM_BUTTON_Pin:
    {
      selectTeamButtonPressed = true;
      break;
    }
    case ADD_SCORE_BUTTON_Pin:
    {
      addScoreButtonPressed = true;
      HAL_TIM_Base_Start_IT(&htim3);

//      if (HAL_GPIO_ReadPin(REMOVE_SCORE_BUTTON_GPIO_Port, REMOVE_SCORE_BUTTON_Pin) == GPIO_PIN_RESET)
//      {
//        HAL_TIM_Base_Start_IT(&htim3);
//      }
//      else
//      {
//        addScoreButtonPressed = true;
//      }
      break;
    }
    case REMOVE_SCORE_BUTTON_Pin:
    {
      if (HAL_GPIO_ReadPin(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin) == GPIO_PIN_RESET)
      {
        HAL_TIM_Base_Start_IT(&htim3);
      }
      else
      {
        removeScoreButtonPressed = true;
      }
      break;
    }
    default: break;
  }
}

// Timer interrupt handler
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if (htim->Instance == TIM1 &&
      HAL_GPIO_ReadPin(START_RESET_BUTTON_GPIO_Port, START_RESET_BUTTON_Pin) == GPIO_PIN_RESET)
  {
    startButtonHeld = true;
  }
  else if (htim->Instance == TIM3 &&
           HAL_GPIO_ReadPin(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin) == GPIO_PIN_RESET &&
           HAL_GPIO_ReadPin(REMOVE_SCORE_BUTTON_GPIO_Port, REMOVE_SCORE_BUTTON_Pin) == GPIO_PIN_RESET)
  {
    addAndRemoveScoreButtonsHeld = true;
  }
  else if (htim->Instance == TIM3 &&
      HAL_GPIO_ReadPin(ADD_SCORE_BUTTON_GPIO_Port, ADD_SCORE_BUTTON_Pin) == GPIO_PIN_RESET)
  {
    addAndRemoveScoreButtonsHeld = true;
  }
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
