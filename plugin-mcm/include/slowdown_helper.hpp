#pragma once

#include "easings.hpp"
#include "util.hpp"

template <arithmetic easing_type, easing<easing_type> easing_func = easings::linear<easing_type>>
class untyped_slowdown_helper final 
{
public:
    bool can_run() const
    {
        return !is_running() && Util::ApproximatelyEqual(Util::GetGlobalTimeMult(), 1.f);
    }

    bool is_running() const noexcept 
    {
        return isActive;
    }

    void update()
    {
        if (is_running()) {
            deltaTime = std::clamp(*Offsets::g_DeltaRealTime(), min_delta, max_delta);
            accumulate += deltaTime;

            float prevMultiplier = currentMultiplier;
            while (accumulate >= min_delta) {
                elapsedTime += min_delta;
                prevMultiplier = currentMultiplier;
                currentMultiplier = easing_func::interpolate(elapsedTime, duration, initialMultiplier, targetMultiplier);
                accumulate -= min_delta;
            }
            
            const float alpha = accumulate / min_delta;
            const float nextMultiplier = std::clamp(currentMultiplier * alpha + prevMultiplier * (1 - alpha), 0.f, 1.f);

            Util::SetGlobalTimeMult(nextMultiplier);

            if (Util::ApproximatelyGreaterOrEqual(elapsedTime, duration)) {
                stop();
            }
        }
    }

    void start(float mult, float dur) 
    {
        if (is_running()) {
            return;
        }

        duration = dur;
        initialMultiplier = mult;
        currentMultiplier = std::clamp(Util::GetGlobalTimeMult(), 0.f, 1.f);

        isActive = true;
    }

    void stop() 
    {
        accumulate = 0.f;
        elapsedTime = 0.f;
        duration = 0.f;
        currentMultiplier = 0.f;

        if (is_running()) {
            Util::SetGlobalTimeMult(1.f);
            isActive = false;
        }
    }

    untyped_slowdown_helper() noexcept = default;
    untyped_slowdown_helper(const untyped_slowdown_helper&) = delete;
    untyped_slowdown_helper(untyped_slowdown_helper&&) = delete;
    untyped_slowdown_helper& operator=(const untyped_slowdown_helper&) = delete;
    untyped_slowdown_helper& operator=(untyped_slowdown_helper&&) = delete;
private:
    constexpr static float min_delta { 0.016f };
    constexpr static float max_delta { 0.033f };

    bool isActive;
    float deltaTime;
    float accumulate;

    float elapsedTime;
    float duration;
    float initialMultiplier;
    float currentMultiplier;
    float targetMultiplier { 1.f };
};

template <typename T>
using slowdown_helper = untyped_slowdown_helper<float, T>;