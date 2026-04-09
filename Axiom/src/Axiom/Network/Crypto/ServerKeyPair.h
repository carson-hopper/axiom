#pragma once

#include "Axiom/Core/Base.h"

#include <openssl/evp.h>
#include <openssl/rsa.h>

#include <cstdint>
#include <vector>

namespace Axiom {

	class ServerKeyPair {
	public:
		ServerKeyPair();
		~ServerKeyPair();

		ServerKeyPair(const ServerKeyPair&) = delete;
		ServerKeyPair& operator=(const ServerKeyPair&) = delete;

		const std::vector<uint8_t>& PublicKeyDer() const { return m_PublicKeyDer; }

		std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext) const;

	private:
		EVP_PKEY* m_KeyPair = nullptr;
		std::vector<uint8_t> m_PublicKeyDer;
	};

}
