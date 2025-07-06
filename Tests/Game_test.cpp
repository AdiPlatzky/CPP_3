#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include "../Roles/Judge.h"
#include <memory>


class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Game - addPlayer and getActivePlayers") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);

    auto active = game.getActivePlayers();
    CHECK(active.size() == 2);
    CHECK(active[0] == "Alice");
    CHECK(active[1] == "Bob");
}

TEST_CASE("Game - getCurrentPlayer before and after nextTurn") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);

    CHECK(game.getCurrentPlayer().getName() == "Alice");
    game.nextTurn();
    CHECK(game.getCurrentPlayer().getName() == "Bob");
    game.nextTurn();
    CHECK(game.getCurrentPlayer().getName() == "Alice");
}

TEST_CASE("Game - no players added") {
    Game game;
    CHECK_THROWS(game.getCurrentPlayer());
    CHECK(game.getActivePlayers().empty());
}

TEST_CASE("Game - add same player twice") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p1); // תלוי אם מותר או לא
    CHECK(game.getActivePlayers().size() == 2);
}

TEST_CASE("Game - manipulate coin pool directly") {
    Game game;
    int& pool = game.getCoinPool();
    pool = -100; // שינוי חריג ישיר
    CHECK(game.getCoinPool() == -100);

    game.addToCoinPool(150);
    CHECK(game.getCoinPool() == 50);
}

TEST_CASE("Game - performGather by non-current player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    // Bob is not current player
    CHECK_THROWS(game.performGather(*p2));
}

TEST_CASE("Game - performTax by dead player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alive", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    p1->deactivate();

    CHECK_THROWS(game.performTax(*p1));
}

TEST_CASE("Game - performBribe twice in same turn") {
    Game game;
    auto p1 = std::make_shared<Player>("Briber", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Target", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    p1->addCoins(6);
    game.performBribe(*p1);
    CHECK_THROWS(game.performBribe(*p1));
}

TEST_CASE("Game - arrest a dead player") {
    Game game;
    auto p1 = std::make_shared<Player>("Cop", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Dead", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);
    p2->deactivate();

    CHECK_THROWS(game.performArrest(*p1, *p2));
}

TEST_CASE("Game - sanction and coup edge cases") {
    Game game;
    auto p1 = std::make_shared<Player>("One", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Two", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    p1->addCoins(6);
    game.performSanction(*p1, *p2);
    CHECK(p2->isActive());

    p1->addCoins(6);
    auto coupResult = game.performCoup(*p1, *p2);
    CHECK(coupResult.success);         // לוודא שהפעולה חוקית
    CHECK(coupResult.requiresBlocking); // לוודא שיש אפשרות חסימה
    CHECK(p2->isActive());             // עדיין פעיל עד שנפעיל apply!
    game.applyCoup(*p1, *p2);          // *זה השלב החדש*
    CHECK_FALSE(p2->isActive());       // עכשיו הוא אמור להיות מודח
}


TEST_CASE("Game - invest with current player") {
    Game game;
    auto p1 = std::make_shared<Player>("Investor", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    CHECK_NOTHROW(game.performInvest(*p1));
}

TEST_CASE("Game - game over and winner logic") {
    Game game;
    auto p1 = std::make_shared<Player>("Winner", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Loser", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    p2->deactivate();
    game.checkGameOver();

    CHECK(game.isGameOver());
    CHECK(game.getWinner() == "Winner");
}
