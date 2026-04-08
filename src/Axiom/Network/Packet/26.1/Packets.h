#pragma once

// ----- Handshake -------------------------------------------------
#include "Axiom/Network/Packet/26.1/Handshake/HandshakePacket.h"

// ----- Status ----------------------------------------------------
#include "Axiom/Network/Packet/26.1/Status/StatusRequestPacket.h"
#include "Axiom/Network/Packet/26.1/Status/PingRequestPacket.h"

// ----- Login -----------------------------------------------------
#include "Axiom/Network/Packet/26.1/Login/LoginHelloPacket.h"
#include "Axiom/Network/Packet/26.1/Login/EncryptionResponsePacket.h"
#include "Axiom/Network/Packet/26.1/Login/LoginAcknowledgedPacket.h"

// ----- Configuration ---------------------------------------------
#include "Axiom/Network/Packet/26.1/Config/ClientInformationPacket.h"
#include "Axiom/Network/Packet/26.1/Config/SelectKnownPacksPacket.h"
#include "Axiom/Network/Packet/26.1/Config/FinishConfigurationPacket.h"
#include "Axiom/Network/Packet/26.1/Config/KeepAlivePacket.h"

// ----- Play - Core -----------------------------------------------
#include "Axiom/Network/Packet/26.1/Play/PlayKeepAlivePacket.h"
#include "Axiom/Network/Packet/26.1/Play/PlayerLoadedPacket.h"
#include "Axiom/Network/Packet/26.1/Play/ChunkBatchReceivedPacket.h"
#include "Axiom/Network/Packet/26.1/Play/AcceptTeleportationPacket.h"
#include "Axiom/Network/Packet/26.1/Play/ClientTickEndPacket.h"

// ----- Play - Movement -------------------------------------------
#include "Axiom/Network/Packet/26.1/Play/MovePlayerPositionPacket.h"

// ----- Play - Interaction ----------------------------------------
#include "Axiom/Network/Packet/26.1/Play/PlayerActionPacket.h"
#include "Axiom/Network/Packet/26.1/Play/UseItemOnPacket.h"
#include "Axiom/Network/Packet/26.1/Play/SetCarriedItemPacket.h"
#include "Axiom/Network/Packet/26.1/Play/SetCreativeModeSlotPacket.h"

// ----- Play - Chat -----------------------------------------------
#include "Axiom/Network/Packet/26.1/Play/Chat/ChatPacket.h"
#include "Axiom/Network/Packet/26.1/Play/Chat/PlayerChatPacket.h"
#include "Axiom/Network/Packet/26.1/Play/Chat/SystemChatPacket.h"
