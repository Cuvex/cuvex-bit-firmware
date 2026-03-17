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

#include "stm32u5xx_hal.h"
#include "main.h"
#include "spi_host.h"
#include <stdbool.h>
#include <string.h>
#include <stdint.h>

#define MIN_DELAY_BETWEEN_CS_LOW_AND_SPI_CLK 500
#define hspi_fpc2530 hspi1

extern SPI_HandleTypeDef hspi1;

volatile bool fpc2530_irq_active = false;
volatile bool fpc2530_spi_sensor_rx_tx_done = false;

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
static void delay_us(uint32_t us)
{
    /**
     * Each loop takes 6 clockcycles on a Cortex M4.
     * Calculate number of loops based on cycles in loop and CPU frequency.
     */
    uint32_t loops = SystemCoreClock / 1000000 * us / 6;

    while(loops--) {
        __asm__("nop");
    }
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void spi_host_init(void)
{

}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void spi_host_reset(void)
{

}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void spi_host_disable(void)
{

}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint32_t spi_host_rx_data_available()
{
    uint32_t active = fpc2530_irq_active;
    fpc2530_irq_active = false;
    return active;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void spi_host_rx_data_clear()
{

}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
int spi_host_transmit(uint8_t *data, size_t size, uint32_t timeout, int flush)
{
    HAL_SPI_StateTypeDef spi_state;
    HAL_StatusTypeDef status = HAL_ERROR;

    do {
        spi_state = HAL_SPI_GetState(&hspi_fpc2530);
        if (spi_state == HAL_SPI_STATE_ERROR) {
            goto exit;
        }
    } while (spi_state != HAL_SPI_STATE_READY);

    HAL_GPIO_WritePin(HDAC_CS_N_GPIO_Port, HDAC_CS_N_Pin, GPIO_PIN_RESET);
    fpc2530_spi_sensor_rx_tx_done = false;
    // Delay here to let device wakeup and start SPI block before starting transfer
    delay_us(MIN_DELAY_BETWEEN_CS_LOW_AND_SPI_CLK);

#if 1
    do {
        status = HAL_SPI_Transmit(&hspi_fpc2530, data, size, timeout);
        if (status == HAL_ERROR) {
            goto restore;
        }
    } while (status == HAL_BUSY);
#else
    status = HAL_SPI_TransmitReceive_DMA(&hspi_sensor, data, data, write_size + read_size);
    if (status != HAL_OK) {
        goto restore;
    }
    while (!fpc_spi_sensor_rx_tx_done) {
    }
#endif

restore:
    if (flush) {
        HAL_GPIO_WritePin(HDAC_CS_N_GPIO_Port, HDAC_CS_N_Pin, GPIO_PIN_SET);
    }

exit:

    return (status == HAL_OK) ? 0 : -1;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
int spi_host_receive(uint8_t *data, size_t size, uint32_t timeout, int keep_cs_low)
{
    HAL_StatusTypeDef status = HAL_ERROR;

    HAL_GPIO_WritePin(HDAC_CS_N_GPIO_Port, HDAC_CS_N_Pin, GPIO_PIN_RESET);
    // Delay here to let device wakeup and start SPI block before starting transfer
    delay_us(MIN_DELAY_BETWEEN_CS_LOW_AND_SPI_CLK);

    do {
        status = HAL_SPI_Receive(&hspi_fpc2530, data, size, timeout);
        if (status == HAL_ERROR) {
            goto restore;
        }
    } while (status == HAL_BUSY);

restore:
    if (!keep_cs_low || status != HAL_OK) {
        HAL_GPIO_WritePin(HDAC_CS_N_GPIO_Port, HDAC_CS_N_Pin, GPIO_PIN_SET);
    }

    return (status == HAL_OK) ? 0 : -1;
}


