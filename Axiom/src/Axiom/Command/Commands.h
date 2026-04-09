#pragma once

/**
 * @file Commands.h
 * @brief Convenience header that includes all built-in commands.
 */

#include "Axiom/Command/Commands/Server/StopCommand.h"

#if AX_DEBUG
#include "Axiom/Command/Commands/Server/RestartCommand.h"
#endif

#include "Axiom/Command/Commands/HelpCommand.h"
#include "Axiom/Command/Commands/GamemodeCommand.h"
#include "Axiom/Command/Commands/DifficultyCommand.h"
#include "Axiom/Command/Commands/TimeCommand.h"
#include "Axiom/Command/Commands/WeatherCommand.h"
#include "Axiom/Command/Commands/TeleportCommand.h"
#include "Axiom/Command/Commands/SetWorldSpawnCommand.h"
#include "Axiom/Command/Commands/GameruleCommand.h"
#include "Axiom/Command/Commands/ListCommand.h"
