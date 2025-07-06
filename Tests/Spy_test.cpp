//
// Created by 12adi on 19/06/2025.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Spy.h"
#include <memory>

using namespace std;

class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Spy Role Tests") {
    Game game;
    auto spy = std::make_shared<Player>("Spy", std::make_unique<Spy>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(spy);
    game.addPlayer(other);

    SUBCASE("Spy can block arrest") {
        CHECK(spy->canBlock("arrest") == true);
        CHECK(spy->canBlock("coup") == false);
    }

    SUBCASE("Spy gets coin information") {
        spy->addCoins(3);
        other->addCoins(7);
        auto coinCounts = game.getPlayersCoinCounts();
        CHECK(coinCounts["Spy"] == 3);
        CHECK(coinCounts["Other"] == 7);
    }

    SUBCASE("Spy blocks arrest and sees attacker coins") {
        other->addCoins(5);
        game.nextTurn(); // Switch to other player

        // Spy should see attacker's coins and block arrest
        spy->getRole()->onArrest(*spy, *other, game);
        CHECK(other->isBlocked("arrest") == true);
    }

    SUBCASE("Spy planned arrest functionality") {
        game.markPlannedArrest(*spy, *other);
        CHECK(game.isArrestBlockedNextTurn(*other) == true);

        game.clearPlannedArrest(*other);
        CHECK(game.isArrestBlockedNextTurn(*other) == false);
    }
}
