#include "axpch.h"
#include "Axiom/Core/Log.h"

#include <spdlog/sinks/basic_file_sink.h>

#include <cstdlib>
#include <mutex>

namespace Axiom {

	// ----- Truecolor ANSI helpers -----------------------------------

	namespace {

		struct RgbColor {
			int red;
			int green;
			int blue;
		};

		// Matches the Java AxiomLogger level colors exactly
		constexpr RgbColor ColorTrace{170, 170, 170}; // GRAY       0xAAAAAA
		constexpr RgbColor ColorDebug{170, 170, 170}; // GRAY       0xAAAAAA
		constexpr RgbColor ColorInfo{255, 170, 0};     // GOLD       0xFFAA00
		constexpr RgbColor ColorWarn{255, 85, 255};    // LIGHT_PURPLE 0xFF55FF
		constexpr RgbColor ColorError{255, 85, 85};    // RED        0xFF5555
		constexpr RgbColor ColorCritical{170, 0, 0};   // DARK_RED   0xAA0000

		std::string Rgb(const RgbColor& color) {
			return "\033[38;2;" + std::to_string(color.red) + ";"
				+ std::to_string(color.green) + ";"
				+ std::to_string(color.blue) + "m";
		}

		const std::string& Reset() {
			static const std::string reset = "\033[0m";
			return reset;
		}

		const RgbColor& ColorForLevel(spdlog::level::level_enum level) {
			switch (level) {
				case spdlog::level::trace:    return ColorTrace;
				case spdlog::level::debug:    return ColorDebug;
				case spdlog::level::info:     return ColorInfo;
				case spdlog::level::warn:     return ColorWarn;
				case spdlog::level::err:      return ColorError;
				case spdlog::level::critical: return ColorCritical;
				default:                      return ColorInfo;
			}
		}

		bool DetectColorSupport() {
			if (std::getenv("NO_COLOR") != nullptr) {
				return false;
			}
			if (std::getenv("FORCE_COLOR") != nullptr) {
				return true;
			}
			// Xcode's console doesn't support ANSI escape codes
			if (std::getenv("__XCODE_BUILT_PRODUCTS_DIR_PATHS") != nullptr) {
				return false;
			}
			const char* term = std::getenv("TERM");
			if (term != nullptr && std::string(term) == "dumb") {
				return false;
			}
			#if defined(AX_PLATFORM_WINDOWS)
				return std::getenv("WT_SESSION") != nullptr
					|| std::getenv("ANSICON") != nullptr
					|| (term != nullptr && term[0] != '\0');
			#else
				return true;
			#endif
		}

		// Custom sink that uses truecolor ANSI for the prefix
		class TruecolorConsoleSink : public spdlog::sinks::base_sink<std::mutex> {
		public:
			TruecolorConsoleSink()
				: m_ColorEnabled(DetectColorSupport()) {}

		protected:
			void sink_it_(const spdlog::details::log_msg& message) override {
				spdlog::memory_buf_t formatted;

				auto time = spdlog::details::os::localtime(
					std::chrono::system_clock::to_time_t(message.time));
				char timestamp[9];
				std::snprintf(timestamp, sizeof(timestamp), "%02d:%02d:%02d",
					time.tm_hour, time.tm_min, time.tm_sec);

				auto levelView = spdlog::level::to_string_view(message.level);
				std::string levelName(levelView.data(), levelView.size());
				for (auto& character : levelName) {
					character = static_cast<char>(std::toupper(static_cast<unsigned char>(character)));
				}

				if (m_ColorEnabled) {
					const auto& color = ColorForLevel(message.level);
					auto colorCode = Rgb(color);

					// [HH:mm:ss] LEVEL: message
					// Prefix is colored, message is default
					formatted.append(std::string_view(colorCode));
					formatted.append(std::string_view("["));
					formatted.append(std::string_view(timestamp, 8));
					formatted.append(std::string_view("] "));
					formatted.append(levelName);
					formatted.append(std::string_view(":"));
					formatted.append(std::string_view(Reset()));
					formatted.append(std::string_view(" "));
					formatted.append(std::string_view(message.payload.data(), message.payload.size()));
					formatted.append(std::string_view("\n"));
				} else {
					formatted.append(std::string_view("["));
					formatted.append(std::string_view(timestamp, 8));
					formatted.append(std::string_view("] "));
					formatted.append(levelName);
					formatted.append(std::string_view(": "));
					formatted.append(std::string_view(message.payload.data(), message.payload.size()));
					formatted.append(std::string_view("\n"));
				}

				fwrite(formatted.data(), sizeof(char), formatted.size(), stdout);
				fflush(stdout);
			}

			void flush_() override {
				fflush(stdout);
			}

		private:
			bool m_ColorEnabled;
		};

	} // anonymous namespace

	// ----- Log initialization ---------------------------------------

	Ref<spdlog::logger> Log::s_CoreLogger;
	Ref<spdlog::logger> Log::s_PluginLogger;

	void Log::Init() {
		std::vector<spdlog::sink_ptr> coreSinks;
		coreSinks.emplace_back(CreateRef<TruecolorConsoleSink>());
		coreSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("logs/axiom.log", true));
		coreSinks[1]->set_pattern("[%T] [%l] %v");

		s_CoreLogger = CreateRef<spdlog::logger>("Axiom", begin(coreSinks), end(coreSinks));
		spdlog::register_logger(s_CoreLogger);
		s_CoreLogger->set_level(spdlog::level::trace);
		s_CoreLogger->flush_on(spdlog::level::trace);

		std::vector<spdlog::sink_ptr> pluginSinks;
		pluginSinks.emplace_back(CreateRef<TruecolorConsoleSink>());
		pluginSinks.emplace_back(CreateRef<spdlog::sinks::basic_file_sink_mt>("logs/axiom.log", false));
		pluginSinks[1]->set_pattern("[%T] [%l] [%n] %v");

		s_PluginLogger = CreateRef<spdlog::logger>("Plugin", begin(pluginSinks), end(pluginSinks));
		spdlog::register_logger(s_PluginLogger);
		s_PluginLogger->set_level(spdlog::level::trace);
		s_PluginLogger->flush_on(spdlog::level::trace);
	}

}
