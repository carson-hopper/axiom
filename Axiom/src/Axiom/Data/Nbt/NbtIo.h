#pragma once

#include "Axiom/Data/Nbt/NbtCompound.h"
#include "Axiom/Data/Nbt/NbtListImpl.h"
#include "Axiom/Network/NetworkBuffer.h"

#include <string>
#include <vector>

namespace Axiom {

	/**
	 * NBT I/O entry points. Handles the two framing variants used by
	 * Minecraft: network format (unnamed root compound) and file format
	 * (named root compound), plus gzip compression for level.dat files.
	 */
	class NbtIo {
	public:
		// ----- Network format (1.20.3+ chat, server protocol) ----------

		/** Write a root compound with no name (just type + payload). */
		static void WriteNetwork(const Ref<NbtCompound>& root, NetworkBuffer& buffer);

		/** Read a root compound with no name. */
		static Ref<NbtCompound> ReadNetwork(NetworkBuffer& buffer);

		// ----- File format (chunk NBT, level.dat) ----------------------

		/** Write a root compound with a name (type + name + payload). */
		static void WriteFile(const Ref<NbtCompound>& root, NetworkBuffer& buffer,
			const std::string& name = "");

		/** Read a root compound with a name; the name is discarded. */
		static Ref<NbtCompound> ReadFile(NetworkBuffer& buffer);

		// ----- Gzip helpers (level.dat, playerdata/*.dat) --------------

		/** Serialize a root compound to gzip-compressed bytes. */
		static std::vector<uint8_t> WriteGzipCompressed(const Ref<NbtCompound>& root,
			const std::string& rootName = "");

		/** Decompress gzip bytes and parse as a named root compound. */
		static Ref<NbtCompound> ReadGzipCompressed(const std::vector<uint8_t>& data);
	};

}
