#pragma once

#define SI_CONVERT_GENERIC 1
#include <SimpleIni.h>

struct Settings {
    static inline float fCooldownTime { 0.f };

    static inline bool bTimedBlockEnabled { true };
    static inline float fTimedBlockSlowdownDuration { 0.6f }; //0.7
    static inline float fTimedBlockSlowdownMult { 0.6f };
    static inline float fTimedBlockWindow { 0.2f };

    static inline bool bPerfectBlockEnabled { true };
    static inline float fPerfectBlockSlowdownDuration { 1.2f };
    static inline float fPerfectBlockSlowdownMult { 0.4f };
    static inline float fPerfectBlockWindow { 0.05f };

    static inline bool bPerfectBlockDamageMultEnabled { true };
    static inline unsigned int iPerfectBlockDamageMultDuration { 4 };
    static inline float fPerfectBlockDamageMult { 2.f };

    static void ReadSettings();
    static void ReadSettingBool(CSimpleIniA& ini, const char* sectionName, const char* settingName, bool& setting);
    static void ReadSettingFloat(CSimpleIniA& ini, const char* sectionName, const char* settingName, float& setting);
    static void ReadSettingUInt32(CSimpleIniA& ini, const char* sectionName, const char* settingName, uint32_t& setting);
};