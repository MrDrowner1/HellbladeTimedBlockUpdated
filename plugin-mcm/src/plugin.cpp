#include "forms.hpp"
#include "hooks.hpp"
#include "papyrus.hpp"
#include "block_handler.hpp"
#include "settings.hpp"

#include <cctype>
#include <fstream>

// The SMF package's stub Hellblade_MCM.pex hides this version's MCM menu, the real script declares the OnConfigClose native
static bool IsSMFStubInstalled() {
    std::ifstream file("Data/Scripts/Hellblade_MCM.pex", std::ios::binary);
    if (!file)
        return false;
    std::string bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    std::ranges::transform(bytes, bytes.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return !bytes.contains("onconfigclose");
}

static void WarnIfBothVersionsInstalled() {
    static bool warned = false;
    if (warned || !IsSMFStubInstalled())
        return;
    warned = true;
    logger::warn("Found the SKSE Menu Framework version's Hellblade_MCM.pex next to the MCM version"sv);
    RE::DebugMessageBox("Hellblade - Timed Block: the MCM and SKSE Menu Framework versions are both installed, so neither settings menu works. Uninstall one of them.");
}

static void SKSEMessage(SKSE::MessagingInterface::Message* message) {
    switch (message->type) {
        case SKSE::MessagingInterface::kPostLoad:
        case SKSE::MessagingInterface::kPostPostLoad:
            BlockHandler::Singleton().RequestModAPIs();
            BlockHandler::Singleton().TryRegisterPrecisionCallback();
            break;
        case SKSE::MessagingInterface::kPostLoadGame:
        case SKSE::MessagingInterface::kNewGame:
            BlockHandler::Singleton().Reset();
            Forms::Update();
            WarnIfBothVersionsInstalled();
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::ReadSettings();
            Forms::Load();
            break;
    }
}

SKSEPluginLoad(const SKSE::LoadInterface* a_skse) {
#ifndef NDEBUG
    const auto level = spdlog::level::trace;
    auto sink = std::make_shared<spdlog::sinks::msvc_sink_mt>();
#else
    const auto level = spdlog::level::trace;
    auto logPath = logger::log_directory();
    *logPath /= fmt::format("{}.log"sv, SKSE::PluginDeclaration::GetSingleton()->GetName());

    auto sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(logPath->string(), true);
#endif

	auto log = std::make_shared<spdlog::logger>("global log"s, std::move(sink));
	log->set_level(level);
	log->flush_on(level);

	spdlog::set_default_logger(std::move(log));
	spdlog::set_pattern("[%l] %v"s);

    SKSE::Init(a_skse);
    Hooks::Install();
    Papyrus::Register();

    if (const auto intfc = SKSE::GetMessagingInterface())
        intfc->RegisterListener(SKSEMessage);

    return true;
}