#include "AesCipher.h"

#include <stdexcept>

namespace Axiom {

	AesCipher::AesCipher(const std::vector<uint8_t>& sharedSecret) {
		if (sharedSecret.size() != 16) {
			throw std::runtime_error("Shared secret must be 16 bytes");
		}

		// AES-128-CFB8 — key and IV are both the shared secret
		m_EncryptContext = EVP_CIPHER_CTX_new();
		m_DecryptContext = EVP_CIPHER_CTX_new();

		if (!m_EncryptContext || !m_DecryptContext) {
			throw std::runtime_error("Failed to create cipher contexts");
		}

		if (EVP_EncryptInit_ex(m_EncryptContext, EVP_aes_128_cfb8(), nullptr,
			sharedSecret.data(), sharedSecret.data()) != 1) {
			throw std::runtime_error("Failed to init AES encrypt");
		}

		if (EVP_DecryptInit_ex(m_DecryptContext, EVP_aes_128_cfb8(), nullptr,
			sharedSecret.data(), sharedSecret.data()) != 1) {
			throw std::runtime_error("Failed to init AES decrypt");
		}
	}

	AesCipher::~AesCipher() {
		if (m_EncryptContext) {
			EVP_CIPHER_CTX_free(m_EncryptContext);
		}
		if (m_DecryptContext) {
			EVP_CIPHER_CTX_free(m_DecryptContext);
		}
	}

	std::vector<uint8_t> AesCipher::Encrypt(const std::vector<uint8_t>& plaintext) {
		std::vector<uint8_t> output(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
		int outputLength = 0;

		if (EVP_EncryptUpdate(m_EncryptContext, output.data(), &outputLength,
			plaintext.data(), static_cast<int>(plaintext.size())) != 1) {
			throw std::runtime_error("AES encryption failed");
		}

		output.resize(outputLength);
		return output;
	}

	std::vector<uint8_t> AesCipher::Decrypt(const std::vector<uint8_t>& ciphertext) {
		std::vector<uint8_t> output(ciphertext.size() + EVP_MAX_BLOCK_LENGTH);
		int outputLength = 0;

		if (EVP_DecryptUpdate(m_DecryptContext, output.data(), &outputLength,
			ciphertext.data(), static_cast<int>(ciphertext.size())) != 1) {
			throw std::runtime_error("AES decryption failed");
		}

		output.resize(outputLength);
		return output;
	}

}
