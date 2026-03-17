/*
 *****************************************************************************
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

#include <gui/screen_flow_encrypt_part_2_screen/screen_flow_encrypt_part_2View.hpp>
#include "main.h"

extern struct cuvex cuvex;
extern HASH_HandleTypeDef hhash;
extern CRYP_HandleTypeDef hcryp;
extern RNG_HandleTypeDef hrng;

typedef struct{
	uint8_t ipad[64];
	uint8_t opad[64];
}hmac_sha256_ctx_t;

screen_flow_encrypt_part_2View::screen_flow_encrypt_part_2View(): actual_pwd(0), total_pwds(1), mandatory_pwds(1), biometric_enable(0), pwd_raw{0}, pwd_sha256{0}, pwd_combined_sha256{0}, iv_aes_gcm{0}, pwds{0}, pwds_sha256{0}, pwds_sha256_concat{0}, pwds_key_pbkdf2{0}, salt_pbkdf2{0}
{

}

void screen_flow_encrypt_part_2View::setupScreen()
{
	screen_flow_encrypt_part_2ViewBase::setupScreen();
	screen_flow_encrypt_part_2View::setScreenMode();
	screen_flow_encrypt_part_2View::setScreenLanguage();
	screen_flow_encrypt_part_2View::changeScreen(GUI_TO_MAIN_SCREEN_FLOW_ENCRYPT);

	memset(keyboard1_text_typed_passwordBuffer, 	0x00, sizeof(keyboard1_text_typed_passwordBuffer));
	memset(keyboard2_text_typed_passwordBuffer, 	0x00, sizeof(keyboard2_text_typed_passwordBuffer));
	memset(keyboard_text_typed_aliasBuffer, 	 	0x00, sizeof(keyboard_text_typed_aliasBuffer));
	keyboard1_password.clearBuffer();
	keyboard2_password.clearBuffer();
	keyboard_alias.clearBuffer();

	if(cuvex.wallet.flag_new == true){
		cuvex.encrypt.text_type = TEXT_TYPE_FROM_WALLET_BIP39;
	}
}

void screen_flow_encrypt_part_2View::tearDownScreen()
{
	screen_flow_encrypt_part_2ViewBase::tearDownScreen();
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::tickEventScreen()
{
	static uint8_t last_samples_remaining = 0, tick_aux = 0;

	/*** Password - Keyboard 1 (password field 1) ***/
	if(keyboard1_password.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard1_text_typed_passwordBuffer, keyboard1_password.getBuffer(), KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE) != 0)
		{
			/*** Obtaining typed text in widget ***/
			Unicode::snprintf(keyboard1_text_typed_passwordBuffer, KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE, keyboard1_password.getBuffer());
			keyboard1_text_typed_password.setWideTextAction(touchgfx::WIDE_TEXT_NONE);

			/*** Text alignment within area ***/
			if(keyboard1_text_typed_password.getTextWidth() <= 160){
				keyboard1_text_typed_password.setPosition(160-(keyboard1_text_typed_password.getTextWidth()/2), 40, keyboard1_text_typed_password.getTextWidth(), 20);
			}
			else{
				keyboard1_text_typed_password.setPosition(76, 40, 160, 20);
			}

			/*** Reset and filling of buffer with hidden password ***/
			for(int i=0; i<KEYBOARD1_TEXT_TYPED_HIDE_PASSWORD_SIZE; i++){
				keyboard1_text_typed_hide_passwordBuffer[i] = 0x00;
			}
			for(int i=0; i<Unicode::strlen(keyboard1_text_typed_passwordBuffer); i++){
				keyboard1_text_typed_hide_passwordBuffer[i] = 8226;	//U+8226 (bullet '•')
			}

			/*** Placeholder status (visible or hidden) ***/
			if(keyboard1_text_typed_passwordBuffer[0] == 0x00){
				placeholder_1_password.setVisible(true);
			}
			else{
				placeholder_1_password.setVisible(false);
			}

			/*** Color of text with password instruction information ***/
			if(cuvex.info.mode == DARK){
				text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
			}
			else{
				text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0x3F,0x3F,0x51));
			}

			/*** Area update ***/
			keyboard1_text_area_password.invalidate();
			text_info_2_password.invalidate();
		}
	}

	/*** Password - Keyboard 2 (password field 2) ***/
	if(keyboard2_password.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard2_text_typed_passwordBuffer, keyboard2_password.getBuffer(), KEYBOARD2_TEXT_TYPED_PASSWORD_SIZE) != 0)
		{
			/*** Obtaining typed text in widget ***/
			Unicode::snprintf(keyboard2_text_typed_passwordBuffer, KEYBOARD2_TEXT_TYPED_PASSWORD_SIZE, keyboard2_password.getBuffer());
			keyboard2_text_typed_password.setWideTextAction(touchgfx::WIDE_TEXT_NONE);

			/*** Text alignment within area ***/
			if(keyboard2_text_typed_password.getTextWidth() <= 160){
				keyboard2_text_typed_password.setPosition(160-(keyboard2_text_typed_password.getTextWidth()/2), 75, keyboard2_text_typed_password.getTextWidth(), 20);
			}
			else{
				keyboard2_text_typed_password.setPosition(76, 75, 160, 20);
			}

			/*** Reset and filling of buffer with hidden password ***/
			for(int i=0; i<KEYBOARD2_TEXT_TYPED_HIDE_PASSWORD_SIZE; i++){
				keyboard2_text_typed_hide_passwordBuffer[i] = 0x00;
			}
			for(int i=0; i<Unicode::strlen(keyboard2_text_typed_passwordBuffer); i++){
				keyboard2_text_typed_hide_passwordBuffer[i] = 8226;	//U+8226 (bullet '•')
			}

			/*** Placeholder status (visible or hidden) ***/
			if(keyboard2_text_typed_passwordBuffer[0] == 0x00){
				placeholder_2_password.setVisible(true);
			}
			else{
				placeholder_2_password.setVisible(false);
			}

			/*** Color of text with password instruction information ***/
			if(cuvex.info.mode == DARK){
				text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
			}
			else{
				text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0x3F,0x3F,0x51));
			}

			/*** Area update ***/
			keyboard2_text_area_password.invalidate();
			text_info_2_password.invalidate();
		}
	}

	/*** Alias - Keyboard (card alias) ***/
	if(keyboard_alias.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard_text_typed_aliasBuffer, keyboard_alias.getBuffer(), KEYBOARD_TEXT_TYPED_ALIAS_SIZE) != 0)
		{
			Unicode::snprintf(keyboard_text_typed_aliasBuffer, KEYBOARD_TEXT_TYPED_ALIAS_SIZE, keyboard_alias.getBuffer());
			keyboard_text_typed_alias.setWideTextAction(touchgfx::WIDE_TEXT_NONE);

			if(keyboard_text_typed_alias.getTextWidth() <= 160){
				keyboard_text_typed_alias.setPosition(160-(keyboard_text_typed_alias.getTextWidth()/2), 75, keyboard_text_typed_alias.getTextWidth(), 20);
			}
			else{
				keyboard_text_typed_alias.setPosition(76, 75, 160, 20);
			}

			keyboard_text_area_alias.invalidate();
		}
	}

	/*** Enroll ***/
	if(s5_3_enrollFingerprint.isVisible() == true)
	{
		if((fingerprint_success_image.isVisible() == true) || (fingerprint_error_image.isVisible() == true))
		{
			if(tick_aux < 18){
				tick_aux++;
			}
			else{
				tick_aux = 0;

				cuvex.fingerprint.enroll_samples_remaining = ENROLL_NUM_SAMPLES;
				screen_flow_encrypt_part_2View::getTemplate(GUI_TO_MAIN_GET_TEMPLATE);

				if(cuvex.fingerprint.id_template == 1){
					fingerprint_right_hand_texts.setVisible(true);
					fingerprint_left_hand_texts.setVisible(false);
					fingerprint_common_texts.setVisible(false);
				}
				else{
					fingerprint_right_hand_texts.setVisible(false);
					fingerprint_left_hand_texts.setVisible(true);
					fingerprint_common_texts.setVisible(false);
				}

				fingerprint_1_image.setVisible(true);
				fingerprint_2_image.setVisible(false);
				fingerprint_3_image.setVisible(false);
				fingerprint_error_image.setVisible(false);
				fingerprint_success_image.setVisible(false);

				background.invalidate();
			}
		}
		else
		{
			if((last_samples_remaining != cuvex.fingerprint.enroll_samples_remaining) && (cuvex.fingerprint.enroll_samples_remaining < ENROLL_NUM_SAMPLES))
			{
				last_samples_remaining = cuvex.fingerprint.enroll_samples_remaining;

				Unicode::snprintf(fingerprint_common_info2Buffer, FINGERPRINT_COMMON_INFO2_SIZE, "%d", ENROLL_NUM_SAMPLES - cuvex.fingerprint.enroll_samples_remaining);

				fingerprint_right_hand_texts.setVisible(false);
				fingerprint_left_hand_texts.setVisible(false);
				fingerprint_common_texts.setVisible(true);
				fingerprint_1_image.setVisible(false);
				fingerprint_2_image.setVisible(false);
				fingerprint_3_image.setVisible(false);
				fingerprint_error_image.setVisible(false);
				fingerprint_success_image.setVisible(false);

				if(cuvex.fingerprint.enroll_samples_remaining <= 10){
					fingerprint_3_image.setVisible(true);
				}
				else{
					fingerprint_2_image.setVisible(true);
				}

				background.invalidate();
			}
		}
	}

	/*** Processing ***/
	if(s7_processing.isVisible() == true)
	{
		/*** Generation of the records data ***/
		screen_flow_encrypt_part_2View::generateRecordData1_Alias();
		screen_flow_encrypt_part_2View::generateRecordData2_Cryptogram();
		screen_flow_encrypt_part_2View::generateRecordData3_Information();

		if(total_pwds != mandatory_pwds){
			screen_flow_encrypt_part_2View::generateRecordData4_Multisignature();
		}

		/*** Mount volume (USB-MSC) ***/
		screen_flow_encrypt_part_2View::changeVolume(GUI_TO_MAIN_FLOW_ENCRYPT_VOLUME_MOUNT);

		/*** Clearing keyboard and display buffers ***/
		memset(keyboard_text_typed_aliasBuffer, 0x00, sizeof(keyboard_text_typed_aliasBuffer));
		keyboard_alias.clearBuffer();

		/*** Selecting visible/hidden elements on the screen ***/
		s7_processing.setVisible(false);
		s8_success.setVisible(true);
		success_msg_1.setVisible(true);
		success_msg_2.setVisible(false);

		/*** Screen update ***/
		background.invalidate();
	}
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::changeScreen(uint8_t screen)
{
	presenter->changeScreen(screen);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::changeVolume(uint8_t state)
{
	presenter->changeVolume(state);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::getTemplate(uint8_t state)
{
	presenter->getTemplate(state);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::updateFingerprint(uint16_t state)
{
	if(state == MAIN_TO_GUI_GET_TEMPLATE_SUCCESS)
	{
		if(cuvex.fingerprint.id_template == 1)
		{
			cuvex.fingerprint.id_template = 2;
			Unicode::snprintf(fingerprint_common_info2Buffer, FINGERPRINT_COMMON_INFO2_SIZE, "%d", ENROLL_NUM_SAMPLES + 1);

			/*** Selecting visible/hidden elements on the screen ***/
			fingerprint_1_image.setVisible(false);
			fingerprint_2_image.setVisible(false);
			fingerprint_3_image.setVisible(false);
			fingerprint_error_image.setVisible(false);
			fingerprint_success_image.setVisible(true);

			/*** Screen update ***/
			background.invalidate();
		}
		else
		{
			if(cuvex.info.language == SPANISH)
			{
				if(total_pwds > 1)
				{
					Unicode::snprintf(text_bio_success_infoBuffer, TEXT_BIO_SUCCESS_INFO_SIZE, "%d", actual_pwd);

					if(total_pwds != actual_pwd){
						Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "CONTINUAR");
					}
					else{
						Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "GRABAR CRIPTOGRAMA");
					}
				}
				else{
					Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "GRABAR CRIPTOGRAMA");
				}
			}
			else
			{
				if(total_pwds > 1)
				{
					Unicode::snprintf(text_bio_success_infoBuffer, TEXT_BIO_SUCCESS_INFO_SIZE, "%d", actual_pwd);

					if(total_pwds != actual_pwd){
						Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "CONTINUE");
					}
					else{
						Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "RECORD CRYPTOGRAM");
					}
				}
				else{
					Unicode::snprintf(btn_bio_successBuffer, BTN_BIO_SUCCESS_SIZE, "RECORD CRYPTOGRAM");
				}
			}

			/*** Selecting visible/hidden elements on the screen ***/
			s5_1_typePasword.setVisible(false);
			s5_2_successPassword.setVisible(false);
			s5_3_enrollFingerprint.setVisible(false);
			s5_4_successBiometric.setVisible(true);

			/*** Screen update ***/
			background.invalidate();
		}
	}

	if(state == MAIN_TO_GUI_GET_TEMPLATE_ERROR)
	{
		Unicode::snprintf(fingerprint_common_info2Buffer, FINGERPRINT_COMMON_INFO2_SIZE, "%d", ENROLL_NUM_SAMPLES + 1);

		/*** Selecting visible/hidden elements on the screen ***/
		fingerprint_1_image.setVisible(false);
		fingerprint_2_image.setVisible(false);
		fingerprint_3_image.setVisible(false);
		fingerprint_error_image.setVisible(true);
		fingerprint_success_image.setVisible(false);

		/*** Screen update ***/
		background.invalidate();
	}
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::hideKeyboardPasswordPressed()
{
	text_info_1_password.setPosition(0, 60, 320, 20);
	text_info_2_password.setPosition(0, 152, 320, 13);
	keyboard1_text_typed_hide_password.setPosition(76, 90, 160, 20);
	keyboard2_text_typed_hide_password.setPosition(76, 125, 160, 20);
	keyboard1_text_area_password.setXY(45, 85);
	keyboard2_text_area_password.setXY(45, 120);
	keyboard1_text_area_selected_password.setXY(45, 85);
	keyboard2_text_area_selected_password.setXY(45, 120);
	keyboard1_pwd_eye_password.setXY(245, 85);
	keyboard2_pwd_eye_password.setXY(245, 120);
	keyboard1_btn_show_password.setPosition(76, 85, 169, 30);
	keyboard2_btn_show_password.setPosition(76, 120, 169, 30);
	placeholder_1_password.setPosition(62, 90, 180, 20);
	placeholder_2_password.setPosition(62, 125, 180, 20);

	if(keyboard1_text_typed_password.getTextWidth() <= 160){
		keyboard1_text_typed_password.setPosition(160-(keyboard1_text_typed_password.getTextWidth()/2), 90, keyboard1_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard1_text_typed_password.setPosition(76, 90, 160, 20);
	}

	if(keyboard2_text_typed_password.getTextWidth() <= 160){
		keyboard2_text_typed_password.setPosition(160-(keyboard2_text_typed_password.getTextWidth()/2), 125, keyboard2_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard2_text_typed_password.setPosition(76, 125, 160, 20);
	}

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard1_password.setVisible(false);
	keyboard1_btn_show_password.setVisible(true);
	keyboard1_text_area_selected_password.setVisible(false);
	keyboard2_password.setVisible(false);
	keyboard2_btn_show_password.setVisible(true);
	keyboard2_text_area_selected_password.setVisible(false);
	keyboard_btn_hide_password.setVisible(false);
	keyboard_btn_enter_password.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::show1KeyboardPasswordPressed()
{
	text_info_1_password.setPosition(0, 10, 320, 20);
	text_info_2_password.setPosition(0, 102, 320, 13);
	keyboard1_text_typed_hide_password.setPosition(76, 40, 160, 20);
	keyboard2_text_typed_hide_password.setPosition(76, 75, 160, 20);
	keyboard1_text_area_password.setXY(45, 35);
	keyboard2_text_area_password.setXY(45, 70);
	keyboard1_text_area_selected_password.setXY(45, 35);
	keyboard2_text_area_selected_password.setXY(45, 70);
	keyboard1_pwd_eye_password.setXY(245, 35);
	keyboard2_pwd_eye_password.setXY(245, 70);
	keyboard1_btn_show_password.setPosition(76, 35, 169, 30);
	keyboard2_btn_show_password.setPosition(76, 70, 169, 30);
	placeholder_1_password.setPosition(62, 40, 180, 20);
	placeholder_2_password.setPosition(62, 75, 180, 20);

	if(keyboard1_text_typed_password.getTextWidth() <= 160){
		keyboard1_text_typed_password.setPosition(160-(keyboard1_text_typed_password.getTextWidth()/2), 40, keyboard1_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard1_text_typed_password.setPosition(76, 40, 160, 20);
	}

	if(keyboard2_text_typed_password.getTextWidth() <= 160){
		keyboard2_text_typed_password.setPosition(160-(keyboard2_text_typed_password.getTextWidth()/2), 75, keyboard2_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard2_text_typed_password.setPosition(76, 75, 160, 20);
	}

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard1_password.setVisible(true);
	keyboard1_btn_show_password.setVisible(false);
	keyboard1_text_area_selected_password.setVisible(true);
	keyboard2_password.setVisible(false);
	keyboard2_btn_show_password.setVisible(true);
	keyboard2_text_area_selected_password.setVisible(false);
	keyboard_btn_hide_password.setVisible(true);
	keyboard_btn_enter_password.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::show2KeyboardPasswordPressed()
{
	text_info_1_password.setPosition(0, 10, 320, 20);
	text_info_2_password.setPosition(0, 102, 320, 13);
	keyboard1_text_typed_hide_password.setPosition(76, 40, 160, 20);
	keyboard2_text_typed_hide_password.setPosition(76, 75, 160, 20);
	keyboard1_text_area_password.setXY(45, 35);
	keyboard2_text_area_password.setXY(45, 70);
	keyboard1_text_area_selected_password.setXY(45, 35);
	keyboard2_text_area_selected_password.setXY(45, 70);
	keyboard1_pwd_eye_password.setXY(245, 35);
	keyboard2_pwd_eye_password.setXY(245, 70);
	keyboard1_btn_show_password.setPosition(76, 35, 169, 30);
	keyboard2_btn_show_password.setPosition(76, 70, 169, 30);
	placeholder_1_password.setPosition(62, 40, 180, 20);
	placeholder_2_password.setPosition(62, 75, 180, 20);

	if(keyboard1_text_typed_password.getTextWidth() <= 160){
		keyboard1_text_typed_password.setPosition(160-(keyboard1_text_typed_password.getTextWidth()/2), 40, keyboard1_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard1_text_typed_password.setPosition(76, 40, 160, 20);
	}

	if(keyboard2_text_typed_password.getTextWidth() <= 160){
		keyboard2_text_typed_password.setPosition(160-(keyboard2_text_typed_password.getTextWidth()/2), 75, keyboard2_text_typed_password.getTextWidth(), 20);
	}
	else{
		keyboard2_text_typed_password.setPosition(76, 75, 160, 20);
	}

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard1_password.setVisible(false);
	keyboard1_btn_show_password.setVisible(true);
	keyboard1_text_area_selected_password.setVisible(false);
	keyboard2_password.setVisible(true);
	keyboard2_btn_show_password.setVisible(false);
	keyboard2_text_area_selected_password.setVisible(true);
	keyboard_btn_hide_password.setVisible(true);
	keyboard_btn_enter_password.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::enterKeyboardPasswordPressed()
{
	bool flag_upercase = false, flag_lowercase = false, flag_simbol = false, flag_number = false;
	bool flag_passwords_ok = false;

	/*** Checking that typed texts meet the conditions (minimum 12 characters, uppercase, lowercase, numbers, and symbols) ***/
	if((Unicode::strlen(keyboard1_text_typed_passwordBuffer) >= 12) && (Unicode::strlen(keyboard2_text_typed_passwordBuffer) >= 12))
	{
		if(Unicode::strncmp(keyboard1_text_typed_passwordBuffer, keyboard2_text_typed_passwordBuffer, KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE) == 0)
		{
			for(int i=0; i<KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE; i++)
			{
				if((keyboard1_text_typed_passwordBuffer[i] >= 32) && (keyboard1_text_typed_passwordBuffer[i] <= 47)){			//Symbol (Unicode: 32-47)
					flag_simbol = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 48) && (keyboard1_text_typed_passwordBuffer[i] <= 57)){		//Number (Unicode: 48-57)
					flag_number = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 58) && (keyboard1_text_typed_passwordBuffer[i] <= 64)){		//Symbol (Unicode: 58-64)
					flag_simbol = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 65) && (keyboard1_text_typed_passwordBuffer[i] <= 90)){		//Uppercase (Unicode: 65-90)
					flag_upercase = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 91) && (keyboard1_text_typed_passwordBuffer[i] <= 96)){		//Symbol (Unicode: 91-96)
					flag_simbol = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 97) && (keyboard1_text_typed_passwordBuffer[i] <= 122)){		//Lowercase (Unicode: 97-122)
					flag_lowercase = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 123) && (keyboard1_text_typed_passwordBuffer[i] <= 126)){	//Symbol (Unicode: 123-126)
					flag_simbol = true;
				}
				else if((keyboard1_text_typed_passwordBuffer[i] >= 160) && (keyboard1_text_typed_passwordBuffer[i] <= 191)){	//Symbol (Unicode: 160-191)
					flag_simbol = true;
				}
				else if(keyboard1_text_typed_passwordBuffer[i] == 8364){	//Symbol '€' (Unicode: 8364)
					flag_simbol = true;
				}
			}

			if((flag_upercase == true) && (flag_lowercase == true) && (flag_number == true) && (flag_simbol == true))	//Passwords contain "uppercase", "lowercase", "numbers", and "symbols"
			{
				flag_passwords_ok = true;
			}
		}
	}

	/*** If both passwords meet the conditions ***/
	if(flag_passwords_ok == true)
	{
		/*** Obtaining the typed text (raw password) + Obtaining its SHA-256 hash ***/
		memset(pwds[actual_pwd], 0x00, KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE);
		memset(pwds_sha256[actual_pwd], 0x00, 32);

		for(int i=0; i<KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE; i++)
		{
			if(keyboard1_text_typed_passwordBuffer[i] == 8364){	//If '€' (8364) is converted to the character '¶' (182) to only occupy 1 byte
				pwds[actual_pwd][i] = 182;
			}
			else{
				pwds[actual_pwd][i] = (uint8_t) keyboard1_text_typed_passwordBuffer[i];
			}
		}

		HAL_HASHEx_SHA256_Start(&hhash, pwds[actual_pwd], strlen((char *) pwds[actual_pwd]), pwds_sha256[actual_pwd], HAL_MAX_DELAY);

		/*** Selection of texts to display in the field of correct password information (based on language and number of signers) ***/
		if(cuvex.info.language == SPANISH)
		{
			if(total_pwds > 1)
			{
				Unicode::snprintf(text_pwd_success_infoBuffer, TEXT_PWD_SUCCESS_INFO_SIZE, "%d", actual_pwd+1);

				if(biometric_enable == 1){
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CREAR BIOMETRIA");
				}
				else{
					if(total_pwds != actual_pwd+1){
						Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CONTINUAR");
					}
					else{
						Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "GRABAR CRIPTOGRAMA");
					}
				}
			}
			else{
				if(biometric_enable == 1){
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CREAR BIOMETRIA");
				}
				else{
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "GRABAR CRIPTOGRAMA");
				}
			}
		}
		else
		{
			if(total_pwds > 1)
			{
				Unicode::snprintf(text_pwd_success_infoBuffer, TEXT_PWD_SUCCESS_INFO_SIZE, "%d", actual_pwd+1);

				if(biometric_enable == 1){
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CREATE BIOMETRICS");
				}
				else{
					if(total_pwds != actual_pwd+1){
						Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CONTINUE");
					}
					else{
						Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "RECORD CRYPTOGRAM");
					}
				}
			}
			else{
				if(biometric_enable == 1){
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "CREATE BIOMETRICS");
				}
				else{
					Unicode::snprintf(btn_pwd_successBuffer, BTN_PWD_SUCCESS_SIZE, "RECORD CRYPTOGRAM");
				}
			}
		}

		/*** Clearing keyboard and display buffers ***/
		memset(keyboard1_text_typed_passwordBuffer, 0x00, sizeof(keyboard1_text_typed_passwordBuffer));
		memset(keyboard2_text_typed_passwordBuffer, 0x00, sizeof(keyboard2_text_typed_passwordBuffer));
		memset(keyboard1_text_typed_hide_passwordBuffer, 0x00, sizeof(keyboard1_text_typed_hide_passwordBuffer));
		memset(keyboard2_text_typed_hide_passwordBuffer, 0x00, sizeof(keyboard2_text_typed_hide_passwordBuffer));
		keyboard1_password.clearBuffer();
		keyboard2_password.clearBuffer();

		/*** Selecting visible/hidden elements on the screen ***/
		s5_1_typePasword.setVisible(false);
		s5_2_successPassword.setVisible(true);

		if(cuvex.info.mode == DARK){
			text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		}
		else{
			text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0x3F,0x3F,0x51));
		}
	}
	else
	{
		text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0xE7,0x44,0x3E));
	}

	/*** Screen update ***/
	background.invalidate();
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
void screen_flow_encrypt_part_2View::hideKeyboardAliasPressed()
{
	keyboard_text_info_1_alias.setPosition(0, 70, 320, 20);
	keyboard_text_info_2_alias.setPosition(0, 90, 320, 20);
	keyboard_text_area_alias.setXY(45, 120);
	keyboard_btn_enter_alias.setPosition(245, 120, 30, 30);

	if(keyboard_text_typed_alias.getTextWidth() <= 160){
		keyboard_text_typed_alias.setPosition(160-(keyboard_text_typed_alias.getTextWidth()/2), 125, keyboard_text_typed_alias.getTextWidth(), 20);
	}
	else{
		keyboard_text_typed_alias.setPosition(76, 125, 160, 20);
	}

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard_alias.setVisible(false);
	keyboard_btn_hide_alias.setVisible(false);
	keyboard_btn_show_alias.setVisible(true);
	keyboard_btn_enter_alias.setVisible(true);
	btn_assign_alias.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::showKeyboardAliasPressed()
{
	keyboard_text_info_1_alias.setPosition(0, 20, 320, 20);
	keyboard_text_info_2_alias.setPosition(0, 40, 320, 20);
	keyboard_text_area_alias.setXY(45, 70);
	keyboard_btn_enter_alias.setPosition(245, 70, 30, 30);

	if(keyboard_text_typed_alias.getTextWidth() <= 160){
		keyboard_text_typed_alias.setPosition(160-(keyboard_text_typed_alias.getTextWidth()/2), 75, keyboard_text_typed_alias.getTextWidth(), 20);
	}
	else{
		keyboard_text_typed_alias.setPosition(76, 75, 160, 20);
	}

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard_alias.setVisible(true);
	keyboard_btn_hide_alias.setVisible(true);
	keyboard_btn_show_alias.setVisible(false);
	keyboard_btn_enter_alias.setVisible(true);
	btn_assign_alias.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::enterKeyboardAliasPressed()
{
	if(keyboard_text_typed_aliasBuffer[0] != 0x00)
	{
		/*** Selecting visible/hidden elements on the screen ***/
		s6_alias.setVisible(false);
		s7_processing.setVisible(true);

		/*** Screen update ***/
		background.invalidate();
	}
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::eye1Pressed()
{
	if(keyboard1_pwd_eye_password.getCurrentlyDisplayedBitmap() == BITMAP_OJO_ABIERTO_ID){
		keyboard1_text_typed_password.setVisible(true);
		keyboard1_text_typed_hide_password.setVisible(false);
	}
	else{
		keyboard1_text_typed_password.setVisible(false);
		keyboard1_text_typed_hide_password.setVisible(true);
	}

	keyboard1_text_area_password.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::eye2Pressed()
{
	if(keyboard2_pwd_eye_password.getCurrentlyDisplayedBitmap() == BITMAP_OJO_ABIERTO_ID){
		keyboard2_text_typed_password.setVisible(true);
		keyboard2_text_typed_hide_password.setVisible(false);
	}
	else{
		keyboard2_text_typed_password.setVisible(false);
		keyboard2_text_typed_hide_password.setVisible(true);
	}

	keyboard2_text_area_password.invalidate();
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
void screen_flow_encrypt_part_2View::passwordSuccessPressed()
{
	actual_pwd++;

	/*** Biometrics ***/
	if(biometric_enable == 1)
	{
		uint8_t signature_sha256[32] = {0}, password_template[64] = {0};

		/*** Concatenate pwd_sha256 with cuvex_signature_sha256 ***/
		HAL_HASHEx_SHA256_Start(&hhash, (uint8_t *) cuvex.signature_buffer, SIGNATURE_SIZE, signature_sha256, HAL_MAX_DELAY);
		memcpy(password_template, pwds_sha256[actual_pwd-1], 32);
		memcpy(password_template + 32, signature_sha256, 32);

		/*** Update enable state, user template and remaining samples ***/
		cuvex.fingerprint.enable = true;
		cuvex.fingerprint.id_user = actual_pwd;
		cuvex.fingerprint.id_template = 1;
		cuvex.fingerprint.enroll_samples_remaining = ENROLL_NUM_SAMPLES;

		/*** Template 1 --> PBKDF2 salt + PBKDF2 key derived + AES iv ***/
		getRNG16Bytes(cuvex.fingerprint.signer[actual_pwd-1].template_1_pbkdf2_salt);
		pbkdf2_sha256(password_template, 64, cuvex.fingerprint.signer[actual_pwd-1].template_1_pbkdf2_salt, 16, 50000, cuvex.fingerprint.signer[actual_pwd-1].template_1_pbkdf2_key_derived, 32);
		getRNG16Bytes(cuvex.fingerprint.signer[actual_pwd-1].template_1_aes_iv);
		cuvex.fingerprint.signer[actual_pwd-1].template_1_aes_iv[12] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_1_aes_iv[13] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_1_aes_iv[14] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_1_aes_iv[15] = 0x02;

		/*** Template 2 --> PBKDF2 salt + PBKDF2 key derived + AES iv ***/
		getRNG16Bytes(cuvex.fingerprint.signer[actual_pwd-1].template_2_pbkdf2_salt);
		pbkdf2_sha256(password_template, 64, cuvex.fingerprint.signer[actual_pwd-1].template_2_pbkdf2_salt, 16, 50000, cuvex.fingerprint.signer[actual_pwd-1].template_2_pbkdf2_key_derived, 32);
		getRNG16Bytes(cuvex.fingerprint.signer[actual_pwd-1].template_2_aes_iv);
		cuvex.fingerprint.signer[actual_pwd-1].template_2_aes_iv[12] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_2_aes_iv[13] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_2_aes_iv[14] = 0x00;
		cuvex.fingerprint.signer[actual_pwd-1].template_2_aes_iv[15] = 0x02;

		/*** Queue msg to start enroll ***/
		screen_flow_encrypt_part_2View::getTemplate(GUI_TO_MAIN_GET_TEMPLATE);

		/*** Default texts and images + Set visual finger to enroll ***/
		if(total_pwds == 1){
			fingerprint_header.setVisible(false);
		}
		else{
			fingerprint_header.setVisible(true);
			Unicode::snprintf(fingerprint_headerBuffer, FINGERPRINT_HEADER_SIZE, "%d", actual_pwd);
		}

		fingerprint_right_hand_texts.setVisible(true);
		fingerprint_left_hand_texts.setVisible(false);
		fingerprint_common_texts.setVisible(false);
		fingerprint_1_image.setVisible(true);
		fingerprint_2_image.setVisible(false);
		fingerprint_3_image.setVisible(false);
		fingerprint_error_image.setVisible(false);
		fingerprint_success_image.setVisible(false);
	}

	/*** Selecting texts to display in the information fields ***/
	if(actual_pwd != total_pwds)
	{
		Unicode::UnicodeChar degree[] = {0x00B0,0};

		if(cuvex.info.language == SPANISH){
			if(total_pwds > 1){
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Persona N%s%d", degree, actual_pwd+1);
			}
			else{
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Password para cifrar");
			}
		}
		else{
			if(total_pwds > 1){
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Person N%s%d", degree, actual_pwd+1);
			}
			else{
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Password to encrypt");
			}
		}
	}

	/*** Selecting visible/hidden elements on the screen ***/
	if(biometric_enable == 1)
	{
		s5_1_typePasword.setVisible(false);
		s5_2_successPassword.setVisible(false);
		s5_3_enrollFingerprint.setVisible(true);
		s5_4_successBiometric.setVisible(false);
	}
	else
	{
		if(actual_pwd != total_pwds)
		{
			s5_1_typePasword.setVisible(true);
			s5_2_successPassword.setVisible(false);
			s5_3_enrollFingerprint.setVisible(false);
			s5_4_successBiometric.setVisible(false);
			placeholder_1_password.setVisible(true);
			placeholder_2_password.setVisible(true);
		}
		else
		{
			s5_password.setVisible(false);
			s6_alias.setVisible(true);
		}
	}

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::biometricsSuccessPressed()
{
	/*** Selecting visible/hidden elements on the screen ***/
	if(actual_pwd != total_pwds)
	{
		s5_1_typePasword.setVisible(true);
		s5_2_successPassword.setVisible(false);
		s5_3_enrollFingerprint.setVisible(false);
		s5_4_successBiometric.setVisible(false);
		placeholder_1_password.setVisible(true);
		placeholder_2_password.setVisible(true);
	}
	else{
		s5_password.setVisible(false);
		s6_alias.setVisible(true);
	}

	/*** Screen update ***/
	background.invalidate();
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
void screen_flow_encrypt_part_2View::biometricYesPressed()
{
	/*** Update biometric variable flag ***/
	biometric_enable = 0;

	/*** Selecting visible/hidden elements on the screen ***/
	s3_1_askBiometricControl.setVisible(false);
	s3_2_confirmBiometricControl.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::biometricNoPressed()
{
	/*** Update biometric variable flag ***/
	biometric_enable = 0;

	/*** Selecting visible/hidden elements on the screen ***/
	s3_biometricControl.setVisible(false);
	s4_multiSignature.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::biometricContinuePressed()
{
	/*** Update biometric variable flag ***/
	biometric_enable = 1;

	/*** Selecting visible/hidden elements on the screen ***/
	s3_biometricControl.setVisible(false);
	s4_multiSignature.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::biometricBackPressed()
{
	/*** Update biometric variable flag ***/
	biometric_enable = 0;

	/*** Selecting visible/hidden elements on the screen ***/
	s3_1_askBiometricControl.setVisible(true);
	s3_2_confirmBiometricControl.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
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
void screen_flow_encrypt_part_2View::multisignedYesPressed()
{
	/*** Selecting visible/hidden elements on the screen ***/
	s4_1_askMultiSignature.setVisible(false);
	s4_2_numMultiSignature.setVisible(true);
	text_info_multi_2.setVisible(true);
	text_info_multi_3.setVisible(false);
	text_info_multi_4.setVisible(false);
	text_info_multi_5.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::multisignedNoPressed()
{
	/*** Selecting visible/hidden elements on the screen ***/
	s4_multiSignature.setVisible(false);
	s5_password.setVisible(true);
	s5_1_typePasword.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::multisignedPlusPressed()
{
	Unicode::UnicodeChar aux_str[5] = {0};

	/*** Update of the number of passwords ***/
	if((text_info_multi_2.isVisible() == true) && (text_info_multi_3.isVisible() == false))
	{
		if(total_pwds < 6){
			Unicode::itoa(++total_pwds, aux_str, 5, 10);
			Unicode::snprintf(text_multi_numBuffer, TEXT_MULTI_NUM_SIZE, aux_str);
		}
	}
	else
	{
		if(mandatory_pwds < total_pwds){
			Unicode::itoa(++mandatory_pwds, aux_str, 5, 10);
			Unicode::snprintf(text_multi_numBuffer, TEXT_MULTI_NUM_SIZE, aux_str);
		}

		if((mandatory_pwds < total_pwds) && (biometric_enable == 0)){
			text_info_multi_4.setVisible(true);
			text_info_multi_5.setVisible(true);
		}
		else{
			text_info_multi_4.setVisible(false);
			text_info_multi_5.setVisible(false);
		}
	}

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::multisignedMinusPressed()
{
	Unicode::UnicodeChar aux_str[5] = {0};

	/*** Update of the number of passwords ***/
	if((text_info_multi_2.isVisible() == true) && (text_info_multi_3.isVisible() == false))
	{
		if(total_pwds > 1){
			Unicode::itoa(--total_pwds, aux_str, 5, 10);
			Unicode::snprintf(text_multi_numBuffer, TEXT_MULTI_NUM_SIZE, aux_str);
		}
	}
	else
	{
		if(mandatory_pwds > 1){
			Unicode::itoa(--mandatory_pwds, aux_str, 5, 10);
			Unicode::snprintf(text_multi_numBuffer, TEXT_MULTI_NUM_SIZE, aux_str);
		}

		if((mandatory_pwds < total_pwds) && (biometric_enable == 0)){
			text_info_multi_4.setVisible(true);
			text_info_multi_5.setVisible(true);
		}
		else{
			text_info_multi_4.setVisible(false);
			text_info_multi_5.setVisible(false);
		}
	}

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::multisignedSelectPressed()
{
	Unicode::UnicodeChar degree[] = {0x00B0,0};

	/*** Assign variables ***/
	cuvex.tag.multisigned_total = total_pwds;
	cuvex.tag.multisigned_mandatory = mandatory_pwds;

	/*** Selecting visible/hidden elements on the screen ***/
	if((total_pwds > 1) && (text_info_multi_2.isVisible() == true) && (text_info_multi_3.isVisible() == false))
	{
		mandatory_pwds = total_pwds;

		text_info_multi_2.setVisible(false);
		text_info_multi_3.setVisible(true);
		text_info_multi_4.setVisible(false);
		text_info_multi_5.setVisible(false);
	}
	else
	{
		if(cuvex.info.language == SPANISH){
			if(total_pwds > 1){
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Persona N%s%d", degree, actual_pwd+1);
			}
			else{
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Password para cifrar");
			}
		}
		else{
			if(total_pwds > 1){
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Person N%s%d", degree, actual_pwd+1);
			}
			else{
				Unicode::snprintf(text_info_1_passwordBuffer, TEXT_INFO_1_PASSWORD_SIZE, "Password to encrypt");
			}
		}

		s4_multiSignature.setVisible(false);
		s5_password.setVisible(true);
		s5_1_typePasword.setVisible(true);
	}

	/*** Screen update ***/
	background.invalidate();
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
void screen_flow_encrypt_part_2View::success1Pressed()
{
	/*** Selecting visible/hidden elements on the screen ***/
	success_msg_1.setVisible(false);
	success_msg_2.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::success2Pressed()
{
	NVIC_SystemReset();
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::setScreenMode()
{
	/*** Setting screen elements based on mode (dark/light) ***/
	if(cuvex.info.mode == DARK)
	{
		background.setColor(touchgfx::Color::getColorFromRGB(0x3F,0x3F,0x51));
		/***/
		text_info_biometric_1.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_biometric_2.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_biometric_3.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_biometric_yes.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_biometric_no.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_biometric_continue.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_biometric_back.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		/***/
		text_info_multi_1.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_multi_2.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_multi_3.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_multi_4.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_multi_5.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_multi_num.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_multi_yes.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_multi_no.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_multi_select.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_multi_plus.setIconBitmaps(Bitmap(BITMAP_PLUS_DARK_ID), Bitmap(BITMAP_PLUS_DARK_ID));
		btn_multi_minus.setIconBitmaps(Bitmap(BITMAP_MINUS_DARK_ID), Bitmap(BITMAP_MINUS_DARK_ID));
		/***/
		keyboard_btn_enter_password.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard1_text_typed_password.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard2_text_typed_password.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard1_text_typed_hide_password.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard2_text_typed_hide_password.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		text_info_1_password.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_info_2_password.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_pwd_success.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		text_pwd_success_info.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		fingerprint_header.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_right_hand_info1.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_right_hand_info2.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_left_hand_info1.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_left_hand_info2.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_common_info1.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		fingerprint_common_info2.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		text_bio_success_info.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_bio_success.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		/***/
		btn_assign_alias.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard_text_info_1_alias.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard_text_info_2_alias.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		processing_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		success_1_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		success_2_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_success_1.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_success_2.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
	}

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::setScreenLanguage()
{
	/*** Text configuration based on selected language (Spanish/English) ***/
	if(cuvex.info.language == SPANISH){
		Texts::setLanguage(SP);
	}
	else{
		Texts::setLanguage(GB);
	}

	/*** Screen update ***/
	background.invalidate();
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::generateRecordData1_Alias()
{
	memset(cuvex.tag.alias, 0x00, SIZE_ALIAS);

	for(int i=0; i<SIZE_ALIAS; i++)
	{
		if(keyboard_text_typed_aliasBuffer[i] == 8364){	//If '€' (8364) is converted to the character '¶' (182) to only occupy 1 byte
			cuvex.tag.alias[i] = 182;
		}
		else{
			cuvex.tag.alias[i] = (uint8_t) keyboard_text_typed_aliasBuffer[i];
		}
	}

	cuvex.tag.alias_lenght = strlen((char *) cuvex.tag.alias);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::generateRecordData2_Cryptogram()
{
	uint8_t text_to_encrypt[SIZE_CRYPT] = {0};

	memset(cuvex.tag.cryptogram, 0x00, SIZE_CRYPT);
	memset(pwds_sha256_concat, 0x00, sizeof(pwds_sha256_concat));
	memset(pwds_key_pbkdf2, 0x00, sizeof(pwds_key_pbkdf2));
	memset(salt_pbkdf2, 0x00, sizeof(salt_pbkdf2));

	/*** 1) Sort the keys lexicographically ***/
	sortPasswordsLexicographically(pwds_sha256, total_pwds);

	/*** 2) Concatenate keys ***/
	for(int i=0; i<total_pwds; i++){
		memcpy(&pwds_sha256_concat[i*32], pwds_sha256[i], 32);
	}

	if(biometric_enable == 1){
		uint8_t signature_sha256[32] = {0};
		HAL_HASHEx_SHA256_Start(&hhash, (uint8_t *) cuvex.signature_buffer, SIGNATURE_SIZE, signature_sha256, HAL_MAX_DELAY);
		memcpy(&pwds_sha256_concat[total_pwds*32], signature_sha256, 32);
	}

	/*** 3) Get salt + PBKDF2-HMAC-SHA256 ***/
	getRNG16Bytes(salt_pbkdf2);

	if(biometric_enable == 1){
		pbkdf2_sha256(pwds_sha256_concat, 32*(total_pwds + 1), salt_pbkdf2, sizeof(salt_pbkdf2), 50000, pwds_key_pbkdf2, sizeof(pwds_key_pbkdf2));
	}
	else{
		pbkdf2_sha256(pwds_sha256_concat, 32*total_pwds, salt_pbkdf2, sizeof(salt_pbkdf2), 50000, pwds_key_pbkdf2, sizeof(pwds_key_pbkdf2));
	}

	/*** 4) AES-256 peripheral configuration --> password + initialization vector (12Bytes nonce + 4B counter) ***/
	getRNG16Bytes(iv_aes_gcm);
	iv_aes_gcm[12] = 0x00;
	iv_aes_gcm[13] = 0x00;
	iv_aes_gcm[14] = 0x00;
	iv_aes_gcm[15] = 0x02;

	configAESPeripheral(pwds_key_pbkdf2, iv_aes_gcm);

	/*** 5) Format the text to be encrypted + Generate AES-256 cryptogram ***/
	switch(cuvex.encrypt.text_type)
	{
	case TEXT_TYPE_NONE:
	default:
		break;

	case TEXT_TYPE_BIP39:
		/*** Format the text to be encrypted ***/
		strcat((char*) text_to_encrypt, (const char*) "[bip39]");
		for(int i=0; i<cuvex.encrypt.total_words; i++){
			strcat((char*) text_to_encrypt, (const char*) ",");
			strcat((char*) text_to_encrypt, (const char*) cuvex.encrypt.words_to_encrypt[i]);
		}
		strcat((char*) text_to_encrypt, (const char*) "[passphrase]");
		strcat((char*) text_to_encrypt, (char*) cuvex.encrypt.buff_passphrase);

		/*** Generate cryptogram ***/
		generateCryptogram(text_to_encrypt);
		break;

	case TEXT_TYPE_SLIP39:
		/*** Format the text to be encrypted ***/
		strcat((char*) text_to_encrypt, (const char*) "[slip39]");
		for(int i=0; i<cuvex.encrypt.total_words; i++){
			strcat((char*) text_to_encrypt, (const char*) ",");
			strcat((char*) text_to_encrypt, (const char*) cuvex.encrypt.words_to_encrypt[i]);
		}
		strcat((char*) text_to_encrypt, (const char*) "[passphrase]");
		strcat((char*) text_to_encrypt, (char*) cuvex.encrypt.buff_passphrase);

		/*** Generate cryptogram ***/
		generateCryptogram(text_to_encrypt);
		break;

	case TEXT_TYPE_XMR:
		/*** Format the text to be encrypted ***/
		strcat((char*) text_to_encrypt, (const char*) "[xmr]");
		for(int i=0; i<cuvex.encrypt.total_words; i++){
			strcat((char*) text_to_encrypt, (const char*) ",");
			strcat((char*) text_to_encrypt, (const char*) cuvex.encrypt.words_to_encrypt[i]);
		}
		strcat((char*) text_to_encrypt, (const char*) "[passphrase]");
		strcat((char*) text_to_encrypt, (char*) cuvex.encrypt.buff_passphrase);

		/*** Generate cryptogram ***/
		generateCryptogram(text_to_encrypt);
		break;

	case TEXT_TYPE_PLAINTEXT:
		/*** Format the text to be encrypted ***/
		strcat((char*) text_to_encrypt, (char*) "[plain-text]");
		strcat((char*) text_to_encrypt, (char*) cuvex.encrypt.buff_plain_text);

		/*** Generate cryptogram ***/
		generateCryptogram(text_to_encrypt);
		break;

	case TEXT_TYPE_FROM_WALLET_BIP39:
		/*** Format the text to be encrypted ***/
		strcat((char*) text_to_encrypt, (const char*) "{bip39}");
		for(int i=0; i<24; i++){
			strcat((char*) text_to_encrypt, (const char*) ",");
			strcat((char*) text_to_encrypt, (const char*) cuvex.wallet.words_to_encrypt[i]);
		}
		strcat((char*) text_to_encrypt, (const char*) "{passder}");
		strcat((char*) text_to_encrypt, (char*) cuvex.wallet.pass_deriv);
		strcat((char*) text_to_encrypt, (const char*) "{prikey}");
		strcat((char*) text_to_encrypt, (char*) cuvex.wallet.zprv_key);
		strcat((char*) text_to_encrypt, (const char*) "{pubkey}");
		strcat((char*) text_to_encrypt, (char*) cuvex.wallet.zpub_key);

		/*** Generate cryptogram ***/
		generateCryptogram(text_to_encrypt);
		break;
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::generateRecordData3_Information()
{
    enc_fields_t enc;
    uint16_t ascii_len = 0;
    char enc_token[ENC_TOKEN_LEN + 1] = {0};

    /*** Clear destination buffer --> Record info format: ["E123456789K"][salt_pbkdf2][iv_aes_gcm] ***/
    memset(cuvex.tag.information, 0x00, SIZE_INFORMATION);

    /*** Fill encoder fields ***/
    enc.f1 = FW_VER_F1;					//vf1 (0-9)
    enc.f2 = FW_VER_F2;					//vf2 (0-9)
    enc.f3 = FW_VER_F3;					//vf3 (0-9)
    enc.hw = HARDWARE_VERSION() - '0';	//vh  (1-9)
    enc.mx = total_pwds;          		//tot (1-6)
    enc.my = mandatory_pwds;     		//man (1-6)
    enc.p  = 0;							//pkt (0-1)
    enc.c  = 0;							//clo (0-1)
    enc.b  = biometric_enable ? 1 : 0;	//bit (0-1)

    /*** Encode reduced token ***/
    enc_encode_fields_to_reduced(&enc, enc_token, sizeof(enc_token));

    /*** ASCII section + Get length of ASCII section ***/
    memcpy(cuvex.tag.information, enc_token, ENC_TOKEN_LEN);
    ascii_len = strlen((char *) cuvex.tag.information);

    /*** BINARY section ***/
    memcpy(cuvex.tag.information + ascii_len, salt_pbkdf2, sizeof(salt_pbkdf2));
    memcpy(cuvex.tag.information + ascii_len + sizeof(salt_pbkdf2), iv_aes_gcm, sizeof(iv_aes_gcm));

    /*** Information lenght ***/
    cuvex.tag.information_lenght = ascii_len + sizeof(salt_pbkdf2) + sizeof(iv_aes_gcm);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::generateRecordData4_Multisignature()
{
	char comb_buffer[10][KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE] = {0};

	memset(cuvex.tag.multisignature, 0x00, SIZE_MULTISIGN);
	generateCombinations(0, 0, comb_buffer);
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::configAESPeripheral(uint8_t keyAES[], uint8_t ivAES[])
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
void screen_flow_encrypt_part_2View::generateCryptogram(uint8_t *text_to_encrypt)
{
	/*** Convert text to encrypt under format uint32_t --> Data is packed/unpacked in big-endian order (AES hardware requirement) ***/
	for(uint8_t k=0; k<SIZE_CRYPT_MSG; k++){
		cuvex.tag.new_text_to_encrypt[k] = ((uint32_t) text_to_encrypt[k*4] << 24) | ((uint32_t) text_to_encrypt[(k*4)+1] << 16) | ((uint32_t) text_to_encrypt[(k*4)+2] << 8) | ((uint32_t) text_to_encrypt[(k*4)+3]);
	}

	/*** Encrypt the text ***/
	HAL_CRYP_Encrypt(&hcryp, (uint32_t *) cuvex.tag.new_text_to_encrypt, SIZE_CRYPT_MSG, (uint32_t *) cuvex.tag.new_text_encrypted, HAL_MAX_DELAY);

	/*** Convert text encrypted under format uint8_t ***/
	for(uint8_t z=0; z<SIZE_CRYPT_MSG; z++){
		cuvex.tag.cryptogram[z*4] 	  = (uint8_t) (cuvex.tag.new_text_encrypted[z] >> 24);
		cuvex.tag.cryptogram[(z*4)+1] = (uint8_t) (cuvex.tag.new_text_encrypted[z] >> 16);
		cuvex.tag.cryptogram[(z*4)+2] = (uint8_t) (cuvex.tag.new_text_encrypted[z] >> 8);
		cuvex.tag.cryptogram[(z*4)+3] = (uint8_t) (cuvex.tag.new_text_encrypted[z]);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::generateCombinations(int start, int index, char comb_buffer[][KEYBOARD1_TEXT_TYPED_PASSWORD_SIZE])
{
	uint32_t text_to_encrypt_u32[8] = {0}, text_encrypted_u32[8] = {0};
	uint8_t text_encrypted_u8[32] = {0};
	uint8_t salt_pbkdf2_combined[16] = {0};
	uint8_t pwds_sha256_combined[320] = {0};
	uint8_t pwds_key_pbkdf2_combined[32] = {0};
	uint8_t iv_aes_gcm_combined[16] = {0};
	static int count_bytes = 0;

	if(index == mandatory_pwds)
	{
		/*** 1) Concatenate keys combined (already ordered) ***/
		for(int i=0; i<mandatory_pwds; i++){
			memcpy(pwds_sha256_combined + (i*32), comb_buffer[i], 32);
		}

		if(biometric_enable == 1){
			uint8_t signature_sha256[32] = {0};
			HAL_HASHEx_SHA256_Start(&hhash, (uint8_t *) cuvex.signature_buffer, SIGNATURE_SIZE, signature_sha256, HAL_MAX_DELAY);
			memcpy(pwds_sha256_combined + (mandatory_pwds*32), signature_sha256, 32);
		}

		/*** 2) Get salt + PBKDF2-HMAC-SHA256 ***/
		getRNG16Bytes(salt_pbkdf2_combined);

		if(biometric_enable == 1){
			pbkdf2_sha256(pwds_sha256_combined, 32*(mandatory_pwds + 1), salt_pbkdf2_combined, sizeof(salt_pbkdf2_combined), 50000, pwds_key_pbkdf2_combined, sizeof(pwds_key_pbkdf2_combined));
		}
		else{
			pbkdf2_sha256(pwds_sha256_combined, 32*mandatory_pwds, salt_pbkdf2_combined, sizeof(salt_pbkdf2_combined), 50000, pwds_key_pbkdf2_combined, sizeof(pwds_key_pbkdf2_combined));
		}


		/*** 3) AES-256 peripheral configuration --> password + initialization vector (12Bytes nonce + 4B counter) ***/
		getRNG16Bytes(iv_aes_gcm_combined);
		iv_aes_gcm_combined[12] = 0x00;
		iv_aes_gcm_combined[13] = 0x00;
		iv_aes_gcm_combined[14] = 0x00;
		iv_aes_gcm_combined[15] = 0x02;

		configAESPeripheral(pwds_key_pbkdf2_combined, iv_aes_gcm_combined);

		/*** Convert text to encrypt under format uint32_t --> Data is packed/unpacked in big-endian order (AES hardware requirement) ***/
		for(uint8_t k=0; k<32/4; k++){
			text_to_encrypt_u32[k] = ((uint32_t) pwds_key_pbkdf2[k*4] << 24) | ((uint32_t) pwds_key_pbkdf2[(k*4)+1] << 16) | ((uint32_t) pwds_key_pbkdf2[(k*4)+2] << 8) | ((uint32_t) pwds_key_pbkdf2[(k*4)+3]);
		}

		/*** Encrypt the text ***/
		HAL_CRYP_Encrypt(&hcryp, (uint32_t *) text_to_encrypt_u32, 32/4, (uint32_t *) text_encrypted_u32, HAL_MAX_DELAY);

		/*** Convert text encrypted under format uint8_t ***/
		for(uint8_t z=0; z<32/4; z++)
		{
			text_encrypted_u8[z*4] 	   = (uint8_t) (text_encrypted_u32[z] >> 24);
			text_encrypted_u8[(z*4)+1] = (uint8_t) (text_encrypted_u32[z] >> 16);
			text_encrypted_u8[(z*4)+2] = (uint8_t) (text_encrypted_u32[z] >> 8);
			text_encrypted_u8[(z*4)+3] = (uint8_t) (text_encrypted_u32[z]);
		}

		/*** Concatenate combination in buffer ***/
		memcpy(cuvex.tag.multisignature + count_bytes, text_encrypted_u8, 32);
		memcpy(cuvex.tag.multisignature + count_bytes + 32, salt_pbkdf2_combined, 16);
		memcpy(cuvex.tag.multisignature + count_bytes + 32 + 16, iv_aes_gcm_combined, 16);
		count_bytes = count_bytes + 64;
		cuvex.tag.multisignature_lenght = count_bytes;

		/*** Function return ***/
		return;
	}

	for(int i=start; i<=total_pwds-(mandatory_pwds-index); i++)
	{
		memcpy(comb_buffer[index], pwds_sha256[i], 32);
		generateCombinations(i+1, index+1, comb_buffer);
	}
}

/*************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************
 *************************************************************************************************************************************************************************************************************/

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_2View::sortPasswordsLexicographically(uint8_t pwds[10][32], uint8_t num_pwd)
{
	uint8_t tmp[32] = {0};

	for(int i=0; i<num_pwd-1; i++)
	{
		for(int j=i+1; j<num_pwd; j++)
		{
			if(memcmp(pwds[i], pwds[j], 32) > 0)
			{
				memcpy(tmp, pwds[i], 32);
				memcpy(pwds[i], pwds[j], 32);
				memcpy(pwds[j], tmp, 32);
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
uint8_t screen_flow_encrypt_part_2View::getRNG16Bytes(uint8_t *buffer)
{
	uint32_t random32 = 0x00;

	for(int i=0; i<4; i++)
	{
		if(HAL_RNG_GenerateRandomNumber(&hrng, &random32) != HAL_OK){
			return ERROR;
		}

		buffer[i*4 + 0] = (uint8_t) (random32 >> 24);
		buffer[i*4 + 1] = (uint8_t) (random32 >> 16);
		buffer[i*4 + 2] = (uint8_t) (random32 >> 8);
		buffer[i*4 + 3] = (uint8_t) (random32);
	}

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
static uint8_t sha256_hw(const uint8_t *data, size_t len, uint8_t out[32])
{
	if(HAL_HASHEx_SHA256_Start(&hhash, (uint8_t *) data, len, out, HAL_MAX_DELAY) != HAL_OK){
		return ERROR;
	}

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
static uint8_t hmac_sha256_init(hmac_sha256_ctx_t *ctx, const uint8_t *key, size_t key_len)
{
	uint8_t key_block[64] = {0};

	/*** Key normalization ***/
	if(key_len > 64){
		if(sha256_hw(key, key_len, key_block) != SUCCESS){
			return ERROR;
		}
	}
	else{
		memcpy(key_block, key, key_len);
	}

	/*** Precompute ipad/opad ***/
	for(int i=0; i<64; i++){
		ctx->ipad[i] = key_block[i] ^ 0x36;
		ctx->opad[i] = key_block[i] ^ 0x5C;
	}

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
static uint8_t hmac_sha256_fast(const hmac_sha256_ctx_t *ctx, const uint8_t *data, size_t data_len, uint8_t out[32])
{
	uint8_t inner_hash[32];
	uint8_t inner_buf[64 + data_len];
	uint8_t outer_buf[64 + 32];

	/*** inner = SHA256(ipad || data) ***/
	memcpy(inner_buf, ctx->ipad, 64);
	memcpy(inner_buf + 64, data, data_len);

	if(sha256_hw(inner_buf, sizeof(inner_buf), inner_hash) != SUCCESS){
		return ERROR;
	}

	/*** outer = SHA256(opad || inner_hash) ***/
	memcpy(outer_buf, ctx->opad, 64);
	memcpy(outer_buf + 64, inner_hash, 32);

	return sha256_hw(outer_buf, sizeof(outer_buf), out);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t screen_flow_encrypt_part_2View::pbkdf2_sha256(const uint8_t *password, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, uint8_t *derived_key, size_t dk_len)
{
	hmac_sha256_ctx_t hmac_ctx;
	uint8_t U[32], T[32];
	uint8_t salt_block[salt_len + 4];
	uint32_t block_index = 1;

	/*** Init HMAC context ***/
	if(hmac_sha256_init(&hmac_ctx, password, pass_len) != SUCCESS){
		return ERROR;
	}

	memcpy(salt_block, salt, salt_len);

	while(dk_len > 0)
	{
		/***salt || INT(block_index) (big-endian) ***/
		salt_block[salt_len + 0] = (block_index >> 24) & 0xFF;
		salt_block[salt_len + 1] = (block_index >> 16) & 0xFF;
		salt_block[salt_len + 2] = (block_index >> 8) & 0xFF;
		salt_block[salt_len + 3] = block_index & 0xFF;

		/*** U1 ***/
		if(hmac_sha256_fast(&hmac_ctx, salt_block, sizeof(salt_block), U) != SUCCESS){
			return ERROR;
		}

		memcpy(T, U, 32);

		/*** U2 .. Uc ***/
		for(uint32_t i=1; i<iterations; i++){
			if(hmac_sha256_fast(&hmac_ctx, U, 32, U) != SUCCESS){
				return ERROR;
			}

			for(int j=0; j<32; j++){
				T[j] ^= U[j];
			}
		}

		/*** Copy block to output ***/
		size_t copy_len = (dk_len > 32) ? 32 : dk_len;

		memcpy(derived_key, T, copy_len);

		derived_key += copy_len;
		dk_len -= copy_len;
		block_index++;
	}

	return SUCCESS;
}

/*
 *
 * Software --> 50.000 iter = 7500ms
 *
 */

///**************************************************************************************************************************************
// ***** Function 	: N/A
// ***** Description 	: N/A
// ***** Parameters 	: N/A
// ***** Response 	: N/A
// **************************************************************************************************************************************/
//uint8_t screen_flow_encrypt_part_2View::pbkdf2_sha256(const uint8_t *password, size_t pass_len, const uint8_t *salt, size_t salt_len, uint32_t iterations, uint8_t *derived_key, size_t dk_len)
//{
//	const mbedtls_md_info_t *md_info = mbedtls_md_info_from_type(MBEDTLS_MD_SHA256);
//
//	if(md_info != NULL)
//	{
//		/*** Init HMAC context ***/
//		mbedtls_md_context_t ctx;
//		mbedtls_md_init(&ctx);
//
//		/*** PBKDF2-HMAC derivation + Clear HMAC context ***/
//		if(mbedtls_md_setup(&ctx, md_info, 1) == 0){
//			if(mbedtls_pkcs5_pbkdf2_hmac(&ctx, password, pass_len, salt, salt_len, iterations, dk_len, derived_key) == 0){
//				mbedtls_md_free(&ctx);
//				return SUCCESS;
//			}
//		}
//		mbedtls_md_free(&ctx);
//	}
//
//	return ERROR;
//}

