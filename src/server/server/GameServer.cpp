#include "GameServer.h"
#include "state/BloodBowlGame.h"
#include "state/GameStateManager.h"
#include "state/Team.h"
#include "state/TeamManager.h"
#include "state/Setup.h"
#include "engine/Engine.h"
#include "network/NetworkCommand.h"
#include <cstring>
#include <iostream>
#include <sstream>
#include <random>
#include <chrono>

namespace server {

    // Structure pour stocker les données d'upload
    struct ConnectionInfo {
        std::string postData;
    };

    GameServer::GameServer(uint16_t port) : daemon(nullptr), port(port) {}

    GameServer::~GameServer() {
        stop();
    }

    bool GameServer::start() {
        daemon = MHD_start_daemon(
            MHD_USE_THREAD_PER_CONNECTION,
            port,
            nullptr, nullptr,
            &GameServer::handleRequest, this,
            MHD_OPTION_END
        );

        if (daemon == nullptr) {
            std::cerr << "Failed to start server on port " << port << std::endl;
            return false;
        }

        std::cout << "Server started on port " << port << std::endl;
        return true;
    }

    void GameServer::stop() {
        if (daemon) {
            MHD_stop_daemon(daemon);
            daemon = nullptr;
            std::cout << "Server stopped" << std::endl;
        }
    }

    bool GameServer::isRunning() const {
        return daemon != nullptr;
    }

    std::string GameServer::generateGameId() {
        static std::random_device rd;
        static std::mt19937 gen(rd());
        static std::uniform_int_distribution<> dis(100000, 999999);
        return "game_" + std::to_string(dis(gen));
    }

    std::shared_ptr<GameSession> GameServer::getSession(const std::string& gameId) {
        std::lock_guard<std::mutex> lock(sessionsMutex);
        auto it = sessions.find(gameId);
        if (it != sessions.end()) {
            return it->second;
        }
        return nullptr;
    }

    struct MHD_Response* GameServer::createJsonResponse(const Json::Value& json, int statusCode) {
        Json::StreamWriterBuilder writer;
        std::string jsonStr = Json::writeString(writer, json);

        auto* response = MHD_create_response_from_buffer(
            jsonStr.length(),
            (void*)jsonStr.c_str(),
            MHD_RESPMEM_MUST_COPY
        );

        MHD_add_response_header(response, "Content-Type", "application/json");
        MHD_add_response_header(response, "Access-Control-Allow-Origin", "*");

        return response;
    }

    struct MHD_Response* GameServer::createErrorResponse(const std::string& error, int statusCode) {
        Json::Value json;
        json["error"] = error;
        return createJsonResponse(json, statusCode);
    }

    struct MHD_Response* GameServer::handleCreateGame(const std::string& body) {
        try {
            Json::Value requestJson;
            Json::CharReaderBuilder reader;
            std::string errors;
            std::istringstream iss(body);

            if (!Json::parseFromStream(reader, iss, &requestJson, &errors)) {
                return createErrorResponse("Invalid JSON: " + errors);
            }

            // Récupérer les IDs des équipes
            if (!requestJson.isMember("teamAId") || !requestJson.isMember("teamBId")) {
                return createErrorResponse("Missing teamAId or teamBId");
            }

            int teamAId = requestJson["teamAId"].asInt();
            int teamBId = requestJson["teamBId"].asInt();

            // Charger les équipes depuis TeamManager
            auto& teamManager = state::TeamManager::getInstance();
            auto teamAPtr = teamManager.getTeamById(teamAId);
            auto teamBPtr = teamManager.getTeamById(teamBId);

            if (!teamAPtr || !teamBPtr) {
                return createErrorResponse("Team not found");
            }

            // Créer le jeu
            auto game = std::make_shared<state::BloodBowlGame>(*teamAPtr, *teamBPtr);
            auto engine = std::make_shared<engine::Engine>(game);

            // Générer un ID de session
            std::string gameId = generateGameId();

            auto session = std::make_shared<GameSession>(gameId, game, engine);

            // Stocker la session
            {
                std::lock_guard<std::mutex> lock(sessionsMutex);
                sessions[gameId] = session;
            }

            // Réponse
            Json::Value response;
            response["gameId"] = gameId;
            response["message"] = "Game created successfully";

            return createJsonResponse(response);

        } catch (const std::exception& e) {
            return createErrorResponse(std::string("Error creating game: ") + e.what());
        }
    }

    struct MHD_Response* GameServer::handleJoinGame(const std::string& gameId, const std::string& body) {
        auto session = getSession(gameId);
        if (!session) {
            return createErrorResponse("Game not found", MHD_HTTP_NOT_FOUND);
        }

        // Parser le body pour obtenir l'ID de l'équipe
        Json::Value requestJson;
        Json::CharReaderBuilder reader;
        std::string errors;
        std::istringstream iss(body);

        int teamId = -1;
        if (Json::parseFromStream(reader, iss, &requestJson, &errors)) {
            if (requestJson.isMember("teamId")) {
                teamId = requestJson["teamId"].asInt();
                std::cout << "Player joining with team ID: " << teamId << std::endl;
            }
        }

        Json::Value response;
        response["gameId"] = gameId;
        response["message"] = "Joined game successfully";
        if (teamId != -1) {
            response["teamId"] = teamId;
        }

        return createJsonResponse(response);
    }

    struct MHD_Response* GameServer::handleGetState(const std::string& gameId) {
        auto session = getSession(gameId);
        if (!session) {
            return createErrorResponse("Game not found", MHD_HTTP_NOT_FOUND);
        }

        std::lock_guard<std::mutex> lock(session->gameMutex);

        // Sérialiser l'état du jeu
        Json::Value gameState = state::GameStateManager::getInstance().serializeGame(*session->game);
        gameState["version"] = session->version;

        return createJsonResponse(gameState);
    }

    struct MHD_Response* GameServer::handleCommand(const std::string& gameId, const std::string& body) {
        auto session = getSession(gameId);
        if (!session) {
            return createErrorResponse("Game not found", MHD_HTTP_NOT_FOUND);
        }

        try {
            Json::Value commandJson;
            Json::CharReaderBuilder reader;
            std::string errors;
            std::istringstream iss(body);

            if (!Json::parseFromStream(reader, iss, &commandJson, &errors)) {
                return createErrorResponse("Invalid JSON: " + errors);
            }

            std::lock_guard<std::mutex> lock(session->gameMutex);

            // Désérialiser la commande
            auto command = network::NetworkCommand::deserializeCommand(commandJson, session->game);

            if (!command) {
                return createErrorResponse("Failed to deserialize command");
            }

            // Ajouter et exécuter la commande
            session->engine->addCommand(std::move(command));
            session->engine->executeCommand();

            // Incrémenter la version
            session->version++;

            // Retourner le nouvel état
            Json::Value response = state::GameStateManager::getInstance().serializeGame(*session->game);
            response["version"] = session->version;

            return createJsonResponse(response);

        } catch (const std::exception& e) {
            return createErrorResponse(std::string("Error executing command: ") + e.what());
        }
    }

    struct MHD_Response* GameServer::handlePoll(const std::string& gameId, int clientVersion) {
        auto session = getSession(gameId);
        if (!session) {
            return createErrorResponse("Game not found", MHD_HTTP_NOT_FOUND);
        }

        // Simple polling sans long-polling pour l'instant
        std::lock_guard<std::mutex> lock(session->gameMutex);

        Json::Value response;
        response["version"] = session->version;

        if (session->version != clientVersion) {
            response["updated"] = true;
            response["gameState"] = state::GameStateManager::getInstance().serializeGame(*session->game);
        } else {
            response["updated"] = false;
        }

        return createJsonResponse(response);
    }

    MHD_Result GameServer::handleRequest(void* cls,
                                   struct MHD_Connection* connection,
                                   const char* url,
                                   const char* method,
                                   const char* version,
                                   const char* upload_data,
                                   size_t* upload_data_size,
                                   void** con_cls) {

        auto* server = static_cast<GameServer*>(cls);

        // Gérer les données POST
        if (*con_cls == nullptr) {
            auto* info = new ConnectionInfo();
            *con_cls = info;
            return MHD_YES;
        }

        auto* info = static_cast<ConnectionInfo*>(*con_cls);

        if (*upload_data_size != 0) {
            info->postData.append(upload_data, *upload_data_size);
            *upload_data_size = 0;
            return MHD_YES;
        }

        // Parser l'URL
        std::string urlStr(url);
        std::string methodStr(method);
        struct MHD_Response* response = nullptr;
        int statusCode = MHD_HTTP_OK;

        // Routes
        if (methodStr == "POST" && urlStr == "/game/create") {
            response = server->handleCreateGame(info->postData);
        }
        else if (methodStr == "POST" && urlStr.find("/game/join/") == 0) {
            std::string gameId = urlStr.substr(11); // Après "/game/join/"
            response = server->handleJoinGame(gameId, info->postData);
        }
        else if (methodStr == "GET" && urlStr.find("/game/state/") == 0) {
            std::string gameId = urlStr.substr(12); // Après "/game/state/"
            response = server->handleGetState(gameId);
        }
        else if (methodStr == "POST" && urlStr.find("/game/command/") == 0) {
            std::string gameId = urlStr.substr(14); // Après "/game/command/"
            response = server->handleCommand(gameId, info->postData);
        }
        else if (methodStr == "GET" && urlStr.find("/game/poll/") == 0) {
            std::string path = urlStr.substr(11); // Après "/game/poll/"
            size_t slashPos = path.find('/');
            std::string gameId = path.substr(0, slashPos);
            int clientVersion = 0;
            if (slashPos != std::string::npos) {
                clientVersion = std::stoi(path.substr(slashPos + 1));
            }
            response = server->handlePoll(gameId, clientVersion);
        }
        else {
            response = server->createErrorResponse("Route not found", MHD_HTTP_NOT_FOUND);
            statusCode = MHD_HTTP_NOT_FOUND;
        }

        MHD_Result ret = MHD_queue_response(connection, statusCode, response);
        MHD_destroy_response(response);

        delete info;
        *con_cls = nullptr;

        return ret;
    }

} // namespace server

