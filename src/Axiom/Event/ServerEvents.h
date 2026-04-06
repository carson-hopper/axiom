#pragma once

#include "Axiom/Event/Event.h"

namespace Axiom {

	class ServerStartEvent : public Event {
	public:
		ServerStartEvent() = default;

		AX_EVENT_CLASS_TYPE(ServerStart)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)
	};

	class ServerStopEvent : public Event {
	public:
		ServerStopEvent() = default;

		AX_EVENT_CLASS_TYPE(ServerStop)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)
	};

	class ServerTickEvent : public Event {
	public:
		explicit ServerTickEvent(uint64_t tickNumber)
			: m_TickNumber(tickNumber) {}

		uint64_t TickNumber() const { return m_TickNumber; }

		std::string ToString() const override {
			return "ServerTickEvent: " + std::to_string(m_TickNumber);
		}

		AX_EVENT_CLASS_TYPE(ServerTick)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)

	private:
		uint64_t m_TickNumber;
	};

}
