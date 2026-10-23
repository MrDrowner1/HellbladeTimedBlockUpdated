#pragma once

struct Forms
{
    // static inline RE::TESObjectWEAP*   SKY_Unarmed;
    // static inline RE::BGSMaterialType* SKY_MaterialArmorHeavy;
    // static inline RE::BGSMaterialType* SKY_MaterialArmorLight;
    // static inline RE::BGSMaterialType* SKY_MaterialCloth;
    // static inline RE::BGSMaterialType* SKY_MaterialShieldLight;
    // static inline RE::BGSMaterialType* SKY_MaterialShieldHeavy;
    static inline RE::BGSAction* ActionBlockHit;
    static inline RE::BGSAction* ActionLargeRecoil;
    static inline RE::BGSAction* ActionLeftRelease;

    static inline RE::TESGlobal*              HB_DamageBoostEnabled;
    // static inline RE::BGSSoundDescriptorForm* HB_TimedBlockSD;
    static inline RE::BGSSoundDescriptorForm* HB_TimedBlock01SD;
    static inline RE::BGSSoundDescriptorForm* HB_TimedBlock02SD;
    static inline RE::BGSSoundDescriptorForm* HB_PerfectBlockSD;
    static inline RE::EffectSetting*          HB_DamageBoostEffect;
    static inline RE::EffectSetting*          HB_PerfectBlockEffect;
    static inline RE::SpellItem*              HB_PerfectBlockSpell;
    // static inline RE::SpellItem*              HB_StaggerCooldownSpell;

    static void Load();
    static void Update();
};