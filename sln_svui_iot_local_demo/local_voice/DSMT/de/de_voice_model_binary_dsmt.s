/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.section .rodata
.align 4

.global de_model_begin

de_model_begin:
.incbin "./de/oob_demo_de_pack_WithMapID.bin"
de_model_end:
