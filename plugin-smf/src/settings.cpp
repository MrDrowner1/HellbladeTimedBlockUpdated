#include "forms.hpp"
#include "settings.hpp"
#include "util.hpp"

void Settings::ReadSettings() {
    constexpr auto settingsPathDefault = L"Data/MCM/Config/HellbladeTimedBlock/settings.ini";
    constexpr auto settingsPath = L"Data/MCM/Settings/HellbladeTimedBlock.ini";
    // SMF port: the SKSE Menu Framework page saves here, read last so it wins over the old MCM files.
    constexpr auto smfSettingsPath = L"Data/SKSE/Plugins/HellbladeTimedBlock.ini";

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
    ReadMCMSettings(smfSettingsPath);
}

void Settings::Save() {
    constexpr auto smfSettingsPath = L"Data/SKSE/Plugins/HellbladeTimedBlock.ini";

    CSimpleIniA ini;
    ini.SetUnicode();
    ini.SetBoolValue("Settings", "bTimedBlockEnabled", bTimedBlockEnabled);
    ini.SetDoubleValue("Settings", "fTimedBlockWindow", fTimedBlockWindow);
    ini.SetDoubleValue("Settings", "fTimedBlockSlowdownDuration", fTimedBlockSlowdownDuration);
    ini.SetDoubleValue("Settings", "fTimedBlockSlowdownMult", fTimedBlockSlowdownMult);
    ini.SetBoolValue("Settings", "bPerfectBlockEnabled", bPerfectBlockEnabled);
    ini.SetDoubleValue("Settings", "fPerfectBlockWindow", fPerfectBlockWindow);
    ini.SetDoubleValue("Settings", "fPerfectBlockSlowdownDuration", fPerfectBlockSlowdownDuration);
    ini.SetDoubleValue("Settings", "fPerfectBlockSlowdownMult", fPerfectBlockSlowdownMult);
    ini.SetBoolValue("Settings", "bPerfectBlockDamageMultEnabled", bPerfectBlockDamageMultEnabled);
    ini.SetLongValue("Settings", "iPerfectBlockDamageMultDuration", static_cast<long>(iPerfectBlockDamageMultDuration));
    ini.SetDoubleValue("Settings", "fPerfectBlockDamageMult", fPerfectBlockDamageMult);
    ini.SetDoubleValue("Settings", "fCooldownTime", fCooldownTime);

    if (ini.SaveFile(smfSettingsPath) < SI_OK)
        logger::warn("Failed to save settings to: Data/SKSE/Plugins/HellbladeTimedBlock.ini"sv);
    else
        logger::info("Saved settings to: Data/SKSE/Plugins/HellbladeTimedBlock.ini"sv);
}

// The MCM's defaultValue entries (MCM/Config/HellbladeTimedBlock/config.json).
void Settings::ResetToDefaults() {
    bTimedBlockEnabled = true;
    fTimedBlockWindow = 0.2f;
    fTimedBlockSlowdownDuration = 0.6f;
    fTimedBlockSlowdownMult = 0.6f;
    bPerfectBlockEnabled = true;
    fPerfectBlockWindow = 0.05f;
    fPerfectBlockSlowdownDuration = 1.2f;
    fPerfectBlockSlowdownMult = 0.4f;
    bPerfectBlockDamageMultEnabled = true;
    iPerfectBlockDamageMultDuration = 4;
    fPerfectBlockDamageMult = 2.f;
    fCooldownTime = 0.f;
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