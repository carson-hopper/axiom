#pragma once

/**
 * @file Commands.h
 * @brief Convenience header that includes all built-in commands.
 */

#include "Axiom/Commands/StopCommand.h"

#if AX_DEBUG
#include "Axiom/Commands/RestartCommand.h"
#endif

#include "Axiom/Commands/HelpCommand.h"
