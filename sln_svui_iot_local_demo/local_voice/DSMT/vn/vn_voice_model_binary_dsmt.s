/*
 * Copyright 2024 NXP
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.section .rodata
.align 4

.global vn_model_begin

vn_model_begin:
.incbin "./vn/oob_demo_vn_pack_WithMapID.bin"
vn_model_end:
