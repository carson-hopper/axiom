#pragma once

#include <cstdint>

namespace Axiom {

    enum class PacketState : int32_t {
        Handshake = 0,
        Status,
        Login,
        Configuration,
        Play
    };

}
