/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2025 NXP
 * SPDX-License-Identifier: BSD-3-Clause
 * Generated linker script file for MIMXRT1062xxxxB
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v24.12 [Build 148] [2025-01-10] on Jan. 28, 2025, 9:35:15 p.m.
 */

MEMORY
{
  /* Define each memory region */
  BOARD_FLASH (rx) : ORIGIN = 0x60200000, LENGTH = 0xa00000 /* 10M bytes (alias Flash) */  
  SRAM_DTC (rwx) : ORIGIN = 0x20000000, LENGTH = 0x70000 /* 448K bytes (alias RAM) */  
  SRAM_ITC (rwx) : ORIGIN = 0x0, LENGTH = 0x10000 /* 64K bytes (alias RAM2) */  
  SRAM_OC_NON_CACHEABLE (rwx) : ORIGIN = 0x20200000, LENGTH = 0x40000 /* 256K bytes (alias RAM3) */  
  SRAM_OC_CACHEABLE (rwx) : ORIGIN = 0x20240000, LENGTH = 0x40000 /* 256K bytes (alias RAM4) */  
}

  /* Define a symbol for the top of each memory region */
  __base_BOARD_FLASH = 0x60200000  ; /* BOARD_FLASH */  
  __base_Flash = 0x60200000 ; /* Flash */  
  __top_BOARD_FLASH = 0x60200000 + 0xa00000 ; /* 10M bytes */  
  __top_Flash = 0x60200000 + 0xa00000 ; /* 10M bytes */  
  __base_SRAM_DTC = 0x20000000  ; /* SRAM_DTC */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM_DTC = 0x20000000 + 0x70000 ; /* 448K bytes */  
  __top_RAM = 0x20000000 + 0x70000 ; /* 448K bytes */  
  __base_SRAM_ITC = 0x0  ; /* SRAM_ITC */  
  __base_RAM2 = 0x0 ; /* RAM2 */  
  __top_SRAM_ITC = 0x0 + 0x10000 ; /* 64K bytes */  
  __top_RAM2 = 0x0 + 0x10000 ; /* 64K bytes */  
  __base_SRAM_OC_NON_CACHEABLE = 0x20200000  ; /* SRAM_OC_NON_CACHEABLE */  
  __base_RAM3 = 0x20200000 ; /* RAM3 */  
  __top_SRAM_OC_NON_CACHEABLE = 0x20200000 + 0x40000 ; /* 256K bytes */  
  __top_RAM3 = 0x20200000 + 0x40000 ; /* 256K bytes */  
  __base_SRAM_OC_CACHEABLE = 0x20240000  ; /* SRAM_OC_CACHEABLE */  
  __base_RAM4 = 0x20240000 ; /* RAM4 */  
  __top_SRAM_OC_CACHEABLE = 0x20240000 + 0x40000 ; /* 256K bytes */  
  __top_RAM4 = 0x20240000 + 0x40000 ; /* 256K bytes */  
