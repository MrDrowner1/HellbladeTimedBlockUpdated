#pragma once

#include <atomic>

#include <TrueDirectionalMovementAPI.h>

#include "easings.hpp"
#include "slowdown_helper.hpp"

namespace PRECISION_API
{
    class IVPrecision3;
    struct PrecisionHitData;
    struct PreHitCallbackReturn;
}

class BlockHandler final
{
public:
    static BlockHandler& Singleton()
    {
        static BlockHandler self;
        return self;
    }

    bool TryBlock(RE::Actor* attacker, RE::Actor* target);
    PRECISION_API::PreHitCallbackReturn TryBlock(const PRECISION_API::PrecisionHitData& hitData);
    void Update();
    void Reset();
    
    void RequestModAPIs();
    bool TryRegisterPrecisionCallback();

    BlockHandler() noexcept = default;
    BlockHandler(const BlockHandler&) = delete;
    BlockHandler(BlockHandler&&) = delete;
    BlockHandler& operator=(const BlockHandler&) = delete;
    BlockHandler& operator=(BlockHandler&&) = delete;
    
    static inline std::atomic<bool> Hit { };
private:
    bool TryBlockInternal(RE::Actor* attacker, RE::Actor* target, const RE::NiPoint3& hitPos = RE::NiPoint3::Zero());

    float deltaTime;
    float deltaTimeRealTime;
    float timeSpentBlocking;
    float timeCooldownRemaining;
    slowdown_helper<easings::testing<float>> slowHelper;

    static inline struct 
    {
        struct {
            PRECISION_API::IVPrecision3* g_ptr;
            bool isPreHitRegistered;
        } Precision;

        struct {
            TDM_API::IVTDM2* g_ptr;
            bool IsActorTarget(RE::Actor* actor) { return g_ptr && actor && g_ptr->GetCurrentTarget() == actor->GetHandle(); };
        } TDM;
    } API { };
};