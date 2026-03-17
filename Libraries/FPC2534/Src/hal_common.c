/*
 * Copyright (c) 2024 Fingerprint Cards AB
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *   https://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */


/**
 * @file    hal_common.c
 * @brief   Implementation of common hal functions.
 */

#include "stm32u5xx_hal.h"
#include "main.h"
#include "hal_common.h"
#include <unistd.h>
#include <string.h>
#include <stdbool.h>

extern volatile bool fpc2530_irq_active;

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void HAL_GPIO_EXTI_Rising_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == HDAC_IRQ_Pin)
	{
		fpc2530_irq_active = true;
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void HAL_GPIO_EXTI_Falling_Callback(uint16_t GPIO_Pin)
{

}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void hal_reset_device(void)
{
	HAL_GPIO_WritePin(HDAC_RST_N_GPIO_Port, HDAC_RST_N_Pin, GPIO_PIN_RESET);
	HAL_Delay(10);
	HAL_GPIO_WritePin(HDAC_RST_N_GPIO_Port, HDAC_RST_N_Pin, GPIO_PIN_SET);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void hal_set_if_config(hal_if_config_t config)
{
	switch(config)
	{
	case HAL_IF_CONFIG_UART:
		HAL_GPIO_WritePin(HDAC_CONFIG_GPIO_Port, HDAC_CONFIG_Pin, GPIO_PIN_SET);
		break;

	case HAL_IF_CONFIG_SPI:
	default:
		HAL_GPIO_WritePin(HDAC_CONFIG_GPIO_Port, HDAC_CONFIG_Pin, GPIO_PIN_RESET);
		break;
	}
}
