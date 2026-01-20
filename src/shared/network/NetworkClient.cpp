#include "NetworkClient.h"
#include "state/BloodBowlGame.h"
#include "state/GameStateManager.h"
#include <boost/asio.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <iostream>
#include <sstream>

namespace beast = boost::beast;
namespace http = beast::http;
namespace net = boost::asio;
using tcp = net::ip::tcp;

namespace network {

    NetworkClient::NetworkClient(const std::string& host, uint16_t port)
        : serverHost(host), serverPort(port), currentVersion(0) {
    }

    NetworkClient::~NetworkClient() = default;

    Json::Value NetworkClient::httpRequest(const std::string& method, const std::string& path, const std::string& body) {
        try {
            // Résoudre l'hôte
            tcp::resolver resolver(ioContext);
            auto const results = resolver.resolve(serverHost, std::to_string(serverPort));

            // Créer le socket et se connecter
            beast::tcp_stream stream(ioContext);
            stream.connect(results);

            // Créer la requête HTTP
            http::request<http::string_body> req;
            if (method == "POST") {
                req.method(http::verb::post);
            } else {
                req.method(http::verb::get);
            }
            req.target(path);
            req.version(11);
            req.set(http::field::host, serverHost);
            req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);
            req.set(http::field::content_type, "application/json");

            if (!body.empty()) {
                req.body() = body;
                req.prepare_payload();
            }

            // Envoyer la requête
            http::write(stream, req);

            // Recevoir la réponse
            beast::flat_buffer buffer;
            http::response<http::string_body> res;
            http::read(stream, buffer, res);

            // Fermer la connexion
            beast::error_code ec;
            stream.socket().shutdown(tcp::socket::shutdown_both, ec);

            // Parser la réponse JSON
            Json::Value responseJson;
            Json::CharReaderBuilder reader;
            std::string errors;
            std::istringstream iss(res.body());

            if (!Json::parseFromStream(reader, iss, &responseJson, &errors)) {
                std::cerr << "Error parsing JSON response: " << errors << std::endl;
                return Json::Value();
            }

            return responseJson;

        } catch (const std::exception& e) {
            std::cerr << "HTTP request error: " << e.what() << std::endl;
            return Json::Value();
        }
    }

    Json::Value NetworkClient::httpPost(const std::string& path, const Json::Value& body) {
        Json::StreamWriterBuilder writer;
        std::string bodyStr = Json::writeString(writer, body);
        return httpRequest("POST", path, bodyStr);
    }

    Json::Value NetworkClient::httpGet(const std::string& path) {
        return httpRequest("GET", path, "");
    }

    bool NetworkClient::createGame(int teamAId, int teamBId) {
        Json::Value requestBody;
        requestBody["teamAId"] = teamAId;
        requestBody["teamBId"] = teamBId;

        Json::Value response = httpPost("/game/create", requestBody);

        if (response.isMember("gameId")) {
            gameId = response["gameId"].asString();
            currentVersion = 0;
            std::cout << "Game created with ID: " << gameId << std::endl;
            return true;
        }

        if (response.isMember("error")) {
            std::cerr << "Error creating game: " << response["error"].asString() << std::endl;
        }
        return false;
    }

    bool NetworkClient::joinGame(const std::string& gameIdToJoin, int teamId) {
        Json::Value requestBody;
        requestBody["teamId"] = teamId;

        Json::Value response = httpPost("/game/join/" + gameIdToJoin, requestBody);

        if (response.isMember("gameId")) {
            gameId = gameIdToJoin;
            currentVersion = 0;
            myTeamId = teamId; // Joiner controls the team they joined with
            std::cout << "Joined game: " << gameId << " with team ID: " << teamId << std::endl;
            return true;
        }

        if (response.isMember("error")) {
            std::cerr << "Error joining game: " << response["error"].asString() << std::endl;
        }
        return false;
    }

    std::shared_ptr<state::BloodBowlGame> NetworkClient::getGameState() {
        if (gameId.empty()) {
            std::cerr << "No active game" << std::endl;
            return nullptr;
        }

        Json::Value response = httpGet("/game/state/" + gameId);

        if (response.isMember("error")) {
            std::cerr << "Error getting game state: " << response["error"].asString() << std::endl;
            return nullptr;
        }

        if (response.isMember("version")) {
            currentVersion = response["version"].asInt();
        }

        // Désérialiser l'état du jeu
        try {
            auto game = state::GameStateManager::getInstance().deserializeGame(response);
            return game;
        } catch (const std::exception& e) {
            std::cerr << "Error deserializing game state: " << e.what() << std::endl;
            return nullptr;
        }
    }

    bool NetworkClient::sendCommand(const Json::Value& commandJson) {
        if (gameId.empty()) {
            std::cerr << "No active game" << std::endl;
            return false;
        }

        Json::Value response = httpPost("/game/command/" + gameId, commandJson);

        if (response.isMember("error")) {
            std::cerr << "Error sending command: " << response["error"].asString() << std::endl;
            return false;
        }

        if (response.isMember("version")) {
            currentVersion = response["version"].asInt();
        }

        return true;
    }

    bool NetworkClient::pollUpdates(std::shared_ptr<state::BloodBowlGame>& outGame) {
        if (gameId.empty()) {
            return false;
        }

        std::string path = "/game/poll/" + gameId + "/" + std::to_string(currentVersion);
        Json::Value response = httpGet(path);

        if (response.isMember("error")) {
            std::cerr << "Error polling updates: " << response["error"].asString() << std::endl;
            return false;
        }

        if (response.isMember("updated") && response["updated"].asBool()) {
            if (response.isMember("version")) {
                currentVersion = response["version"].asInt();
            }

            if (response.isMember("gameState")) {
                try {
                    outGame = state::GameStateManager::getInstance().deserializeGame(response["gameState"]);
                    return true;
                } catch (const std::exception& e) {
                    std::cerr << "Error deserializing updated game state: " << e.what() << std::endl;
                    return false;
                }
            }
        }

        return false;
    }

} // namespace network

