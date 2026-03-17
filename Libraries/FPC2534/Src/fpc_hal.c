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
 * @file    fpc_hal.c
 * @brief   Implementation of the fpc_hal API.
 */

#include <stdarg.h>
#include <unistd.h>
#include <string.h>
#include "main.h"
#include "spi_host.h"
#include "hal_common.h"
#include "fpc_api.h"
#include "fpc_hal.h"

extern void SystemClock_Config(void);

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
fpc_result_t fpc_hal_init(void)
{
	hal_set_if_config(HAL_IF_CONFIG_SPI);

	return FPC_RESULT_OK;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
fpc_result_t fpc_hal_tx(uint8_t *data, size_t len, uint32_t timeout, int flush)
{
	int rc = spi_host_transmit(data, len, timeout, flush);

	return rc == 0 ? FPC_RESULT_OK : FPC_RESULT_FAILURE;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
fpc_result_t fpc_hal_rx(uint8_t *data, size_t len, uint32_t timeout, int keep_cs_low)
{
	int rc = spi_host_receive(data, len, timeout, keep_cs_low);

	return rc == 0 ? FPC_RESULT_OK : FPC_RESULT_FAILURE;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
int fpc_hal_data_available(void)
{
	return spi_host_rx_data_available();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
fpc_result_t fpc_hal_wfi(void)
{
	__WFI();
	return FPC_RESULT_OK;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void fpc_hal_delay_ms(uint32_t ms)
{
	//HAL_Delay(ms);
	osDelay(ms);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void fpc_sample_logf(const char *format, ...)
{
#ifdef DEBUG_PRINTF_FINGERPRINT
	va_list arglist;

	va_start(arglist, format);
	vprintf(format, arglist);
	va_end(arglist);

	putchar('\r');
	putchar('\n');
#endif
}
