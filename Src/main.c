/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stdlib.h"
#include "ili9341.h"
#include "ili9341_gfx.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
	#define LED_RED 		0
	#define LED_GREEN 	1
	#define LED_BLUE 		2

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
	ili9341_t *_screen;
	ili9341_text_attr_t my_attr;
	int x;
	
	// Systick
	volatile int flag_systick = 0;
	
	//LED
	volatile int led_color = LED_RED;
	
	// Ultrason
	volatile float distance_US = 0.0;
	
	// Key
	int key = 0;
	
	// UART
	unsigned Tx_data_word = 0xFE0000FF;
	uint8_t *Tx_data = (uint8_t *) &Tx_data_word;
	uint8_t Rx_data[4] = {0};
	uint8_t Rx_buff[1] = {0};
	uint8_t Rx_temp[2] = {0};
	int tx_timer = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
unsigned Update_Mes(unsigned * x);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
#ifndef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

void HAL_SYSTICK_Callback(void) {
	static int timer = 0;
	static int led_timer = 0;

	timer++;
	led_timer++;
	tx_timer++;
	
	// Sets flag when time elapsed
	if (timer == 500){
		flag_systick = 1;
		timer = 0;
	}
	
	// Change LED color when time elapsed
	if (led_timer > 2000){
		led_color = (led_color+1)%3;
		led_timer = 0;
	}
	
	// Update the message sent when time elapsed
	if (tx_timer == 1234){
		Update_Mes(&Tx_data_word);
		tx_timer = 0;
	}	
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	static int risingIC = 0;
	static int edge1Time = 0;
	static int edge2Time = 0;

	//First edge (Rising edge)
	if(risingIC == 0) 
	{
		edge1Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		risingIC = 1;
	}
	
	//Second edge (Falling edge)
	else
	{
		edge2Time = HAL_TIM_ReadCapturedValue(htim, TIM_CHANNEL_4);
		risingIC = 0;
		
		distance_US=abs(edge2Time-edge1Time)/58.82;
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	static int uart_state = 1;
	static int uart_cnt = 0;
	char c = *Rx_buff;
	
	HAL_UART_Receive_IT (&huart5, (uint8_t *) Rx_buff, sizeof(Rx_buff)); // Prepare la reception pour un prochain byte de data (mode normal)
	
	// Machine à état
	if (c==255) {
		uart_state = 1;
		uart_cnt = 0;
	} else {
		switch (uart_state){
			case 1:
				Rx_temp[uart_cnt] = c;
				uart_cnt++;
				if (uart_cnt == 2){
					uart_state = 2;
				}
				break;
			
			case 2:
				if (c == 254){
					Rx_data[1] = Rx_temp[0];
					Rx_data[2] = Rx_temp[1];
				}
				uart_state = 3;
				break;
				
			case 3:
				uart_state = 3;
				break;

		}
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == DETECT_Pin) 
	{
		if(HAL_GPIO_ReadPin(DETECT_GPIO_Port,DETECT_Pin)==GPIO_PIN_SET) 
		{
			HAL_TIM_PWM_Stop(&htim3, TIM_CHANNEL_4);
		}
		else
		{
			HAL_TIM_PWM_Start(&htim3, TIM_CHANNEL_4);
		}
	}
}

// Set a row for the Key
void selectRow (int r) {
	if (r==1) HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(R1_GPIO_Port, R1_Pin, GPIO_PIN_SET);
	if (r==2) HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(R2_GPIO_Port, R2_Pin, GPIO_PIN_SET);
	if (r==3) HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(R3_GPIO_Port, R3_Pin, GPIO_PIN_SET);
	if (r==4) HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, GPIO_PIN_RESET);
	else HAL_GPIO_WritePin(R4_GPIO_Port, R4_Pin, GPIO_PIN_SET);
}

// Read the column of the Key
int readCol() {
	int result = 0;
	if (HAL_GPIO_ReadPin(C1_GPIO_Port, C1_Pin) == GPIO_PIN_RESET) result += 1;
	if (HAL_GPIO_ReadPin(C2_GPIO_Port, C2_Pin) == GPIO_PIN_RESET) result += 2;
	if (HAL_GPIO_ReadPin(C3_GPIO_Port, C3_Pin) == GPIO_PIN_RESET) result += 4;
	if (HAL_GPIO_ReadPin(C4_GPIO_Port, C4_Pin) == GPIO_PIN_RESET) result += 8;
	return result;
}

// Look if a key is pressed
int keyPressed() {
	int rowPos=1; // Current row position
	int rowValue; // Value read from the current row
	for (rowPos=1; rowPos <= 4; rowPos++) {
		selectRow(rowPos); HAL_Delay(10);
		rowValue=readCol();
		if (rowValue != 0) { // a key is pressed
			int result = 4*(rowPos-1);
			while (!(rowValue & 1)) { // test if bit #0 is false
				result++;
				rowValue>>=1;
			}
			while (readCol() != 0); // key no more pressed
			return result+1;
		}
	}
	return -1;
}

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
  MX_USART2_UART_Init();
  MX_UART5_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
	
	// Ultrason
	HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
	HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_4);
	
	// UART
	HAL_UART_Transmit_DMA (&huart5, (uint8_t *) Tx_data, sizeof(Tx_data));
	HAL_UART_Receive_IT (&huart5, (uint8_t *) Rx_buff, sizeof(Rx_buff));
	
	// LCD
  _screen = ili9341_new(
      &hspi1,
      VOID_TFT_RESET_GPIO_Port, VOID_TFT_RESET_Pin,
      TFT_CS_GPIO_Port,    TFT_CS_Pin,
      TFT_DC_GPIO_Port,    TFT_DC_Pin,
      isoLandscape,
      NULL,  NULL,
      NULL, NULL,
      itsNotSupported,
      itnNormalized);
	
	ili9341_fill_screen(_screen, ILI9341_BLACK);	
	my_attr.font = &ili9341_font_16x26;
	my_attr.bg_color = ILI9341_BLACK;
	my_attr.fg_color = ILI9341_WHITE;
	my_attr.origin_x = 0;
	my_attr.origin_y = 0;
	
	
	char bufferTxt[10] = {0};
	sprintf(bufferTxt, "TESTBENCH");
	
	my_attr.origin_x = 30;
	my_attr.origin_y = 15;
	ili9341_draw_string(_screen, my_attr, bufferTxt);
	
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
		while(!flag_systick);
		flag_systick = 0;
		
		printf(".");
		
		// Key
		int keyTemp;
		if ((keyTemp = keyPressed()) != -1){
			key = keyTemp;
		}
		char bufferKey[10] = {0};
		sprintf(bufferKey, "Key : %i ", key);
		my_attr.origin_x = 30;
		my_attr.origin_y = 60;
		ili9341_draw_string(_screen, my_attr, bufferKey);
			
		// LED
		char bufferLED[15] = {0};
		switch (led_color){
			case LED_RED:
				sprintf(bufferLED, "LED : RED on  ");
				HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_SET);
				break;
			case LED_GREEN:
				sprintf(bufferLED, "LED : GREEN on");
				HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_RESET);
				HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_SET);
				break;
			case LED_BLUE:
				sprintf(bufferLED, "LED : BLUE on ");
				HAL_GPIO_WritePin(RED_GPIO_Port, RED_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(GREEN_GPIO_Port, GREEN_Pin, GPIO_PIN_SET);
				HAL_GPIO_WritePin(BLUE_GPIO_Port, BLUE_Pin, GPIO_PIN_RESET);
				break;
		}
		my_attr.origin_x = 30;
		my_attr.origin_y = 90;
		ili9341_draw_string(_screen, my_attr, bufferLED);
		
		// US
		char bufferUS[15] = {0};
		sprintf(bufferUS, "US : %i cm     ", (int) distance_US);
		
		my_attr.origin_x = 30;
		my_attr.origin_y = 120;
		ili9341_draw_string(_screen, my_attr, bufferUS);		
			
		// IR
		char bufferTX[20] = {0};
		char bufferRX[20] = {0};
		sprintf(bufferTX, "TX : %i %i       ",Tx_data[1],Tx_data[2]);
		sprintf(bufferRX, "RX : %i %i       ",Rx_data[1],Rx_data[2]);

		my_attr.origin_x = 30;
		my_attr.origin_y = 150;
		ili9341_draw_string(_screen, my_attr, bufferTX);

		my_attr.origin_x = 30;
		my_attr.origin_y = 180;
		ili9341_draw_string(_screen, my_attr, bufferRX);
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
/**
* @brief Retargets the C library printf function to the USART.
* @param None
* @retval None
*/
PUTCHAR_PROTOTYPE
{
/* Place your implementation of fputc here */
/* e.g. write a character to the USART2 and Loop until the end
of transmission */
HAL_UART_Transmit(&huart2, (uint8_t *)&ch, 1, 0xFFFF);
return ch;
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
