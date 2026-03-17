/**
 ******************************************************************************
 * @file           : mainTask.c
 * @brief          : Main task (freeRTOS)
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

#include "mainTask.h"
#include "ramdisk_fatfs.h"

USBD_HandleTypeDef USBD_Device;
uint8_t MSC_EpAdd_Inst[2] = {MSC_EPIN_ADDR, MSC_EPOUT_ADDR};

bool flag_wait_for_files_1 = false, flag_wait_for_files_2 = false, flag_wait_for_files_3 = false;

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void main_Task(void const * argument)
{
	/*** Initialize struct "cuvex" to default values (part 1) ***/
	clearCuvexStruct_1();

	/*** Read specific memory sectors ***/
	readFlash(DEV_ALIAS_ADDR, cuvex.device_alias_buffer, DEV_ALIAS_SIZE);
	readFlash(EEPROM_ADDR, cuvex.eeprom_buffer, EEPROM_SIZE);
	readFlash(SIGNATURE_ADDR, cuvex.signature_buffer, SIGNATURE_SIZE);

	/*** Load data from EEPROM into RAM (init EEPROM, if not data to load) ***/
	if(memcmp(cuvex.eeprom_buffer + 96, ">>>>>Footer<<<<<", 16) != 0){
		initEEPROM();
	}
	loadEEPROM();

	/*** Enable LCD + Initialize struct "cuvex" to default values (part 2) + delay  ***/
	HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET);
	clearCuvexStruct_2();
	osDelay(250);

	/*** Main loop ***/
	for(;;)
	{
		processGuiToMainQueue();
		processFpToMainQueue();
		stateMachineScreens();
		osDelay(10);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void processGuiToMainQueue(void)
{
	uint16_t rcv_data_gui = 0;

	if((osMessageQueueGetCount(guiToMainQueueHandle) > 0) && (osMessageQueueGet(guiToMainQueueHandle, &rcv_data_gui, 0, 0) == osOK))
	{
		switch(rcv_data_gui)
		{
		default:
			break;

			/***********************************************************************************************************/
		case GUI_TO_MAIN_SCREEN_INIT:
			cuvex.screen = SCREEN_INIT;
			clearCuvexStruct_2();
			break;

		case GUI_TO_MAIN_SCREEN_MAIN_MENU:
			cuvex.screen = SCREEN_MAIN_MENU;
			clearCuvexStruct_2();
			break;

		case GUI_TO_MAIN_SCREEN_FLOW_ENCRYPT:
			cuvex.screen = SCREEN_FLOW_ENCRYPT;
			break;

		case GUI_TO_MAIN_SCREEN_FLOW_DECRYPT:
			cuvex.screen = SCREEN_FLOW_DECRYPT;
			break;

		case GUI_TO_MAIN_SCREEN_FLOW_WALLET:
			cuvex.screen = SCREEN_FLOW_WALLET;
			break;

		case GUI_TO_MAIN_SCREEN_FLOW_PSBT:
			cuvex.screen = SCREEN_FLOW_PSBT;
			break;

		case GUI_TO_MAIN_SCREEN_FLOW_SETTINGS:
			cuvex.screen = SCREEN_FLOW_SETTINGS;
			break;

			/***********************************************************************************************************/
		case GUI_TO_MAIN_FLOW_ENCRYPT_VOLUME_MOUNT:
			flowEncryptMountVolume();
			break;

		case GUI_TO_MAIN_FLOW_DECRYPT_VOLUME_MOUNT:
			flowDecryptMountVolume();
			break;

		case GUI_TO_MAIN_FLOW_PSBT_VOLUME_MOUNT_1:
			flowPsbtMountVolume(1);
			break;

		case GUI_TO_MAIN_FLOW_PSBT_VOLUME_MOUNT_2:
			flowPsbtMountVolume(2);
			break;

			/***********************************************************************************************************/
		case GUI_TO_MAIN_FLASH_SAVE_SETTINGS:
			editEEPROM();
			break;

		case GUI_TO_MAIN_FLASH_SAVE_SETTINGS_AND_DEVICE_ALIAS:
			editEEPROM();
			saveDeviceAlias();
			break;

		case GUI_TO_MAIN_FLASH_ERASE_SIGNATURE:
			eraseSignature();
			break;

			/***********************************************************************************************************/
		case GUI_TO_MAIN_GET_TEMPLATE:
			if(osMessageQueueGetSpace(mainToFpQueueHandle) > 0){
				osMessageQueuePut(mainToFpQueueHandle, (void*)&(int){MAIN_TO_FP_GET_TEMPLATE}, 0, 0);
			}
			break;

		case GUI_TO_MAIN_CHECK_TEMPLATE:
			if(osMessageQueueGetSpace(mainToFpQueueHandle) > 0){
				osMessageQueuePut(mainToFpQueueHandle, (void*)&(int){MAIN_TO_FP_CHECK_TEMPLATE}, 0, 0);
			}
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
void processFpToMainQueue(void)
{
	uint16_t rcv_data_fp = 0;

	if((osMessageQueueGetCount(fpToMainQueueHandle) > 0) && (osMessageQueueGet(fpToMainQueueHandle, &rcv_data_fp, 0, 0) == osOK))
	{
		switch(rcv_data_fp)
		{
		default:
			break;

			/***********************************************************************************************************/
		case FP_TO_MAIN_GET_TEMPLATE_SUCCESS:
			if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
				osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_GET_TEMPLATE_SUCCESS}, 0, 0);
			}
			break;

			/***********************************************************************************************************/
		case FP_TO_MAIN_GET_TEMPLATE_ERROR:
			if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
				osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_GET_TEMPLATE_ERROR}, 0, 0);
			}
			break;

			/***********************************************************************************************************/
		case FP_TO_MAIN_CHECK_TEMPLATE_SUCCESS:
			if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
				osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_CHECK_TEMPLATE_SUCCESS}, 0, 0);
			}
			break;

			/***********************************************************************************************************/
		case FP_TO_MAIN_CHECK_TEMPLATE_ERROR:
			if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
				osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_CHECK_TEMPLATE_ERROR}, 0, 0);
			}
			break;

			/***********************************************************************************************************/
		case FP_TO_MAIN_GENERAL_ERROR:
			if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
				osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_GENERAL_ERROR}, 0, 0);
			}
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
void stateMachineScreens(void)
{
	switch(cuvex.screen)
	{
	case SCREEN_INIT:
	case SCREEN_MAIN_MENU:
	case SCREEN_FLOW_ENCRYPT:
	case SCREEN_FLOW_WALLET:
	case SCREEN_FLOW_SETTINGS:
	default:
		break;

	case SCREEN_FLOW_DECRYPT:
		flowDecryptReadFiles();
		break;

	case SCREEN_FLOW_PSBT:
		flowPsbtReadFiles();
		break;
	}
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void clearCuvexStruct_1(void)
{
	memset(&cuvex.screen, 0x00, sizeof(cuvex.screen));								//Clear struct "cuvex" root
	memset(&cuvex.device_alias_buffer, 0x00, sizeof(cuvex.device_alias_buffer));	//...
	memset(&cuvex.eeprom_buffer, 0x00, sizeof(cuvex.eeprom_buffer));				//...
	memset(&cuvex.signature_buffer, 0x00, sizeof(cuvex.signature_buffer));			//...
	memset(&cuvex.info, 0x00, sizeof(cuvex.info));									//Clear struct "cuvex.info"
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void clearCuvexStruct_2(void)
{
	memset(&cuvex.tag, 0x00, sizeof(cuvex.tag));					//Clear struct "cuvex.tag"
	memset(&cuvex.psbt, 0x00, sizeof(cuvex.psbt));					//Clear struct "cuvex.psbt"
	memset(&cuvex.encrypt, 0x00, sizeof(cuvex.encrypt));			//Clear struct "cuvex.encrypt"
	memset(&cuvex.decrypt, 0x00, sizeof(cuvex.decrypt));			//Clear struct "cuvex.decrypt"
	memset(&cuvex.wallet, 0x00, sizeof(cuvex.wallet));				//Clear struct "cuvex.wallet"
	memset(&cuvex.fingerprint, 0x00, sizeof(cuvex.fingerprint));	//Clear struct "cuvex.fingerprint"
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void readFlash(uint32_t address, char* data, size_t data_size)
{
	uint32_t i=0;

	for(i = 0; i < data_size; i+=1){
		data[i] = *((char*)address+i);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void writeFlash(uint32_t address, char* data, size_t data_size)
{
	uint32_t *data_p = (uint32_t*)data;

	HAL_FLASH_Unlock();
	FLASH_WaitForLastOperation(100);

	for(int i=0; i<data_size; i+=16, data_p+=4, address+=16){
		if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD, address, (uint32_t)data_p) != HAL_OK){
			NVIC_SystemReset();
		}
	}

	FLASH_WaitForLastOperation(100);
	HAL_FLASH_Lock();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void eraseFlash(uint32_t Banks, uint32_t Page, uint32_t NbPages)
{
	FLASH_EraseInitTypeDef EraseInitStruct;
	uint32_t PageError;

	HAL_FLASH_Unlock();

	FLASH_WaitForLastOperation(100);

	EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
	EraseInitStruct.Banks       = Banks;
	EraseInitStruct.Page        = Page;
	EraseInitStruct.NbPages     = NbPages;

	if(HAL_FLASHEx_Erase(&EraseInitStruct, &PageError) != HAL_OK){
		NVIC_SystemReset();
	}

	FLASH_WaitForLastOperation(100);
	HAL_FLASH_Lock();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void initEEPROM(void)
{
	/*** Buffer reset ***/
	memset(cuvex.eeprom_buffer, 0x00, EEPROM_SIZE);

	/*** Header (16 Bytes) ***/
	memcpy(cuvex.eeprom_buffer, ">>>>>Header<<<<<", 16);

	/*** Mapped fields (10 x 16 Bytes) ***/
	memcpy(cuvex.eeprom_buffer + 16, "#B:100", 	 6);	//Brightness  (1-100)
	memcpy(cuvex.eeprom_buffer + 32, "#M:LIGHT", 8);	//Mode (LIGHT, DARK)
	memcpy(cuvex.eeprom_buffer + 48, "#L:GB", 	 5);	//Language (GB, SP)

	/*** Footer (11 Bytes) ***/
	memcpy(cuvex.eeprom_buffer + 96, ">>>>>Footer<<<<<", 16);

	/*** Saved to flash memory ***/
	eraseFlash(FLASH_BANK_2, EEPROM_PAGE, 1);
	writeFlash(EEPROM_ADDR, cuvex.eeprom_buffer, EEPROM_SIZE);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void loadEEPROM(void)
{
	char brightness_str[5] = {0};

	/*** Brightness (1-100) ***/
	memcpy(brightness_str, cuvex.eeprom_buffer + 16 + 3, 5);
	cuvex.info.brightness = atoi(brightness_str);
	TIM1->CCR1 = cuvex.info.brightness;

	/*** Mode (LIGHT, DARK) ***/
	if(memcmp(cuvex.eeprom_buffer + 32, "#M:DARK", 7) == 0){
		cuvex.info.mode = DARK;
	}
	else{
		cuvex.info.mode = LIGHT;
	}

	/*** Language (GB, SP) ***/
	if(memcmp(cuvex.eeprom_buffer + 48, "#L:SP", 5) == 0){
		cuvex.info.language = SPANISH;
	}
	else{
		cuvex.info.language = ENGLISH;
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void editEEPROM(void)
{
	static uint8_t last_brightness = 0, last_mode = 0, last_language = 0;
	char brightness_str[5] = {0};

	if((last_brightness != cuvex.info.brightness) || (last_mode != cuvex.info.mode) || (last_language != cuvex.info.language))
	{
		/*** Reset buffer + "header" + "footer" ***/
		memset(cuvex.eeprom_buffer, 0x00, EEPROM_SIZE);
		memcpy(cuvex.eeprom_buffer, ">>>>>Header<<<<<", 16);
		memcpy(cuvex.eeprom_buffer + 96, ">>>>>Footer<<<<<", 16);

		/*** Brightness ***/
		if((cuvex.info.brightness >= 1) && (cuvex.info.brightness <= 100)){
			itoa(cuvex.info.brightness, brightness_str, 10);
			memcpy(cuvex.eeprom_buffer + 16, "#B:", 3);
			memcpy(cuvex.eeprom_buffer + 16 + 3, brightness_str, 5);
		}

		/*** Mode ***/
		if(cuvex.info.mode == DARK){
			memcpy(cuvex.eeprom_buffer + 32, "#M:DARK", 7);
		}
		else{
			memcpy(cuvex.eeprom_buffer + 32, "#M:LIGHT", 8);
		}

		/*** Language ***/
		if(cuvex.info.language == SPANISH){
			memcpy(cuvex.eeprom_buffer + 48, "#L:SP", 5);
		}
		else{
			memcpy(cuvex.eeprom_buffer + 48, "#L:GB", 5);
		}

		/*** Save in flash memory ***/
		eraseFlash(FLASH_BANK_2, EEPROM_PAGE, 1);
		writeFlash(EEPROM_ADDR, cuvex.eeprom_buffer, EEPROM_SIZE);

		/*** Save last values in RAM memory ***/
		last_brightness = cuvex.info.brightness;
		last_mode 		= cuvex.info.mode;
		last_language 	= cuvex.info.language;
	}
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void saveDeviceAlias(void)
{
	eraseFlash(FLASH_BANK_2, DEV_ALIAS_PAGE, 1);
	writeFlash(DEV_ALIAS_ADDR, cuvex.device_alias_buffer, DEV_ALIAS_SIZE);
	NVIC_SystemReset();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void eraseSignature(void)
{
	eraseFlash(FLASH_BANK_2, SIGNATURE_PAGE, 1);
	NVIC_SystemReset();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void processTagInformation(void)
{
	enc_fields_t enc;
	char *ptr = 0x00, *bin_start = 0x00;
	int x=0, y=0;

	/*** Defaults ***/
	memset(cuvex.tag.fw_version, 0x00, sizeof(cuvex.tag.fw_version));
	cuvex.tag.multisigned_total = 0;
	cuvex.tag.multisigned_mandatory = 0;
	cuvex.tag.biometric = 0;

	/*
	 * NEW --> Try "reduced format" (ASCII + binary)
	 */
	if(cuvex.tag.information[0] == 'E')
	{
		uint8_t *info = cuvex.tag.information;

		char token[ENC_TOKEN_LEN + 1] = {0};
		memcpy(token, info, ENC_TOKEN_LEN);
		token[ENC_TOKEN_LEN] = '\0';

		if(enc_decode_reduced_to_fields(token, &enc) == ENC_OK)
		{
			/*** ASCII --> Get firmware version info + Get signed tag info + Get Cuvex BIT identifier tag info ***/
			snprintf(cuvex.tag.fw_version, sizeof(cuvex.tag.fw_version), "v%u.%u.%u", enc.f1, enc.f2, enc.f3);
			cuvex.tag.multisigned_total = enc.mx;
			cuvex.tag.multisigned_mandatory = enc.my;
			cuvex.tag.biometric = enc.b;

			/*** BINARY --> Get salt_PBKDF2 and iv_AE ***/
			bin_start = info + ENC_TOKEN_LEN;
			memcpy(cuvex.tag.salt_pbkdf2, bin_start, 16);
			memcpy(cuvex.tag.iv_aes_gcm, bin_start + 16, 16);
		}
	}

	/*
	 * LEGACY --> Try "complet format" (ASCII)
	 */
	if(bin_start == 0x00)
	{
		/*** Get encripted tag info ***/
		ptr = strstr((char *) cuvex.tag.information, "ENC,");

		if(ptr != 0x00)
		{
			/*** Get firmware version info (vX.Y.Z) ***/
			memcpy(cuvex.tag.fw_version, ptr + 4, 6);

			/*** Get signed tag info ***/
			ptr = strstr((char *) cuvex.tag.information, ",M-");

			if(ptr != 0x00)
			{
				if(sscanf(ptr+3, "%d:%d", &x, &y) == 2)
				{
					if((y <= x) && (x >= 1) && (x <= 6) && (y >= 1) && (y <= 6)){
						cuvex.tag.multisigned_total = (uint8_t) x;
						cuvex.tag.multisigned_mandatory = (uint8_t) y;
					}
				}
				else if(sscanf(ptr+3, "%d", &x) == 1)
				{
					if((x >= 1) && (x <= 6)){
						cuvex.tag.multisigned_total = (uint8_t) x;
						cuvex.tag.multisigned_mandatory = (uint8_t) x;
					}
				}
			}
		}
	}
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void USB_MSC_Mount(void)
{
	/*** Initialize USB Device ***/
	USBD_Init(&USBD_Device, &Class_Desc, 0);
	USBD_RegisterClassComposite(&USBD_Device, USBD_MSC_CLASS, CLASS_TYPE_MSC, MSC_EpAdd_Inst);
	USBD_CMPSIT_SetClassID(&USBD_Device, CLASS_TYPE_MSC, 0);
	USBD_MSC_RegisterStorage(&USBD_Device, &USBD_DISK_fops);

	/*** Start USB Device ***/
	USBD_Start(&USBD_Device);

	/*** Led indicator ***/
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_RESET);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void USB_MSC_Unmount(void)
{
	/*** Stop USB Device ***/
	USBD_Stop(&USBD_Device);

	/*** Led indicator ***/
	HAL_GPIO_WritePin(LED_RED_GPIO_Port, LED_RED_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_GREEN_GPIO_Port, LED_GREEN_Pin, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED_BLUE_GPIO_Port, LED_BLUE_Pin, GPIO_PIN_SET);
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void configAESPeripheral(uint8_t keyAES[], uint8_t ivAES[])
{
	__ALIGN_BEGIN uint32_t HeaderAES_aux[1] __ALIGN_END = {0x00000000};

	for(uint8_t a=0; a<8; a++){
		cuvex.tag.new_keyAES[a] = keyAES[a*4] * 0x1000000 + keyAES[(a*4)+1] * 0x10000 + keyAES[(a*4)+2] * 0x100 + keyAES[(a*4)+3];
	}

	for(uint8_t b=0; b<4; b++){
		cuvex.tag.new_ivAES[b] = ivAES[b*4] * 0x1000000 + ivAES[(b*4)+1] * 0x10000 + ivAES[(b*4)+2] * 0x100 + ivAES[(b*4)+3];
	}

	HAL_CRYP_DeInit(&hcryp);

	hcryp.Init.DataType = CRYP_NO_SWAP;
	hcryp.Init.KeySize = CRYP_KEYSIZE_256B;
	hcryp.Init.pKey = (uint32_t *) cuvex.tag.new_keyAES;
	hcryp.Init.pInitVect = (uint32_t *) cuvex.tag.new_ivAES;
	hcryp.Init.Algorithm = CRYP_AES_GCM_GMAC;
	hcryp.Init.Header = (uint32_t *) HeaderAES_aux;
	hcryp.Init.HeaderSize = 1;
	hcryp.Init.DataWidthUnit = CRYP_DATAWIDTHUNIT_WORD;
	hcryp.Init.HeaderWidthUnit = CRYP_HEADERWIDTHUNIT_WORD;
	hcryp.Init.KeyIVConfigSkip = CRYP_KEYIVCONFIG_ALWAYS;
	hcryp.Init.KeyMode = CRYP_KEYMODE_NORMAL;

	if (HAL_CRYP_Init(&hcryp) != HAL_OK){
		Error_Handler();
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
static void encryptTemplate(uint8_t *template_buf, const uint8_t *key, const uint8_t *iv, const uint8_t *salt, uint8_t signer, uint8_t template_id)
{
	uint8_t flag[16] = {'[','f','l','a','g','_','s','i','g','n','e','r','0','_','0',']'};
	flag[12] = '0' + (signer + 1);
	flag[14] = '0' + template_id;

	/*** 1) Config AES peripheral ***/
	configAESPeripheral((uint8_t *) key, (uint8_t *) iv);

	/*** 2) Append flag before encrypt ***/
	memcpy(template_buf + SIZE_TEMPLATE_EXT0, flag, 16);

	/*** 3) Generate cryptogram --> [ciphertext] = [template_raw][flag_raw] ***/
	HAL_CRYP_Encrypt(&hcryp, (uint32_t *) template_buf, SIZE_TEMPLATE_EXT1/4, (uint32_t *) template_buf, HAL_MAX_DELAY);

	/*** 4) Append metadata after encrypt --> [ciphertext][salt][iv] ***/
	memcpy(template_buf + SIZE_TEMPLATE_EXT1, salt, 16);
	memcpy(template_buf + SIZE_TEMPLATE_EXT1 + 16, iv, 16);
}

/*
 *
 *
 *
 */

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void flowEncryptMountVolume(void)
{
	/*** Initialize a fresh FATFS filesystem on the RAM disk ***/
	RAMDisk_FATFS_Init();

	/*** Write mandatory files: alias, cryptogram, information ***/
	FS_FlowEncrypt_WriteTagAlias(cuvex.tag.alias, cuvex.tag.alias_lenght);
	FS_FlowEncrypt_WriteTagCryptogram(cuvex.tag.cryptogram, sizeof(cuvex.tag.cryptogram));
	FS_FlowEncrypt_WriteTagInformation(cuvex.tag.information, cuvex.tag.information_lenght);

	/*** Write optional file: multisigned (if multisigned 2 of 3 selected) ***/
	if(cuvex.tag.multisigned_mandatory < cuvex.tag.multisigned_total){
		FS_FlowEncrypt_WriteTagMultisignature(cuvex.tag.multisignature, cuvex.tag.multisignature_lenght);
	}

	/*** Write optional files: signer_x_template_y (if biometric selected) ***/
	if(cuvex.fingerprint.enable == true)
	{
		for(int i=0; i<cuvex.fingerprint.id_user; i++)
		{
			/*** Encrypt --> Template 1 + Template 2 ***/
			encryptTemplate(cuvex.fingerprint.signer[i].template_1, cuvex.fingerprint.signer[i].template_1_pbkdf2_key_derived, cuvex.fingerprint.signer[i].template_1_aes_iv, cuvex.fingerprint.signer[i].template_1_pbkdf2_salt, i, 1);
			encryptTemplate(cuvex.fingerprint.signer[i].template_2, cuvex.fingerprint.signer[i].template_2_pbkdf2_key_derived, cuvex.fingerprint.signer[i].template_2_aes_iv, cuvex.fingerprint.signer[i].template_2_pbkdf2_salt, i, 2);

			/*** Write files --> Template 1 + Template 2 ***/
			FS_FlowEncrypt_WriteTagSignerTemplate(i, 1, cuvex.fingerprint.signer[i].template_1, sizeof(cuvex.fingerprint.signer[i].template_1));
			FS_FlowEncrypt_WriteTagSignerTemplate(i, 2, cuvex.fingerprint.signer[i].template_2, sizeof(cuvex.fingerprint.signer[i].template_2));
		}
	}

	/*** Show the RAM disk via USB MSC ***/
	USB_MSC_Mount();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void flowDecryptMountVolume(void)
{
	/*** Initialize a fresh FATFS filesystem on the RAM disk ***/
	RAMDisk_FATFS_Init();

	/*** Write files ***/
	FS_FlowDecrypt_WriteGetCryptogramFiles();

	/*** Show the RAM disk via USB MSC ***/
	USB_MSC_Mount();

	/*** Update flag ***/
	flag_wait_for_files_1 = true;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void flowDecryptReadFiles(void)
{
	uint8_t ret = 0;

	if(flag_wait_for_files_1 == true)
	{
		if(FS_FlowDecrypt_CheckPresenceMandatoryCryptogramFiles() == SUCCESS)
		{
			osDelay(2500);
			ret += FS_FlowDecrypt_ReadTagAlias(cuvex.tag.alias, sizeof(cuvex.tag.alias));
			ret += FS_FlowDecrypt_ReadTagCryptogram(cuvex.tag.cryptogram, sizeof(cuvex.tag.cryptogram));
			ret += FS_FlowDecrypt_ReadTagInformation(cuvex.tag.information, sizeof(cuvex.tag.information));

			if(ret == SUCCESS)
			{
				processTagInformation();

				if(cuvex.tag.multisigned_mandatory < cuvex.tag.multisigned_total)
				{
					ret += FS_FlowDecrypt_ReadTagMultisignature(cuvex.tag.multisignature, sizeof(cuvex.tag.multisignature));
				}

				if(cuvex.tag.biometric == 1)
				{
					for(int i=0; i<cuvex.tag.multisigned_total; i++)
					{
						/*** Read templates files ***/
						ret += FS_FlowDecrypt_ReadTagSignerTemplate(i, 1, cuvex.fingerprint.signer[i].template_1, sizeof(cuvex.fingerprint.signer[i].template_1));
						ret += FS_FlowDecrypt_ReadTagSignerTemplate(i, 2, cuvex.fingerprint.signer[i].template_2, sizeof(cuvex.fingerprint.signer[i].template_2));

						/*** Get template 1 metadata ***/
						memcpy(cuvex.fingerprint.signer[i].template_1_pbkdf2_salt, cuvex.fingerprint.signer[i].template_1 + SIZE_TEMPLATE_EXT1, 16);
						memcpy(cuvex.fingerprint.signer[i].template_1_aes_iv, cuvex.fingerprint.signer[i].template_1 + SIZE_TEMPLATE_EXT1 + 16, 16);

						/*** Get template 2 metadata ***/
						memcpy(cuvex.fingerprint.signer[i].template_2_pbkdf2_salt, cuvex.fingerprint.signer[i].template_2 + SIZE_TEMPLATE_EXT1, 16);
						memcpy(cuvex.fingerprint.signer[i].template_2_aes_iv, cuvex.fingerprint.signer[i].template_2 + SIZE_TEMPLATE_EXT1 + 16, 16);
					}
				}

				/*** Put msg in queue + Unmount USB + Update flag ***/
				if(ret == SUCCESS)
				{
					if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
						osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_FLOW_DECRYPT_FILES_READED}, 0, 0);
						USB_MSC_Unmount();
						flag_wait_for_files_1 = false;
					}
				}
			}
		}
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void flowPsbtMountVolume(uint8_t part)
{
	switch(part)
	{
	case 1:
		/*** Initialize a fresh FATFS filesystem on the RAM disk ***/
		RAMDisk_FATFS_Init();

		/*** Write files ***/
		FS_FlowPsbt_WriteGetNotSignedPsbtFiles();

		/*** Show the RAM disk via USB MSC ***/
		USB_MSC_Mount();

		/*** Update flag ***/
		flag_wait_for_files_2 = true;
		break;

	case 2:
		/*** Write files ***/
		FS_FlowPsbt_WriteSignedPsbt1();
		FS_FlowPsbt_WriteSignedPsbt2(cuvex.psbt.signed_base64, strlen(cuvex.psbt.signed_base64));

		/*** Show the RAM disk via USB MSC ***/
		USB_MSC_Mount();
		break;

	default:
		break;
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void flowPsbtReadFiles(void)
{
	uint8_t ret = 0;

	/*** Check and get PSBT not signed files ***/
	if(flag_wait_for_files_2 == true)
	{
		if(FS_FlowPsbt_CheckPresenceNotSignedPsbtFiles() == SUCCESS)
		{
			osDelay(2500);

			cuvex.psbt.type = PSBT_TYPE_V0_NOT_SIGNED;

			ret += FS_FlowPsbt_ReadNotSignedPsbt1(cuvex.psbt.not_signed_info, sizeof(cuvex.psbt.not_signed_info));
			ret += FS_FlowPsbt_ReadNotSignedPsbt2(cuvex.psbt.not_signed_base64, sizeof(cuvex.psbt.not_signed_base64));

			if(ret == SUCCESS)
			{
				/*** Unmount USB + delay + write files + Mount USB + Update flags ***/
				USB_MSC_Unmount();
				osDelay(2500);
				FS_FlowPsbt_WriteGetCryptogramFiles();
				USB_MSC_Mount();
				flag_wait_for_files_2 = false;
				flag_wait_for_files_3 = true;
			}
		}
	}

	/*** Check and get PSBT cryptogram files ***/
	if(flag_wait_for_files_3 == true)
	{
		if(FS_FlowPsbt_CheckPresenceMandatoryCryptogramFiles() == SUCCESS)
		{
			osDelay(2500);
			ret += FS_FlowPsbt_ReadTagAlias(cuvex.tag.alias, sizeof(cuvex.tag.alias));
			ret += FS_FlowPsbt_ReadTagCryptogram(cuvex.tag.cryptogram, sizeof(cuvex.tag.cryptogram));
			ret += FS_FlowPsbt_ReadTagInformation(cuvex.tag.information, sizeof(cuvex.tag.information));

			if(ret == SUCCESS)
			{
				processTagInformation();

				if(cuvex.tag.multisigned_mandatory < cuvex.tag.multisigned_total)
				{
					ret += FS_FlowPsbt_ReadTagMultisignature(cuvex.tag.multisignature, sizeof(cuvex.tag.multisignature));
				}

				if(cuvex.tag.biometric == 1)
				{
					for(int i=0; i<cuvex.tag.multisigned_total; i++)
					{
						/*** Read templates files ***/
						ret += FS_FlowPsbt_ReadTagSignerTemplate(i, 1, cuvex.fingerprint.signer[i].template_1, sizeof(cuvex.fingerprint.signer[i].template_1));
						ret += FS_FlowPsbt_ReadTagSignerTemplate(i, 2, cuvex.fingerprint.signer[i].template_2, sizeof(cuvex.fingerprint.signer[i].template_2));

						/*** Get template 1 metadata ***/
						memcpy(cuvex.fingerprint.signer[i].template_1_pbkdf2_salt, cuvex.fingerprint.signer[i].template_1 + SIZE_TEMPLATE_EXT1, 16);
						memcpy(cuvex.fingerprint.signer[i].template_1_aes_iv, cuvex.fingerprint.signer[i].template_1 + SIZE_TEMPLATE_EXT1 + 16, 16);

						/*** Get template 2 metadata ***/
						memcpy(cuvex.fingerprint.signer[i].template_2_pbkdf2_salt, cuvex.fingerprint.signer[i].template_2 + SIZE_TEMPLATE_EXT1, 16);
						memcpy(cuvex.fingerprint.signer[i].template_2_aes_iv, cuvex.fingerprint.signer[i].template_2 + SIZE_TEMPLATE_EXT1 + 16, 16);
					}
				}

				/*** Put msg in queue + Unmount USB + Update flag ***/
				if(ret == SUCCESS)
				{
					if(osMessageQueueGetSpace(mainToGuiQueueHandle) > 0){
						osMessageQueuePut(mainToGuiQueueHandle, (void*)&(int){MAIN_TO_GUI_FLOW_PSBT_FILES_READED}, 0, 0);
						USB_MSC_Unmount();
						flag_wait_for_files_3 = false;
					}
				}
			}
		}
	}
}



