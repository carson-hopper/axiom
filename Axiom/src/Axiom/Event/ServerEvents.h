#pragma once

#include "Axiom/Event/Event.h"

#include <cstdint>

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
		explicit ServerTickEvent(const uint64_t tickNumber)
			: m_TickNumber(tickNumber) {}

		uint64_t TickNumber() const { return m_TickNumber; }

		AX_EVENT_CLASS_TYPE(ServerTick)
		AX_EVENT_CLASS_CATEGORY(EventCategoryServer)

	private:
		uint64_t m_TickNumber;
	};

}
