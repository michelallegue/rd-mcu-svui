/*
 * GENERATED FILE - DO NOT EDIT
 * Copyright 2008-2013 Code Red Technologies Ltd,
 * Copyright 2013-2024 NXP
 * Generated linker script file for MIMXRT1062xxxxB
 * Created from memory.ldt by FMCreateLinkMemory
 * Using Freemarker v2.3.30
 * MCUXpresso IDE v11.10.0 [Build 3148] [2024-07-03] on Oct 24, 2024, 3:09:06 PM
 */

MEMORY
{
  /* Define each memory region */
  BOARD_FLASH (rx) : ORIGIN = 0x60000000, LENGTH = 0x40000 /* 256K bytes (alias Flash) */  
  SRAM_DTC (rwx) : ORIGIN = 0x20000000, LENGTH = 0x40000 /* 256K bytes (alias RAM) */  
  SRAM_ITC (rwx) : ORIGIN = 0x0, LENGTH = 0x40000 /* 256K bytes (alias RAM2) */  
  SRAM_OC_NON_CACHEABLE (rwx) : ORIGIN = 0x20200000, LENGTH = 0x80000 /* 512K bytes (alias RAM3) */  
  SRAM_OC_CACHEABLE (rwx) : ORIGIN = 0x20280000, LENGTH = 0x0 /* 0M bytes (alias RAM4) */  
}

  /* Define a symbol for the top of each memory region */
  __base_BOARD_FLASH = 0x60000000  ; /* BOARD_FLASH */  
  __base_Flash = 0x60000000 ; /* Flash */  
  __top_BOARD_FLASH = 0x60000000 + 0x40000 ; /* 256K bytes */  
  __top_Flash = 0x60000000 + 0x40000 ; /* 256K bytes */  
  __base_SRAM_DTC = 0x20000000  ; /* SRAM_DTC */  
  __base_RAM = 0x20000000 ; /* RAM */  
  __top_SRAM_DTC = 0x20000000 + 0x40000 ; /* 256K bytes */  
  __top_RAM = 0x20000000 + 0x40000 ; /* 256K bytes */  
  __base_SRAM_ITC = 0x0  ; /* SRAM_ITC */  
  __base_RAM2 = 0x0 ; /* RAM2 */  
  __top_SRAM_ITC = 0x0 + 0x40000 ; /* 256K bytes */  
  __top_RAM2 = 0x0 + 0x40000 ; /* 256K bytes */  
  __base_SRAM_OC_NON_CACHEABLE = 0x20200000  ; /* SRAM_OC_NON_CACHEABLE */  
  __base_RAM3 = 0x20200000 ; /* RAM3 */  
  __top_SRAM_OC_NON_CACHEABLE = 0x20200000 + 0x80000 ; /* 512K bytes */  
  __top_RAM3 = 0x20200000 + 0x80000 ; /* 512K bytes */  
  __base_SRAM_OC_CACHEABLE = 0x20280000  ; /* SRAM_OC_CACHEABLE */  
  __base_RAM4 = 0x20280000 ; /* RAM4 */  
  __top_SRAM_OC_CACHEABLE = 0x20280000 + 0x0 ; /* 0M bytes */  
  __top_RAM4 = 0x20280000 + 0x0 ; /* 0M bytes */  
