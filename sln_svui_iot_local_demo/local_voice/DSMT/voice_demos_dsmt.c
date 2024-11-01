/*
 * Copyright 2023-2024 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_DSMT_ASR

#include "demo_definitions_dsmt.h"
#include "cn_voice_demos_dsmt.h"
#include "de_voice_demos_dsmt.h"
#include "en_voice_demos_dsmt.h"
#include "fr_voice_demos_dsmt.h"
#include "hi_voice_demos_dsmt.h"
#include "nl_voice_demos_dsmt.h"
#include "th_voice_demos_dsmt.h"
#include "vn_voice_demos_dsmt.h"
#include "sln_voice_demo.h"

sln_voice_demo_t const * const all_voice_demos_dsmt[] = {
    &demo_change_demo_en,
    &demo_elevator_en,
    &demo_washing_machine_en,
    &demo_smart_home_en,
    &demo_change_demo_fr,
    &demo_elevator_fr,
    &demo_washing_machine_fr,
    &demo_smart_home_fr,
    &demo_change_demo_de,
    &demo_elevator_de,
    &demo_washing_machine_de,
    &demo_smart_home_de,
    &demo_change_demo_cn,
    &demo_elevator_cn,
    &demo_washing_machine_cn,
    &demo_smart_home_cn,
    &demo_change_demo_hi,
    &demo_elevator_hi,
    &demo_washing_machine_hi,
    &demo_smart_home_hi,
    &demo_change_demo_nl,
    &demo_elevator_nl,
    &demo_washing_machine_nl,
    &demo_smart_home_nl,
    &demo_change_demo_th,
    &demo_elevator_th,
    &demo_washing_machine_th,
    &demo_smart_home_th,
    &demo_change_demo_vn,
    &demo_elevator_vn,
    &demo_washing_machine_vn,
    &demo_smart_home_vn,
    NULL // end with NULL to signal list ending
};

#endif /* ENABLE_DSMT_ASR */
