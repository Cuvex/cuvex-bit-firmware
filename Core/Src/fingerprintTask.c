/**
 ******************************************************************************
 * @file           : fingerprintTask.c
 * @brief          : Fingerprint task (freeRTOS)
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

#include "fingerprintTask.h"

static const fpc_cmd_callbacks_t cmd_cb = {
		.on_error = on_error,
		.on_status = on_status,
		.on_enroll = on_enroll,
		.on_identify = on_identify,
		.on_system_config_get = on_system_config_get,
		.on_data_transfer_done = on_data_transfer_done
};

struct flag
{
	bool device_ready;
	bool system_config_read;
	bool identify_done;
	bool identify_match;
	bool finger_detect;
	bool finger_lift;
	bool data_transfer_done;
};

struct fp_sensor
{
	uint8_t power_state;
	uint8_t app_state;
	uint8_t type_fp_action;
	uint16_t device_state;
	fpc_system_config_t system_config;
	struct flag flag;
};

struct fp_sensor fp_sensor = {0};

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void fingerprint_Task(void const * argument)
{
	fpc_result_t result;

	/*** Default struct values + delay ***/
	defaultStructValues();
	osDelay(250);

	/*** Main loop ***/
	for(;;)
	{
		/*** Process queue msg's ***/
		processMainToFpQueue();

		/*** State machine ***/
		switch(fp_sensor.power_state)
		{
		case FP_SENSOR_POWER_OFF:
		default:
			break;

			/*************************************************************************************************************************************************************************************************************************/
		case FP_SENSOR_POWER_ON:
			if(fpc_hal_data_available())	//Poll IRQ pin (non-blocking). This yields CPU to other tasks
			{
				result = fpc_host_sample_handle_rx_data();

				if((result != FPC_RESULT_OK) && (result != FPC_PENDING_OPERATION))
				{
#ifdef DEBUG_PRINTF_FINGERPRINT
					printf("Bad incoming data (%d)... Wait and try again in some sec...\r\n", result);
#endif
					fpc_hal_delay_ms(100);
					spi_host_rx_data_clear();   //Clear SPI buffer
				}
			}
			process_state();
			break;

			/*************************************************************************************************************************************************************************************************************************/
		case FP_SENSOR_ENABLE:
			HAL_GPIO_WritePin(HDAC_EN_GPIO_Port, HDAC_EN_Pin, GPIO_PIN_SET);	//Enable fingerprint sensor
			osDelay(100);														//Wait time
			fpc_host_sample_init((fpc_cmd_callbacks_t*) &cmd_cb);				//Initialize host sample with callbacks
			hal_reset_device();													//Reset device
			osDelay(100);														//Wait time
			fp_sensor.power_state = FP_SENSOR_POWER_ON;							//Change state
			fp_sensor.app_state = FP_APP_STATE_WAIT_READY;						//...
			break;

			/*************************************************************************************************************************************************************************************************************************/
		case FP_SENSOR_DISABLE:
			HAL_GPIO_WritePin(HDAC_EN_GPIO_Port, HDAC_EN_Pin, GPIO_PIN_RESET);	//Disable fingerprint sensor
			osDelay(100);														//Wait time
			defaultStructValues();
			break;
		}

		osDelay(10);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void defaultStructValues(void)
{
	fp_sensor.power_state = FP_SENSOR_POWER_OFF;
	fp_sensor.app_state = FP_APP_STATE_WAIT_READY;
	fp_sensor.type_fp_action = 0;
	fp_sensor.device_state = 0;
	fp_sensor.flag.device_ready = false;
	fp_sensor.flag.system_config_read = false;
	fp_sensor.flag.identify_done = false;
	fp_sensor.flag.identify_match = false;
	fp_sensor.flag.finger_detect = false;
	fp_sensor.flag.finger_lift = false;
	fp_sensor.flag.data_transfer_done = false;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void processMainToFpQueue(void)
{
	uint16_t rcv_data = 0;

	if((osMessageQueueGetCount(mainToFpQueueHandle) > 0) && (osMessageQueueGet(mainToFpQueueHandle, &rcv_data, 0, 0) == osOK))
	{
		switch(rcv_data)
		{
		default:
			fp_sensor.type_fp_action = 0;
			break;

		case MAIN_TO_FP_GET_TEMPLATE:
			fp_sensor.type_fp_action = 1;
			fp_sensor.power_state = FP_SENSOR_ENABLE;
			fp_sensor.app_state = FP_APP_STATE_WAIT_READY;
			break;

		case MAIN_TO_FP_CHECK_TEMPLATE:
			fp_sensor.type_fp_action = 2;
			fp_sensor.power_state = FP_SENSOR_ENABLE;
			fp_sensor.app_state = FP_APP_STATE_WAIT_READY;
			break;
		}
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void process_state(void)
{
	static bool enter = false;
	uint16_t next_state = fp_sensor.app_state;

	/*
	 * State machine
	 */
	switch(fp_sensor.app_state)
	{
	default:
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_READY:
		if(fp_sensor.flag.device_ready == true){
			enter = false;
			next_state = FP_APP_STATE_WAIT_GET_CONFIG_SETTINGS;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_GET_CONFIG_SETTINGS:
		if(enter == true){
			fp_sensor.flag.system_config_read = false;
			fpc_cmd_system_config_get_request(FPC_SYS_CFG_TYPE_CUSTOM);
		}

		if(fp_sensor.flag.system_config_read == true){
			next_state = FP_APP_STATE_WAIT_FACTORY_RESET_ENABLED;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_FACTORY_RESET_ENABLED:
		if(enter == true){
			fp_sensor.device_state = 0;
			fp_sensor.system_config.sys_flags |= CFG_SYS_FLAG_ALLOW_FACTORY_RESET;
			fpc_cmd_system_config_set_request(&fp_sensor.system_config);
		}

		if(fp_sensor.device_state != 0){
			next_state = FP_APP_STATE_WAIT_FACTORY_RESET;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_FACTORY_RESET:
		if(enter == true){
			fp_sensor.device_state = 0;
			fpc_cmd_factory_reset_request();
		}

		if(fp_sensor.device_state != 0){
			next_state = FP_APP_STATE_WAIT_SET_CONFIG_SETTINGS;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_SET_CONFIG_SETTINGS:
		if(enter == true){
			fp_sensor.device_state = 0;
			fp_sensor.system_config.sys_flags |= CFG_SYS_FLAG_ALLOW_FACTORY_RESET;
			fp_sensor.system_config.finger_scan_interval_ms = 250;
			fp_sensor.system_config.enroll_touches = ENROLL_NUM_SAMPLES;
			fp_sensor.system_config.idfy_max_consecutive_fails = 2;
			fpc_cmd_system_config_set_request(&fp_sensor.system_config);
		}

		if(fp_sensor.device_state != 0){
			next_state = FP_APP_STATE_WAIT_READY_AFTER_RESTART;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_READY_AFTER_RESTART:
		if(enter == true){
			fp_sensor.flag.device_ready = false;
			hal_reset_device();
		}

		if(fp_sensor.flag.device_ready == true){
			if(fp_sensor.type_fp_action == 1){
				next_state = FP_APP_STATE_WAIT_ENROLL;
			}
			else if(fp_sensor.type_fp_action == 2){
				next_state = FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_1;
			}
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
		/*************************************************************************************************************************************************************************************************************************/
		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_ENROLL:
		if(enter == true){
			fp_sensor.device_state |= STATE_ENROLL;
			fpc_id_type_t id_type = {ID_TYPE_GENERATE_NEW, 0};
			fpc_cmd_enroll_request(&id_type);
		}

		if((fp_sensor.device_state & (STATE_ENROLL | STATE_FINGER_DOWN)) == 0){
			next_state = FP_APP_STATE_WAIT_ENROLL_VERIFY;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_ENROLL_VERIFY:
		if(enter == true){
			fp_sensor.flag.finger_detect = false;
			fp_sensor.flag.identify_done = false;
			fp_sensor.flag.finger_lift = false;
			fpc_id_type_t id = {ID_TYPE_SPECIFIED, 1};
			fpc_cmd_identify_request(&id, 0);
		}

		if((fp_sensor.flag.finger_detect == true) && (fp_sensor.flag.identify_done == true) && (fp_sensor.flag.finger_lift == true))
		{
			if(fp_sensor.flag.identify_match == true){
				next_state = FP_APP_STATE_WAIT_ENROLL_GET_TEMPLATE;
			}
			else{
				next_state = FP_APP_STATE_WAIT_ENROLL_FAILED;
			}
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_ENROLL_GET_TEMPLATE:
		if(enter == true){
			fp_sensor.flag.data_transfer_done = false;
			fpc_cmd_get_template_data_request(1);
		}

		if(fp_sensor.flag.data_transfer_done == true){
			next_state = FP_APP_STATE_WAIT_ENROLL_SUCCESS;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_ENROLL_FAILED:
		if(enter == true){
			enrollFail();
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_ENROLL_SUCCESS:
		if(enter == true){
			enrollSuccess();
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
		/*************************************************************************************************************************************************************************************************************************/
		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_1:
		if(enter == true){
			fp_sensor.flag.data_transfer_done = false;
			fpc_cmd_put_template_data_request(1, cuvex.fingerprint.signer[cuvex.fingerprint.id_user - 1].template_1, SIZE_TEMPLATE_EXT0);
		}

		if(fp_sensor.flag.data_transfer_done == true){
			next_state = FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_2;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_CHECK_PUT_TEMPLATE_2:
		if(enter == true){
			fp_sensor.flag.data_transfer_done = false;
			fpc_cmd_put_template_data_request(2, cuvex.fingerprint.signer[cuvex.fingerprint.id_user - 1].template_2, SIZE_TEMPLATE_EXT0);
		}

		if(fp_sensor.flag.data_transfer_done == true){
			next_state = FP_APP_STATE_WAIT_CHECK_VERIFY;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_CHECK_VERIFY:
		if(enter == true){
			fp_sensor.flag.device_ready = false;
			fp_sensor.flag.finger_detect = false;
			fp_sensor.flag.identify_done = false;
			fp_sensor.flag.finger_lift = false;
			fpc_id_type_t id = {ID_TYPE_ALL, 0};
			fpc_cmd_identify_request(&id, 0);
		}

		if((fp_sensor.flag.device_ready == true) && (fp_sensor.flag.finger_detect == true) && (fp_sensor.flag.identify_done == true) /*&& (fp_sensor.flag.finger_lift == true)*/){
			if(fp_sensor.flag.identify_match == true){
				next_state = FP_APP_STATE_WAIT_CHECK_SUCCESS;
			}
			else{
				next_state = FP_APP_STATE_WAIT_CHECK_FAILED;
			}
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_CHECK_FAILED:
		if(enter == true){
			checkFail();
			osDelay(500);
		}

		if(fp_sensor.flag.finger_lift == true){
			next_state = FP_APP_STATE_WAIT_CHECK_VERIFY;
		}
		break;

		/*************************************************************************************************************************************************************************************************************************/
	case FP_APP_STATE_WAIT_CHECK_SUCCESS:
		if(enter == true){
			checkSuccess();
			osDelay(500);
		}
		break;
	}

	/*
	 * Next state
	 */
	if(next_state != fp_sensor.app_state)
	{
#ifdef DEBUG_PRINTF_FINGERPRINT
		printf("\r\n##################### State transition %d -> %d #####################\r\n\r\n", fp_sensor.app_state, next_state);
#endif
		fp_sensor.app_state = next_state;
		enter = true;
	}
	else{
		enter = false;
	}
}

/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_error(uint16_t error)
{
	generalError();

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("[Callback_on_error] - Error (%d)\r\n", error);
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_status(uint16_t event, uint16_t state)
{
	if(state & STATE_APP_FW_READY){
		fp_sensor.flag.device_ready = true;
	}

	if(event == EVENT_FINGER_DETECT){
		fp_sensor.flag.finger_detect = true;
	}

	if(event == EVENT_FINGER_LOST){
		fp_sensor.flag.finger_lift = true;
	}

	fp_sensor.device_state = state;

#ifdef DEBUG_PRINTF_FINGERPRINT
	if(state & STATE_APP_FW_READY){
		printf("[Callback_on_status] - Device ready\r\n");
	}
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_enroll(uint8_t feedback, uint8_t samples_remaining)
{
	extern char *get_enroll_feedback_str_(uint8_t feedback);
	cuvex.fingerprint.enroll_samples_remaining = samples_remaining;

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("[Callback_on_enroll] - Enroll samples remaining: %d, feedback: %s (%d)\r\n", samples_remaining, get_enroll_feedback_str_(feedback), feedback);
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_identify(int is_match, uint16_t id)
{
	fp_sensor.flag.identify_match = is_match ? true : false;
	fp_sensor.flag.identify_done = true;

#ifdef DEBUG_PRINTF_FINGERPRINT
	if(is_match){
		printf("[Callback_on_identify] - Identify match on id #%d\r\n", id);
	}
	else{
		printf("[Callback_on_identify] - Identify no match\r\n");
	}
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_system_config_get(fpc_system_config_t *cfg)
{
	fp_sensor.system_config = *cfg;
	fp_sensor.flag.system_config_read = true;

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("[Callback_on_system_config_get]\r\n");
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void on_data_transfer_done(uint8_t *data, size_t size)
{
	if(data && size)
	{
		uint8_t idx = cuvex.fingerprint.id_user - 1;

		if(cuvex.fingerprint.id_template == 1){
			memset(cuvex.fingerprint.signer[idx].template_1, 0x00, sizeof(cuvex.fingerprint.signer[idx].template_1));
			memcpy(cuvex.fingerprint.signer[idx].template_1,  data, size);
		}
		else if(cuvex.fingerprint.id_template == 2){
			memset(cuvex.fingerprint.signer[idx].template_2, 0x00, sizeof(cuvex.fingerprint.signer[idx].template_2));
			memcpy(cuvex.fingerprint.signer[idx].template_2,  data, size);
		}
	}

	fp_sensor.flag.data_transfer_done = true;

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("[on_data_transfer_done] - size = %u\r\n", (unsigned) size);
#endif
}

/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/
/*************************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void enrollSuccess(void)
{
	fp_sensor.type_fp_action = 0;
	fp_sensor.power_state = FP_SENSOR_DISABLE;

	if(osMessageQueueGetSpace(fpToMainQueueHandle) > 0){
		osMessageQueuePut(fpToMainQueueHandle, (void*)&(int){FP_TO_MAIN_GET_TEMPLATE_SUCCESS}, 0, 0);
	}

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("\r\n\r\n------ ENROLL SUCCESS ------\r\n\r\n");
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void enrollFail(void)
{
	fp_sensor.type_fp_action = 0;
	fp_sensor.power_state = FP_SENSOR_DISABLE;

	if(osMessageQueueGetSpace(fpToMainQueueHandle) > 0){
		osMessageQueuePut(fpToMainQueueHandle, (void*)&(int){FP_TO_MAIN_GET_TEMPLATE_ERROR}, 0, 0);
	}

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("\r\n\r\n------ ENROLL FAIL ------\r\n\r\n");
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void checkSuccess(void)
{
	fp_sensor.type_fp_action = 0;
	fp_sensor.power_state = FP_SENSOR_DISABLE;

	if(osMessageQueueGetSpace(fpToMainQueueHandle) > 0){
		osMessageQueuePut(fpToMainQueueHandle, (void*)&(int){FP_TO_MAIN_CHECK_TEMPLATE_SUCCESS}, 0, 0);
	}

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("\r\n\r\n------ CHECK SUCCESS ------\r\n\r\n");
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void checkFail(void)
{
	if(osMessageQueueGetSpace(fpToMainQueueHandle) > 0){
		osMessageQueuePut(fpToMainQueueHandle, (void*)&(int){FP_TO_MAIN_CHECK_TEMPLATE_ERROR}, 0, 0);
	}

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("\r\n\r\n------ CHECK FAIL ------\r\n\r\n");
#endif
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void generalError(void)
{
	fp_sensor.type_fp_action = 0;
	fp_sensor.power_state = FP_SENSOR_DISABLE;

	if(osMessageQueueGetSpace(fpToMainQueueHandle) > 0){
		osMessageQueuePut(fpToMainQueueHandle, (void*)&(int){FP_TO_MAIN_GENERAL_ERROR}, 0, 0);
	}

#ifdef DEBUG_PRINTF_FINGERPRINT
	printf("\r\n\r\n------ GENERAL ERROR ------\r\n\r\n");
#endif
}

