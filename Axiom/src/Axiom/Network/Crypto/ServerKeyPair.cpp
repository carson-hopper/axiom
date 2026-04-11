#include "ServerKeyPair.h"

#include "Axiom/Core/Log.h"

#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#include <stdexcept>

namespace Axiom {

	ServerKeyPair::ServerKeyPair() {
		EVP_PKEY_CTX* context = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
		if (!context) {
			throw std::runtime_error("Failed to create EVP_PKEY_CTX");
		}

		if (EVP_PKEY_keygen_init(context) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to init keygen");
		}

		if (EVP_PKEY_CTX_set_rsa_keygen_bits(context, 2048) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to set RSA key size");
		}

		if (EVP_PKEY_keygen(context, &m_KeyPair) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to generate RSA keypair");
		}

		EVP_PKEY_CTX_free(context);

		// Export public key as DER
		unsigned char* derBuffer = nullptr;
		const int derLength = i2d_PUBKEY(m_KeyPair, &derBuffer);
		if (derLength <= 0) {
			throw std::runtime_error("Failed to export public key as DER");
		}

		m_PublicKeyDer.assign(derBuffer, derBuffer + derLength);
		OPENSSL_free(derBuffer);

		AX_CORE_INFO("Generated 2048-bit RSA keypair ({} bytes public key)", m_PublicKeyDer.size());
	}

	ServerKeyPair::~ServerKeyPair() {
		if (m_KeyPair) {
			EVP_PKEY_free(m_KeyPair);
		}
	}

	std::vector<uint8_t> ServerKeyPair::Decrypt(const std::vector<uint8_t>& ciphertext) const {
		EVP_PKEY_CTX* context = EVP_PKEY_CTX_new(m_KeyPair, nullptr);
		if (!context) {
			throw std::runtime_error("Failed to create decrypt context");
		}

		if (EVP_PKEY_decrypt_init(context) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to init decrypt");
		}

		if (EVP_PKEY_CTX_set_rsa_padding(context, RSA_PKCS1_PADDING) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to set RSA padding");
		}

		// Determine output size
		size_t outputLength = 0;
		if (EVP_PKEY_decrypt(context, nullptr, &outputLength,
			ciphertext.data(), ciphertext.size()) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("Failed to determine decrypt output size");
		}

		std::vector<uint8_t> plaintext(outputLength);
		if (EVP_PKEY_decrypt(context, plaintext.data(), &outputLength,
			ciphertext.data(), ciphertext.size()) <= 0) {
			EVP_PKEY_CTX_free(context);
			throw std::runtime_error("RSA decryption failed");
		}

		EVP_PKEY_CTX_free(context);
		plaintext.resize(outputLength);
		return plaintext;
	}

}
