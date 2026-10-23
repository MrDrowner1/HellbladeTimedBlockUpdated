#include <PrecisionAPI.h>

#include "forms.hpp"
#include "block_handler.hpp"
#include "settings.hpp"
#include "util.hpp"

bool BlockHandler::TryBlock(RE::Actor* attacker, RE::Actor* target) 
{   
    if (!attacker || !(target && target->IsPlayerRef()))
        return false;

    auto GetBipedIndex = [target]() {
        if (Util::Actor_GetEquippedShield(target) != nullptr) {
            return RE::BIPED_OBJECT::kShield;
        }

        if (const auto process = target->GetActorRuntimeData().currentProcess) {
            for (int i = 0; i < static_cast<int>(RE::AIProcess::Hand::kTotal); i++) {
                const auto equipped = process->equippedObjects[i];
                const auto weapon = equipped ? equipped->As<RE::TESObjectWEAP>() : nullptr;

                if (weapon == nullptr) {
                    continue;
                }

                switch (weapon->GetWeaponType()) {
                    case RE::WEAPON_TYPE::kOneHandSword:
                        return RE::BIPED_OBJECT::kOneHandSword;
                    case RE::WEAPON_TYPE::kOneHandDagger:
                        return RE::BIPED_OBJECT::kOneHandDagger;
                    case RE::WEAPON_TYPE::kOneHandAxe:
                        return RE::BIPED_OBJECT::kOneHandAxe;
                    case RE::WEAPON_TYPE::kOneHandMace:
                        return RE::BIPED_OBJECT::kOneHandMace;
                    case RE::WEAPON_TYPE::kHandToHandMelee:
                    case RE::WEAPON_TYPE::kTwoHandSword:
                    case RE::WEAPON_TYPE::kTwoHandAxe:
                        return RE::BIPED_OBJECT::kTwoHandMelee;
                    case RE::WEAPON_TYPE::kStaff:
                        return RE::BIPED_OBJECT::kStaff;
                    default: continue;
                }
            }

            if (std::ranges::all_of(process->equippedObjects, [](RE::TESForm* form) { return form == nullptr; })) {
                return RE::BIPED_OBJECT::kTwoHandMelee;
            }
        }

        return RE::BIPED_OBJECT::kNone;
    };

    const auto bipedIndex = GetBipedIndex();
    if (bipedIndex == RE::BIPED_OBJECT::kNone) {
        return false;
    }

    const auto biped = target->GetCurrentBiped();
    const auto node = biped ? biped->objects[bipedIndex].partClone.get() : nullptr;
    return TryBlockInternal(attacker, target, node ? node->worldBound.center : RE::NiPoint3::Zero());
}

PRECISION_API::PreHitCallbackReturn BlockHandler::TryBlock(const PRECISION_API::PrecisionHitData& hitData) 
{
    const auto attacker = hitData.attacker;
    const auto defender = hitData.target && hitData.target->IsPlayerRef() ? hitData.target->As<RE::Actor>() : nullptr;

    // auto node = Util::GetNiObjectFromCollidable(hitData.hitRigidBody->GetCollidable());
    bool ret = attacker && defender && TryBlockInternal(attacker, defender, hitData.hitPos);
    return { .bIgnoreHit = ret, .modifiers = {} };
}

void BlockHandler::Update() 
{
    deltaTime = *Offsets::g_DeltaTime();
    deltaTimeRealTime = *Offsets::g_DeltaRealTime();

    timeSpentBlocking += deltaTime;
    if (!RE::PlayerCharacter::GetSingleton()->IsBlocking() || slowHelper.is_running()) {
        timeSpentBlocking = 0.f;
    }

    if (timeCooldownRemaining > 0.f) {
        timeCooldownRemaining -= deltaTime;
    }

    slowHelper.update();
}

void BlockHandler::Reset() 
{
    timeSpentBlocking = 0.f;
    slowHelper.stop();
}

void BlockHandler::RequestModAPIs()
{
    if (!API.Precision.g_ptr) {
        if (auto ptr = reinterpret_cast<PRECISION_API::IVPrecision3*>(PRECISION_API::RequestPluginAPI(PRECISION_API::InterfaceVersion::V3))) {
            API.Precision.g_ptr = ptr;
            logger::info("Got Precision API"sv);
        }
    }

    if (!API.TDM.g_ptr) {
        if (auto ptr = reinterpret_cast<TDM_API::IVTDM2*>(TDM_API::RequestPluginAPI())) {
            API.TDM.g_ptr = ptr;
            logger::info("Got TrueDirectionalMovement API"sv);
        }
    }
}

bool BlockHandler::TryRegisterPrecisionCallback() 
{
    if (!API.Precision.g_ptr) {
        return false;
    }

    if (!API.Precision.isPreHitRegistered) {
        auto result = API.Precision.g_ptr->AddPreHitCallback(SKSE::GetPluginHandle(), [this](const auto& hitData) {
            return TryBlock(hitData);
        });

        switch (result) {
            case PRECISION_API::APIResult::OK:
                logger::info("Registered for Precision PreHitCallback"sv);
                API.Precision.isPreHitRegistered = true;
                break;
            case PRECISION_API::APIResult::AlreadyRegistered:
            case PRECISION_API::APIResult::NotRegistered:
                break;
        }
    }

    return API.Precision.isPreHitRegistered;
}

bool BlockHandler::TryBlockInternal(RE::Actor* attacker, RE::Actor* target, const RE::NiPoint3& hitPos) 
{
    if (Util::DefinitelyGreater(timeCooldownRemaining, 0.f)) {
        return false;
    }

    if (timeSpentBlocking <= 0.f || !(Util::IsInBlockAngle(target, attacker) || API.TDM.IsActorTarget(attacker))) {
        return false;
    }

    // float epsilon = std::numeric_limits<float>::epsilon();
    // if (API.Precision.g_ptr) {
    //     const float collisionLength = API.Precision.g_ptr->GetAttackCollisionCapsuleLength(attacker->GetHandle(), PRECISION_API::RequestedAttackCollisionType::Current);
    //     if (Util::ApproximatelyEqual(collisionLength, 0.f))
    //         epsilon = 0.5f;
    // }

    float epsilon = 0.5f;
    if (const auto precision = API.Precision.g_ptr) {
        const float collisionLength = precision->GetAttackCollisionCapsuleLength(attacker->GetHandle(), PRECISION_API::RequestedAttackCollisionType::Current);
        if (Util::DefinitelyGreater(collisionLength, 0.f))
            epsilon = std::numeric_limits<float>::epsilon();
    }

    const bool isInPerfectBlockWindow = Settings::bPerfectBlockEnabled && Util::ApproximatelyGreaterOrEqual(Settings::fPerfectBlockWindow, timeSpentBlocking, epsilon);
    const bool isInTimedBlockWindow = isInPerfectBlockWindow || (Settings::bTimedBlockEnabled && Util::ApproximatelyGreaterOrEqual(Settings::fTimedBlockWindow, timeSpentBlocking, epsilon));

    if (isInTimedBlockWindow) {
        // 1.3.5.1 (Nexus 87645 hotfix, never pushed to GitLab): Hit is atomic and doubles as a re-entrancy guard, checked before the slowdown and spell
        bool expected = false;
        if (!Hit.compare_exchange_strong(expected, true)) {
            return false;
        }

        if (isInPerfectBlockWindow) {
            if (slowHelper.can_run()) {
                slowHelper.start(Settings::fPerfectBlockSlowdownMult, Settings::fPerfectBlockSlowdownDuration);
            }
            Util::CastSpell(target, target, Forms::HB_PerfectBlockSpell);
        }

        if (Settings::bTimedBlockEnabled || (Settings::bPerfectBlockEnabled && slowHelper.can_run())) {
            slowHelper.start(Settings::fTimedBlockSlowdownMult, Settings::fTimedBlockSlowdownDuration);
        }

        RE::HitData hitData;
        Util::HitData_Ctor(&hitData);
        Util::HitData_Populate(&hitData, attacker, target, attacker->GetAttackingWeapon());

        hitData.hitPosition = { hitPos.x, hitPos.y, hitPos.z };
        hitData.flags |= RE::HitData::Flag::kBlocked;

        bool isWeaponBlock = false;

        if (!Util::Actor_GetEquippedShield(target)) {
            isWeaponBlock = true;
            hitData.flags |= RE::HitData::Flag::kBlockWithWeapon;
        }

        SKSE::GetTaskInterface()->AddTask([target, isInPerfectBlockWindow, isWeaponBlock]() {
            Util::TryPlaySoundAt(target, isWeaponBlock ? Forms::HB_TimedBlock01SD : Forms::HB_TimedBlock02SD, 0.9f);
            if (isInPerfectBlockWindow)
                Util::TryPlaySoundAt(target, Forms::HB_PerfectBlockSD);
        });

        // 1.3.5.1: kPowerAttack is no longer set on hitData.attackData, it's shared by every actor of the race (player too) and made light attacks cost power-attack stamina
        const bool isLeftAttack = hitData.attackData && hitData.attackData->IsLeftAttack();

        // 1.3.5.1: damage first, then the recoil/block reactions
        Util::Actor_DealDamage(attacker, target, nullptr, isLeftAttack);

        constexpr const std::string_view GRAPH_VAR_NO_STAGGER = "bNoStagger"sv;

        bool bNoStagger = false;
        attacker->GetGraphVariableBool(GRAPH_VAR_NO_STAGGER, bNoStagger);
        attacker->SetGraphVariableBool(GRAPH_VAR_NO_STAGGER, true);

        // 1.3.5.1: the attacker's recoil is sent to its animation graph instead of performed, and the defender performs ActionLeftRelease after the block hit
        // 1.3.5.1 also dropped the PlayerControls attackBlockHandler->heldTimeMs reset
        // Fork: in the Hellblade parry window (HBTB_TimedBlock, Nexus 50658 patch) blockHitStart plays the parry, ActionLeftRelease would end the block and cut it off
        bool isParry = false;
        target->GetGraphVariableBool("HBTB_TimedBlock"sv, isParry);

        Util::NotifyAnimationGraph(Forms::ActionLargeRecoil, attacker);
        Util::PerformAction(Forms::ActionBlockHit, target);
        if (!isParry) {
            Util::PerformAction(Forms::ActionLeftRelease, target);
        }

        attacker->SetGraphVariableBool(GRAPH_VAR_NO_STAGGER, bNoStagger);

        timeCooldownRemaining = Settings::fCooldownTime;
        Hit = false;

        return true;
    }

    return false;
}