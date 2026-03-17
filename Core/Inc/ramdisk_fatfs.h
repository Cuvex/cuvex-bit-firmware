/**
 ******************************************************************************
 * @file           : mainTask.h
 * @brief          : Header for mainTask.c file.
 *                   This file contains the common defines of the mainTask.
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

#ifndef INC_RAMDISK_FATFS_H_
#define INC_RAMDISK_FATFS_H_

/********************************************************************** Include's **********************************************************************/
#include "stm32u5xx_hal.h"

/********************************************************************* Functions's *********************************************************************/
uint8_t RAMDisk_FATFS_Init(void);
uint8_t FS_Read_SingleFile(const char *path, uint8_t *buf, uint32_t buf_size);
/***/
uint8_t FS_FlowEncrypt_WriteTagAlias(const uint8_t *data, uint32_t length);
uint8_t FS_FlowEncrypt_WriteTagCryptogram(const uint8_t *data, uint32_t length);
uint8_t FS_FlowEncrypt_WriteTagInformation(const uint8_t *data, uint32_t length);
uint8_t FS_FlowEncrypt_WriteTagMultisignature(const uint8_t *data, uint32_t length);
uint8_t FS_FlowEncrypt_WriteTagSignerTemplate(uint8_t signer, uint8_t template_id, const uint8_t *data, uint32_t length);
/***/
uint8_t FS_FlowDecrypt_WriteGetCryptogramFiles(void);
uint8_t FS_FlowDecrypt_CheckPresenceMandatoryCryptogramFiles(void);
uint8_t FS_FlowDecrypt_ReadTagAlias(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowDecrypt_ReadTagCryptogram(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowDecrypt_ReadTagInformation(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowDecrypt_ReadTagMultisignature(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowDecrypt_ReadTagSignerTemplate(uint8_t signer, uint8_t template_id, uint8_t *buf, uint32_t buf_size);
/***/
uint8_t FS_FlowPsbt_WriteGetNotSignedPsbtFiles(void);
uint8_t FS_FlowPsbt_WriteGetCryptogramFiles(void);
uint8_t FS_FlowPsbt_CheckPresenceNotSignedPsbtFiles(void);
uint8_t FS_FlowPsbt_CheckPresenceMandatoryCryptogramFiles(void);
uint8_t FS_FlowPsbt_ReadNotSignedPsbt1(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadNotSignedPsbt2(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadTagAlias(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadTagCryptogram(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadTagInformation(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadTagMultisignature(uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_ReadTagSignerTemplate(uint8_t signer, uint8_t template_id, uint8_t *buf, uint32_t buf_size);
uint8_t FS_FlowPsbt_WriteSignedPsbt1(void);
uint8_t FS_FlowPsbt_WriteSignedPsbt2(const uint8_t *data, uint32_t length);

#endif /* INC_RAMDISK_FATFS_H_ */
