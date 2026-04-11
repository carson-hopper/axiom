#include "axpch.h"
#include "Axiom/Core/ConsoleInput.h"

#include "Axiom/Core/Log.h"

#include <iostream>

#if defined(AX_PLATFORM_MACOS) || defined(AX_PLATFORM_LINUX)
#include <csignal>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
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
