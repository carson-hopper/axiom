#include "MojangAuth.h"

#include "Axiom/Core/Log.h"

#include <openssl/ssl.h>
#include <openssl/bio.h>
#include <openssl/err.h>

#include <sstream>

namespace Axiom {

	namespace {

		std::string HttpsGet(const std::string& host, const std::string& path) {
			SSL_CTX* sslContext = SSL_CTX_new(TLS_client_method());
			if (!sslContext) {
				AX_CORE_ERROR("Failed to create SSL context");
				return "";
			}

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
			if (ssl) {
				SSL_set_tlsext_host_name(ssl, host.c_str());
			}

			if (BIO_do_connect(bio) <= 0) {
				AX_CORE_ERROR("Failed to connect to {}", host);
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
