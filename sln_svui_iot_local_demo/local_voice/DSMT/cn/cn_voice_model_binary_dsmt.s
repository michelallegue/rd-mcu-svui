/*
 * Copyright 2021 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

.section .rodata
.align 4

.global cn_model_begin

cn_model_begin:
.incbin "./cn/oob_demo_cn_pack_WithMapID.bin"
cn_model_end:
