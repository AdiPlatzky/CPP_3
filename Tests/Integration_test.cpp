//
// Created by 12adi on 06/07/2025.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Judge.h"
#include "../Roles/General.h"
#include "../Roles/Governor.h"
#include "../Roles/Merchant.h"
#include "../Roles/Spy.h"
#include "../Roles/Baron.h"
#include <memory>
using namespace std;


class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Game - Full Game Simulation") {
    SUBCASE("Complete 4-player game") {
        Game game;
        auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
        auto baron = std::make_shared<Player>("Baron", std::make_unique<Baron>());
        auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
        auto general = std::make_shared<Player>("General", std::make_unique<General>());

        game.addPlayer(governor);
        game.addPlayer(baron);
        game.addPlayer(judge);
        game.addPlayer(general);

        CHECK(game.getPlayer().size() == 4);
        CHECK(game.isGameOver() == false);

        // Round 1
        CHECK(game.getCurrentPlayer().getName() == "Governor");
        game.performTax(*governor);
        game.applyTax(*governor);
        CHECK(governor->getCoins() == 3);
        game.nextTurn();

        CHECK(game.getCurrentPlayer().getName() == "Baron");
        baron->addCoins(3);
        game.performInvest(*baron);
        CHECK(baron->getCoins() == 6);
        game.nextTurn();

        CHECK(game.getCurrentPlayer().getName() == "Judge");
        game.performGather(*judge);
        game.applyGather(*judge);
        CHECK(judge->getCoins() == 1);
        game.nextTurn();

        CHECK(game.getCurrentPlayer().getName() == "General");
        game.performGather(*general);
        game.applyGather(*general);
        CHECK(general->getCoins() == 1);
        game.nextTurn();

        // Continue until someone can coup
        while (!game.isGameOver()) {
            auto& current = game.getCurrentPlayer();
            if (current.getCoins() >= 10) {
                // Must coup
                for (auto& target : game.getPlayer()) {
                    if (target->isActive() && target.get() != &current) {
                        auto result = game.performCoup(current, *target);
                        if (result.success) {
                            game.applyCoup(current, *target);
                            break;
                        }
                    }
                }
            } else if (current.getCoins() >= 7) {
                // Can coup
                for (auto& target : game.getPlayer()) {
                    if (target->isActive() && target.get() != &current) {
                        auto result = game.performCoup(current, *target);
                        if (result.success) {
                            game.applyCoup(current, *target);
                            break;
                        }
                    }
                }
            } else {
                // Gather coins
                auto result = game.performGather(current);
                if (result.success) {
                    game.applyGather(current);
                }
            }

            game.checkGameOver();
            if (!game.isGameOver()) {
                game.nextTurn();
            }

            // Safety break to prevent infinite loop
            static int turnCount = 0;
            if (++turnCount > 1000) break;
        }

        if (game.isGameOver()) {
            CHECK_NOTHROW(game.getWinner());
        }
    }
}