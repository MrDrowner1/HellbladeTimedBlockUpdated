# Hellblade - Timed Block - 1.7 Support

A fork of [Hellblade - Timed Block - AE Support](https://www.nexusmods.com/skyrimspecialedition/mods/87645)
(source: [moon_river/hellbladere](https://gitlab.com/moon_river/hellbladere)) for Skyrim SE/AE up to 1.7.x, built on
CommonLibSSE-NG. Two versions to chose from: classic MCM menu version, or SKSE Menu Framework version.

| Version | Settings menu | Source |
|---|---|---|
| **MCM** | the original MCM Helper menu, plus damage multiplier and time scale sliders | `plugin-mcm/` + `mcm/` |
| **SKSE Menu Framework** | an SKSE Menu Framework page | `plugin-smf/` + `papyrus/` |

## Fixes in both versions
- **Nexus 1.3.5.1 fixes included.** The public source stops at 1.3.2, this fork ports the 1.3.5.1 changes:
  - Light attacks no longer turn into power attacks after a timed block. 1.3.2 set the power-attack flag on the
    attacker's race attack data, which every actor of that race shares, the player included.
  - Damage lands before the reactions.
  - The attacker's recoil goes straight to its animation graph.
  - A re-entrancy guard on the block hit.
- **Block stays up after a perfect block.** Inside the behaviour's timed-block window (`HBTB_TimedBlock`) the plugin no
  longer sends `ActionLeftRelease` after the block hit, so holding the block key keeps you guarding.
- **Stability fixes.** Null checks on a few game objects, an earlier re-entrancy check, and a lazy delta-time lookup.
- **Configurable damage multiplier.** The ESP's perfect-block damage boost adds 1 to `AttackDamageMult` (2x damage).
  The new `fPerfectBlockDamageMult` setting sets that boost in memory.
- **Configurable slowdown strength.** `fTimedBlockSlowdownMult` and `fPerfectBlockSlowdownMult` were unused in the AE Support
  ini. Now you can actually control the time slowdown during a block.

## SKSE Menu Framework version
- Settings page with every MCM option, the same ranges and the help text as tooltips.
  Edits are saved to `SKSE/Plugins/HellbladeTimedBlock.ini`. "Reset to defaults" is included.
- Old MCM values (`MCM/Settings/HellbladeTimedBlock.ini`) are still read, and the SMF ini overrides them.
- The MCM menu script is replaced by a stub (`papyrus/Hellblade_MCM.psc`).

## Optional patch for MaxsuBlockOverhaul
`maxsu-patch/` lets you attack the recoiling attacker during the slowdown. MaxsuBlockOverhaul takes `blockHitStart`
for its own block-hit state, which only ends on its own events. The slowdown stretches those events, so you can't
attack until it ends. The patch adds `&& (HBTB_TimedBlock == 0)` to Maxsu's two transitions, so inside the window the
event goes to Hellblade's state. Regenerate behaviours with Pandora or Nemesis after installing.

## Requirements
- [Hellblade - Timed Block - AE Support](https://www.nexusmods.com/skyrimspecialedition/mods/87645)
- [Hellblade - Timed Block](https://www.nexusmods.com/skyrimspecialedition/mods/50658)
- Address Library for SKSE Plugins
- MCM version: [MCM Helper](https://www.nexusmods.com/skyrimspecialedition/mods/53000) and SkyUI
- SMF version: [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
- Optional: Precision and True Directional Movement
- Maxsu patch: MaxsuBlockOverhaul, plus Pandora or Nemesis

Load this mod after Hellblade - Timed Block - AE Support.

## Building
Both plugins build the same way. You need CMake ≥ 3.23, a C++23 MSVC or clang-cl toolchain, and vcpkg with a
CommonLibSSE-NG port that supports the game version you target (plus `fmt`, `spdlog`, `simpleini`).
```
cmake -S plugin-smf -B build-smf -DCMAKE_TOOLCHAIN_FILE=<vcpkg>/scripts/buildsystems/vcpkg.cmake
cmake --build build-smf --config Release
```
For the MCM version, use `plugin-mcm` and `build-mcm`.

## Credits
- **Distar**: the original author of [Hellblade - Timed Block](https://www.nexusmods.com/skyrimspecialedition/mods/50658)
- **thisdarkocean** (moon_river on GitLab): author of
  [Hellblade - Timed Block - AE Support](https://www.nexusmods.com/skyrimspecialedition/mods/87645)
  ([source](https://gitlab.com/moon_river/hellbladere)), which this fork is based on
- **max-su-2019**: [MaxsuBlockOverhaul](https://github.com/max-su-2019/MaxuBlockOverhaul), which the optional patch edits
- **Thiago099**: [SKSE Menu Framework](https://www.nexusmods.com/skyrimspecialedition/mods/120352)
  ([source](https://github.com/Thiago099/SKSE-Menu-Framework-2)), used by the SMF version
- **Parapets**: [MCM Helper](https://www.nexusmods.com/skyrimspecialedition/mods/53000), used by the MCM version
- **Ershin**: [Precision](https://www.nexusmods.com/skyrimspecialedition/mods/72347) and
  [True Directional Movement](https://www.nexusmods.com/skyrimspecialedition/mods/51614), whose API headers ship in `plugin-*/extern/`
- [CommonLibSSE-NG](https://github.com/CharmedBaryon/CommonLibSSE-NG) and its contributors

## Permissions
The original mod's permissions allow modified releases with credit to its author, and donation points must stay off for
files that use its assets.
