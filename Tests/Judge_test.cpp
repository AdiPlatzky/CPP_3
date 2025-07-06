//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Judge.h"
#include <memory>

using namespace std;

class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Judge Role Tests") {
    Game game;
    auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(judge);
    game.addPlayer(other);

    SUBCASE("Judge can block bribe") {
        CHECK(judge->canBlock("bribe") == true);
        CHECK(judge->canBlock("tax") == false);
    }

    SUBCASE("Judge blocks bribe action") {
        other->addCoins(10);
        game.nextTurn(); // Switch to other player
        other->setBonusAction(true);
        // Judge should block the bonus action
        judge->getRole()->onBribe(*other, *other, game);
        CHECK(other->hasBonusAction() == false);
    }

    SUBCASE("Judge penalty on sanction - מספיק מטבעות") {
        other->addCoins(10); // ודאי שיש מספיק לשלם קנס
        game.nextTurn();
        auto result = game.performSanction(*other, *judge);
        CHECK(result.success == false);
        int initial_coins = other->getCoins();
        judge->getRole()->onSanction(*other, *judge, game);
        CHECK(other->getCoins() < initial_coins); // Should have paid penalty
    }
}
