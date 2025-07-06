#include <stdexcept>
#include "doctest.h"
#include "../Player.h"
#include "../Role.h"

// Dummy role for independence
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

// === Basic Functionality ===
TEST_CASE("Player basic construction") {
    Player p("Alice", std::make_unique<DummyRole>());
    CHECK(p.getName() == "Alice");
    CHECK(p.getCoins() == 0);
    CHECK(p.isActive() == true);
    CHECK(p.getRole()->getName() == "Dummy");
}

TEST_CASE("Player constructor with null role") {
    CHECK_THROWS_AS(Player("NoRole", nullptr), std::invalid_argument);
}

TEST_CASE("Player constructor with empty name") {
    Player p("", std::make_unique<DummyRole>());
    CHECK(p.getName() == "");
}

// === Coin Management ===
TEST_CASE("Player coin management - regular and edge cases") {
    Player p("Bob", std::make_unique<DummyRole>());
    p.addCoins(5);
    CHECK(p.getCoins() == 5);

    p.removeCoins(3);
    CHECK(p.getCoins() == 2);

    CHECK_THROWS_AS(p.removeCoins(5), std::runtime_error);
    CHECK_THROWS_AS(p.addCoins(-2), std::invalid_argument);

    p.addCoins(10);
    p.removeCoins(10);
    CHECK(p.getCoins() == 2); // from earlier
}

TEST_CASE("Negative coin logic with no exception") {
    Player p("EdgeCase", std::make_unique<DummyRole>());
   // p.addCoins(-10);  // If allowed by impl
    CHECK_THROWS_AS(p.addCoins(-10), std::invalid_argument);

   // CHECK(p.getCoins() <= 0);
}

// === Deactivation ===
TEST_CASE("Player deactivation behavior") {
    Player p("Eve", std::make_unique<DummyRole>());
    p.deactivate();
    CHECK_FALSE(p.isActive());
    p.addCoins(5);
    CHECK(p.getCoins() >= 0);
}

// === Blocking System ===
TEST_CASE("Player blocking system") {
    Player p("Charlie", std::make_unique<DummyRole>());
    CHECK_FALSE(p.isBlocked("tax"));
    p.blockAction("tax",1);
    p.blockAction("tax",1);
    CHECK(p.isBlocked("tax"));
    p.blockAction("",1);
    CHECK(p.isBlocked(""));
    p.clearBlocks();
    CHECK_FALSE(p.isBlocked("tax"));
    CHECK_FALSE(p.isBlocked(""));
}

TEST_CASE("Block logic extended") {
    Player p("BlockMaster", std::make_unique<DummyRole>());
    p.blockAction("steal",1);
    CHECK(p.isBlocked("steal"));
    p.blockAction("steal",1); // redundant
    p.clearBlocks();
    CHECK_FALSE(p.isBlocked("steal"));
}

// === Bonus Action Logic ===
TEST_CASE("Player bonus action logic") {
    Player p("Dana", std::make_unique<DummyRole>());

    CHECK_FALSE(p.hasBonusAction());
    CHECK_FALSE(p.consumeBonusAction());

    p.setBonusAction(true);
    CHECK(p.hasBonusAction());

    CHECK(p.consumeBonusAction());
    CHECK_FALSE(p.hasBonusAction());
    CHECK_FALSE(p.consumeBonusAction());

    p.setBonusAction(false);
    CHECK_FALSE(p.hasBonusAction());
}

TEST_CASE("Player - Construction and Basic Operations") {
    SUBCASE("Valid player construction") {
        auto role = std::make_unique<DummyRole>();
        Player p("TestPlayer", std::move(role));
        CHECK(p.getName() == "TestPlayer");
        CHECK(p.getCoins() == 0);
        CHECK(p.isActive() == true);
        CHECK(p.getRole()->getName() == "Dummy");
    }

    SUBCASE("Player with null role throws exception") {
        CHECK_THROWS_AS(Player("Player", nullptr), std::invalid_argument);
    }

    SUBCASE("Player with empty name") {
        auto role = std::make_unique<DummyRole>();
        Player p("", std::move(role));
        CHECK(p.getName() == "");
        CHECK(p.isActive() == true);
    }

    SUBCASE("Player with very long name") {
        std::string longName(1000, 'A');
        auto role = std::make_unique<DummyRole>();
        Player p(longName, std::move(role));
        CHECK(p.getName() == longName);
    }

    SUBCASE("Player with special characters in name") {
        auto role = std::make_unique<DummyRole>();
        Player p("!@#$%^&*()", std::move(role));
        CHECK(p.getName() == "!@#$%^&*()");
    }
}

TEST_CASE("Player - Coin Management") {
    auto role = std::make_unique<DummyRole>();
    Player p("TestPlayer", std::move(role));

    SUBCASE("Add coins normally") {
        p.addCoins(5);
        CHECK(p.getCoins() == 5);
        p.addCoins(3);
        CHECK(p.getCoins() == 8);
    }

    SUBCASE("Add zero coins") {
        p.addCoins(0);
        CHECK(p.getCoins() == 0);
    }

    SUBCASE("Add negative coins throws exception") {
        CHECK_THROWS_AS(p.addCoins(-1), std::invalid_argument);
        CHECK_THROWS_AS(p.addCoins(-100), std::invalid_argument);
    }

    SUBCASE("Remove coins normally") {
        p.addCoins(10);
        p.removeCoins(3);
        CHECK(p.getCoins() == 7);
        p.removeCoins(7);
        CHECK(p.getCoins() == 0);
    }

    SUBCASE("Remove more coins than available") {
        p.addCoins(5);
        CHECK_THROWS_AS(p.removeCoins(6), std::runtime_error);
        CHECK_THROWS_AS(p.removeCoins(10), std::runtime_error);
    }

    SUBCASE("Remove coins from player with zero coins") {
        CHECK_THROWS_AS(p.removeCoins(1), std::runtime_error);
    }

    SUBCASE("Large coin operations") {
        p.addCoins(999999);
        CHECK(p.getCoins() == 999999);
        p.removeCoins(500000);
        CHECK(p.getCoins() == 499999);
    }
}

TEST_CASE("Player - Activation and Deactivation") {
    auto role = std::make_unique<DummyRole>();
    Player p("TestPlayer", std::move(role));

    SUBCASE("Player starts active") {
        CHECK(p.isActive() == true);
    }

    SUBCASE("Deactivate player") {
        p.deactivate();
        CHECK(p.isActive() == false);
    }

    SUBCASE("Deactivate already inactive player") {
        p.deactivate();
        p.deactivate(); // Should not throw
        CHECK(p.isActive() == false);
    }

    SUBCASE("Player can still manage coins after deactivation") {
        p.addCoins(10);
        p.deactivate();
        CHECK(p.getCoins() == 10);
        p.addCoins(5);
        CHECK(p.getCoins() == 15);
        p.removeCoins(3);
        CHECK(p.getCoins() == 12);
    }
}

TEST_CASE("Player - Action Blocking System") {
    auto role = std::make_unique<DummyRole>();
    Player p("TestPlayer", std::move(role));

    SUBCASE("Block action for one turn") {
        p.blockAction("gather", 1);
        CHECK(p.isBlocked("gather") == true);
        CHECK(p.isBlocked("tax") == false);
    }

    SUBCASE("Block action for multiple turns") {
        p.blockAction("tax", 3);
        CHECK(p.isBlocked("tax") == true);
        p.clearBlocks();
        CHECK(p.isBlocked("tax") == true); // Still blocked for 2 more turns
        p.clearBlocks();
        CHECK(p.isBlocked("tax") == true); // Still blocked for 1 more turn
        p.clearBlocks();
        CHECK(p.isBlocked("tax") == false); // Now unblocked
    }

    SUBCASE("Block multiple actions") {
        p.blockAction("gather", 1);
        p.blockAction("tax", 2);
        p.blockAction("bribe", 3);
        CHECK(p.isBlocked("gather") == true);
        CHECK(p.isBlocked("tax") == true);
        CHECK(p.isBlocked("bribe") == true);
        CHECK(p.isBlocked("arrest") == false);
    }

    SUBCASE("Clear blocks properly") {
        p.blockAction("gather", 1);
        p.blockAction("tax", 2);
        p.clearBlocks();
        CHECK(p.isBlocked("gather") == false);
        CHECK(p.isBlocked("tax") == true);
        p.clearBlocks();
        CHECK(p.isBlocked("tax") == false);
    }

    SUBCASE("Block with zero turns") {
        p.blockAction("gather", 0);
        CHECK(p.isBlocked("gather") == false);
    }

    SUBCASE("Block same action multiple times") {
        p.blockAction("gather", 1);
        p.blockAction("gather", 2); // Should overwrite
        CHECK(p.isBlocked("gather") == true);
        p.clearBlocks();
        CHECK(p.isBlocked("gather") == true); // Still blocked for 1 more turn
    }
}

TEST_CASE("Player - Bonus Action System") {
    auto role = std::make_unique<DummyRole>();
    Player p("TestPlayer", std::move(role));

    SUBCASE("No bonus action initially") {
        CHECK(p.hasBonusAction() == false);
        CHECK(p.consumeBonusAction() == false);
    }

    SUBCASE("Set and consume bonus action") {
        p.setBonusAction(true);
        CHECK(p.hasBonusAction() == true);
        CHECK(p.consumeBonusAction() == true);
        CHECK(p.hasBonusAction() == false);
        CHECK(p.consumeBonusAction() == false);
    }

    SUBCASE("Set bonus action to false") {
        p.setBonusAction(true);
        p.setBonusAction(false);
        CHECK(p.hasBonusAction() == false);
    }

    SUBCASE("Multiple bonus action sets") {
        p.setBonusAction(true);
        p.setBonusAction(true); // Should reset
        CHECK(p.hasBonusAction() == true);
        CHECK(p.consumeBonusAction() == true);
        CHECK(p.hasBonusAction() == false);
    }
}

TEST_CASE("Player - Bribe Usage Tracking") {
    auto role = std::make_unique<DummyRole>();
    Player p("TestPlayer", std::move(role));

    SUBCASE("Initially no bribe used") {
        CHECK(p.hasUsedBribeThisTurn() == false);
    }

    SUBCASE("Set bribe used") {
        p.setBribeUsedThisTurn(true);
        CHECK(p.hasUsedBribeThisTurn() == true);
    }

    SUBCASE("Reset bribe usage") {
        p.setBribeUsedThisTurn(true);
        p.setBribeUsedThisTurn(false);
        CHECK(p.hasUsedBribeThisTurn() == false);
    }
}