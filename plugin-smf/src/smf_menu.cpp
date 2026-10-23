#include "smf_menu.hpp"
#include "forms.hpp"
#include "settings.hpp"

#include "SKSEMenuFramework.h"

// The old MCM Helper menu (same ranges and help text) as an SKSE Menu Framework page, finished edits apply at once and are saved to Data/SKSE/Plugins/HellbladeTimedBlock.ini

namespace {
    using namespace ImGuiMCP;

    // MCM help text, shown on hover
    void Help(const char* text) {
        if (IsItemHovered())
            SetTooltip("%s", text);
    }

    bool Slider(const char* label, float& value, float min, float max, const char* format, const char* help) {
        SliderFloat(label, &value, min, max, format);
        Help(help);
        if (!IsItemDeactivatedAfterEdit())
            return false;
        // Ctrl+click typing can leave the range
        value = std::clamp(value, min, max);
        return true;
    }

    void Commit() {
        Settings::Save();
        // Forms::Update writes game forms (global + spell effect duration and magnitude): do it on the game thread
        SKSE::GetTaskInterface()->AddTask([] { Forms::Update(); });
    }

    void __stdcall RenderSettings() {
        bool commit = false;

        SeparatorText("Timed Block Options");
        commit |= Checkbox("Enable Timed Block", &Settings::bTimedBlockEnabled);
        BeginDisabled(!Settings::bTimedBlockEnabled);
        commit |= Slider("Time Window##timed", Settings::fTimedBlockWindow, 0.1f, 2.0f, "%.2f s",
            "The maximum length of time, after raising a shield or weapon, that the player is able to perform a timed block.");
        commit |= Slider("Duration##timed", Settings::fTimedBlockSlowdownDuration, 0.3f, 1.0f, "%.2f s",
            "The duration of the slowed time following a successful timed block.");
        commit |= Slider("Time Scale##timed", Settings::fTimedBlockSlowdownMult, 0.1f, 1.0f, "%.2f",
            "How fast time runs at the peak of the slowdown after a timed block, 1.0 is normal speed.");
        EndDisabled();

        SeparatorText("Perfect Block Options");
        commit |= Checkbox("Enable Perfect Block", &Settings::bPerfectBlockEnabled);
        BeginDisabled(!Settings::bPerfectBlockEnabled);
        commit |= Slider("Time Window##perfect", Settings::fPerfectBlockWindow, 0.05f, 1.0f, "%.2f s",
            "The maximum length of time, after raising a shield or weapon, that the player is able to perform a perfect block.");
        commit |= Slider("Duration##perfect", Settings::fPerfectBlockSlowdownDuration, 1.0f, 3.0f, "%.2f s",
            "The duration of the slowed time following a successful perfect block.");
        commit |= Slider("Time Scale##perfect", Settings::fPerfectBlockSlowdownMult, 0.1f, 1.0f, "%.2f",
            "How fast time runs at the peak of the slowdown after a perfect block, 1.0 is normal speed.");
        commit |= Checkbox("Enable Damage Multiplier", &Settings::bPerfectBlockDamageMultEnabled);
        Help("Weapon damage will be increased for a period of time after performing a perfect block.");
        BeginDisabled(!Settings::bPerfectBlockDamageMultEnabled);
        int duration = static_cast<int>(Settings::iPerfectBlockDamageMultDuration);
        if (SliderInt("Multiplier Duration", &duration, 1, 6, "%d s"))
            Settings::iPerfectBlockDamageMultDuration = static_cast<unsigned int>(std::clamp(duration, 1, 6));
        Help("The length of time that the players attack damage will be increased for.");
        commit |= IsItemDeactivatedAfterEdit();
        commit |= Slider("Damage Multiplier", Settings::fPerfectBlockDamageMult, 1.0f, 5.0f, "%.1fx",
            "How many times the players attack damage is multiplied while the boost lasts.");
        EndDisabled();
        EndDisabled();

        // the MCM greyed this out with perfect block off, but the cooldown applies to timed blocks too
        SeparatorText("Miscellaneous");
        commit |= Slider("Cooldown Time", Settings::fCooldownTime, 0.0f, 1.0f, "%.2f s",
            "The amount of time that has to pass before another timed block can be performed.");

        Spacing();
        if (Button("Reset to defaults")) {
            Settings::ResetToDefaults();
            commit = true;
        }

        if (commit)
            Commit();
    }
}

void SMFMenu::Register() {
    if (!SKSEMenuFramework::IsInstalled() || !GetMenuFrameworkModule()) {
        logger::warn("SKSE Menu Framework not loaded; settings come from the ini files only"sv);
        return;
    }
    SKSEMenuFramework::SetSection("Hellblade - Timed Block");
    SKSEMenuFramework::AddSectionItem("Settings", RenderSettings);
    logger::info("SKSE Menu Framework page registered"sv);
}
