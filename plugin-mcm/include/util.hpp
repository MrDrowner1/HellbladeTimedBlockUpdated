#pragma once

// #include "forms.hpp"

template <typename T> concept arithmetic = std::is_arithmetic_v<T>;

namespace Offsets
{
    inline float* g_DeltaTime() { static const auto ptr = (float*)REL::VariantID(523660, 410199, 0x30C3A08).address(); return ptr; }
    inline float* g_DeltaRealTime() { static const auto ptr = (float*)REL::VariantID(523661, 410200, 0x30C3A0C).address(); return ptr; }

    inline constexpr REL::RelocationID Actor_DealDamage { 37673, 38627 };
    inline constexpr REL::RelocationID Actor_GetEquippedShield { 37624, 38577 };
    inline constexpr REL::RelocationID AIProcess_GetCurrentlyEquippedWeapon { 38781, 39806 };
    inline constexpr REL::VariantID BGSImpactDataSet_GetImpactData { 20408, 20860, 0x2D4C00 };
    inline constexpr REL::RelocationID BSTimer_GetSingleton { 523657, 410196 };
    inline constexpr REL::RelocationID BSTimer_SetGlobalTimeMultiplier { 66988, 68245 };
    inline constexpr REL::VariantID HitData_Ctor { 42826, 43995, 0x76D000 };
    inline constexpr REL::VariantID HitData_Populate { 42832, 44001, 0x76D400 };
    inline constexpr REL::RelocationID NiObjectFromCollidable { 76160, 77988 };
    inline constexpr REL::RelocationID PerformAction { 40551, 41557 };
    inline constexpr REL::RelocationID PlaceParticleEffect { 29219, 30072 };
    inline constexpr REL::RelocationID PlayIdle { 38290, 39256 };
    inline constexpr REL::RelocationID TryStagger { 36700, 37710 };
};

namespace Util
{
    template <typename T> 
    bool ApproximatelyEqual(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon()) {
        const T ax = std::abs(a);
        const T bx = std::abs(b);

        return std::abs(a - b) <= (ax < bx ? bx : ax) * epsilon;
    }

    template <typename T>
    bool ApproximatelyGreaterOrEqual(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon()) {
        return a > b || ApproximatelyEqual(a, b, epsilon);
    }

    template <typename T>
    bool ApproximatelyLessOrEqual(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon()) {
        return a < b || ApproximatelyEqual(a, b, epsilon);
    }

    template <typename T>
    bool DefinitelyGreater(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon()) {
        const T ax = std::abs(a);
        const T bx = std::abs(b);

        return (a - b) > (ax < bx ? bx : ax) * epsilon;
    }

    template <typename T>
    bool DefinitelyLess(const T& a, const T& b, T epsilon = std::numeric_limits<T>::epsilon()) {
        const T ax = std::abs(a);
        const T bx = std::abs(b);

        return (b - a) > (ax < bx ? bx : ax) * epsilon;
    }

    template <typename T> requires arithmetic<T>
    void Limit(T& in, const T& min, const T& max) {
        if (min > max) 
            return;

        LimitLower(in, min);
        LimitHigher(in, max);
    }

    template <typename T> requires arithmetic<T>
    void LimitLower(T& in, const T& min) {
        if (in < min) {
            in = min;
        }
    }

    template <typename T> requires arithmetic<T>
    void LimitHigher(T& in, const T& max) {
        if (in > max) {
            in = max;
        }
    }

    // RE::BGSImpactData* GetBlockImpact(RE::Actor* attacker, RE::Actor* target) {
    //     auto GetAttackImpactSet = [](RE::Actor* actor) -> RE::BGSImpactDataSet* {
    //         if (actor) {
    //             const auto entry = actor->GetAttackingWeapon();
    //             const auto bound = entry ? entry->GetObject() : nullptr;
    //             const auto weapon = bound ? bound->As<RE::TESObjectWEAP>() : nullptr;

    //             if (const auto impactDataSet = weapon ? weapon->impactDataSet : nullptr)
    //                 return impactDataSet;

    //             if (const auto race = actor->GetRace())
    //                 return race->impactDataSet;
    //         }

    //         return nullptr;
    //     };

    //     auto GetBlockMaterial = [](RE::Actor* actor) -> RE::BGSMaterialType* {
    //         if (!actor)
    //             return nullptr;

    //         auto process = actor->GetActorRuntimeData().currentProcess;
    //         if (!process || !process->high)
    //             return nullptr;

    //         const auto left = process->GetEquippedLeftHand();
    //         if (const auto shield = left ? left->As<RE::TESObjectARMO>() : nullptr)
    //             return shield->altBlockMaterialType;

    //         const auto right = process->GetEquippedRightHand();
    //         if (const auto weapon = right ? right->As<RE::TESObjectWEAP>() : nullptr; weapon && weapon->IsMelee()) {
    //             if (weapon == Forms::SKY_Unarmed) {
    //                 if (const auto hands = actor->GetWornArmor(RE::BGSBipedObjectForm::BipedObjectSlot::kHands)) {
    //                     switch (hands->GetArmorType()) {
    //                         case RE::BGSBipedObjectForm::ArmorType::kLightArmor:
    //                             return Forms::SKY_MaterialArmorLight;
    //                         case RE::BGSBipedObjectForm::ArmorType::kHeavyArmor:
    //                             return Forms::SKY_MaterialArmorHeavy;
    //                         case RE::BGSBipedObjectForm::ArmorType::kClothing:
    //                             return Forms::SKY_MaterialCloth;
    //                     }
    //                 }
    //             }
                
    //             return weapon->altBlockMaterialType;
    //         }

    //         if (const auto race = actor->GetRace())
    //             return race->bloodImpactMaterial;

    //         return nullptr;
    //     };

    //     const auto impactSet = GetAttackImpactSet(attacker);
    //     const auto blockMaterial = GetBlockMaterial(target);

    //     return BGSImpactDataSet_GetImpactData(impactSet, blockMaterial);
    // }

    auto Actor_DealDamage(RE::Actor* aggressor, RE::Actor* target, RE::Projectile* sourceProjectile, bool isLeftHand = false) -> void*;
    auto Actor_GetEquippedShield(RE::Actor* actor) -> RE::TESObjectREFR*;
    auto AIProcess_GetCurrentlyEquippedWeapon(RE::AIProcess* self, bool isLeftHand = false) -> RE::InventoryEntryData*;
    auto BGSImpactDataSet_GetImpactData(RE::BGSImpactDataSet* impactSet, RE::BGSMaterialType* material) -> RE::BGSImpactData*;
    auto CastSpell(RE::Actor* caster, RE::Actor* target, RE::SpellItem* spell) -> void;
    auto GetNiObjectFromCollidable(const RE::hkpCollidable* collidable) -> RE::NiAVObject*;
    auto HitData_Ctor(RE::HitData* self) -> void;
    auto HitData_Populate(RE::HitData* self, RE::Actor* a_aggressor, RE::Actor* a_target, RE::InventoryEntryData* a_weapon, bool isLeftHand = false) -> void;
    auto IsInBlockAngle(RE::Actor* defender, RE::TESObjectREFR* refr) -> bool;
    auto NotifyAnimationGraph(RE::BGSAction* action, RE::Actor* actor) -> void;
    auto PerformAction(RE::BGSAction* action, RE::Actor* actor, RE::TESObjectREFR* target = nullptr) -> bool;
    auto TryStagger(RE::Actor* target, float staggerMult, RE::Actor* aggressor) -> void;
    auto TryPlaySoundAt(const RE::TESObjectREFR* refr, RE::BGSSoundDescriptorForm* descriptor) -> bool;
    auto TryPlaySoundAt(const RE::TESObjectREFR* refr, RE::BGSSoundDescriptorForm* descriptor, float volume) -> bool;

    void   SetGlobalTimeMult(float multiplier);
    float& GetGlobalTimeMult();

    // RE::BSTempEffectParticle* PlaceParticleEffect(RE::TESObjectCELL* cell, float lifetime, const char* model, const RE::NiMatrix3& normal, const RE::NiPoint3& pos, float scale, std::uint32_t flags, RE::NiAVObject* target) {
    //     using func_t = decltype(&PlaceParticleEffect);
    //     REL::Relocation<func_t> func { Offsets::PlaceParticleEffect };
    //     return func(cell, lifetime, model, normal, pos, scale, flags, target);
    // }

    // void PlayImpact(RE::Actor* refr, RE::BGSImpactData* impact, RE::NiPointer<RE::NiAVObject> node, RE::NiPoint3 pos) {
    //     PlaceParticleEffect(refr->GetParentCell(), 0.0f, impact->model.data(), node->world.rotate, pos, 1.f, 4u, node.get());
    // } 
}