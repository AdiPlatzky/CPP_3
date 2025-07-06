//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/General.h"
#include <memory>

using namespace std;

class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("General Role Tests") {
    Game game;
    auto general = std::make_shared<Player>("General", std::make_unique<General>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(general);
    game.addPlayer(other);

    SUBCASE("General can block coup") {
        CHECK(general->canBlock("coup") == true);
        CHECK(general->canBlock("arrest") == false);
    }

    SUBCASE("General defends against coup") {
        other->addCoins(10);
        general->addCoins(5);
        game.nextTurn(); // Switch to other player

        // General should defend if has 5+ coins
        int initial_coins = general->getCoins();
        general->getRole()->onCoup(*other, *general, game);
        if (initial_coins >= 5) {
            CHECK(general->getCoins() == initial_coins - 5);
        }
    }

    SUBCASE("General compensation on arrest") {
        other->addCoins(5);
        general->addCoins(5);
        game.nextTurn(); // Switch to other player

        int initial_attacker = other->getCoins();
        int initial_general = general->getCoins();

        general->getRole()->onArrest(*other, *general, game);
        // General should get coin back and attacker loses one
        CHECK(other->getCoins() < initial_attacker);
        CHECK(general->getCoins() > initial_general);
    }
}