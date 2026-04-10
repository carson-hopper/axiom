#include "axpch.h"
#include "Axiom/Data/Nbt/NbtIo.h"

#include "Axiom/Core/Log.h"

#include <zlib.h>

#include <stdexcept>

namespace Axiom {

	// ----- Factory -----------------------------------------------------

	Ref<NbtTag> CreateNbtTag(const NbtTagType type) {
		switch (type) {
			case NbtTagType::Byte:      return CreateRef<NbtByte>();
			case NbtTagType::Short:     return CreateRef<NbtShort>();
			case NbtTagType::Int:       return CreateRef<NbtInt>();
			case NbtTagType::Long:      return CreateRef<NbtLong>();
			case NbtTagType::Float:     return CreateRef<NbtFloat>();
			case NbtTagType::Double:    return CreateRef<NbtDouble>();
			case NbtTagType::ByteArray: return CreateRef<NbtByteArray>();
			case NbtTagType::String:    return CreateRef<NbtString>();
			case NbtTagType::List:      return CreateRef<NbtList>();
			case NbtTagType::Compound:  return CreateRef<NbtCompound>();
			case NbtTagType::IntArray:  return CreateRef<NbtIntArray>();
			case NbtTagType::LongArray: return CreateRef<NbtLongArray>();
			case NbtTagType::End:       return nullptr;
		}
		return nullptr;
	}

	// ----- Network format ---------------------------------------------

	void NbtIo::WriteNetwork(const Ref<NbtCompound>& root, NetworkBuffer& buffer) {
		if (!root) {
			buffer.WriteByte(0); // TAG_End
			return;
		}
		buffer.WriteByte(static_cast<uint8_t>(NbtTagType::Compound));
		root->Write(buffer);
	}

	Ref<NbtCompound> NbtIo::ReadNetwork(NetworkBuffer& buffer) {
		const auto type = static_cast<NbtTagType>(buffer.ReadByte());
		if (type != NbtTagType::Compound) {
			return nullptr;
		}
		auto root = CreateRef<NbtCompound>();
		root->Read(buffer);
		return root;
	}

	// ----- File format -------------------------------------------------

	void NbtIo::WriteFile(const Ref<NbtCompound>& root, NetworkBuffer& buffer,
		const std::string& name) {
		if (!root) {
			buffer.WriteByte(0);
			return;
		}
		buffer.WriteByte(static_cast<uint8_t>(NbtTagType::Compound));
		buffer.WriteUnsignedShort(static_cast<uint16_t>(name.size()));
		for (const char character : name) {
			buffer.WriteByte(static_cast<uint8_t>(character));
		}
		root->Write(buffer);
	}

	Ref<NbtCompound> NbtIo::ReadFile(NetworkBuffer& buffer) {
		const auto type = static_cast<NbtTagType>(buffer.ReadByte());
		if (type != NbtTagType::Compound) {
			return nullptr;
		}
		const uint16_t nameLength = static_cast<uint16_t>(buffer.ReadShort()) & 0xFFFFU;
		for (uint16_t index = 0; index < nameLength; index++) {
			buffer.ReadByte();
		}
		auto root = CreateRef<NbtCompound>();
		root->Read(buffer);
		return root;
	}

	// ----- Gzip helpers (15 + 16 window bits = gzip wrapper) -----------

	std::vector<uint8_t> NbtIo::WriteGzipCompressed(const Ref<NbtCompound>& root,
		const std::string& rootName) {

		NetworkBuffer raw;
		WriteFile(root, raw, rootName);

		z_stream stream{};
		if (deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED,
				15 + 16, 8, Z_DEFAULT_STRATEGY) != Z_OK) {
			throw std::runtime_error("NbtIo: failed to initialize gzip deflate stream");
		}

		const auto& input = raw.Data();
		stream.next_in = const_cast<Bytef*>(input.data());
		stream.avail_in = static_cast<uInt>(input.size());

		std::vector<uint8_t> output;
		output.resize(deflateBound(&stream, static_cast<uLong>(input.size())));
		stream.next_out = output.data();
		stream.avail_out = static_cast<uInt>(output.size());

		const int result = deflate(&stream, Z_FINISH);
		if (result != Z_STREAM_END) {
			deflateEnd(&stream);
			throw std::runtime_error("NbtIo: gzip deflate failed");
		}

		output.resize(stream.total_out);
		deflateEnd(&stream);
		return output;
	}

	Ref<NbtCompound> NbtIo::ReadGzipCompressed(const std::vector<uint8_t>& data) {
		z_stream stream{};
		if (inflateInit2(&stream, 15 + 32) != Z_OK) {
			throw std::runtime_error("NbtIo: failed to initialize gzip inflate stream");
		}

		stream.next_in = const_cast<Bytef*>(data.data());
		stream.avail_in = static_cast<uInt>(data.size());

		std::vector<uint8_t> decompressed;
		decompressed.resize(data.size() * 4);

		while (true) {
			stream.next_out = decompressed.data() + stream.total_out;
			stream.avail_out = static_cast<uInt>(decompressed.size() - stream.total_out);

			const int result = inflate(&stream, Z_NO_FLUSH);
			if (result == Z_STREAM_END) break;
			if (result != Z_OK) {
				inflateEnd(&stream);
				throw std::runtime_error("NbtIo: gzip inflate failed");
			}
			if (stream.avail_out == 0) {
				decompressed.resize(decompressed.size() * 2);
			}
		}

		decompressed.resize(stream.total_out);
		inflateEnd(&stream);

		NetworkBuffer buffer(std::move(decompressed));
		return ReadFile(buffer);
	}

}
