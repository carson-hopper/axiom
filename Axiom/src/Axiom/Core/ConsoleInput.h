#pragma once

#include "Axiom/Core/TaskQueue.h"
#include "Axiom/Core/ConsolePrompt.h"

#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <vector>

namespace Axiom {

/**
 * Dedicated thread that reads lines from stdin
 * with tab completion and command history,
 * queuing results for main-thread processing.
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

	/** Access the prompt builder for adding segments. */
	ConsolePrompt& Prompt() { return m_Prompt; }

private:
	void ReadLoop();
	std::string ReadLine(const std::string& prompt, int promptVisibleLength = -1);
	void RefreshLine(const std::string& prompt, int promptVisibleLength, const std::string& buffer, int cursor);

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
