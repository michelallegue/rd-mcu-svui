/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.section .rodata
.align 4

.global fr_model_begin

fr_model_begin:
.incbin "./fr/oob_demo_fr_pack_WithMapID.bin"
fr_model_end:
