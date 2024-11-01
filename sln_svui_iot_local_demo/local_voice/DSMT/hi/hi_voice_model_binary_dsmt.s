/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.section .rodata
.align 4

.global hi_model_begin

hi_model_begin:
.incbin "./hi/oob_demo_hi_pack_WithMapID.bin"
hi_model_end:
