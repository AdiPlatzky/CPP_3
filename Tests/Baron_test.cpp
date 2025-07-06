//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Baron.h"
#include <memory>

using namespace std;

class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Baron Role Tests") {
    Game game;
    auto baron = std::make_shared<Player>("Baron", std::make_unique<Baron>());
    game.addPlayer(baron);

    SUBCASE("Baron invest action") {
        baron->addCoins(5);
        auto result = game.performInvest(*baron);
        CHECK(result.success == true);
        CHECK(baron->getCoins() == 8); // 5 - 3 + 6 = 8
    }


    SUBCASE("Baron invest with exactly 3 coins") {
        baron->addCoins(3);
        auto result = game.performInvest(*baron);
        CHECK(result.success == true);
        CHECK(baron->getCoins() == 6); // 3 - 3 + 6 = 6
    }

    SUBCASE("Baron compensation on sanction") {
        auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
        game.addPlayer(other);

        other->addCoins(5);
        game.nextTurn(); // Switch to other player
        auto result = game.performSanction(*other, *baron);
        CHECK(result.success == true);
        // Baron should receive 1 coin compensation
        int initial_coins = baron->getCoins();
        // The baron's onSanction should be called
        baron->getRole()->onSanction(*baron, *other, game);
        CHECK(baron->getCoins() == initial_coins + 1);
    }
}