/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#if ENABLE_VIT_ASR

#include "demo_definitions_vit.h"
#include "sln_voice_demo.h"

#include "de_voice_demos_vit.h"
#include "en_voice_demos_vit.h"
#include "fr_voice_demos_vit.h"
#include "cn_voice_demos_vit.h"

sln_voice_demo_t const * const all_voice_demos_vit[] = {
    &demo_elevator_en,
    &demo_washing_machine_en,
    &demo_smart_home_en,
    &demo_elevator_fr,
    &demo_washing_machine_fr,
    &demo_smart_home_fr,
    &demo_elevator_de,
    &demo_washing_machine_de,
    &demo_smart_home_de,
    &demo_elevator_cn,
    &demo_washing_machine_cn,
    &demo_smart_home_cn,
    NULL // end with NULL to signal list ending
};

#endif /* ENABLE_VIT_ASR */
