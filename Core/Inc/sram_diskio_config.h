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

#ifndef __SRAM_DISKIO_CONFIG_H
#define __SRAM_DISKIO_CONFIG_H

/********************************************************************** Include's **********************************************************************/
#include <stdint.h>
#include "main.h"

/********************************************************************* Variable's **********************************************************************/
extern uint8_t ramdisk[];

/********************************************************************** Define's ***********************************************************************/
#define SRAM_DISK_SIZE			(RAMDISK_SECTOR_SIZE * RAMDISK_SECTORS)
#define BLOCK_SIZE				RAMDISK_SECTOR_SIZE
#define SRAM_DISK_BASE_ADDR		((uint32_t)ramdisk)

#endif /* __SRAM_DISKIO_CONFIG_H */
