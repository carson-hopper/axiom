#include "Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace Axiom {

	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_PluginLogger;

	void Log::Init() {
		std::vector<spdlog::sink_ptr> sinks;
		sinks.emplace_back(CreateRef<spdlog::sinks::stdout_color_sink_mt>());
		sinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("logs/axiom.log", true));

		sinks[0]->set_pattern("%^[%T] [%n] %v%$");
		sinks[1]->set_pattern("[%T] [%l] [%n] %v");

		s_CoreLogger = CreateRef<spdlog::logger>("Axiom", begin(sinks), end(sinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		s_PluginLogger = CreateRef<spdlog::logger>("Plugin", begin(sinks), end(sinks));
		spdlog::register_logger(s_PluginLogger);
		s_PluginLogger->set_level(spdlog::level::trace);
		s_PluginLogger->flush_on(spdlog::level::trace);
	}

}
