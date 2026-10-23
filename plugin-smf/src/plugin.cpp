#include "forms.hpp"
#include "hooks.hpp"
#include "smf_menu.hpp"
#include "block_handler.hpp"
#include "settings.hpp"

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
            break;
        case SKSE::MessagingInterface::kDataLoaded:
            Settings::ReadSettings();
            Forms::Load();
            SMFMenu::Register();
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

    if (const auto intfc = SKSE::GetMessagingInterface())
        intfc->RegisterListener(SKSEMessage);

    return true;
}