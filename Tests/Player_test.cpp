#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <stdexcept>
#include "doctest.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include "../Roles/Judge.h"
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
