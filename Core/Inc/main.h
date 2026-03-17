/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
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
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32u5xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <inttypes.h>
#include <ctype.h>
#include <cmsis_os2.h>
#include "bip39_lib.h"
#include "slip39_lib.h"
#include "xmr_lib.h"
#include "usbd_storage.h"
#include "usbd_core.h"
#include "usbd_desc.h"
#include "usbd_composite_builder.h"
#include "usbd_msc.h"
#include "info_codec.h"
/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/

/*** Printf's ***/
#ifdef __GNUC__
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif

/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define HDAC_EN_Pin GPIO_PIN_13
#define HDAC_EN_GPIO_Port GPIOC
#define LED_RED_Pin GPIO_PIN_1
#define LED_RED_GPIO_Port GPIOC
#define LED_GREEN_Pin GPIO_PIN_2
#define LED_GREEN_GPIO_Port GPIOC
#define LED_BLUE_Pin GPIO_PIN_3
#define LED_BLUE_GPIO_Port GPIOC
#define HDAC_CONFIG_Pin GPIO_PIN_1
#define HDAC_CONFIG_GPIO_Port GPIOA
#define HDAC_CS_N_Pin GPIO_PIN_4
#define HDAC_CS_N_GPIO_Port GPIOA
#define HDAC_SCK_Pin GPIO_PIN_5
#define HDAC_SCK_GPIO_Port GPIOA
#define HDAC_MISO_Pin GPIO_PIN_6
#define HDAC_MISO_GPIO_Port GPIOA
#define HDAC_MOSI_Pin GPIO_PIN_7
#define HDAC_MOSI_GPIO_Port GPIOA
#define HDAC_IRQ_Pin GPIO_PIN_0
#define HDAC_IRQ_GPIO_Port GPIOB
#define HDAC_IRQ_EXTI_IRQn EXTI0_IRQn
#define HDAC_RST_N_Pin GPIO_PIN_1
#define HDAC_RST_N_GPIO_Port GPIOB
#define HW_VER_1_Pin GPIO_PIN_6
#define HW_VER_1_GPIO_Port GPIOC
#define HW_VER_2_Pin GPIO_PIN_7
#define HW_VER_2_GPIO_Port GPIOC
#define HW_VER_3_Pin GPIO_PIN_8
#define HW_VER_3_GPIO_Port GPIOC
#define LCD_BACKLIGHT_Pin GPIO_PIN_8
#define LCD_BACKLIGHT_GPIO_Port GPIOA
#define LCD_RESET_Pin GPIO_PIN_9
#define LCD_RESET_GPIO_Port GPIOA
#define TOUCH_RESET_Pin GPIO_PIN_4
#define TOUCH_RESET_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */
/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/

/*
 * DEFINE's
 */

#define FIRMWARE_VERSION			"1.2.0"
#define HARDWARE_VERSION() 			('1' + ((HAL_GPIO_ReadPin(GPIOC, HW_VER_3_Pin) == GPIO_PIN_SET) << 2) + ((HAL_GPIO_ReadPin(GPIOC, HW_VER_2_Pin) == GPIO_PIN_SET) << 1) + ((HAL_GPIO_ReadPin(GPIOC, HW_VER_1_Pin) == GPIO_PIN_SET) << 0))
#define FW_VER_F1   				(FIRMWARE_VERSION[0] - '0')
#define FW_VER_F2   				(FIRMWARE_VERSION[2] - '0')
#define FW_VER_F3   				(FIRMWARE_VERSION[4] - '0')

//#define DEBUG_PRINTF
//#define DEBUG_PRINTF_FINGERPRINT
//#define DEBUG_PSBT_PRINTF
//#define DEBUG_PRINTF_DECRYPT

#define RAMDISK_SECTOR_SIZE   		512				//256KB (512*512)
#define RAMDISK_SECTORS       		512   			//...

#define SIZE_ALIAS 					25
#define SIZE_INFORMATION			50
#define SIZE_MULTISIGN				1300
#define SIZE_CRYPT					768
#define SIZE_CRYPT_MSG				SIZE_CRYPT/4
#define SIZE_TEMPLATE_EXT0			18000			//[template_raw]
#define SIZE_TEMPLATE_EXT1			18016			//[ciphertext] = [template_raw][flag_raw]
#define SIZE_TEMPLATE_EXT2			18048			//[ciphertext][flag][salt][iv]

#define DEV_ALIAS_ADDR				0x083FA000		//"DEV_ALIAS" (bank 2) 	--> Address
#define DEV_ALIAS_PAGE				253				//"DEV_ALIAS" (bank 2) 	--> Page number
#define DEV_ALIAS_SIZE				30				//"DEV_ALIAS" (bank 2) 	--> Máx size
#define EEPROM_ADDR					0x083FC000		//"EEPROM" (bank 2) 	--> Address
#define EEPROM_PAGE					254				//"EEPROM" (bank 2) 	--> Page number
#define EEPROM_SIZE					112				//"EEPROM" (bank 2)		--> Máx size
#define SIGNATURE_ADDR				0x083FE000		//"SIGNATURE" (bank 2) 	--> Address
#define SIGNATURE_PAGE				255				//"SIGNATURE" (bank 2) 	--> Page number
#define SIGNATURE_SIZE				62				//"SIGNATURE" (bank 2) 	--> Máx size

#define ENROLL_NUM_SAMPLES			20

/*
 * STRUCT's
 */

struct tag
{
	/*** Tag data stored/readed directly ***/
	uint8_t alias[SIZE_ALIAS];
	uint8_t cryptogram[SIZE_CRYPT];
	uint8_t information[SIZE_INFORMATION];				//"ENC,vXX.XX.XX(Y),M-X:X,P-X,C-X,BITX,saltPBKDF2ivAES"
	uint8_t multisignature[SIZE_MULTISIGN];				//"Num.Comb.+comb1+comb2+...+combN"

	/*** Tag data getted/setted ***/
	uint8_t multisigned_total;
	uint8_t multisigned_mandatory;
	uint8_t biometric;
	uint8_t fw_version[10];
	uint8_t salt_pbkdf2[16];
	uint8_t iv_aes_gcm[16];

	/*** Variables to use in AES-256 processing ***/
	uint32_t new_text_to_encrypt[SIZE_CRYPT_MSG];
	uint32_t new_text_encrypted[SIZE_CRYPT_MSG];
	uint32_t new_text_to_decrypt[SIZE_CRYPT_MSG];
	uint32_t new_text_decrypted[SIZE_CRYPT_MSG];
	uint32_t new_keyAES[8];
	uint32_t new_ivAES[4];

	/*** Others ***/
	uint16_t alias_lenght;
	uint16_t information_lenght;
	uint16_t multisignature_lenght;
};

struct psbt
{
	uint8_t type;
	uint8_t not_signed_info[100];
	uint8_t not_signed_base64[7500];
	uint8_t signed_info[100];
	uint8_t signed_base64[7500];
};

struct info
{
	uint8_t brightness;
	uint8_t language;
	uint8_t mode;
};

struct encrypt
{
	uint8_t  text_type;
	uint16_t total_words;
	uint8_t  actual_pwd;
	uint8_t  words_to_encrypt[55][5];	//54 words... 4 characters per word...
	uint8_t  buff_passphrase[105];		//100 characters
	uint8_t  buff_plain_text[505];		//500 characters
};

struct decrypt
{
	uint8_t cryptogram_decrypted[SIZE_CRYPT];
	uint8_t derivation_path[50];
	uint8_t derived_address[100];
};

struct wallet
{
	bool flag_new;
	uint8_t dice_selected[4];
	uint8_t coin_dice_values[5];
	uint8_t words_to_encrypt[24][5];
	uint8_t pass_deriv[250];
	uint8_t zprv_key[125];
	uint8_t zpub_key[125];
};

struct signer
{
	uint8_t template_1[SIZE_TEMPLATE_EXT2];
	uint8_t template_2[SIZE_TEMPLATE_EXT2];
	uint8_t template_1_pbkdf2_key_derived[32];
	uint8_t template_2_pbkdf2_key_derived[32];
	uint8_t template_1_pbkdf2_salt[16];
	uint8_t template_2_pbkdf2_salt[16];
	uint8_t template_1_aes_iv[16];
	uint8_t template_2_aes_iv[16];
};

struct fingerprint
{
	bool enable;
	uint8_t id_user;
	uint8_t id_template;
	uint8_t enroll_samples_remaining;

	struct signer signer[6];
};

struct cuvex
{
	uint8_t screen;
	char device_alias_buffer[DEV_ALIAS_SIZE];
	char eeprom_buffer[EEPROM_SIZE];
	char signature_buffer[SIGNATURE_SIZE];
	struct info info;
	struct tag tag;
	struct psbt psbt;
	struct encrypt encrypt;
	struct decrypt decrypt;
	struct wallet wallet;
	struct fingerprint fingerprint;
};

/*
 * ENUM's
 */

enum gui_to_main_queue_msg
{
	/*** GUI screen msgs ***/
	GUI_TO_MAIN_SCREEN_INIT = 0,
	GUI_TO_MAIN_SCREEN_MAIN_MENU,
	GUI_TO_MAIN_SCREEN_FLOW_ENCRYPT,
	GUI_TO_MAIN_SCREEN_FLOW_DECRYPT,
	GUI_TO_MAIN_SCREEN_FLOW_WALLET,
	GUI_TO_MAIN_SCREEN_FLOW_PSBT,
	GUI_TO_MAIN_SCREEN_FLOW_SETTINGS,

	/*** Flash msgs ***/
	GUI_TO_MAIN_FLASH_SAVE_SETTINGS,
	GUI_TO_MAIN_FLASH_SAVE_SETTINGS_AND_DEVICE_ALIAS,
	GUI_TO_MAIN_FLASH_ERASE_SIGNATURE,

	/*** Volume msgs  ***/
	GUI_TO_MAIN_FLOW_ENCRYPT_VOLUME_MOUNT,
	GUI_TO_MAIN_FLOW_DECRYPT_VOLUME_MOUNT,
	GUI_TO_MAIN_FLOW_PSBT_VOLUME_MOUNT_1,
	GUI_TO_MAIN_FLOW_PSBT_VOLUME_MOUNT_2,

	/*** Fingerprint msgs  ***/
	GUI_TO_MAIN_GET_TEMPLATE,
	GUI_TO_MAIN_CHECK_TEMPLATE
};

enum main_to_gui_queue_msg
{
	/*** Volume msgs  ***/
	MAIN_TO_GUI_FLOW_DECRYPT_FILES_READED = 0,
	MAIN_TO_GUI_FLOW_PSBT_FILES_READED,

	/*** Fingerprint msgs  ***/
	MAIN_TO_GUI_GET_TEMPLATE_SUCCESS,
	MAIN_TO_GUI_GET_TEMPLATE_ERROR,
	MAIN_TO_GUI_CHECK_TEMPLATE_SUCCESS,
	MAIN_TO_GUI_CHECK_TEMPLATE_ERROR,
	MAIN_TO_GUI_GENERAL_ERROR
};

enum main_to_fingerprint_queue_msg
{
	MAIN_TO_FP_GET_TEMPLATE = 0,
	MAIN_TO_FP_CHECK_TEMPLATE
};

enum fingerprint_to_main_queue_msg
{
	FP_TO_MAIN_GET_TEMPLATE_SUCCESS = 0,
	FP_TO_MAIN_GET_TEMPLATE_ERROR,
	FP_TO_MAIN_CHECK_TEMPLATE_SUCCESS,
	FP_TO_MAIN_CHECK_TEMPLATE_ERROR,
	FP_TO_MAIN_GENERAL_ERROR
};

enum screen_state
{
	SCREEN_INIT = 0,
	SCREEN_MAIN_MENU,
	SCREEN_FLOW_ENCRYPT,
	SCREEN_FLOW_DECRYPT,
	SCREEN_FLOW_WALLET,
	SCREEN_FLOW_PSBT,
	SCREEN_FLOW_SETTINGS
};

enum mode
{
	LIGHT = 0,
	DARK
};

enum language
{
	ENGLISH = 0,
	SPANISH
};

enum text_type
{
	TEXT_TYPE_NONE = 0,
	TEXT_TYPE_BIP39,
	TEXT_TYPE_SLIP39,
	TEXT_TYPE_XMR,
	TEXT_TYPE_PLAINTEXT,
	TEXT_TYPE_FROM_WALLET_BIP39
};

enum dice_colour
{
	DICE_COLOUR_1 = 1,
	DICE_COLOUR_2,
	DICE_COLOUR_3,
	DICE_COLOUR_4,
	DICE_COLOUR_5,
	DICE_COLOUR_6,
	DICE_COLOUR_7,
	DICE_COLOUR_8
};

enum dice_selected_value
{
	DICE_SELECTED_VALUE_1 = 101,
	DICE_SELECTED_VALUE_2,
	DICE_SELECTED_VALUE_3,
	DICE_SELECTED_VALUE_4
};

enum psbt_type
{
	PSBT_TYPE_NONE = 0,
	PSBT_TYPE_V0_NOT_SIGNED,
	PSBT_TYPE_V0_SIGNED,
	PSBT_TYPE_OTHER_STATE
};

/*********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************
 *********************************************************************************************************************************************************************************************************/
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
