#include "NetworkCommand.h"
#include "engine/Command.h"
#include "engine/Move.h"
#include "engine/Block.h"
#include "engine/Pass.h"
#include "engine/PickUpBall.h"
#include "engine/EndTurn.h"
#include "state/BloodBowlGame.h"
#include "state/Character.h"
#include "state/Team.h"
#include <stdexcept>

namespace network {

    Json::Value NetworkCommand::serializeCommand(const engine::Command* command) {
        Json::Value json;

        if (!command) {
            return json;
        }

        engine::CommandTypeId typeId = command->getCommandTypeId();
        json["type"] = static_cast<int>(typeId);

        // Selon le type de commande, sérialiser les données spécifiques
        switch (typeId) {
            case engine::MoveId: {
                auto* moveCmd = dynamic_cast<const engine::Move*>(command);
                if (moveCmd) {
                    // Les détails du Move sont gérés par le serveur
                    // Le client n'a besoin que du type
                }
                break;
            }
            case engine::BlockId: {
                auto* blockCmd = dynamic_cast<const engine::Block*>(command);
                if (blockCmd) {
                    // Idem pour Block
                }
                break;
            }
            case engine::PassId: {
                // Idem pour Pass
                break;
            }
            case engine::EndTurnId: {
                // Pas de données supplémentaires
                break;
            }
            default:
                break;
        }

        return json;
    }

    std::unique_ptr<engine::Command> NetworkCommand::deserializeCommand(
        const Json::Value& json,
        const std::shared_ptr<state::BloodBowlGame>& game) {

        if (!json.isMember("type")) {
            throw std::runtime_error("Command JSON missing 'type' field");
        }

        int typeInt = json["type"].asInt();
        auto typeId = static_cast<engine::CommandTypeId>(typeInt);

        switch (typeId) {
            case engine::MoveId: {
                if (!json.isMember("characterId") || !json.isMember("targetX") || !json.isMember("targetY")) {
                    throw std::runtime_error("Move command missing required fields");
                }

                int charId = json["characterId"].asInt();
                int targetX = json["targetX"].asInt();
                int targetY = json["targetY"].asInt();

                auto character = findCharacterById(game, charId);
                if (!character) {
                    throw std::runtime_error("Character not found for Move command");
                }

                return std::make_unique<engine::Move>(character, std::make_pair(targetX, targetY));
            }
            case engine::BlockId: {
                if (!json.isMember("attackerId") || !json.isMember("defenderId")) {
                    throw std::runtime_error("Block command missing required fields");
                }

                int attackerId = json["attackerId"].asInt();
                int defenderId = json["defenderId"].asInt();

                auto attacker = findCharacterById(game, attackerId);
                auto defender = findCharacterById(game, defenderId);

                if (!attacker || !defender) {
                    throw std::runtime_error("Character not found for Block command");
                }

                return std::make_unique<engine::Block>(attacker, defender);
            }
            case engine::PassId: {
                if (!json.isMember("passerId") || !json.isMember("receiverId")) {
                    throw std::runtime_error("Pass command missing required fields");
                }

                int passerId = json["passerId"].asInt();
                int receiverId = json["receiverId"].asInt();

                auto passer = findCharacterById(game, passerId);
                auto receiver = findCharacterById(game, receiverId);

                if (!passer || !receiver) {
                    throw std::runtime_error("Character not found for Pass command");
                }

                return std::make_unique<engine::Pass>(passer, receiver);
            }
            case engine::PickUpBallId: {
                if (!json.isMember("characterId")) {
                    throw std::runtime_error("PickUpBall command missing characterId");
                }

                int charId = json["characterId"].asInt();
                auto character = findCharacterById(game, charId);

                if (!character) {
                    throw std::runtime_error("Character not found for PickUpBall command");
                }

                return std::make_unique<engine::PickUpBall>(character);
            }
            case engine::EndTurnId: {
                return std::make_unique<engine::EndTurn>();
            }
            default:
                throw std::runtime_error("Unknown command type");
        }
    }

    std::shared_ptr<state::Character> NetworkCommand::findCharacterById(
        const std::shared_ptr<state::BloodBowlGame>& game,
        int characterId) {

        if (!game) {
            return nullptr;
        }

        // Chercher dans l'équipe A
        for (const auto& charPtr : game->getTeamA().getCharacters()) {
            if (charPtr && charPtr->getId() == characterId) {
                return charPtr;
            }
        }

        // Chercher dans l'équipe B
        for (const auto& charPtr : game->getTeamB().getCharacters()) {
            if (charPtr && charPtr->getId() == characterId) {
                return charPtr;
            }
        }

        return nullptr;
    }

} // namespace network

