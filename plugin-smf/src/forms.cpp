#include "forms.hpp"
#include "settings.hpp"

void Forms::Load() {
    const auto dataHandler = RE::TESDataHandler::GetSingleton();
    if (dataHandler == nullptr)
        return;

    constexpr const std::string_view SKYRIM_PLUGIN_NAME = "Skyrim.esm"sv;
    ActionBlockHit = dataHandler->LookupForm<RE::BGSAction>(0x13AF4, SKYRIM_PLUGIN_NAME);
    ActionLargeRecoil = dataHandler->LookupForm<RE::BGSAction>(0x13EC8, SKYRIM_PLUGIN_NAME);
    ActionLeftRelease = dataHandler->LookupForm<RE::BGSAction>(0x13451, SKYRIM_PLUGIN_NAME);
    // SKY_Unarmed = dataHandler->LookupForm<RE::TESObjectWEAP>(0x1F4, skyrim);
    // SKY_MaterialArmorHeavy = dataHandler->LookupForm<RE::BGSMaterialType>(0x388FB, skyrim);
    // SKY_MaterialArmorLight = dataHandler->LookupForm<RE::BGSMaterialType>(0x388FC, skyrim);
    // SKY_MaterialCloth = dataHandler->LookupForm<RE::BGSMaterialType>(0x12F37, skyrim);
    // SKY_MaterialShieldLight = dataHandler->LookupForm<RE::BGSMaterialType>(0x16978, skyrim);
    // SKY_MaterialShieldHeavy = dataHandler->LookupForm<RE::BGSMaterialType>(0x16979, skyrim);

    constexpr const std::string_view PLUGIN_NAME = "HellbladeTimedBlock.esp"sv;
    HB_DamageBoostEnabled = dataHandler->LookupForm<RE::TESGlobal>(0x808, PLUGIN_NAME);
    HB_TimedBlock01SD = dataHandler->LookupForm<RE::BGSSoundDescriptorForm>(0x80C, PLUGIN_NAME);
    HB_TimedBlock02SD = dataHandler->LookupForm<RE::BGSSoundDescriptorForm>(0x80D, PLUGIN_NAME);
    HB_PerfectBlockSD = dataHandler->LookupForm<RE::BGSSoundDescriptorForm>(0x804, PLUGIN_NAME);
    HB_DamageBoostEffect = dataHandler->LookupForm<RE::EffectSetting>(0x80B, PLUGIN_NAME);
    HB_PerfectBlockEffect = dataHandler->LookupForm<RE::EffectSetting>(0x802, PLUGIN_NAME);
    HB_PerfectBlockSpell = dataHandler->LookupForm<RE::SpellItem>(0x803, PLUGIN_NAME);
    // HB_StaggerCooldownSpell = dataHandler->LookupForm<RE::SpellItem>(0x80A, PLUGIN_NAME);

    std::vector<RE::TESForm*> forms { 
        HB_DamageBoostEnabled, HB_TimedBlock01SD, HB_TimedBlock02SD, HB_PerfectBlockSD, HB_DamageBoostEffect, HB_PerfectBlockEffect, HB_PerfectBlockSpell 
    };
    
    if (const auto missing = std::ranges::count_if(forms, [](auto form) { return form == nullptr; }); missing > 0) {
        const auto err = fmt::format("Could not find {} forms in {}"sv, missing, PLUGIN_NAME);
        stl::report_and_error(err);
    }
}

void Forms::Update() {
    HB_DamageBoostEnabled->value = Settings::bPerfectBlockDamageMultEnabled ? 1.f : 0.f;
    for (auto& eff : HB_PerfectBlockSpell->effects) {
        if (eff->baseEffect != Forms::HB_DamageBoostEffect)
            continue;

        eff->effectItem.duration = Settings::iPerfectBlockDamageMultDuration;
        // The ESP's effect adds its magnitude to AttackDamageMult, whose base is 1
        eff->effectItem.magnitude = Settings::fPerfectBlockDamageMult - 1.f;
        logger::info("Set perfect block damage boost to {}x for {} seconds"sv, Settings::fPerfectBlockDamageMult, Settings::iPerfectBlockDamageMultDuration);
        break;
    }
}