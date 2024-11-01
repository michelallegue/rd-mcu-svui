/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef VIT_FR_FR_STRINGS_TO_ACTIONS_VIT_H_
#define VIT_FR_FR_STRINGS_TO_ACTIONS_VIT_H_

#if ENABLE_VIT_ASR

#include "demo_actions_vit.h"
#include "stdint.h"

const int16_t actions_ww_fr[] = {
    kWakeWord_Detected, // "Salut NXP"
};

const int16_t actions_elevator_fr[] = {
    kElevator_FirstFloor,     // "Premier étage"
    kElevator_SecondFloor,    // "Deuxième étage"
    kElevator_ThirdFloor,     // "Troisième étage"
    kElevator_FourthFloor,    // "Quatrième étage"
    kElevator_FifthFloor,     // "Cinquième étage"
    kElevator_MainLobby,      // "Entrée principale"
    kElevator_GroundFloor,    // "Rez-de-chaussée"
    kElevator_BasementFloor,  // "Sous-sol"
    kElevator_OpenDoor,       // "Ouvrir porte"
    kElevator_CloseDoor,      // "Fermer porte"
    kElevator_ChangeDemo,     // "Changer de démo"
    kElevator_ChangeLanguage, // "Changer de langue"
    kElevator_Elevator,       // "Ascenseur"
    kElevator_WashingMachine, // "Lave-Linge"
    kElevator_SmartHome,      // "Maison Connectée"
    kElevator_English,        // "Anglais"
    kElevator_French,         // "Francais"
    kElevator_German,         // "Allemand"
    kElevator_Chinese,        // "Chinois"
};

const int16_t actions_washing_machine_fr[] = {
    kWashingMachine_Delicate,       // "Lavage délicat"
    kWashingMachine_Normal,         // "Lavage normal"
    kWashingMachine_HeavyDuty,      // "Lavage en profondeur"
    kWashingMachine_Whites,         // "Lavage blanc"
    kWashingMachine_Start,          // "Commencer"
    kWashingMachine_Cancel,         // "Annuler"
    kWashingMachine_ChangeDemo,     // "Changer de démo"
    kWashingMachine_ChangeLanguage, // "Changer de langue"
    kWashingMachine_Elevator,       // "Ascenseur"
    kWashingMachine_WashingMachine, // "Lave-Linge"
    kWashingMachine_SmartHome,      // "Maison Connectée"
    kWashingMachine_English,        // "Anglais"
    kWashingMachine_French,         // "Francais"
    kWashingMachine_German,         // "Allemand"
    kWashingMachine_Chinese,        // "Chinois"
};

const int16_t actions_smart_home_fr[] = {
    kSmartHome_TurnOnTheLights,   // "Allumer lumière"
    kSmartHome_TurnOffTheLights,  // "Éteindre lumière"
    kSmartHome_TemperatureHigher, // "Augmenter température"
    kSmartHome_TemperatureLower,  // "Diminuer température"
    kSmartHome_OpenTheWindow,     // "Ouvrir fenêtre"
    kSmartHome_CloseTheWindow,    // "Fermer fenêtre"
    kSmartHome_MakeItBrighter,    // "Augmenter luminosité"
    kSmartHome_MakeItDarker,      // "Diminuer luminosité"
    kSmartHome_ChangeDemo,        // "Changer de démo"
    kSmartHome_ChangeLanguage,    // "Changer de langue"
    kSmartHome_Elevator,          // "Ascenseur"
    kSmartHome_WashingMachine,    // "Lave-Linge"
    kSmartHome_SmartHome,         // "Maison Connectée"
    kSmartHome_English,           // "Anglais"
    kSmartHome_French,            // "Francais"
    kSmartHome_German,            // "Allemand"
    kSmartHome_Chinese,           // "Chinois"
};

#endif /* ENABLE_VIT_ASR */
#endif /* VIT_FR_FR_STRINGS_TO_ACTIONS_VIT_H_ */
