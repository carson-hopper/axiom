#pragma once

#include "Axiom/Command/CommandNode.h"
#include "Axiom/Core/Base.h"

#include <string>

namespace Axiom {

	/**
	 * Abstract base class for server commands.
	 *
	 * Each command describes itself as a Brigadier-style node tree rooted
	 * in a LiteralNode. The tree captures all sub-commands, arguments, and
	 * their execution callbacks. Tab completion and dispatch both walk the
	 * same tree.
	 *
	 * Example:
	 * @code
	 * class KillCommand : public Command {
	 * public:
	 *     const std::string& Name() const override {
	 *         static std::string name = "kill";
	 *         return name;
	 *     }
	 *     const std::string& Description() const override {
	 *         static std::string desc = "Removes an entity from the world";
	 *         return desc;
	 *     }
	 *     int RequiredPermissionLevel() const override { return 2; }
	 *
	 *     Ref<LiteralNode> BuildTree() override {
	 *         auto root = Literal("kill");
	 *         root->Requires(RequiredPermissionLevel());
	 *         root->Then(Argument("target", CreateRef<EntityParser>())
	 *             ->Executes([](CommandSourceStack& source, auto&) {
	 *                 // ...
	 *                 return 1;
	 *             }));
	 *         return root;
	 *     }
	 * };
	 * @endcode
	 */
	class Command : public virtual RefCounted {
	public:
		~Command() override = default;

		/** Unique command identifier (what the user types). */
		virtual const std::string& Name() const = 0;

		/** Brief description for help listings. */
		virtual const std::string& Description() const = 0;

		/** Permission level required to run this command (0 = anyone). */
		virtual int RequiredPermissionLevel() const { return 0; }

		/**
		 * Named permission required to run this command.
		 * Empty string means no named permission is required.
		 *
		 * When both this and RequiredPermissionLevel are declared,
		 * passing either check grants access (OR semantics).
		 */
		virtual const std::string& RequiredPermission() const {
			static const std::string empty;
			return empty;
		}

		/** Build the Brigadier node tree for this command. */
		virtual Ref<LiteralNode> BuildTree() = 0;
	};

}
