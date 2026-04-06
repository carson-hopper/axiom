#pragma once

#include <openssl/sha.h>

#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

namespace Axiom {

	/**
	 * Computes the Minecraft-style "server hash" used for Mojang authentication.
	 *
	 * Minecraft uses a non-standard SHA-1 hash representation:
	 * the hash is interpreted as a signed two's complement big-endian
	 * integer and formatted as a hexadecimal string, possibly with a
	 * leading '-' for negative values and no leading zeros.
	 */
	inline std::string MinecraftServerHash(
		const std::string& serverId,
		const std::vector<uint8_t>& sharedSecret,
		const std::vector<uint8_t>& publicKey) {

		SHA_CTX sha;
		SHA1_Init(&sha);
		SHA1_Update(&sha, serverId.data(), serverId.size());
		SHA1_Update(&sha, sharedSecret.data(), sharedSecret.size());
		SHA1_Update(&sha, publicKey.data(), publicKey.size());

		unsigned char digest[SHA_DIGEST_LENGTH];
		SHA1_Final(digest, &sha);

		// Check if negative (high bit set)
		bool negative = (digest[0] & 0x80) != 0;

		if (negative) {
			// Two's complement: invert all bytes and add 1
			bool carry = true;
			for (int i = SHA_DIGEST_LENGTH - 1; i >= 0; i--) {
				digest[i] = ~digest[i];
				if (carry) {
					if (digest[i] == 0xFF) {
						digest[i] = 0x00;
					} else {
						digest[i]++;
						carry = false;
					}
				}
			}
		}

		// Convert to hex string without leading zeros
		std::ostringstream hexStream;
		bool leadingZero = true;
		for (int i = 0; i < SHA_DIGEST_LENGTH; i++) {
			if (leadingZero && digest[i] == 0) {
				continue;
			}
			if (leadingZero) {
				hexStream << std::hex << static_cast<int>(digest[i]);
				leadingZero = false;
			} else {
				hexStream << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(digest[i]);
			}
		}

		std::string result = hexStream.str();
		if (result.empty()) {
			result = "0";
		}

		return negative ? "-" + result : result;
	}

}
