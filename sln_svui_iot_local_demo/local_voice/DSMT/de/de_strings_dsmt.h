/*
 * Copyright 2023 NXP.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef DSMT_DE_DE_STRINGS_DSMT_H_
#define DSMT_DE_DE_STRINGS_DSMT_H_

#if ENABLE_DSMT_ASR

/* German demos strings below.
 * These must match the DSMTv2 groups */

const char * const ww_de[] = {
        "Hallo NXP",
        "Hallo NXP ^1",
};

const char * const cmd_change_demo_de[] = {
        "Aufzug",
        "Waschmaschine",
        "Heimautomatisierung",
};

const char * const cmd_elevator_de[] = {
        "Erste etage",
        "Zweite etage",
        "Dritte etage",
        "Vierte etage",
        "Fünfte etage",
        "Hauptlobby",
        "Erdgeschoss",
        "Untergeschoss",
        "Öffne die tür",
        "Schließe die tür",
        "Anwendung wechseln",
};

const char * const cmd_washing_machine_de[] = {
        "Feinwäsche",
        "Normaler",
        "Stark verschmutze",
        "Weiß",
        "Starten",
        "Abbrechen",
        "Anwendung wechseln",
};

const char * const cmd_smart_home_de[] = {
        "Licht einschalten",
        "Licht ausschalten",
        "Temperatur erhöhen",
        "Temperatur verringern",
        "Fenster hoch",
        "Fenster runter",
        "Heller",
        "Dunkler",
        "Anwendung wechseln",
};

#endif /* ENABLE_DSMT_ASR */
#endif /* DSMT_DE_DE_STRINGS_DSMT_H_ */
