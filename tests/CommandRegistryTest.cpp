#include "TestFramework.h"

#include "Axiom/Command/CommandRegistry.h"
#include "Axiom/Command/Command.h"
#include "Axiom/Command/CommandSender.h"
#include "Axiom/Chat/ChatComponent.h"

using namespace Axiom;

// Mock command sender for testing
class MockCommandSender : public CommandSender {
public:
	std::vector<std::string> Messages;

	void SendMessage(const Ref<ChatComponent>& message) override {
		Messages.push_back(message->GetPlainText());
	}

	bool IsPlayer() const override { return false; }
	bool HasPermission(const std::string&) const override { return true; }
};

TEST(CommandRegistryTest, RegisterAndDispatch) {
	CommandRegistry registry;
	bool wasCalled = false;

	registry.Register("test", "A test command", [&wasCalled](CommandSender&, const std::vector<std::string>&) {
		wasCalled = true;
	});

	MockCommandSender sender;
	bool result = registry.Dispatch(sender, "test");

	ASSERT_TRUE(result);
	ASSERT_TRUE(wasCalled);
}

TEST(CommandRegistryTest, UnknownCommand) {
	CommandRegistry registry;
	MockCommandSender sender;

	bool result = registry.Dispatch(sender, "unknown");

	ASSERT_FALSE(result);
	ASSERT_EQ(sender.Messages.size(), 1u);
	ASSERT_TRUE(sender.Messages[0].find("Unknown") != std::string::npos);
}

TEST(CommandRegistryTest, CommandArguments) {
	CommandRegistry registry;
	std::vector<std::string> receivedArgs;

	registry.Register("echo", "Echo command", [&receivedArgs](CommandSender&, const std::vector<std::string>& args) {
		receivedArgs = args;
	});

	MockCommandSender sender;
	registry.Dispatch(sender, "echo hello world 123");

	ASSERT_EQ(receivedArgs.size(), 3u);
	ASSERT_EQ(receivedArgs[0], "hello");
	ASSERT_EQ(receivedArgs[1], "world");
	ASSERT_EQ(receivedArgs[2], "123");
}

TEST(CommandRegistryTest, CommandCount) {
	CommandRegistry registry;

	ASSERT_EQ(registry.CommandCount(), 0u);

	registry.Register("cmd1", "Desc1", [](CommandSender&, const std::vector<std::string>&) {});
	ASSERT_EQ(registry.CommandCount(), 1u);

	registry.Register("cmd2", "Desc2", [](CommandSender&, const std::vector<std::string>&) {});
	ASSERT_EQ(registry.CommandCount(), 2u);
}

TEST(CommandRegistryTest, HasCommand) {
	CommandRegistry registry;

	registry.Register("existing", "Desc", [](CommandSender&, const std::vector<std::string>&) {});

	ASSERT_TRUE(registry.HasCommand("existing"));
	ASSERT_FALSE(registry.HasCommand("missing"));
}

TEST(CommandRegistryTest, Unregister) {
	CommandRegistry registry;

	registry.Register("temp", "Desc", [](CommandSender&, const std::vector<std::string>&) {});
	ASSERT_TRUE(registry.HasCommand("temp"));

	registry.Unregister("temp");
	ASSERT_FALSE(registry.HasCommand("temp"));
}

TEST(CommandRegistryTest, TabCompleteCommandName) {
	CommandRegistry registry;

	registry.Register("give", "Give items", [](CommandSender&, const std::vector<std::string>&) {});
	registry.Register("gamemode", "Change gamemode", [](CommandSender&, const std::vector<std::string>&) {});
	registry.Register("help", "Show help", [](CommandSender&, const std::vector<std::string>&) {});

	MockCommandSender sender;
	auto completions = registry.TabComplete(sender, "ga");

	ASSERT_EQ(completions.size(), 2u);
	// Should include "give" and "gamemode"
}

// Custom command class for testing tab completion
class TestTabCommand : public Command {
public:
	const std::string& Name() const override {
		static std::string name = "testtab";
		return name;
	}

	const std::string& Description() const override {
		static std::string desc = "Test tab completion";
		return desc;
	}

	void Execute(CommandSender&, const std::vector<std::string>&) override {}

	std::vector<std::string> TabComplete(CommandSender&, const std::vector<std::string>& args) override {
		if (args.empty() || args.size() == 1) {
			return {"option1", "option2", "other"};
		}
		return {};
	}
};

TEST(CommandRegistryTest, TabCompleteArguments) {
	CommandRegistry registry;
	registry.Register(CreateRef<TestTabCommand>());

	MockCommandSender sender;
	auto completions = registry.TabComplete(sender, "testtab op");

	// Should return matching arguments
	ASSERT_TRUE(completions.size() > 0);
}