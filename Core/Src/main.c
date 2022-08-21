//https://www.programmersought.com/article/21064906427/
//test dma rx with idle interrupt
//tx using normal uart_transmit
//http get with sim868 works

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
void check_resp(void);
void getLEN_restartDMA(void);
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
uint8_t op_name = 1;
/*
1:ATE0
2:AT+CGREG?
3:AT+SAPBR=3,1,"CONTYPE","GPRS"
4:AT+SAPBR=3,1,"APN","XOX"
5:AT+SAPBR=1,1
6:AT+HTTPINIT
7:AT+HTTPPARA="CID",1
8:AT+HTTPPARA="URL","<server ip address>/gpstracker/recv_data.php?name=Lucia&message=Cau"
9:AT+HTTPACTION=0
10:AT+HTTPREAD
11:AT+HTTPTERM 
12:AT+SAPBR=0,1
*/
uint8_t i;
uint8_t mytemp;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int _write(int file, char *ptr, int len)
{
  int i=0;
  for(i=0 ; i<len ; i++) 
    ITM_SendChar((*ptr++));
  return len;
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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
	
	HAL_UART_Receive_DMA(&huart1, UsartType.RX_pData, RX_LEN);  //Enable serial port DMA receiving interrupt
		
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {	
				
		if(UsartType.RX_flag)    	// Sign of received data
    {  
			UsartType.RX_flag=0;	//Clear the received flag
			//HAL_UART_Transmit(&huart2, UsartType.RX_pData, UsartType.RX_Size, 0xFFFF);
			
			if (op_name == 1) {
				if ( UsartType.RX_pData[7] == 0x4F && UsartType.RX_pData[8] == 0x4B ) {	//OK
					printf("echo is off\n");
					op_name = 2;
				} else if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) { //OK
					printf("echo already off\n");
					op_name = 2;
				} else {
					printf("echo error\n");
				}
						
			} else if (op_name == 2) {
				if ( UsartType.RX_pData[10] == 0x30 && UsartType.RX_pData[12] == 0x31 ) {	//+CGREG: 0,1
					printf("registered to network\n");
					op_name = 3;
				} else {
					printf("registration error\n");
				}
						
			} else if (op_name == 3) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("SAPBR CONTYPE GPRS ok\n");
					op_name = 4;
				} else {
					printf("SAPBR CONTYPE GPRS error\n");
					op_name = 11;	//need to HTTPTERM, SAPBR=0,1
				}
				
			} else if (op_name == 4) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("SAPBR APN OK\n");
					op_name = 5;
				} else {
					printf("SAPBR APN error\n");
					op_name = 11;	//need to HTTPTERM, SAPBR=0,1
				}
			
			} else if (op_name == 5) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("SAPBR 1,1 ok\n");
					op_name = 6;
				} else {
					printf("SAPBR 1,1 error\n");
					op_name = 11;	//need to HTTPTERM, SAPBR=0,1
				}
			
			} else if (op_name == 6) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("HTTPINIT ok\n");
					op_name = 7;
				} else {
					printf("HTTPINIT error\n");
				}
						
			} else if (op_name == 7) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("HTTPPARA CID 1 ok\n");
					op_name = 8;
				} else {
					printf("HTTPPARA CID 1 error\n");
				}
						
			} else if (op_name == 8) {
				check_resp();				
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//OK
					printf("HTTPPARA URL OK\n");
					op_name = 9;				
				} else {
					printf("HTTPPARA URL error\n");
				}
			
			} else if (op_name == 9) {
				check_resp();				
				if ( UsartType.RX_pData[23] == 0x32 && UsartType.RX_pData[24] == 0x30 && UsartType.RX_pData[25] == 0x30 ) {	//200
					printf("HTTPACTION=0 OK\n");
					op_name = 10;
				} else {
					printf("HTTPACTION=0 error\n");
					op_name = 11;
				}
			
			} else if (op_name == 10) {
				if ( UsartType.RX_pData[17] == 0x4C ) {	//Lucia says: Cau
					printf("AT+HTTPREAD ok\n");
					op_name = 11;
				} else {
					printf("AT+HTTPREAD error\n");
				}
						
			} else if (op_name == 11) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//ok
					printf("HTTPTERM ok\n");
					op_name = 12;
				} else {
					printf("HTTPTERM error\n");
				}
						
			} else if (op_name == 12) {
				if ( UsartType.RX_pData[2] == 0x4F && UsartType.RX_pData[3] == 0x4B ) {	//ok
					printf("SAPBR=0,1 ok\n");
					op_name = 1;
				} else {
					printf("SAPBR=0,1 error\n");
				}
			}
			
		}
		
		if(UsartType.RX_flag == 0) {
			if (op_name == 1) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"ATE0\r",sizeof("ATE0\r"), 50);
			}
			if (op_name == 2) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+CGREG?\r",sizeof("AT+CGREG?\r"), 50);
			}
			if (op_name == 3) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r",sizeof("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r"), 50);
			}
			if (op_name == 4) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+SAPBR=3,1,\"APN\",\"XOX\"\r",sizeof("AT+SAPBR=3,1,\"APN\",\"XOX\"\r"), 50);
			}
			if (op_name == 5) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+SAPBR=1,1\r",sizeof("AT+SAPBR=1,1\r"), 50);
			}
			if (op_name == 6) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPINIT\r",sizeof("AT+HTTPINIT\r"), 50);
			}			
			if (op_name == 7) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPPARA=\"CID\",1\r",sizeof("AT+HTTPPARA=\"CID\",1\r"), 50);
			}
			if (op_name == 8) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPPARA=\"URL\",\"<server ip address>/gpstracker/recv_data.php?name=Lucia&message=Cau\"\r",sizeof("AT+HTTPPARA=\"URL\",\"<server ip address>/gpstracker/recv_data.php?name=Lucia&message=Cau\"\r"), 50);
			}
			if (op_name == 9) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPACTION=0\r",sizeof("AT+HTTPACTION=0\r"), 50);
				HAL_Delay(2000);
			}
			if (op_name == 10) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPREAD\r",sizeof("AT+HTTPREAD\r"), 50);				
			}
			if (op_name == 11) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+HTTPTERM\r",sizeof("AT+HTTPTERM\r"), 50);				
			}
			if (op_name == 12) {
				HAL_UART_Transmit(&huart1, (uint8_t *)"AT+SAPBR=0,1\r",sizeof("AT+SAPBR=0,1\r"), 50);				
			}
    }
		UsartType.RX_flag = 1;
		HAL_Delay(500);
		getLEN_restartDMA();	//get length of recv msg and restart dma
		HAL_Delay(500);
		
		

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
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* USART1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART1_IRQn);
}

/* USER CODE BEGIN 4 */
void check_resp(void) {
	printf("resp : ");	
	for (i = 0; i < UsartType.RX_Size; ++i)	
  {
    printf("%02X", (uint8_t)(UsartType.RX_pData[i] & 0xFF));		//print uint8_t with 2 digits
  }
	printf("\n");
}

void getLEN_restartDMA(void) {
	HAL_UART_DMAStop(&huart1);  
	mytemp = huart1.hdmarx->Instance->CNDTR; //Get the remaining dma length 
	UsartType.RX_Size =  RX_LEN - mytemp;
	HAL_UART_Receive_DMA(&huart1,UsartType.RX_pData,RX_LEN);  
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
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
