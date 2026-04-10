#pragma once

#include "Axiom/Command/CommandDispatcher.h"
#include "Axiom/Command/CommandNode.h"
#include "Axiom/Command/CommandSourceStack.h"
#include "Axiom/Command/Parsers/ArgumentParser.h"
#include "Axiom/Environment/Entity/Player.h"
#include "Axiom/Network/Packet/Packet.h"
#include "Axiom/Network/Packet/PacketContext.h"

#include <unordered_map>
#include <utility>
#include <vector>

namespace Axiom::Play::Clientbound {

/**
 * Sends the Brigadier command graph to the client so it can
 * render the autocomplete UI and validate arguments locally.
 *
 * When constructed with a target player, the tree is filtered
 * against the player's permissions: nodes the player cannot
 * access (and their subtrees) are omitted entirely so the
 * client never sees or suggests them.
 *
 * Node flags (byte):
 *   0x00 = Root, 0x01 = Literal, 0x02 = Argument
 *   0x04 = Executable
 *   0x08 = Has redirect
 *   0x10 = Has suggestions type
 *
 * The wire format is a flat list of nodes with VarInt child indices,
 * so we walk the provided LiteralNode roots depth-first and assign
 * each node a stable index. The synthetic root is always index 0.
 */
class CommandsPacket : public Packet<CommandsPacket, PID_PLAY_CB_COMMANDS> {
public:
	CommandsPacket() = default;

	explicit CommandsPacket(std::vector<Ref<LiteralNode>> rootCommands)
		: m_RootCommands(std::move(rootCommands)) {}

	CommandsPacket(std::vector<Ref<LiteralNode>> rootCommands, Ref<Player> viewer)
		: m_RootCommands(std::move(rootCommands)), m_Viewer(std::move(viewer)) {}

	std::optional<std::vector<Ref<IChainablePacket>>>
	Handle(const Ref<Connection>&, PacketContext&, NetworkBuffer&) { return std::nullopt; }

	void Write(NetworkBuffer& buffer) override {
		// Build a source stack for the viewer so we
		// can reuse the dispatcher's permission logic.
		CommandSourceStack source = m_Viewer
			? CommandSourceStack(m_Viewer->Name(),
				static_cast<int>(m_Viewer->GetOpLevel()),
				m_Viewer->GetPosition(), m_Viewer)
			: CommandSourceStack::Console();

		// Flatten the tree: assign each node an index, recording children.
		std::vector<Ref<CommandNode>> flat;
		std::unordered_map<CommandNode*, int32_t> indexOf;

		// Reserve index 0 for the synthetic root.
		flat.push_back(nullptr);
		indexOf[nullptr] = 0;

		// Collect the roots the viewer is actually allowed to see.
		std::vector<Ref<LiteralNode>> visibleRoots;
		for (const auto& root : m_RootCommands) {
			if (CommandDispatcher::CheckPermission(source, *root)) {
				visibleRoots.push_back(root);
				Flatten(source, root, flat, indexOf);
			}
		}

		buffer.WriteVarInt(static_cast<int32_t>(flat.size()));

		// Write the synthetic root first.
		buffer.WriteByte(0x00);
		buffer.WriteVarInt(static_cast<int32_t>(visibleRoots.size()));
		for (const auto& root : visibleRoots) {
			buffer.WriteVarInt(indexOf[root.Raw()]);
		}

		// Write all other nodes in flat order.
		for (size_t i = 1; i < flat.size(); i++) {
			WriteNode(buffer, flat[i], indexOf, source);
		}

		// Root node index.
		buffer.WriteVarInt(0);
	}

	auto Fields() { return std::tuple<>(); }

private:
	static void Flatten(CommandSourceStack& source,
		const Ref<CommandNode>& node,
		std::vector<Ref<CommandNode>>& flat,
		std::unordered_map<CommandNode*, int32_t>& indexOf) {

		if (!node || indexOf.find(node.Raw()) != indexOf.end()) {
			return;
		}
		const auto index = static_cast<int32_t>(flat.size());
		indexOf[node.Raw()] = index;
		flat.push_back(node);

		for (const auto& child : node->GetChildren()) {
			if (!CommandDispatcher::CheckPermission(source, *child)) {
				continue;
			}
			Flatten(source, child, flat, indexOf);
		}
	}

	static void WriteNode(NetworkBuffer& buffer, const Ref<CommandNode>& node,
		const std::unordered_map<CommandNode*, int32_t>& indexOf,
		CommandSourceStack& source) {

		buffer.WriteByte(node->NodeFlags());

		// Children indices (only emit those the source can access).
		std::vector<int32_t> visibleChildren;
		for (const auto& child : node->GetChildren()) {
			if (!CommandDispatcher::CheckPermission(source, *child)) {
				continue;
			}
			const auto iterator = indexOf.find(child.Raw());
			if (iterator != indexOf.end()) {
				visibleChildren.push_back(iterator->second);
			}
		}
		buffer.WriteVarInt(static_cast<int32_t>(visibleChildren.size()));
		for (const auto childIndex : visibleChildren) {
			buffer.WriteVarInt(childIndex);
		}

		// Redirect index
		if (node->GetRedirect()) {
			const auto iterator = indexOf.find(node->GetRedirect().Raw());
			buffer.WriteVarInt(iterator != indexOf.end() ? iterator->second : 0);
		}

		// Name (literal + argument)
		const auto type = node->Type();
		if (type == CommandNode::Literal || type == CommandNode::Argument) {
			buffer.WriteString(node->GetName());
		}

		// Parser ID + properties (argument only)
		if (type == CommandNode::Argument) {
			const auto* argumentNode = dynamic_cast<const ArgumentNode*>(node.Raw());
			if (argumentNode && argumentNode->GetParser()) {
				buffer.WriteVarInt(argumentNode->GetParser()->ParserId());
				argumentNode->GetParser()->WriteProperties(buffer);
			} else {
				// Fallback to brigadier:string single word
				buffer.WriteVarInt(5);
				buffer.WriteVarInt(0);
			}
		}

		// Suggestions type (if flag set)
		if (!node->GetSuggestionType().empty()) {
			buffer.WriteString(node->GetSuggestionType());
		}
	}

	std::vector<Ref<LiteralNode>> m_RootCommands;
	Ref<Player> m_Viewer;
};

} // namespace Axiom::Play::Clientbound
