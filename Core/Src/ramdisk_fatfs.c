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

#include <string.h>
#include "ramdisk_fatfs.h"
#include "ff.h"
#include "ff_gen_drv.h"
#include "sram_diskio.h"

static FATFS FatFs;
static char DrivePath[4];

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t RAMDisk_FATFS_Init(void)
{
	MKFS_PARM opt = {0};
	BYTE work[FF_MAX_SS];

	/*** Link the SRAM disk driver so FatFs can access the RAM disk ***/
	if(FATFS_LinkDriver(&SRAMDISK_Driver, DrivePath) != 0){
		return ERROR;
	}

	/*** Configure format options for FAT filesystem ***/
	opt.fmt     = FM_FAT | FM_SFD;
	opt.n_fat   = 1;
	opt.align   = 0;
	opt.n_root  = 32;
	opt.au_size = RAMDISK_SECTOR_SIZE;

	/*** Always format the RAM disk with a new FAT filesystem ***/
	if(f_mkfs(DrivePath, &opt, work, sizeof(work)) != FR_OK){
		return ERROR;
	}

	/*** Mount volume after formatting ***/
	if(f_mount(&FatFs, DrivePath, 1) != FR_OK){
		return ERROR;
	}

	/*** Set FAT volume label (Root Volume Name) ***/
	f_setlabel("CUVEX");

	/*** Create directory "/Cuvex" ***/
	if(f_mkdir("Cuvex") != FR_OK){
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
uint8_t FS_Read_SingleFile(const char *path, uint8_t *buf, uint32_t buf_size)
{
	FIL file;
	UINT read;

	memset(buf, 0x00, buf_size);

	if(f_open(&file, path, FA_READ) != FR_OK){
		return ERROR;
	}

	if(f_read(&file, buf, buf_size, &read) != FR_OK){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
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
uint8_t FS_FlowEncrypt_WriteTagAlias(const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;

	if(f_open(&file, "Cuvex/flow_encrypt_tag_alias.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowEncrypt_WriteTagCryptogram(const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;

	if(f_open(&file, "Cuvex/flow_encrypt_tag_cryptogram.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowEncrypt_WriteTagInformation(const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;

	if(f_open(&file, "Cuvex/flow_encrypt_tag_information.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowEncrypt_WriteTagMultisignature(const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;

	if(f_open(&file, "Cuvex/flow_encrypt_tag_multisignature.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowEncrypt_WriteTagSignerTemplate(uint8_t signer, uint8_t template_id, const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;
	char path[64] = {0};

	snprintf(path, sizeof(path), "Cuvex/flow_encrypt_tag_signer_%d_template_%d.bin", signer+1, template_id);

	if(f_open(&file, path, FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
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
uint8_t FS_FlowDecrypt_WriteGetCryptogramFiles(void)
{
	FIL file;

	if(f_open(&file, "Cuvex/flow_decrypt_get_cryptogram.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowDecrypt_CheckPresenceMandatoryCryptogramFiles(void)
{
	FILINFO finfo;

	if(f_stat("Cuvex/flow_decrypt_tag_alias.bin", &finfo) != FR_OK){
		return ERROR;
	}

	if(f_stat("Cuvex/flow_decrypt_tag_cryptogram.bin", &finfo) != FR_OK){
		return ERROR;
	}

	if(f_stat("Cuvex/flow_decrypt_tag_information.bin", &finfo) != FR_OK){
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
uint8_t FS_FlowDecrypt_ReadTagAlias(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_decrypt_tag_alias.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowDecrypt_ReadTagCryptogram(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_decrypt_tag_cryptogram.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowDecrypt_ReadTagInformation(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_decrypt_tag_information.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowDecrypt_ReadTagMultisignature(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_decrypt_tag_multisignature.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowDecrypt_ReadTagSignerTemplate(uint8_t signer, uint8_t template_id, uint8_t *buf, uint32_t buf_size)
{
    char path[64] = {0};

    snprintf(path, sizeof(path), "Cuvex/flow_decrypt_tag_signer_%d_template_%d.bin", signer+1, template_id);

    return FS_Read_SingleFile(path, buf, buf_size);
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
uint8_t FS_FlowPsbt_WriteGetNotSignedPsbtFiles(void)
{
	FIL file;

	if(f_open(&file, "Cuvex/flow_psbt_get_not_signed.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_WriteGetCryptogramFiles(void)
{
	FIL file;

	/*** Erase previous files ***/
	f_unlink("Cuvex/flow_psbt_not_signed_1.bin");
	f_unlink("Cuvex/flow_psbt_not_signed_2.bin");
	f_unlink("Cuvex/flow_psbt_get_not_signed.txt");

	/*** Create the new file ***/
	if(f_open(&file, "Cuvex/flow_psbt_get_cryptogram.txt", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_CheckPresenceNotSignedPsbtFiles(void)
{
	FILINFO finfo;

	if(f_stat("Cuvex/flow_psbt_not_signed_1.bin", &finfo) != FR_OK){
		return ERROR;
	}

	if(f_stat("Cuvex/flow_psbt_not_signed_2.bin", &finfo) != FR_OK){
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
uint8_t FS_FlowPsbt_CheckPresenceMandatoryCryptogramFiles(void)
{
	FILINFO finfo;

	if(f_stat("Cuvex/flow_psbt_tag_alias.bin", &finfo) != FR_OK){
		return ERROR;
	}

	if(f_stat("Cuvex/flow_psbt_tag_cryptogram.bin", &finfo) != FR_OK){
		return ERROR;
	}

	if(f_stat("Cuvex/flow_psbt_tag_information.bin", &finfo) != FR_OK){
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
uint8_t FS_FlowPsbt_ReadNotSignedPsbt1(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_not_signed_1.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadNotSignedPsbt2(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_not_signed_2.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadTagAlias(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_tag_alias.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadTagCryptogram(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_tag_cryptogram.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadTagInformation(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_tag_information.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadTagMultisignature(uint8_t *buf, uint32_t buf_size)
{
	return FS_Read_SingleFile("Cuvex/flow_psbt_tag_multisignature.bin", buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_ReadTagSignerTemplate(uint8_t signer, uint8_t template_id, uint8_t *buf, uint32_t buf_size)
{
    char path[64] = {0};

    snprintf(path, sizeof(path), "Cuvex/flow_psbt_tag_signer_%d_template_%d.bin", signer+1, template_id);

    return FS_Read_SingleFile(path, buf, buf_size);
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_WriteSignedPsbt1(void)
{
	FIL file;
	uint32_t written;
	const char *text = "PSBT-CARD-SIGNED";
	UINT length = strlen(text);

	/*** Erase previous files ***/
	f_unlink("Cuvex/flow_psbt_get_cryptogram.txt");
	f_unlink("Cuvex/flow_psbt_tag_alias.bin");
	f_unlink("Cuvex/flow_psbt_tag_cryptogram.bin");
	f_unlink("Cuvex/flow_psbt_tag_information.bin");
	f_unlink("Cuvex/flow_psbt_tag_multisignature.bin");

	/*** Create the new file ***/
	if(f_open(&file, "Cuvex/flow_psbt_signed_1.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, text, length, &written) != FR_OK) || (written != length)) {
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}

/**************************************************************************************************************************************
 ***** Function 	: N/A
 ***** Description 	: N/A
 ***** Parameters 	: N/A
 ***** Response 	: N/A
 **************************************************************************************************************************************/
uint8_t FS_FlowPsbt_WriteSignedPsbt2(const uint8_t *data, uint32_t length)
{
	FIL file;
	uint32_t written;

	if(f_open(&file, "Cuvex/flow_psbt_signed_2.bin", FA_CREATE_ALWAYS | FA_WRITE) != FR_OK){
		return ERROR;
	}

	if((f_write(&file, data, length, &written) != FR_OK) || (written != length)){
		f_close(&file);
		return ERROR;
	}

	f_close(&file);

	return SUCCESS;
}


