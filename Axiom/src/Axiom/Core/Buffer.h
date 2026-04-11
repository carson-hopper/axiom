#pragma once

#include "Axiom//Core/Assert.h"

namespace Axiom {

	struct Buffer {
		void* Data = nullptr;
		uint64_t Size = 0;

		Buffer() = default;

		Buffer(const void* data, const uint64_t size = 0)
			: Data(const_cast<void*>(data)), Size(size) { }

		static Buffer Copy(const Buffer& other) {
			Buffer buffer;
			buffer.Allocate(other.Size);
			memcpy(buffer.Data, other.Data, other.Size);
			return buffer;
		}

		static Buffer Copy(const void* data, const uint64_t size) {
			Buffer buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}

		void Allocate(const uint64_t size) {
			delete[] static_cast<char*>(Data);
			Data = nullptr;
			Size = size;

			if (size == 0)
				return;

			Data = new char[size];
		}

		void Release() {
			delete[] static_cast<char*>(Data);
			Data = nullptr;
			Size = 0;
		}

		void ZeroInitialize() {
			if (Data)
				memset(Data, 0, Size);
		}

		template<typename T>
		T& Read(const uint64_t offset = 0) {
			return *static_cast<T*>(static_cast<char*>(Data) + offset);
		}

		template<typename T>
		const T& Read(const uint64_t offset = 0) const {
			return *static_cast<T*>(static_cast<char*>(Data) + offset);
		}

		char* ReadBytes(const uint64_t size, const uint64_t offset) const {
			AX_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			const auto buffer = new char[size];
			memcpy(buffer, static_cast<char*>(Data) + offset, size);
			return buffer;
		}
				
		void Write(const void* data, const uint64_t size, const uint64_t offset = 0) {
			AX_CORE_ASSERT(offset + size <= Size, "Buffer overflow!");
			memcpy(static_cast<char*>(Data) + offset, data, size);
		}

		operator bool() const {
			return static_cast<bool>(Data);
		}

		char& operator[](const int index) {
			return static_cast<char*>(Data)[index];
		}

		char operator[](const int index) const {
			return static_cast<char*>(Data)[index];
		}

		template<typename T>
		T* As() const {
			return static_cast<T*>(Data);
		}

		inline uint64_t GetSize() const { return Size; }
	};

	struct BufferSafe : public Buffer {
		~BufferSafe() {
			Release();
		}

		static BufferSafe Copy(const void* data, const uint64_t size) {
			BufferSafe buffer;
			buffer.Allocate(size);
			memcpy(buffer.Data, data, size);
			return buffer;
		}
	};
}