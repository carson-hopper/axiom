#include "MojangAuth.h"

#include "Axiom/Core/Log.h"

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>

#include <sstream>

namespace Axiom {

	namespace {

		std::string HttpsGet(const std::string& host, const std::string& path) {
			SSL_CTX* sslContext = SSL_CTX_new(TLS_client_method());
			if (!sslContext) {
				AX_CORE_ERROR("Failed to create SSL context");
				return "";
			}

			// Require TLS 1.2 or newer — 1.0 and 1.1 are
			// deprecated and should never be negotiated with
			// Mojang's session server.
			if (SSL_CTX_set_min_proto_version(sslContext, TLS1_2_VERSION) != 1) {
				AX_CORE_ERROR("Failed to set minimum TLS version");
				SSL_CTX_free(sslContext);
				return "";
			}

			// Load the platform CA bundle so peer verification
			// can build a trust chain. Without this, the call
			// below would accept any certificate.
			if (SSL_CTX_set_default_verify_paths(sslContext) != 1) {
				AX_CORE_ERROR("Failed to load default CA verify paths");
				SSL_CTX_free(sslContext);
				return "";
			}

			// Refuse the handshake if the peer certificate does
			// not chain to a trusted root. Without this the
			// session server could be impersonated by any MITM.
			SSL_CTX_set_verify(sslContext, SSL_VERIFY_PEER, nullptr);

			BIO* bio = BIO_new_ssl_connect(sslContext);
			if (!bio) {
				SSL_CTX_free(sslContext);
				AX_CORE_ERROR("Failed to create BIO");
				return "";
			}

			std::string connectString = host + ":443";
			BIO_set_conn_hostname(bio, connectString.c_str());

			SSL* ssl = nullptr;
			BIO_get_ssl(bio, &ssl);
			if (!ssl) {
				AX_CORE_ERROR("Failed to retrieve SSL handle from BIO");
				BIO_free_all(bio);
				SSL_CTX_free(sslContext);
				return "";
			}

			// Send SNI so the server picks the right cert.
			SSL_set_tlsext_host_name(ssl, host.c_str());

			// Pin the expected DNS name. OpenSSL will match it
			// against the certificate's SAN entries during the
			// handshake and fail if none match — this is how
			// hostname verification is wired with SSL_VERIFY_PEER.
			if (SSL_set1_host(ssl, host.c_str()) != 1) {
				AX_CORE_ERROR("Failed to set expected hostname for verification");
				BIO_free_all(bio);
				SSL_CTX_free(sslContext);
				return "";
			}

			if (BIO_do_connect(bio) <= 0) {
				const long verifyResult = SSL_get_verify_result(ssl);
				if (verifyResult != X509_V_OK) {
					AX_CORE_ERROR("TLS handshake with {} failed: certificate verification error {} ({})",
						host, verifyResult,
						X509_verify_cert_error_string(verifyResult));
				} else {
					AX_CORE_ERROR("Failed to connect to {}", host);
				}
				BIO_free_all(bio);
				SSL_CTX_free(sslContext);
				return "";
			}

			// Defense in depth — even with SSL_VERIFY_PEER set,
			// explicitly confirm the verification verdict before
			// trusting any data that arrives on this connection.
			const long verifyResult = SSL_get_verify_result(ssl);
			if (verifyResult != X509_V_OK) {
				AX_CORE_ERROR("Post-handshake verification against {} failed: {} ({})",
					host, verifyResult,
					X509_verify_cert_error_string(verifyResult));
				BIO_free_all(bio);
				SSL_CTX_free(sslContext);
				return "";
			}

			std::string request = "GET " + path + " HTTP/1.1\r\n"
				"Host: " + host + "\r\n"
				"Connection: close\r\n"
				"Accept: application/json\r\n\r\n";

			BIO_write(bio, request.c_str(), static_cast<int>(request.size()));

			std::string response;
			char readBuffer[4096];
			int bytesRead;
			while ((bytesRead = BIO_read(bio, readBuffer, sizeof(readBuffer))) > 0) {
				response.append(readBuffer, bytesRead);
			}

			BIO_free_all(bio);
			SSL_CTX_free(sslContext);

			// Strip HTTP headers — find \r\n\r\n
			auto bodyStart = response.find("\r\n\r\n");
			if (bodyStart == std::string::npos) {
				return "";
			}

			return response.substr(bodyStart + 4);
		}

	} // anonymous namespace

	std::optional<GameProfile> MojangAuth::HasJoined(
		const std::string& username,
		const std::string& serverHash) {

		std::string path = "/session/minecraft/hasJoined?username=" + username
			+ "&serverId=" + serverHash;

		std::string body = HttpsGet("sessionserver.mojang.com", path);

		if (body.empty()) {
			AX_CORE_WARN("Mojang auth returned empty response for {}", username);
			return std::nullopt;
		}

		try {
			auto json = nlohmann::json::parse(body);

			if (!json.contains("id") || !json.contains("name")) {
				AX_CORE_WARN("Mojang auth invalid response for {}", username);
				return std::nullopt;
			}

			GameProfile profile;
			profile.uuid = json["id"].get<std::string>();
			profile.name = json["name"].get<std::string>();
			profile.properties = json.value("properties", nlohmann::json::array());

			return profile;

		} catch (const nlohmann::json::exception& exception) {
			AX_CORE_ERROR("Failed to parse Mojang auth response: {}", exception.what());
			return std::nullopt;
		}
	}

}
