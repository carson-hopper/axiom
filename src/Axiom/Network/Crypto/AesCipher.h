#pragma once

#include "Axiom/Core/Base.h"

#include <openssl/evp.h>

#include <cstdint>
#include <vector>

namespace Axiom {

	class AesCipher {
	public:
		AesCipher(const std::vector<uint8_t>& sharedSecret);
		~AesCipher();

		AesCipher(const AesCipher&) = delete;
		AesCipher& operator=(const AesCipher&) = delete;

		std::vector<uint8_t> Encrypt(const std::vector<uint8_t>& plaintext);
		std::vector<uint8_t> Decrypt(const std::vector<uint8_t>& ciphertext);

	private:
		EVP_CIPHER_CTX* m_EncryptContext = nullptr;
		EVP_CIPHER_CTX* m_DecryptContext = nullptr;
	};

}
