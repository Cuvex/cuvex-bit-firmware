/**
 ******************************************************************************
 * @file           : fingerprintTask.h
 * @brief          : Header for fingerprintTask.c file.
 *                   This file contains the common defines of the fingerprintTask.
 ******************************************************************************
 * @attention
 *
 * Portion Copyright (C) 2024 Semilla3 OÜ.  All Rights Reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#ifndef INC_FINGERPRINTTASK_H_
#define INC_FINGERPRINTTASK_H_

/********************************************************************** Include's **********************************************************************/
#include "stm32u5xx_hal.h"
#include "main.h"
#include "hal_common.h"
#include "fpc_api.h"
#include "fpc_hal.h"
#include "fpc_host_sample.h"
#include "spi_host.h"
#include <stdarg.h>
#include <stdio.h>

/********************************************************************* Functions's *********************************************************************/
void fingerprint_Task(void const * argument);
void defaultStructValues(void);
void processMainToFpQueue(void);
void process_state(void);
void on_error(uint16_t error);
void on_status(uint16_t event, uint16_t state);
//void on_version(char* version);
void on_enroll(uint8_t feedback, uint8_t samples_remaining);
void on_identify(int is_match, uint16_t id);
//void on_list_templates(int num_templates, uint16_t *template_ids);
void on_system_config_get(fpc_system_config_t *cfg);
void on_data_transfer_done(uint8_t *data, size_t size);

void enrollSuccess(void);
void enrollFail(void);
void checkSuccess(void);
void checkFail(void);
void generalError(void);

/********************************************************************* Enum's **********************************************************************/

enum fp_sensor_power_state
{
	FP_SENSOR_POWER_OFF = 0,
	FP_SENSOR_POWER_ON,
	FP_SENSOR_ENABLE,
	FP_SENSOR_DISABLE
};

enum fp_app_state
{
	FP_APP_STATE_WAIT_READY = 0,				//Wait for first ready
	FP_APP_STATE_WAIT_GET_CONFIG_SETTINGS,		//Force new configuration after factroy reset
	FP_APP_STATE_WAIT_FACTORY_RESET_ENABLED,	//...
	FP_APP_STATE_WAIT_FACTORY_RESET,			//...
	FP_APP_STATE_WAIT_SET_CONFIG_SETTINGS,		//...
	FP_APP_STATE_WAIT_READY_AFTER_RESTART,		//Wait for second ready
	FP_APP_STATE_WAIT_ENROLL,					//ENROLL --> Enroll + verify + export template + result (succes, fail)
	FP_APP_STATE_WAIT_ENROLL_VERIFY,			//...
	FP_APP_STATE_WAIT_ENROLL_GET_TEMPLATE,		//...
	FP_APP_STATE_WAIT_ENROLL_FAILED,			//...
	FP_APP_STATE_WAIT_ENROLL_SUCCESS,			//...
	FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_1,		//CHECK --> Import templates + verify + result (succes, fail)
	FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_2,		//...
	FP_APP_STATE_WAIT_CHECK_VERIFY,				//...
	FP_APP_STATE_WAIT_CHECK_FAILED,				//...
	FP_APP_STATE_WAIT_CHECK_SUCCESS				//...
};


/********************************************************************** Extern's ***********************************************************************/
extern osMessageQueueId_t fpToMainQueueHandle;
extern osMessageQueueId_t mainToFpQueueHandle;
extern struct cuvex cuvex;

#endif
