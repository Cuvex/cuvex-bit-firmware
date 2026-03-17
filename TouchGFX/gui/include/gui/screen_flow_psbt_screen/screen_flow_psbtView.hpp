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

#ifndef SCREEN_FLOW_PSBTVIEW_HPP
#define SCREEN_FLOW_PSBTVIEW_HPP

#include <gui_generated/screen_flow_psbt_screen/screen_flow_psbtViewBase.hpp>
#include <gui/screen_flow_psbt_screen/screen_flow_psbtPresenter.hpp>
#include <texts/TextKeysAndLanguages.hpp>
#include <touchgfx/Callback.hpp>
#include "Bitcoin.h"
#include "PSBT.h"
#include <iostream>

class screen_flow_psbtView : public screen_flow_psbtViewBase
{
public:
    screen_flow_psbtView();
    virtual ~screen_flow_psbtView() {}
    virtual void setupScreen();
    virtual void tearDownScreen();
    virtual void tickEventScreen();
    virtual void changeScreen(uint16_t screen);
    virtual void changeVolume(uint16_t state);
    virtual void updateVolume(uint16_t state);
    virtual void checkTemplate(uint16_t state);
    virtual void updateFingerprint(uint16_t state);
    virtual void btnVerifyTransactionPressed();
    virtual void retryCardFormatErrorPressed();
    virtual void retryCardSizeErrorPressed();
    virtual void retryNoCryptogramErrorPressed();
    virtual void koPrivatePasswordBtnPressed();
    virtual void checkReceiverPressed();
    virtual void btnSignPsbtPressed();
    virtual void btnWarningNoChangeAddressPressed();
    virtual void btnSendToAppPressed();
    virtual void success1Pressed();
    virtual void success2Pressed();

    void handlePasswordResult(uint8_t result);
    void handleBiometricResult(uint8_t result);

protected:
	touchgfx::Callback<screen_flow_psbtView, uint8_t> passwordResultCallback;
	touchgfx::Callback<screen_flow_psbtView, uint8_t> biometricResultCallback;

    uint8_t text_type;
	uint8_t	num_pwds;
	uint8_t actual_pwd;
	bool 	pwd_ok;
	uint8_t buff_pri_key[205];						//200 characters
	uint8_t buff_pub_key[205];						//200 characters
	uint8_t pwd_sha256[32];							//32
	uint8_t pwd_combined_sha256[32];				//32
	uint8_t header_aes_gcm[4];						//4
	uint8_t iv_aes_gcm[16];							//16
	uint8_t psbt_total_receivers;
	uint8_t psbt_actual_receiver;
	uint8_t psbt_btn_sign_count;
	bool 	psbt_has_change_address;

    virtual void setScreenMode();
    virtual void setScreenLanguage();
    virtual int getCheckPsbtInfo1();
    virtual int getCheckPsbtInfo2();
    virtual void signPsbtTransaction();
    virtual void passwordSuccess(uint8_t  decrypted_text[SIZE_CRYPT]);
    virtual void printfPsbtInputsMetadata();
    virtual void btnConnectAppMsgPressed();
};

#endif // SCREEN_FLOW_PSBTVIEW_HPP
