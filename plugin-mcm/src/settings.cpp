#include "forms.hpp"
#include "settings.hpp"
#include "util.hpp"

void Settings::ReadSettings() {
    constexpr auto settingsPathDefault = L"Data/MCM/Config/HellbladeTimedBlock/settings.ini";
    constexpr auto settingsPath = L"Data/MCM/Settings/HellbladeTimedBlock.ini";

    auto ReadMCMSettings = [&](std::filesystem::path path) {
        const auto pathStr = path.string();
        if (!std::filesystem::exists(path)) {
            logger::warn("Did not find config at: {}"sv, pathStr);
            return;
        }

        CSimpleIniA ini;
        ini.SetUnicode();

        if (ini.LoadFile(path.c_str()) < SI_OK) {
            logger::warn("Failed to load settings from: {}"sv, pathStr);
            return;
        }

        ReadSettingBool(ini, "Settings", "bTimedBlockEnabled", bTimedBlockEnabled);
        ReadSettingFloat(ini, "Settings", "fTimedBlockSlowdownDuration", fTimedBlockSlowdownDuration);
        ReadSettingFloat(ini, "Settings", "fTimedBlockSlowdownMult", fTimedBlockSlowdownMult);
        ReadSettingFloat(ini, "Settings", "fTimedBlockWindow", fTimedBlockWindow);

        Util::LimitLower(fTimedBlockSlowdownDuration, 0.f);
        Util::LimitLower(fTimedBlockSlowdownMult, 0.1f);
        Util::LimitHigher(fTimedBlockSlowdownMult, 1.f);
        Util::LimitLower(fTimedBlockWindow, 0.f);

        ReadSettingBool(ini, "Settings", "bPerfectBlockEnabled", bPerfectBlockEnabled);
        ReadSettingFloat(ini, "Settings", "fPerfectBlockSlowdownDuration", fPerfectBlockSlowdownDuration);
        ReadSettingFloat(ini, "Settings", "fPerfectBlockSlowdownMult", fPerfectBlockSlowdownMult);
        ReadSettingFloat(ini, "Settings", "fPerfectBlockWindow", fPerfectBlockWindow);
        
        Util::LimitLower(fPerfectBlockSlowdownDuration, 0.f);
        Util::LimitLower(fPerfectBlockSlowdownMult, 0.1f);
        Util::LimitHigher(fPerfectBlockSlowdownMult, 1.f);
        Util::LimitLower(fPerfectBlockWindow, 0.f);

        ReadSettingBool(ini, "Settings", "bPerfectBlockDamageMultEnabled", bPerfectBlockDamageMultEnabled);
        ReadSettingUInt32(ini, "Settings", "iPerfectBlockDamageMultDuration", iPerfectBlockDamageMultDuration);
        ReadSettingFloat(ini, "Settings", "fPerfectBlockDamageMult", fPerfectBlockDamageMult);
        Util::LimitLower(fPerfectBlockDamageMult, 1.f);

        ReadSettingFloat(ini, "Settings", "fCooldownTime", fCooldownTime);
        Util::LimitLower(fCooldownTime, 0.f);

        logger::info("Read settings from: {}"sv, pathStr);
    };

    ReadMCMSettings(settingsPathDefault);
    ReadMCMSettings(settingsPath);
}

void Settings::ReadSettingBool(CSimpleIniA& ini, const char* sectionName, const char* settingName, bool& setting) {
    if (ini.GetValue(sectionName, settingName))
        setting = ini.GetBoolValue(sectionName, settingName);
}

void Settings::ReadSettingFloat(CSimpleIniA& ini, const char* sectionName, const char* settingName, float& setting) {
    if (ini.GetValue(sectionName, settingName))
        setting = static_cast<float>(ini.GetDoubleValue(sectionName, settingName));
}

void Settings::ReadSettingUInt32(CSimpleIniA& ini, const char* sectionName, const char* settingName, uint32_t& setting) {
    if (ini.GetValue(sectionName, settingName))
        setting = static_cast<uint32_t>(ini.GetLongValue(sectionName, settingName));
}