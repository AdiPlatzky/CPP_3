//
// Created by 12adi on 19/06/2025.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Governor.h"
#include <memory>

using namespace std;

class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Governor Role Tests") {
    Game game;
    auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(governor);
    game.addPlayer(other);

    SUBCASE("Governor gets extra coin from tax") {
        auto result = game.performTax(*governor);
        CHECK(result.success == true);
        game.applyTax(*governor);
        CHECK(governor->getCoins() == 3); // 2 + 1 bonus = 3
    }

    SUBCASE("Governor can block tax") {
        CHECK(governor->canBlock("tax") == true);
        CHECK(governor->canBlock("gather") == false);
    }

    SUBCASE("Governor blocks other player's tax") {
        game.nextTurn(); // Switch to other player
        auto result = game.performTax(*other);
        CHECK(result.success == true);
        // When applied, governor should block the tax
        game.applyTax(*other);
        CHECK(other->isBlocked("tax") == true);
    }
}