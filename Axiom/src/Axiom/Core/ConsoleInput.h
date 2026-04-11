#pragma once

#include "Axiom/Core/TaskQueue.h"
#include "Axiom/Core/ConsolePrompt.h"

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace Axiom {

class CommandRegistry;

/**
 * Live values the default prompt pulls from outside the
 * console input. Anything that varies per tick and is owned
 * by higher-level systems (player count, TPS) is fetched
 * through these providers so ConsoleInput itself does not
 * need to know about PacketContext / TickScheduler.
 */
struct PromptMetrics {
	std::function<int()> PlayerCount;
	std::function<float()> Tps;
};

/**
 * Dedicated thread that reads lines from stdin with tab
 * completion and command history, queuing results for
 * main-thread processing.
 */
class ConsoleInput {
public:
	using CompletionProvider = std::function<std::vector<std::string>(const std::string&)>;

	explicit ConsoleInput(TaskQueue& taskQueue);
	~ConsoleInput();

	ConsoleInput(const ConsoleInput&) = delete;
	ConsoleInput& operator=(const ConsoleInput&) = delete;

	void Start(std::function<void(const std::string&)> handler);
	void Stop();
	bool IsRunning() const { return m_Running.load(); }

	/** Set a callback that returns completions for partial input. */
	void SetCompletionProvider(CompletionProvider provider);

	/**
	 * Install the default starship-style prompt — axiom label,
	 * version + build counter in debug, player count, TPS,
	 * resident memory, and process CPU usage. The dynamic
	 * app-owned values come from `metrics`; the rest is
	 * resolved inside ConsoleInput.
	 */
	void ConfigureDefaultPrompt(PromptMetrics metrics);

	/**
	 * Wire the command registry as both the tab-completion
	 * source and the dispatch target for entered lines, then
	 * start the read loop. Convenience wrapper so callers do
	 * not have to hand-roll the completion and handler lambdas.
	 */
	void StartDispatchingCommands(CommandRegistry& commands);

private:
	void ReadLoop();
	std::string ReadLine(const std::string& prompt, int promptVisibleLength = -1);
	void RefreshLine(const std::string& prompt, int promptVisibleLength,
		const std::string& buffer, int cursor);

	TaskQueue& m_TaskQueue;
	std::function<void(const std::string&)> m_Handler;
	CompletionProvider m_CompletionProvider;
	ConsolePrompt m_Prompt;
	std::thread m_Thread;
	std::atomic<bool> m_Running{false};

	std::vector<std::string> m_History;
	static constexpr int MaxHistory = 100;
};

}
