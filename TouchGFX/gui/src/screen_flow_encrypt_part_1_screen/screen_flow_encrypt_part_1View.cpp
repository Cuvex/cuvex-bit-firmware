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

#include <gui/screen_flow_encrypt_part_1_screen/screen_flow_encrypt_part_1View.hpp>
#include "main.h"

extern struct cuvex cuvex;

screen_flow_encrypt_part_1View::screen_flow_encrypt_part_1View(): actual_word(0), index_words(0), valid_word(0), flag_refresh_text_area(0), words_to_check{0}
{

}

void screen_flow_encrypt_part_1View::setupScreen()
{
	screen_flow_encrypt_part_1ViewBase::setupScreen();
	screen_flow_encrypt_part_1View::setScreenMode();
	screen_flow_encrypt_part_1View::setScreenLanguage();
	screen_flow_encrypt_part_1View::changeScreen(GUI_TO_MAIN_SCREEN_FLOW_ENCRYPT);

	memset(keyboard1_text_typed_seedBuffer,   		0x00, sizeof(keyboard1_text_typed_seedBuffer));
	memset(keyboard2_text_typed_seedBuffer,   		0x00, sizeof(keyboard2_text_typed_seedBuffer));
	memset(keyboard3_text_typed_seedBuffer,   		0x00, sizeof(keyboard3_text_typed_seedBuffer));
	memset(keyboard_text_typed_plain_textBuffer,	0x00, sizeof(keyboard_text_typed_plain_textBuffer));
	keyboard1_seed.clearBuffer();
	keyboard2_seed.clearBuffer();
	keyboard3_seed.clearBuffer();
	keyboard_plain_text.clearBuffer();
}

void screen_flow_encrypt_part_1View::tearDownScreen()
{
	screen_flow_encrypt_part_1ViewBase::tearDownScreen();
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
void screen_flow_encrypt_part_1View::tickEventScreen()
{
	uint8_t num_of_lines = 0;
	uint8_t str_prediction[KEYBOARD2_TEXT_PREDICTED_SEED_SIZE] = {0};

	/*** Seed's - Keyboard 1 (numeric) ***/
	if(keyboard1_seed.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard1_text_typed_seedBuffer, keyboard1_seed.getBuffer(), KEYBOARD1_TEXT_TYPED_SEED_SIZE) != 0)
		{
			if(Unicode::atoi(keyboard1_seed.getBuffer()) == 0){
				memset(keyboard1_text_typed_seedBuffer, 0x00, sizeof(keyboard1_text_typed_seedBuffer));
				keyboard1_seed.clearBuffer();
			}
			else if(Unicode::atoi(keyboard1_seed.getBuffer()) > 54){
				keyboard1_text_typed_seed.setColor(touchgfx::Color::getColorFromRGB(0xE7,0x44,0x3E));
			}
			else{
				keyboard1_text_typed_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
			}

			Unicode::snprintf(keyboard1_text_typed_seedBuffer, KEYBOARD1_TEXT_TYPED_SEED_SIZE, keyboard1_seed.getBuffer());
			keyboard1_text_area_seed.invalidate();
		}
	}

	/*** Seed's - Keyboard 2 (words) ***/
	if(keyboard2_seed.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard2_text_typed_seedBuffer, keyboard2_seed.getBuffer(), KEYBOARD2_TEXT_TYPED_SEED_SIZE) != 0)
		{
			Unicode::snprintf(keyboard2_text_typed_seedBuffer, KEYBOARD2_TEXT_TYPED_SEED_SIZE, keyboard2_seed.getBuffer());
			Unicode::snprintf(keyboard2_text_predicted_seedBuffer, KEYBOARD2_TEXT_PREDICTED_SEED_SIZE, keyboard2_seed.getBuffer());

			if(Unicode::strlen(keyboard2_text_typed_seedBuffer) >= 3){
				Unicode::toUTF8(keyboard2_text_predicted_seedBuffer, str_prediction, KEYBOARD2_TEXT_PREDICTED_SEED_SIZE);

				switch(cuvex.encrypt.text_type)
				{
				case TEXT_TYPE_NONE:
				default:
					valid_word = false;
					break;

				case TEXT_TYPE_BIP39:
					valid_word = getBip39Word((char *) str_prediction);
					break;

				case TEXT_TYPE_SLIP39:
					valid_word = getSlip39Word((char *) str_prediction);
					break;

				case TEXT_TYPE_XMR:
					valid_word = getXmrWord((char *) str_prediction);
					break;
				}

				Unicode::snprintf(keyboard2_text_predicted_seedBuffer, KEYBOARD2_TEXT_PREDICTED_SEED_SIZE, (char *) str_prediction);
				keyboard2_text_predicted_seed.setVisible(true);
				keyboard2_text_typed_seed.setVisible(false);
			}
			else{
				keyboard2_text_predicted_seed.setVisible(false);
				keyboard2_text_typed_seed.setVisible(true);
				valid_word = false;
			}

			if(valid_word == true){
				keyboard2_text_predicted_seed.setColor(touchgfx::Color::getColorFromRGB(0x14,0x8E,0x53));
			}
			else{
				keyboard2_text_predicted_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
			}

			keyboard2_text_area_seed.invalidate();
		}
	}

	/*** Seed's - Keyboard 3 (passphrase) ***/
	if(keyboard3_seed.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard3_text_typed_seedBuffer, keyboard3_seed.getBuffer(), KEYBOARD3_TEXT_TYPED_SEED_SIZE) != 0)
		{
			Unicode::snprintf(keyboard3_text_typed_seedBuffer, KEYBOARD3_TEXT_TYPED_SEED_SIZE, keyboard3_seed.getBuffer());
			keyboard3_text_typed_seed.setWideTextAction(touchgfx::WIDE_TEXT_CHARWRAP);

			num_of_lines = keyboard3_text_typed_seed.getTextWidth()/230;

			if(num_of_lines > 1){
				keyboard3_text_typed_seed.setPosition(60, 50-((num_of_lines-1)*20), 230, 60+((num_of_lines-1)*20));
			}
			else{
				keyboard3_text_typed_seed.setPosition(60, 50, 230, 60);
			}

			Unicode::itoa(Unicode::strlen(keyboard3_text_typed_seedBuffer), keyboard3_char_count_seedBuffer, KEYBOARD3_CHAR_COUNT_SEED_SIZE, 10);

			keyboard3_text_area_seed.invalidate();
		}

		if(flag_refresh_text_area == 1)
		{
			num_of_lines = keyboard3_text_typed_seed.getTextWidth()/230;

			if(num_of_lines > 1){
				keyboard3_text_typed_seed.setPosition(60, 50-((num_of_lines-1)*20), 230, 60+((num_of_lines-1)*20));
				keyboard3_text_area_seed.invalidate();
			}

			flag_refresh_text_area = 0;
		}
	}

	/*** PlainText - Keyboard (plain text) ***/
	if(keyboard_plain_text.isVisible() == true)
	{
		if(Unicode::strncmp(keyboard_text_typed_plain_textBuffer, keyboard_plain_text.getBuffer(), KEYBOARD_TEXT_TYPED_PLAIN_TEXT_SIZE) != 0)
		{
			Unicode::snprintf(keyboard_text_typed_plain_textBuffer, KEYBOARD_TEXT_TYPED_PLAIN_TEXT_SIZE, keyboard_plain_text.getBuffer());
			keyboard_text_typed_plain_text.setWideTextAction(touchgfx::WIDE_TEXT_CHARWRAP);

			num_of_lines = keyboard_text_typed_plain_text.getTextWidth()/230;

			if(num_of_lines > 1){
				keyboard_text_typed_plain_text.setPosition(60, 50-((num_of_lines-1)*20), 230, 60+((num_of_lines-1)*20));
			}
			else{
				keyboard_text_typed_plain_text.setPosition(60, 50, 230, 60);
			}

			Unicode::itoa(Unicode::strlen(keyboard_text_typed_plain_textBuffer), keyboard_char_count_plain_textBuffer, KEYBOARD_CHAR_COUNT_PLAIN_TEXT_SIZE, 10);

			keyboard_text_area_plain_text.invalidate();
		}

		if(flag_refresh_text_area == 1)
		{
			num_of_lines = keyboard_text_typed_plain_text.getTextWidth()/230;

			if(num_of_lines > 1){
				keyboard_text_typed_plain_text.setPosition(60, 50-((num_of_lines-1)*20), 230, 60+((num_of_lines-1)*20));
				keyboard_text_area_plain_text.invalidate();
			}

			flag_refresh_text_area = 0;
		}
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
void screen_flow_encrypt_part_1View::changeScreen(uint8_t screen)
{
	presenter->changeScreen(screen);
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
void screen_flow_encrypt_part_1View::hideKeyboard1SeedPressed()
{
	keyboard1_text_info_seed.setPosition(0, 90, 320, 20);
	keyboard1_text_typed_seed.setPosition(76, 125, 169, 20);
	keyboard1_text_area_seed.setXY(45, 120);
	keyboard1_btn_enter_seed.setPosition(245, 120, 30, 30);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard1_seed.setVisible(false);
	keyboard1_btn_hide_seed.setVisible(false);
	keyboard1_btn_show_seed.setVisible(true);
	keyboard1_btn_enter_seed.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::showKeyboard1SeedPressed()
{
	keyboard1_text_info_seed.setPosition(0, 40, 320, 20);
	keyboard1_text_typed_seed.setPosition(76, 75, 169, 20);
	keyboard1_text_area_seed.setXY(45, 70);
	keyboard1_btn_enter_seed.setPosition(245, 70, 30, 30);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard1_seed.setVisible(true);
	keyboard1_btn_hide_seed.setVisible(true);
	keyboard1_btn_show_seed.setVisible(false);
	keyboard1_btn_enter_seed.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::enterKeyboard1SeedPressed()
{
	cuvex.encrypt.total_words = Unicode::atoi(keyboard1_text_typed_seedBuffer);
	actual_word = 1;

	if((cuvex.encrypt.total_words > 0) && (cuvex.encrypt.total_words <= 54))
	{
		/*** Editing information in the next sub-screen ***/
		Unicode::snprintf(keyboard2_text_info_seedBuffer1, KEYBOARD2_TEXT_INFO_SEEDBUFFER1_SIZE, "%d", actual_word);
		Unicode::snprintf(keyboard2_text_info_seedBuffer2, KEYBOARD2_TEXT_INFO_SEEDBUFFER2_SIZE, "%d", cuvex.encrypt.total_words);

		/*** Clearing keyboard and display buffers ***/
		memset(keyboard1_text_typed_seedBuffer, 0x00, sizeof(keyboard1_text_typed_seedBuffer));
		keyboard1_seed.clearBuffer();

		/*** Selecting visible/hidden elements on the screen ***/
		s2_seed_1_typeNumWords.setVisible(false);
		s2_seed_2_typeWords.setVisible(true);

		/*** Screen update ***/
		background.invalidate();
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
void screen_flow_encrypt_part_1View::hideKeyboard2SeedPressed()
{
	keyboard2_text_info_seed.setPosition(0, 90, 320, 20);
	keyboard2_text_typed_seed.setPosition(76, 125, 169, 20);
	keyboard2_text_predicted_seed.setPosition(76, 125, 169, 20);
	keyboard2_text_area_seed.setXY(45, 120);
	keyboard2_btn_enter_seed.setPosition(245, 120, 30, 30);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard2_seed.setVisible(false);
	keyboard2_btn_hide_seed.setVisible(false);
	keyboard2_btn_show_seed.setVisible(true);
	keyboard2_btn_enter_seed.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::showKeyboard2SeedPressed()
{
	keyboard2_text_info_seed.setPosition(0, 40, 320, 20);
	keyboard2_text_typed_seed.setPosition(76, 75, 169, 20);
	keyboard2_text_predicted_seed.setPosition(76, 75, 169, 20);
	keyboard2_text_area_seed.setXY(45, 70);
	keyboard2_btn_enter_seed.setPosition(245, 70, 30, 30);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard2_seed.setVisible(true);
	keyboard2_btn_hide_seed.setVisible(true);
	keyboard2_btn_show_seed.setVisible(false);
	keyboard2_btn_enter_seed.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::enterKeyboard2SeedPressed()
{
	if((keyboard2_text_typed_seedBuffer[0] != 0x00) && (valid_word == true))
	{
		/*** Obtaining the typed text (seed words.. UTF-8 format...) ***/
		Unicode::toUTF8(keyboard2_text_typed_seedBuffer, cuvex.encrypt.words_to_encrypt[actual_word-1], KEYBOARD2_TEXT_TYPED_SEED_SIZE);
		Unicode::toUTF8(keyboard2_text_predicted_seedBuffer, words_to_check[actual_word-1], KEYBOARD2_TEXT_PREDICTED_SEED_SIZE);

		/*** Clearing keyboard and display buffers ***/
		memset(keyboard2_text_typed_seedBuffer, 0x00, sizeof(keyboard2_text_typed_seedBuffer));
		memset(keyboard2_text_predicted_seedBuffer, 0x00, sizeof(keyboard2_text_predicted_seedBuffer));
		keyboard2_seed.clearBuffer();

		/*** Selecting visible/hidden elements on the screen ***/
		if(actual_word == cuvex.encrypt.total_words){
			s2_seed_2_typeWords.setVisible(false);
			s2_seed_3_typePassphrase.setVisible(true);
		}
		else{
			Unicode::snprintf(keyboard2_text_info_seedBuffer1, KEYBOARD2_TEXT_INFO_SEEDBUFFER1_SIZE, "%d", ++actual_word);
			Unicode::snprintf(keyboard2_text_info_seedBuffer2, KEYBOARD2_TEXT_INFO_SEEDBUFFER2_SIZE, "%d", cuvex.encrypt.total_words);
		}

		/*** Screen update ***/
		background.invalidate();
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
void screen_flow_encrypt_part_1View::hideKeyboard3SeedPressed()
{
	keyboard3_text_area_seed.setPosition(20, 40, 280, 160);
	keyboard3_text_typed_seed.setPosition(60, 50, 230, 140);
	keyboard3_btn_up_seed.setPosition(30, 50, 20, 20);
	keyboard3_btn_down_seed.setPosition(30, 170, 20, 20);
	keyboard3_char_count_seed.setPosition(250, 188, 50, 12);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard3_seed.setVisible(false);
	keyboard3_btn_hide_seed.setVisible(false);
	keyboard3_btn_show_seed.setVisible(true);
	keyboard3_btn_enter_seed.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::showKeyboard3SeedPressed()
{
	keyboard3_text_area_seed.setPosition(20, 40, 280, 70);
	keyboard3_text_typed_seed.setPosition(60, 50, 230, 60);
	keyboard3_btn_up_seed.setPosition(30, 50, 20, 20);
	keyboard3_btn_down_seed.setPosition(30, 80, 20, 20);
	keyboard3_char_count_seed.setPosition(250, 98, 50, 12);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard3_seed.setVisible(true);
	keyboard3_btn_hide_seed.setVisible(true);
	keyboard3_btn_show_seed.setVisible(false);
	keyboard3_btn_enter_seed.setVisible(false);

	/*** Screen update ***/
	background.invalidate();

	/*** Auxiliar flag ***/
	flag_refresh_text_area = 1;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::enterKeyboard3SeedPressed()
{
	char buff_aux1[TYPED_CHECK_SEED_SIZE] = {0}, buff_aux2[15] = {0};

	memset(cuvex.encrypt.buff_passphrase, 0x00, KEYBOARD3_TEXT_TYPED_SEED_SIZE);
	memset(typed_check_seedBuffer, 0x00, TYPED_CHECK_SEED_SIZE);

	/*** Obtaining the typed text (passphrase) ***/
	for(int i=0; i<KEYBOARD3_TEXT_TYPED_SEED_SIZE; i++)
	{
		if(keyboard3_text_typed_seedBuffer[i] == 8364){	//If '€' (8364) is converted to the character '¶' (182) to only occupy 1 byte
			cuvex.encrypt.buff_passphrase[i] = 182;
		}
		else{
			cuvex.encrypt.buff_passphrase[i] = (uint8_t) keyboard3_text_typed_seedBuffer[i];
		}
	}

	/*** Generating an array for displaying words + passphrase ***/
	for(int i=0; i<cuvex.encrypt.total_words; i++)
	{
		snprintf(buff_aux2, 15, "%d. ", i+1);
		strcat(buff_aux1, (char *) buff_aux2);
		strcat(buff_aux1, (char *) words_to_check[i]);
		strcat(buff_aux1, (char *) "\n");
	}

	strcat(buff_aux1, (char*) "\nPassphrase:\n");
	strcat(buff_aux1, (char*) cuvex.encrypt.buff_passphrase);

	/*** Assignment of words + passphrase to be displayed in "scrollable" field ***/
	for(int i=0; i<TYPED_CHECK_SEED_SIZE; i++)
	{
		if(buff_aux1[i] == 182){	//If '¶' (182) is converted to the character '€' (8364) for display on screen
			typed_check_seedBuffer[i] = 8364;
		}
		else{
			typed_check_seedBuffer[i] = buff_aux1[i];
		}
	}

	typed_check_seed.setPosition(0, 0, 210, (20*(cuvex.encrypt.total_words+1))+(20*7));
	typed_check_seed.setWideTextAction(touchgfx::WIDE_TEXT_CHARWRAP);

	/*** Clearing keyboard and display buffers ***/
	memset(keyboard3_text_typed_seedBuffer, 0x00, sizeof(keyboard3_text_typed_seedBuffer));
	keyboard3_seed.clearBuffer();

	/*** Selecting visible/hidden elements on the screen ***/
	s2_seed_3_typePassphrase.setVisible(false);
	s2_checkInfo.setVisible(true);
	s2_checkInfo_Seed.setVisible(true);

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
void screen_flow_encrypt_part_1View::hideKeyboardPlainTextPressed()
{
	keyboard_text_area_plain_text.setPosition(20, 40, 280, 160);
	keyboard_text_typed_plain_text.setPosition(60, 50, 230, 140);
	keyboard_btn_up_plain_text.setPosition(30, 50, 20, 20);
	keyboard_btn_down_plain_text.setPosition(30, 170, 20, 20);
	keyboard_char_count_plain_text.setPosition(250, 188, 50, 12);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard_plain_text.setVisible(false);
	keyboard_btn_hide_plain_text.setVisible(false);
	keyboard_btn_show_plain_text.setVisible(true);
	keyboard_btn_enter_plain_text.setVisible(true);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::showKeyboardPlainTextPressed()
{
	keyboard_text_area_plain_text.setPosition(20, 40, 280, 70);
	keyboard_text_typed_plain_text.setPosition(60, 50, 230, 60);
	keyboard_btn_up_plain_text.setPosition(30, 50, 20, 20);
	keyboard_btn_down_plain_text.setPosition(30, 80, 20, 20);
	keyboard_char_count_plain_text.setPosition(250, 98, 50, 12);

	/*** Selecting visible/hidden elements on the screen ***/
	keyboard_plain_text.setVisible(true);
	keyboard_btn_hide_plain_text.setVisible(true);
	keyboard_btn_show_plain_text.setVisible(false);
	keyboard_btn_enter_plain_text.setVisible(false);

	/*** Screen update ***/
	background.invalidate();

	/*** Auxiliar flag ***/
	flag_refresh_text_area = 1;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::enterKeyboardPlainTextPressed()
{
	if(keyboard_text_typed_plain_textBuffer[0] != 0x00)
	{
		memset(cuvex.encrypt.buff_plain_text, 0x00, KEYBOARD_TEXT_TYPED_PLAIN_TEXT_SIZE);
		memset(typed_check_plain_textBuffer, 0x00, TYPED_CHECK_PLAIN_TEXT_SIZE);

		/*** Obtaining the typed text (plain text) ***/
		for(int i=0; i<KEYBOARD_TEXT_TYPED_PLAIN_TEXT_SIZE; i++)
		{
			if(keyboard_text_typed_plain_textBuffer[i] == 8364){	//If '€' (8364) is converted to the character '¶' (182) to only occupy 1 byte
				cuvex.encrypt.buff_plain_text[i] = 182;
			}
			else{
				cuvex.encrypt.buff_plain_text[i] = (uint8_t) keyboard_text_typed_plain_textBuffer[i];
			}
		}

		/*** Assignment of plain text to be displayed in "scrollable" field ***/
		for(int i=0; i<KEYBOARD_TEXT_TYPED_PLAIN_TEXT_SIZE; i++)
		{
			if(cuvex.encrypt.buff_plain_text[i] == 182){	//If '¶' (182) is converted to the character '€' (8364) for display on screen
				typed_check_plain_textBuffer[i] = 8364;
			}
			else{
				typed_check_plain_textBuffer[i] = cuvex.encrypt.buff_plain_text[i];
			}
		}

		typed_check_plain_text.setPosition(0, 0, 220, 20*(typed_check_plain_text.getTextWidth()/220)+2*20);
		typed_check_plain_text.setWideTextAction(touchgfx::WIDE_TEXT_CHARWRAP);

		/*** Clearing keyboard and display buffers ***/
		memset(keyboard_text_typed_plain_textBuffer, 0x00, sizeof(keyboard_text_typed_plain_textBuffer));
		keyboard_plain_text.clearBuffer();

		/*** Selecting visible/hidden elements on the screen ***/
		s2_plainText_1_typePlainText.setVisible(false);
		s2_checkInfo.setVisible(true);
		s2_checkInfo_PlainText.setVisible(true);

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
void screen_flow_encrypt_part_1View::menuBip39Pressed()
{
	/*** Update variable of type of text to encrypt ***/
	cuvex.encrypt.text_type = TEXT_TYPE_BIP39;

	/*** Selecting visible/hidden elements on the screen ***/
	s1_menuSelection.setVisible(false);
	s2_menuContent.setVisible(true);
	s2_seed.setVisible(true);
	s2_plainText.setVisible(false);
	s2_checkInfo.setVisible(false);
	s2_seed_1_typeNumWords.setVisible(true);
	s2_seed_2_typeWords.setVisible(false);
	s2_seed_3_typePassphrase.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::menuSlip39Pressed()
{
	/*** Update variable of type of text to encrypt ***/
	cuvex.encrypt.text_type = TEXT_TYPE_SLIP39;

	/*** Selecting visible/hidden elements on the screen ***/
	s1_menuSelection.setVisible(false);
	s2_menuContent.setVisible(true);
	s2_seed.setVisible(true);
	s2_plainText.setVisible(false);
	s2_checkInfo.setVisible(false);
	s2_seed_1_typeNumWords.setVisible(true);
	s2_seed_2_typeWords.setVisible(false);
	s2_seed_3_typePassphrase.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::menuXmrPressed()
{
	/*** Update variable of type of text to encrypt ***/
	cuvex.encrypt.text_type = TEXT_TYPE_XMR;

	/*** Selecting visible/hidden elements on the screen ***/
	s1_menuSelection.setVisible(false);
	s2_menuContent.setVisible(true);
	s2_seed.setVisible(true);
	s2_plainText.setVisible(false);
	s2_checkInfo.setVisible(false);
	s2_seed_1_typeNumWords.setVisible(true);
	s2_seed_2_typeWords.setVisible(false);
	s2_seed_3_typePassphrase.setVisible(false);

	/*** Screen update ***/
	background.invalidate();
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::menuPlainTextPressed()
{
	/*** Update variable of type of text to encrypt ***/
	cuvex.encrypt.text_type = TEXT_TYPE_PLAINTEXT;

	/*** Selecting visible/hidden elements on the screen ***/
	s1_menuSelection.setVisible(false);
	s2_menuContent.setVisible(true);
	s2_seed.setVisible(false);
	s2_plainText.setVisible(true);
	s2_checkInfo.setVisible(false);
	s2_plainText_1_typePlainText.setVisible(true);

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
void screen_flow_encrypt_part_1View::encryptPressed()
{
	/*** Jump to "encrypt part 2" ***/
	application().gotoscreen_flow_encrypt_part_2ScreenNoTransition();
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
void screen_flow_encrypt_part_1View::btnUpPressed()
{
	uint8_t aux = 0;

	if(s2_seed.isVisible() == true)
	{
		aux = keyboard3_text_typed_seed.getTextWidth()/230;

		if(aux != 0)	//Multi-line
		{
			if(keyboard3_text_area_seed.getHeight() == 70)		//Small box
			{
				if(keyboard3_text_typed_seed.getHeight() > 60){
					keyboard3_text_typed_seed.setPosition(60, keyboard3_text_typed_seed.getY() + 20, 230, keyboard3_text_typed_seed.getHeight() - 20);
				}
			}
			else if(keyboard3_text_area_seed.getHeight() == 160)	//Big box
			{
				if(keyboard3_text_typed_seed.getHeight() > 140){
					keyboard3_text_typed_seed.setPosition(60, keyboard3_text_typed_seed.getY() + 20, 230, keyboard3_text_typed_seed.getHeight() - 20);
				}
			}

			keyboard3_text_area_seed.invalidate();
		}
	}
	else if(s2_plainText.isVisible() == true)
	{
		aux = keyboard_text_typed_plain_text.getTextWidth()/230;

		if(aux != 0)	//Multi-line
		{
			if(keyboard_text_area_plain_text.getHeight() == 70)		//Small box
			{
				if(keyboard_text_typed_plain_text.getHeight() > 60){
					keyboard_text_typed_plain_text.setPosition(60, keyboard_text_typed_plain_text.getY() + 20, 230, keyboard_text_typed_plain_text.getHeight() - 20);
				}
			}
			else if(keyboard_text_area_plain_text.getHeight() == 160)	//Big box
			{
				if(keyboard_text_typed_plain_text.getHeight() > 140){
					keyboard_text_typed_plain_text.setPosition(60, keyboard_text_typed_plain_text.getY() + 20, 230, keyboard_text_typed_plain_text.getHeight() - 20);
				}
			}

			keyboard_text_area_plain_text.invalidate();
		}
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::btnDownPressed()
{
	uint8_t num_of_lines = 0;

	if(s2_seed.isVisible() == true)
	{
		num_of_lines = keyboard3_text_typed_seed.getTextWidth()/230;

		if(num_of_lines)
		{
			if(keyboard3_text_area_seed.getHeight() == 70)		//Small box
			{
				if(keyboard3_text_typed_seed.getHeight() < 60+(20*num_of_lines)){
					keyboard3_text_typed_seed.setPosition(60, keyboard3_text_typed_seed.getY() - 20, 230, keyboard3_text_typed_seed.getHeight() + 20);
				}
			}
			else if(keyboard3_text_area_seed.getHeight() == 160)	//Big box
			{
				if(keyboard3_text_typed_seed.getHeight() < 140+(20*num_of_lines)){
					keyboard3_text_typed_seed.setPosition(60, keyboard3_text_typed_seed.getY() - 20, 230, keyboard3_text_typed_seed.getHeight() + 20);
				}
			}
			keyboard3_text_area_seed.invalidate();
		}
	}
	else if(s2_plainText.isVisible() == true)
	{
		num_of_lines = keyboard_text_typed_plain_text.getTextWidth()/230;

		if(num_of_lines != 0)
		{
			if(keyboard_text_area_plain_text.getHeight() == 70)		//Small box
			{
				if(keyboard_text_typed_plain_text.getHeight() < 60+(20*num_of_lines)){
					keyboard_text_typed_plain_text.setPosition(60, keyboard_text_typed_plain_text.getY() - 20, 230, keyboard_text_typed_plain_text.getHeight() + 20);
				}
			}
			else if(keyboard_text_area_plain_text.getHeight() == 160)	//Big box
			{
				if(keyboard_text_typed_plain_text.getHeight() < 140+(20*num_of_lines)){
					keyboard_text_typed_plain_text.setPosition(60, keyboard_text_typed_plain_text.getY() - 20, 230, keyboard_text_typed_plain_text.getHeight() + 20);
				}
			}
			keyboard_text_area_plain_text.invalidate();
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
void screen_flow_encrypt_part_1View::btnScrollUpPressed()
{
	switch(cuvex.encrypt.text_type)
	{
	case TEXT_TYPE_NONE:
	default:
		break;

	case TEXT_TYPE_BIP39:
	case TEXT_TYPE_SLIP39:
	case TEXT_TYPE_XMR:
		if(typed_check_seed.getY() < 0){
			typed_check_seed.setPosition(0, typed_check_seed.getY() + 20, 210, (20*(cuvex.encrypt.total_words+1))+(20*7));
		}
		scrollable_container_typed_check_seed.invalidate();
		break;

	case TEXT_TYPE_PLAINTEXT:
		if(typed_check_plain_text.getY() < 0){
			typed_check_plain_text.setPosition(0, typed_check_plain_text.getY() + 20, 220, 20*(typed_check_plain_text.getTextWidth()/220)+2*20);
		}
		scrollable_container_typed_check_plain_text.invalidate();
		break;
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void screen_flow_encrypt_part_1View::btnScrollDownPressed()
{
	switch(cuvex.encrypt.text_type)
	{
	case TEXT_TYPE_NONE:
	default:
		break;

	case TEXT_TYPE_BIP39:
	case TEXT_TYPE_SLIP39:
	case TEXT_TYPE_XMR:
		if((typed_check_seed.getY() + 20*(cuvex.encrypt.total_words+1)+(20*7)) > 0){
			typed_check_seed.setPosition(0, typed_check_seed.getY() - 20, 210, (20*(cuvex.encrypt.total_words+1))+(20*7));
		}
		scrollable_container_typed_check_seed.invalidate();
		break;

	case TEXT_TYPE_PLAINTEXT:
		if((typed_check_plain_text.getY() + (20*(typed_check_plain_text.getTextWidth()/220))-1*20) > 0){
			typed_check_plain_text.setPosition(0, typed_check_plain_text.getY() - 20, 220, 20*(typed_check_plain_text.getTextWidth()/220)+2*20);
		}
		scrollable_container_typed_check_plain_text.invalidate();
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
void screen_flow_encrypt_part_1View::btnConnectAppMsgPressed()
{
	/*** Selecting visible/hidden elements on the screen ***/
	s0_connectAppMsg.setVisible(false);
	s1_menuSelection.setVisible(true);

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
void screen_flow_encrypt_part_1View::setScreenMode()
{
	/*** Setting screen elements based on mode (dark/light) ***/
	if(cuvex.info.mode == DARK)
	{
		background.setColor(touchgfx::Color::getColorFromRGB(0x3F,0x3F,0x51));
		/***/
		btn_connect_app_msg.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		connect_app_msg_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		btn_seed_bip39.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_seed_bip39.setTextColors(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED), touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_seed_slip39.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_seed_slip39.setTextColors(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED), touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_seed_xmr.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_seed_xmr.setTextColors(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED), touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		btn_plain_text.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		btn_plain_text.setTextColors(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED), touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		keyboard1_text_typed_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard1_text_info_seed.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard2_text_typed_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard2_text_predicted_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard2_text_info_seed.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard3_text_area_seed.setColor(touchgfx::Color::getColorFromRGB(0xFF,0xFF,0xFF));
		keyboard3_btn_enter_seed.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard3_text_typed_seed.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		typed_check_seed.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard3_text_info_1_seed.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard3_text_info_2_seed.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		keyboard_text_area_plain_text.setColor(touchgfx::Color::getColorFromRGB(0xFF,0xFF,0xFF));
		keyboard_btn_enter_plain_text.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard_text_typed_plain_text.setColor(touchgfx::Color::getColorFromRGB(0,0,0));
		keyboard_text_info_plain_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		typed_check_plain_text.setColor(touchgfx::Color::getColorFromRGB(0xED,0xED,0xED));
		/***/
		btn_encrypt.setBoxWithBorderColors(touchgfx::Color::getColorFromRGB(0x6B,0x6B,0x7D), touchgfx::Color::getColorFromRGB(0x40,0x5C,0xA0), touchgfx::Color::getColorFromRGB(0,0,0), touchgfx::Color::getColorFromRGB(0,0,0));
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
void screen_flow_encrypt_part_1View::setScreenLanguage()
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





