#include "TestFramework.h"

#include "Axiom/Event/EventBus.h"
#include "Axiom/Event/Event.h"

using namespace Axiom;

// Test event types
enum class TestEventType {
	TestEventA,
	TestEventB
};

class TestEvent : public Event {
public:
	TestEvent(int value) : m_Value(value) {}

	EventType Type() const override {
		return static_cast<EventType>(TestEventType::TestEventA);
	}

	int Value() const { return m_Value; }

private:
	int m_Value;
};

class AnotherTestEvent : public Event {
public:
	AnotherTestEvent(const std::string& msg) : m_Message(msg) {}

	EventType Type() const override {
		return static_cast<EventType>(TestEventType::TestEventB);
	}

	const std::string& Message() const { return m_Message; }

private:
	std::string m_Message;
};

TEST(EventBusTest, SubscribeAndPublish) {
	EventBus bus;
	int receivedValue = 0;

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&receivedValue](Event& event) {
			receivedValue = static_cast<TestEvent&>(event).Value();
		},
		EventPriority::Normal,
		"TestPlugin"
	);

	TestEvent testEvent(42);
	bus.Publish(testEvent);

	ASSERT_EQ(receivedValue, 42);
}

TEST(EventBusTest, PriorityOrder) {
	EventBus bus;
	std::vector<int> callOrder;

	// Subscribe with different priorities
	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&callOrder](Event&) { callOrder.push_back(3); },
		EventPriority::High,
		"Test"
	);

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&callOrder](Event&) { callOrder.push_back(1); },
		EventPriority::Lowest,
		"Test"
	);

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&callOrder](Event&) { callOrder.push_back(2); },
		EventPriority::Normal,
		"Test"
	);

	TestEvent event(0);
	bus.Publish(event);

	ASSERT_EQ(callOrder.size(), 3u);
	ASSERT_EQ(callOrder[0], 3); // High priority first
	ASSERT_EQ(callOrder[1], 2); // Normal priority second
	ASSERT_EQ(callOrder[2], 1); // Lowest priority last
}

TEST(EventBusTest, EventFiltering) {
	EventBus bus;
	int eventACount = 0;
	int eventBCount = 0;

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&eventACount](Event&) { eventACount++; },
		EventPriority::Normal,
		"Test"
	);

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventB),
		[&eventBCount](Event&) { eventBCount++; },
		EventPriority::Normal,
		"Test"
	);

	TestEvent eventA(1);
	AnotherTestEvent eventB("test");

	bus.Publish(eventA);
	bus.Publish(eventB);

	ASSERT_EQ(eventACount, 1);
	ASSERT_EQ(eventBCount, 1);
}

TEST(EventBusTest, UnsubscribeAll) {
	EventBus bus;
	int count = 0;

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&count](Event&) { count++; },
		EventPriority::Normal,
		"PluginA"
	);

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&count](Event&) { count++; },
		EventPriority::Normal,
		"PluginB"
	);

	TestEvent event(0);
	bus.Publish(event);
	ASSERT_EQ(count, 2);

	bus.UnsubscribeAll("PluginA");

	bus.Publish(event);
	ASSERT_EQ(count, 3); // Only PluginB's handler called
}

TEST(EventBusTest, MultipleSubscriptions) {
	EventBus bus;
	int count = 0;

	// Multiple handlers for same event
	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&count](Event&) { count++; },
		EventPriority::Normal,
		"Test"
	);

	bus.Subscribe(
		static_cast<EventType>(TestEventType::TestEventA),
		[&count](Event&) { count++; },
		EventPriority::Normal,
		"Test"
	);

	TestEvent event(0);
	bus.Publish(event);

	ASSERT_EQ(count, 2);
}