#include "axpch.h"
#include "Axiom/Core/ConsoleInput.h"

#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Core/Base.h"
#include "Axiom/Core/Log.h"

#include <cstdio>
#include <iostream>
#include <thread>

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
#include <csignal>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#endif

#if defined(AX_PLATFORM_MACOS)
#include <mach/mach.h>
#elif defined(AX_PLATFORM_LINUX)
#include <fstream>
#endif

#ifdef AX_DEBUG
#include "Axiom/Core/BuildCount.generated.h"
#endif

namespace Axiom {

	static int GetTerminalWidth() {
		int width = 80;
#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
		struct winsize windowSize{};
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize) == 0 && windowSize.ws_col > 0) {
			width = windowSize.ws_col;
		}
#endif
		return width;
	}

	/** Resident memory in bytes for the current process. */
	static size_t GetResidentMemoryBytes() {
		size_t memoryBytes = 0;
#if defined(AX_PLATFORM_MACOS)
		struct mach_task_basic_info info{};
		mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
		if (task_info(mach_task_self(), MACH_TASK_BASIC_INFO,
			reinterpret_cast<task_info_t>(&info), &count) == KERN_SUCCESS) {
			memoryBytes = info.resident_size;
		}
#elif defined(AX_PLATFORM_LINUX)
		std::ifstream statusFile("/proc/self/status");
		std::string line;
		while (std::getline(statusFile, line)) {
			if (line.starts_with("VmRSS:")) {
				memoryBytes = std::stoull(line.substr(6)) * 1024;
				break;
			}
		}
#endif
		return memoryBytes;
	}

	/** Process CPU usage as a percent of one core-equivalent. */
	static double GetProcessCpuPercent() {
		double cpuPercent = 0.0;
#if defined(AX_PLATFORM_MACOS)
		thread_array_t threadList;
		mach_msg_type_number_t threadCount;
		if (task_threads(mach_task_self(), &threadList, &threadCount) == KERN_SUCCESS) {
			for (mach_msg_type_number_t i = 0; i < threadCount; i++) {
				thread_basic_info_data_t threadInfo;
				mach_msg_type_number_t infoCount = THREAD_BASIC_INFO_COUNT;
				if (thread_info(threadList[i], THREAD_BASIC_INFO,
					reinterpret_cast<thread_info_t>(&threadInfo), &infoCount) == KERN_SUCCESS) {
					if (!(threadInfo.flags & TH_FLAGS_IDLE)) {
						cpuPercent += static_cast<double>(threadInfo.cpu_usage) / TH_USAGE_SCALE * 100.0;
					}
				}
				mach_port_deallocate(mach_task_self(), threadList[i]);
			}
			vm_deallocate(mach_task_self(),
				reinterpret_cast<vm_address_t>(threadList),
				threadCount * sizeof(thread_t));
		}
#endif
		const unsigned int coreCount = std::thread::hardware_concurrency();
		if (coreCount > 0) {
			cpuPercent /= coreCount;
		}
		return cpuPercent;
	}

	void ConsoleInput::ConfigureDefaultPrompt(PromptMetrics metrics) {
		m_Prompt.AddLeft([]() -> ConsolePrompt::Segment {
			return {"", "axiom", 15, 62};
		});
		m_Prompt.AddLeft([]() -> ConsolePrompt::Segment {
#ifdef AX_DEBUG
			return {"", "v" AX_MINECRAFT_VERSION "-" AX_STRINGIFY_MACRO(AX_COMMIT_COUNT) "+b" AX_STRINGIFY_MACRO(AX_BUILD_COUNT), 15, 33};
#else
			return {"", "v" AX_MINECRAFT_VERSION "-" AX_STRINGIFY_MACRO(AX_COMMIT_COUNT), 15, 33};
#endif
		});

		m_Prompt.AddRight([playerCountProvider = std::move(metrics.PlayerCount)]() -> ConsolePrompt::Segment {
			const int count = playerCountProvider ? playerCountProvider() : 0;
			const std::string text = std::to_string(count) + (count == 1 ? " player" : " players");
			return {"", text, 15, count > 0 ? 62 : 240};
		});
		m_Prompt.AddRight([tpsProvider = std::move(metrics.Tps)]() -> ConsolePrompt::Segment {
			const float tps = tpsProvider ? tpsProvider() : 0.0F;
			char tpsText[16];
			std::snprintf(tpsText, sizeof(tpsText), "%.1f TPS", tps);
			const int background = tps >= 19.0F ? 28 : (tps >= 15.0F ? 136 : 124);
			return {"", tpsText, 15, background};
		});
		m_Prompt.AddRight([]() -> ConsolePrompt::Segment {
			const size_t memoryBytes = GetResidentMemoryBytes();
			char memText[32];
			if (memoryBytes >= 1024ULL * 1024 * 1024) {
				std::snprintf(memText, sizeof(memText), "%.1f GB",
					static_cast<double>(memoryBytes) / (1024.0 * 1024.0 * 1024.0));
			} else {
				std::snprintf(memText, sizeof(memText), "%.0f MB",
					static_cast<double>(memoryBytes) / (1024.0 * 1024.0));
			}
			return {"", memText, 15, 97};
		});
		m_Prompt.AddRight([]() -> ConsolePrompt::Segment {
			const double cpuPercent = GetProcessCpuPercent();
			char cpuText[16];
			std::snprintf(cpuText, sizeof(cpuText), "%.0f%%", cpuPercent);
			const int background = cpuPercent < 50.0 ? 24 : (cpuPercent < 80.0 ? 136 : 124);
			return {"", cpuText, 15, background};
		});

		m_Prompt.SetPromptChar("\xe2\x9d\xaf"); // ❯
		m_Prompt.SetPromptColor(76); // green
	}

	void ConsoleInput::StartDispatchingCommands(CommandRegistry& commands) {
		SetCompletionProvider([&commands](const std::string& partial) -> std::vector<std::string> {
			std::vector<std::string> results;
			for (const auto& name : commands.GetCommandNames()) {
				if (name.starts_with(partial)) {
					results.push_back(name);
				}
			}
			return results;
		});

		Start([&commands](const std::string& input) {
			auto source = CommandSourceStack::Console();
			commands.Dispatch(source, input);
		});
	}

	ConsoleInput::ConsoleInput(TaskQueue& taskQueue)
		: m_TaskQueue(taskQueue) {
	}

	ConsoleInput::~ConsoleInput() {
		Stop();
	}

	void ConsoleInput::Start(std::function<void(const std::string&)> handler) {
		if (m_Running.exchange(true)) return;

		m_Handler = std::move(handler);
		m_Thread = std::thread([this] { ReadLoop(); });

		AX_CORE_INFO("ConsoleInput started");
	}

	void ConsoleInput::Stop() {
		if (!m_Running.exchange(false)) return;

		if (m_Thread.joinable()) {
			m_Thread.detach();
		}

		AX_CORE_INFO("ConsoleInput stopped");
	}

	void ConsoleInput::SetCompletionProvider(CompletionProvider provider) {
		m_CompletionProvider = std::move(provider);
	}

	void ConsoleInput::ReadLoop() {
		bool firstPrompt = true;
		while (m_Running) {
			// Blank line between commands for breathing room
			if (!firstPrompt) {
				write(STDOUT_FILENO, "\n", 1);
			}
			firstPrompt = false;

			// Build starship-style prompt
			const int termWidth = GetTerminalWidth();
			auto built = m_Prompt.Build(termWidth);

			std::string line = ReadLine(built.Styled, built.InputLineVisibleLength);
			if (!m_Running) break;
			if (line.empty()) continue;

			if (m_History.empty() || m_History.back() != line) {
				m_History.push_back(line);
				if (static_cast<int>(m_History.size()) > MaxHistory) {
					m_History.erase(m_History.begin());
				}
			}

			auto handler = m_Handler;
			auto capturedLine = std::move(line);
			m_TaskQueue.Post([handler, capturedLine = std::move(capturedLine)]() {
				handler(capturedLine);
			});
		}

		m_Running = false;
	}

	void ConsoleInput::RefreshLine(const std::string& prompt, int promptVisibleLength,
		const std::string& buffer, int cursor) {

		int visiblePromptLen = (promptVisibleLength >= 0)
			? promptVisibleLength
			: static_cast<int>(prompt.size());

		// Clear line and write prompt + buffer
		std::string output = "\r\033[K" + prompt + buffer;

		// Rebuild and render right badges on this line
		int termWidth = 80;
#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
		struct winsize windowSize{};
		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &windowSize) == 0 && windowSize.ws_col > 0) {
			termWidth = windowSize.ws_col;
		}
#endif
		auto built = m_Prompt.Build(termWidth);
		if (built.RightBadgesVisibleLength > 0) {
			int rightColumn = termWidth - built.RightBadgesVisibleLength + 1;
			int leftUsed = visiblePromptLen + static_cast<int>(buffer.size());
			if (rightColumn > leftUsed + 1) {
				output += "\033[s"; // save cursor
				output += "\033[" + std::to_string(rightColumn) + "G";
				output += built.RightBadges;
				output += "\033[u"; // restore cursor
			}
		}

		// Position cursor correctly
		int backCount = static_cast<int>(buffer.size()) - cursor;
		if (backCount > 0) {
			output += "\033[" + std::to_string(backCount) + "D";
		}
		write(STDOUT_FILENO, output.c_str(), output.size());
	}

	std::string ConsoleInput::ReadLine(const std::string& prompt, int promptVisibleLength) {
		if (promptVisibleLength < 0) promptVisibleLength = static_cast<int>(prompt.size());

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
		if (!isatty(STDIN_FILENO)) {
			write(STDOUT_FILENO, prompt.c_str(), prompt.size());
			std::string line;
			if (!std::getline(std::cin, line)) {
				m_Running = false;
			}
			return line;
		}

		// Enable raw mode
		struct termios original;
		tcgetattr(STDIN_FILENO, &original);
		struct termios raw = original;
		raw.c_lflag &= ~(ECHO | ICANON | ISIG);
		raw.c_iflag &= ~(IXON);
		raw.c_cc[VMIN] = 1;
		raw.c_cc[VTIME] = 0;
		tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

		std::string buffer;
		int cursor = 0;
		int historyIndex = static_cast<int>(m_History.size());

		// Single-line prompt — use the full prompt as the input prefix
		const std::string& inputPrefix = prompt;

		// Render immediately with right badges
		RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);

		while (m_Running) {
			char character = 0;
			if (read(STDIN_FILENO, &character, 1) != 1) break;

			if (character == '\r' || character == '\n') {
				write(STDOUT_FILENO, "\r\n", 2);
				break;
			}

			// Tab completion
			if (character == '\t') {
				if (m_CompletionProvider) {
					auto completions = m_CompletionProvider(buffer);
					if (completions.size() == 1) {
						buffer = completions[0];
						if (buffer.back() != ' ') buffer += ' ';
						cursor = static_cast<int>(buffer.size());
					} else if (completions.size() > 1) {
						std::string prefix = completions[0];
						for (size_t i = 1; i < completions.size(); i++) {
							size_t length = 0;
							while (length < prefix.size() && length < completions[i].size()
								&& prefix[length] == completions[i][length]) {
								length++;
							}
							prefix = prefix.substr(0, length);
						}

						if (prefix.size() > buffer.size()) {
							buffer = prefix;
							cursor = static_cast<int>(buffer.size());
						} else {
							std::string display = "\r\n";
							for (const auto& completion : completions) {
								display += "  \033[36m" + completion + "\033[0m";
							}
							display += "\r\n";
							write(STDOUT_FILENO, display.c_str(), display.size());
						}
					}
				}
				RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);
				continue;
			}

			// Ctrl-C — if buffer has text, clear it; if empty, stop the server
			if (character == 3) {
				if (!buffer.empty()) {
					buffer.clear();
					cursor = 0;
					write(STDOUT_FILENO, "^C\r\n", 4);
					RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);
					continue;
				} else {
					write(STDOUT_FILENO, "^C\r\n", 4);
					tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
					// Raise SIGINT to trigger normal shutdown
					raise(SIGINT);
					m_Running = false;
					return "";
				}
			}

			// Ctrl-D
			if (character == 4 && buffer.empty()) {
				tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
				m_Running = false;
				return "";
			}

			// Backspace
			if (character == 127 || character == 8) {
				if (cursor > 0) {
					buffer.erase(cursor - 1, 1);
					cursor--;
				}
				RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);
				continue;
			}

			// Escape sequences
			if (character == 27) {
				char sequence[2];
				if (read(STDIN_FILENO, &sequence[0], 1) != 1) continue;
				if (read(STDIN_FILENO, &sequence[1], 1) != 1) continue;

				if (sequence[0] == '[') {
					switch (sequence[1]) {
						case 'A':
							if (historyIndex > 0) {
								historyIndex--;
								buffer = m_History[historyIndex];
								cursor = static_cast<int>(buffer.size());
							}
							break;
						case 'B':
							if (historyIndex < static_cast<int>(m_History.size()) - 1) {
								historyIndex++;
								buffer = m_History[historyIndex];
								cursor = static_cast<int>(buffer.size());
							} else {
								historyIndex = static_cast<int>(m_History.size());
								buffer.clear();
								cursor = 0;
							}
							break;
						case 'C':
							if (cursor < static_cast<int>(buffer.size())) cursor++;
							break;
						case 'D':
							if (cursor > 0) cursor--;
							break;
						case 'H': cursor = 0; break;
						case 'F': cursor = static_cast<int>(buffer.size()); break;
					}
				}
				RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);
				continue;
			}

			// Regular character
			if (character >= 32) {
				buffer.insert(cursor, 1, character);
				cursor++;
				RefreshLine(inputPrefix, promptVisibleLength, buffer, cursor);
			}
		}

		tcsetattr(STDIN_FILENO, TCSAFLUSH, &original);
		return buffer;
#else
		write(STDOUT_FILENO, prompt.c_str(), prompt.size());
		std::string line;
		if (!std::getline(std::cin, line)) {
			m_Running = false;
		}
		return line;
#endif
	}

}
