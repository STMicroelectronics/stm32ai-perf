/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : st_port.c
  * @brief          : Helper routines
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


#include"st_port.h"
#include "internally_implemented.h"
#include "submitter_implemented.h"

static char eembc_char=0;
uint8_t pool0[AI_NETWORK_DATA_ACTIVATION_1_SIZE];
ai_handle network ;

ai_buffer* ai_input;
ai_buffer* ai_output;

float Deq(int8_t value, float scale, int8_t zero_point)
{
	float re =0.0;
	re = (float)((value - zero_point) * scale);
  return re;
}

int8_t QuantizeFloatToInt8(float value, float scale, int zero_point)
{
	//int8_t re=0;
  int8_t result = round(value / scale) + zero_point;
  if (result < INT8_MIN)
  {
    result = INT8_MIN;
  }
  if (result > INT8_MAX)
  {
    result = INT8_MAX;
  }
  //re =(int8_t)result;
  return result;
}

//=============================================== AI  INIT ================================================
ai_handle act_addr[] = {pool0};
void AI_Init()
{
  /* Create and initialize an instance of the model */

  ai_network_create_and_init(&network, act_addr, NULL);

  ai_input = ai_network_inputs_get(network, NULL);
  ai_output = ai_network_outputs_get(network, NULL);
}

void AI_Run(int8_t *pIn, int8_t *pOut)
{

  ai_input[0].data = AI_HANDLE_PTR(pIn);
  ai_output[0].data = AI_HANDLE_PTR(pOut);
  ai_network_run(network, ai_input, ai_output);
}
//============================================================================================================

void US_Delay(uint8_t us) // microseconds
{
    int startTick = __HAL_TIM_GET_COUNTER(&htim5);
    while (__HAL_TIM_GET_COUNTER(&htim5) - startTick < us);
}

void clear_Timestamp_Port(void)
{
    __HAL_RCC_GPIOF_CLK_ENABLE();
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);
}
void set_Timestamp_Port(void)
{
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
    __HAL_RCC_GPIOF_CLK_DISABLE();
}

void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */

  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART3_IRQn 1 */
  #if EE_CFG_ENERGY_MODE == 0
  	  HAL_UART_Receive_IT(&huart1, (uint8_t *)&eembc_char, 1);
  	  ee_serial_callback(eembc_char);
  #endif
  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles LPUART1 global interrupt.
  */
void LPUART1_IRQHandler(void)
{
  /* USER CODE BEGIN LPUART1_IRQn 0 */

  /* USER CODE END LPUART1_IRQn 0 */
  HAL_UART_IRQHandler(&hlpuart1);
  /* USER CODE BEGIN LPUART1_IRQn 1 */
  #if EE_CFG_ENERGY_MODE == 1
	  HAL_UART_Receive_IT(&hlpuart1, (uint8_t *)&eembc_char, 1);
	  ee_serial_callback(eembc_char);
  #endif
  /* USER CODE END LPUART1_IRQn 1 */
}

void rxSetInterrupt(uint8_t *pData)
{
#if EE_CFG_ENERGY_MODE == 0
	if (HAL_UART_Receive_IT(&huart1, pData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#else
	if (HAL_UART_Receive_IT(&hlpuart1, pData, 1) != HAL_OK)
	{
		Error_Handler();
	}
#endif
}
void rxInit(void)
{
	rxSetInterrupt((uint8_t *)&eembc_char);
}

/*
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *UartHandle)
{
	if (UartHandle->Instance == USART2)
	{
		ee_serial_callback(g_rxByte);
		rxSetInterrupt((uint8_t *)&g_rxByte);
	}
}
*/
