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

#include <gui/model/Model.hpp>
#include <gui/model/ModelListener.hpp>

Model::Model() : modelListener(0)
{

}

void Model::tick()
{
#ifndef SIMULATOR

	uint16_t rcv_data = 0;

	if((osMessageQueueGetCount(mainToGuiQueueHandle) > 0) && (osMessageQueueGet(mainToGuiQueueHandle, &rcv_data, 0, 0) == osOK))
	{
		switch(rcv_data)
		{
		case MAIN_TO_GUI_FLOW_DECRYPT_FILES_READED:
			modelListener->updateVolume(MAIN_TO_GUI_FLOW_DECRYPT_FILES_READED);
			break;

		case MAIN_TO_GUI_FLOW_PSBT_FILES_READED:
			modelListener->updateVolume(MAIN_TO_GUI_FLOW_PSBT_FILES_READED);
			break;

		case MAIN_TO_GUI_GET_TEMPLATE_SUCCESS:
			modelListener->updateFingerprint(MAIN_TO_GUI_GET_TEMPLATE_SUCCESS);
			break;

		case MAIN_TO_GUI_GET_TEMPLATE_ERROR:
			modelListener->updateFingerprint(MAIN_TO_GUI_GET_TEMPLATE_ERROR);
			break;

		case MAIN_TO_GUI_CHECK_TEMPLATE_SUCCESS:
			modelListener->updateFingerprint(MAIN_TO_GUI_CHECK_TEMPLATE_SUCCESS);
			break;

		case MAIN_TO_GUI_CHECK_TEMPLATE_ERROR:
			modelListener->updateFingerprint(MAIN_TO_GUI_CHECK_TEMPLATE_ERROR);
			break;

		case MAIN_TO_GUI_GENERAL_ERROR:
			modelListener->updateFingerprint(MAIN_TO_GUI_GENERAL_ERROR);
			break;

		default:
			break;
		}
	}

#endif
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
void Model::changeScreen(uint16_t screen)
{
	uint16_t data = screen;

	if(osMessageQueueGetSpace(guiToMainQueueHandle) > 0){
		osMessageQueuePut(guiToMainQueueHandle, &data, 0, 0);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void Model::changeVolume(uint16_t state)
{
	uint16_t data = state;

	if(osMessageQueueGetSpace(guiToMainQueueHandle) > 0){
		osMessageQueuePut(guiToMainQueueHandle, &data, 0, 0);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void Model::changeFlash(uint16_t state)
{
	uint16_t data = state;

	if(osMessageQueueGetSpace(guiToMainQueueHandle) > 0){
		osMessageQueuePut(guiToMainQueueHandle, &data, 0, 0);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void Model::getTemplate(uint16_t state)
{
	uint16_t data = state;

	if(osMessageQueueGetSpace(guiToMainQueueHandle) > 0){
		osMessageQueuePut(guiToMainQueueHandle, &data, 0, 0);
	}
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
void Model::checkTemplate(uint16_t state)
{
	uint16_t data = state;

	if(osMessageQueueGetSpace(guiToMainQueueHandle) > 0){
		osMessageQueuePut(guiToMainQueueHandle, &data, 0, 0);
	}
}





