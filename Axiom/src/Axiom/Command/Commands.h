#pragma once

/**
 * @file Commands.h
 * @brief Convenience header that includes all built-in commands.
 */

#include "Axiom/Command/Commands/Server/StopCommand.h"
#include "Axiom/Command/Commands/Server/OpCommand.h"
#include "Axiom/Command/Commands/Server/DeopCommand.h"

#if AX_DEBUG
#include "Axiom/Command/Commands/Server/RestartCommand.h"
#endif

#include "Axiom/Command/Commands/HelpCommand.h"
#include "Axiom/Command/Commands/Player/GamemodeCommand.h"
#include "Axiom/Command/Commands/Environment/DifficultyCommand.h"
#include "Axiom/Command/Commands/Environment/TimeCommand.h"
#include "Axiom/Command/Commands/Environment/WeatherCommand.h"
#include "Axiom/Command/Commands/Player/TeleportCommand.h"
#include "Axiom/Command/Commands/Environment/SetWorldSpawnCommand.h"
#include "Axiom/Command/Commands/Environment/GameruleCommand.h"
#include "Axiom/Command/Commands/ListCommand.h"
