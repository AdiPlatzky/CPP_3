//
// Created by 12adi on 06/07/2025.
//
#include <stdexcept>
#include "doctest.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include "../Roles/Judge.h"
#include "../Role.h"
#include "../Game.h"


// Dummy role for independence
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Game - Gather Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    SUBCASE("Valid gather action") {
        auto result = game.performGather(*p1);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyGather(*p1);
        CHECK(p1->getCoins() == 1);
    }

    SUBCASE("Gather by wrong player") {
        CHECK_THROWS(game.performGather(*p2)); // Not current player's turn
    }

    SUBCASE("Gather by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performGather(*p1));
    }

    SUBCASE("Gather when blocked") {
        p1->blockAction("gather", 1);
        auto result = game.performGather(*p1);
        CHECK(result.success == false);
    }

    SUBCASE("Gather with 10+ coins") {
        p1->addCoins(10);
        auto result = game.performGather(*p1);
        CHECK(result.success == false); // Must coup instead
    }

    SUBCASE("Gather with insufficient coin pool") {
        game.getCoinPool() = 0;
        auto result = game.performGather(*p1);
        if (result.success) {
            CHECK_THROWS(game.applyGather(*p1));
        }
    }
}

TEST_CASE("Game - Tax Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    SUBCASE("Valid tax action") {
        auto result = game.performTax(*p1);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyTax(*p1);
        CHECK(p1->getCoins() == 2);
    }

    SUBCASE("Tax by wrong player") {
        CHECK_THROWS(game.performTax(*p2));
    }

    SUBCASE("Tax by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performTax(*p1));
    }

    SUBCASE("Tax when blocked") {
        p1->blockAction("tax", 1);
        auto result = game.performTax(*p1);
        CHECK(result.success == false);
    }

    SUBCASE("Tax with 10+ coins") {
        p1->addCoins(10);
        auto result = game.performTax(*p1);
        CHECK(result.success == false);
    }
}

TEST_CASE("Game - Bribe Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    game.addPlayer(p1);

    SUBCASE("Valid bribe action") {
        p1->addCoins(5);
        auto result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 1); // 5 - 4 = 1
        CHECK(p1->hasUsedBribeThisTurn() == true);
    }

    SUBCASE("Bribe with insufficient coins") {
        p1->addCoins(3);
        auto result = game.performBribe(*p1);
        CHECK(result.success == false);
    }

    SUBCASE("Bribe when blocked") {
        p1->addCoins(5);
        p1->blockAction("bribe", 1);
        auto result = game.performBribe(*p1);
        CHECK(result.success == false);
    }

    SUBCASE("Bribe with exactly 4 coins") {
        p1->addCoins(4);
        auto result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
    }
}

TEST_CASE("Game - Arrest Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);


    SUBCASE("Arrest self") {
        auto result = game.performArrest(*p1, *p1);
        CHECK(result.success == false);
    }

    SUBCASE("Arrest inactive player") {
        p2->deactivate();
        CHECK_THROWS(game.performArrest(*p1, *p2));
    }

    SUBCASE("Arrest player with no coins") {
        auto result = game.performArrest(*p1, *p2);
        CHECK(result.success == false);
    }

    SUBCASE("Arrest by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performArrest(*p1, *p2));
    }

    SUBCASE("Arrest with 10+ coins") {
        p1->addCoins(10);
        p2->addCoins(5);
        auto result = game.performArrest(*p1, *p2);
        CHECK(result.success == false);
    }
}

TEST_CASE("Game - Sanction Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    SUBCASE("Valid sanction action") {
        p1->addCoins(5);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 2); // 5 - 3 = 2
        CHECK(p2->isBlocked("gather") == true);
        CHECK(p2->isBlocked("tax") == true);
    }

    SUBCASE("Sanction with insufficient coins") {
        p1->addCoins(2);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == false);
    }

    SUBCASE("Sanction self") {
        p1->addCoins(5);
        auto result = game.performSanction(*p1, *p1);
        CHECK(result.success == false);
    }

    SUBCASE("Sanction inactive player") {
        p1->addCoins(5);
        p2->deactivate();
        CHECK_THROWS(game.performSanction(*p1, *p2));
    }

    SUBCASE("Sanction with exactly 3 coins") {
        p1->addCoins(3);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
    }
}

TEST_CASE("Game - Coup Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    SUBCASE("Valid coup action") {
        p1->addCoins(10);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 3); // 10 - 7 = 3
        CHECK(p2->isActive() == false);
    }

    SUBCASE("Coup with insufficient coins") {
        p1->addCoins(6);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == false);
    }

    SUBCASE("Coup self") {
        p1->addCoins(10);
        auto result = game.performCoup(*p1, *p1);
        CHECK(result.success == false);
    }

    SUBCASE("Coup inactive player") {
        p1->addCoins(10);
        p2->deactivate();
        CHECK_THROWS(game.performCoup(*p1, *p2));
    }

    SUBCASE("Coup when blocked") {
        p1->addCoins(10);
        p1->blockAction("coup", 1);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == false);
    }

    SUBCASE("Coup with exactly 7 coins") {
        p1->addCoins(7);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 0);
        CHECK(p2->isActive() == false);
    }
}
