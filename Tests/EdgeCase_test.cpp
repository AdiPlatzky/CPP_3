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

TEST_CASE("Game - Edge Cases and Stress Tests") {
    SUBCASE("Maximum players") {
        Game game;
        std::vector<std::shared_ptr<Player>> players;

        for (int i = 0; i < 100; ++i) {
            auto player = std::make_shared<Player>("Player" + std::to_string(i),
                                                  std::make_unique<DummyRole>());
            players.push_back(player);
            game.addPlayer(player);
        }

        CHECK(game.getPlayer().size() == 100);
        CHECK(game.getActivePlayers().size() == 100);
    }

    SUBCASE("All players inactive except one") {
        Game game;
        auto p1 = std::make_shared<Player>("Winner", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Loser1", std::make_unique<DummyRole>());
        auto p3 = std::make_shared<Player>("Loser2", std::make_unique<DummyRole>());

        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        p2->deactivate();
        p3->deactivate();
        game.checkGameOver();

        CHECK(game.isGameOver() == true);
        CHECK(game.getWinner() == "Winner");
    }

    SUBCASE("Rapid turn switching with many players") {
        Game game;
        std::vector<std::shared_ptr<Player>> players;

        for (int i = 0; i < 20; ++i) {
            auto player = std::make_shared<Player>("Player" + std::to_string(i),
                                                  std::make_unique<DummyRole>());
            players.push_back(player);
            game.addPlayer(player);
        }

        // Test 1000 turn switches
        for (int i = 0; i < 1000; ++i) {
            std::string expectedPlayer = "Player" + std::to_string(i % 20);
            CHECK(game.getCurrentPlayer().getName() == expectedPlayer);
            game.nextTurn();
        }
    }

   // SUBCASE("Massive coin operations") {
     //   Game game;
       // auto player = std::make_shared<Player>("RichPlayer", std::make_unique<DummyRole>());
        //game.addPlayer(player);

        // Add millions of coins
        //player->addCoins(1000000);
       // CHECK(player->getCoins() == 1000000);

        // Remove large amounts
        //player->removeCoins(500000);
        //CHECK(player->getCoins() == 500000);

        // Test coin pool manipulations
        //game.addToCoinPool(-500000);
        //game.addToCoinPool(1000000);
        //CHECK(game.getCoinPool() > 500000);
    //}

    SUBCASE("Complex blocking scenarios") {
        Game game;
        auto player = std::make_shared<Player>("BlockedPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Block multiple actions for different durations
        player->blockAction("gather", 5);
        player->blockAction("tax", 3);
        player->blockAction("bribe", 1);
        player->blockAction("arrest", 10);

        // Test decreasing blocks over time
        for (int turn = 0; turn < 12; ++turn) {
            bool gather_blocked = (turn < 5);
            bool tax_blocked = (turn < 3);
            bool bribe_blocked = (turn < 1);
            bool arrest_blocked = (turn < 10);

            CHECK(player->isBlocked("gather") == gather_blocked);
            CHECK(player->isBlocked("tax") == tax_blocked);
            CHECK(player->isBlocked("bribe") == bribe_blocked);
            CHECK(player->isBlocked("arrest") == arrest_blocked);

            player->clearBlocks();
        }
    }
}

TEST_CASE("Game - Error Handling and Invalid States") {
  //  SUBCASE("Invalid action on empty game") {
    //    Game game;
      //  CHECK_THROWS(game.getCurrentPlayer());
        //CHECK_THROWS(game.nextTurn());
    //}

    SUBCASE("Actions after game over") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);

        p2->deactivate();
        game.checkGameOver();

        // Game should still allow operations on the remaining player
        CHECK_NOTHROW(game.getCurrentPlayer());
    }

    SUBCASE("Null pointer handling") {
        Game game;
        auto player = std::make_shared<Player>("TestPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Test getPlayerByName with invalid name
        CHECK(game.getPlayerByName("NonExistent") == nullptr);
        CHECK(game.getPlayerByName("") == nullptr);
    }

    SUBCASE("Player with same names") {
        Game game;
        auto p1 = std::make_shared<Player>("SameName", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("SameName", std::make_unique<DummyRole>());

        game.addPlayer(p1);
        game.addPlayer(p2);

        CHECK(game.getPlayer().size() == 2);
        // Both players should exist despite same names
        auto found = game.getPlayerByName("SameName");
        CHECK(found != nullptr);
    }

    SUBCASE("Extreme blocking values") {
        Game game;
        auto player = std::make_shared<Player>("TestPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Test with very large block durations
        player->blockAction("gather", 99999);
        CHECK(player->isBlocked("gather") == true);

        // Test with zero duration
        player->blockAction("tax", 0);
        CHECK(player->isBlocked("tax") == false);

        // Test negative duration (should be handled gracefully)
        player->blockAction("bribe", -1);
        // Behavior depends on implementation
    }
}

TEST_CASE("Game - Multi-Role Interaction Tests") {
    SUBCASE("Governor vs Judge tax blocking") {
        Game game;
        auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
        auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
        game.addPlayer(governor);
        game.addPlayer(judge);

        // Governor should get bonus tax, judge shouldn't interfere with tax
        auto result = game.performTax(*governor);
        CHECK(result.success == true);
        game.applyTax(*governor);
        CHECK(governor->getCoins() == 3); // 2 + 1 governor bonus
    }

    SUBCASE("Baron vs General coup defense") {
        Game game;
        auto baron = std::make_shared<Player>("Baron", std::make_unique<Baron>());
        auto general = std::make_shared<Player>("General", std::make_unique<General>());
        game.addPlayer(baron);
        game.addPlayer(general);

        baron->addCoins(10);
        general->addCoins(5);

        auto result = game.performCoup(*baron, *general);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);

        // General can potentially block the coup
        if (general->getCoins() >= 5) {
            general->getRole()->onCoup(*baron, *general, game);
            // General should survive if he had enough coins
        }
    }

    //SUBCASE("Spy vs Merchant arrest interaction") {
        //Game game;
        //auto spy = std::make_shared<Player>("Spy", std::make_unique<Spy>());
        //auto merchant = std::make_shared<Player>("Merchant", std::make_unique<Merchant>());
        //game.addPlayer(spy);
        //game.addPlayer(merchant);

        //spy->addCoins(3);
        //merchant->addCoins(5);

        // Spy arrests merchant
        //auto result = game.performArrest(*spy, *merchant);
        //CHECK(result.success == true);
        //game.applyArrest(*spy, *merchant);

        // Merchant should defend by paying 2 coins to pool
        // Spy should not get the coin
      //  CHECK(merchant->getCoins() == 3); // 5 - 2 = 3
    //    CHECK(spy->getCoins() == 3); // Should not have gained coin
  //  }

    SUBCASE("Multiple role abilities in one turn") {
        Game game;
        auto merchant = std::make_shared<Player>("Merchant", std::make_unique<Merchant>());
        game.addPlayer(merchant);

        merchant->addCoins(3);

        // Merchant starts turn with 3+ coins, should get bonus
        merchant->getRole()->onTurnStart(*merchant, game);
        CHECK(merchant->getCoins() == 4);

        // Then merchant performs tax
        auto result = game.performTax(*merchant);
        CHECK(result.success == true);
        game.applyTax(*merchant);
        CHECK(merchant->getCoins() == 6); // 4 + 2 = 6
    }
}

TEST_CASE("Game - Complex Scenarios") {
    SUBCASE("Chain reaction eliminations") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        auto p3 = std::make_shared<Player>("Player3", std::make_unique<DummyRole>());
        auto p4 = std::make_shared<Player>("Player4", std::make_unique<DummyRole>());

        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);
        game.addPlayer(p4);

        // Simulate rapid eliminations
        p1->addCoins(20);

        // P1 coups P2
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p2->isActive() == false);
        CHECK(p1->getCoins() == 13);

        game.nextTurn(); // Skip to P3
        game.nextTurn(); // Skip to P4
        game.nextTurn(); // Back to P1

        // P1 coups P3
        result = game.performCoup(*p1, *p3);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p3);
        CHECK(p3->isActive() == false);
        CHECK(p1->getCoins() == 6);

        game.checkGameOver();
        CHECK(game.isGameOver() == false); // P1 and P4 still active

        game.nextTurn(); // Skip to P4
        game.nextTurn(); // Back to P1

        // P1 can't coup P4 yet (only 6 coins)
        result = game.performCoup(*p1, *p4);
        CHECK(result.success == false);
    }

    SUBCASE("Resource exhaustion scenarios") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Drain coin pool to test edge cases
        game.getCoinPool() = 1;

        auto result = game.performGather(*player);
        CHECK(result.success == true);
        game.applyGather(*player);
        CHECK(player->getCoins() == 1);

        // Try gather again with empty pool
        game.getCoinPool() = 0;
        result = game.performGather(*player);
        if (result.success) {
            CHECK_THROWS(game.applyGather(*player));
        }
    }

    SUBCASE("Forced coup scenario") {
        Game game;
        auto p1 = std::make_shared<Player>("RichPlayer", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("TargetPlayer", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);

        p1->addCoins(10);

        // With 10+ coins, player must coup
        auto gather_result = game.performGather(*p1);
        CHECK(gather_result.success == false);

        auto tax_result = game.performTax(*p1);
        CHECK(tax_result.success == false);

        auto bribe_result = game.performBribe(*p1);
        CHECK(bribe_result.success == false);

        // Only coup should work
        auto coup_result = game.performCoup(*p1, *p2);
        CHECK(coup_result.success == true);
    }

    SUBCASE("Blocked action combinations") {
        Game game;
        auto player = std::make_shared<Player>("BlockedPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Block all economic actions
        player->blockAction("gather", 2);
        player->blockAction("tax", 2);
        player->addCoins(5);

        // Only non-blocked actions should work
        CHECK(game.performGather(*player).success == false);
        CHECK(game.performTax(*player).success == false);
        CHECK(game.performBribe(*player).success == true);
    }
}

TEST_CASE("Game - Boundary Value Tests") {
    SUBCASE("Zero coin operations") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);

        CHECK(player->getCoins() == 0);
        CHECK_THROWS(player->removeCoins(1));

        player->addCoins(0);
        CHECK(player->getCoins() == 0);
    }


    SUBCASE("Minimum coins for actions") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);

        // Test minimum coins for sanction (3)
        p1->addCoins(3);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);

        // Test minimum coins for bribe (4)
        p1->addCoins(4);
        result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);

        // Test minimum coins for coup (7)
        p1->addCoins(7);
        result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 0);
        CHECK(p2->isActive() == false);
    }

    SUBCASE("One less than minimum coins") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);

        // Test one less than minimum
        p1->addCoins(2);
        CHECK(game.performSanction(*p1, *p2).success == false);

        p1->addCoins(1); // Now has 3
        p1->removeCoins(1); // Back to 2
        p1->addCoins(1); // Now has 3
        CHECK(game.performBribe(*p1).success == false);

        p1->addCoins(3); // Now has 6
        CHECK(game.performCoup(*p1, *p2).success == false);
    }
}

TEST_CASE("Game - Threading and Concurrency Edge Cases") {
    // Note: These tests simulate potential threading issues
    // even though the current implementation may not be thread-safe

    SUBCASE("Rapid state changes") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Simulate rapid activation/deactivation
        for (int i = 0; i < 1000; ++i) {
            if (i % 2 == 0) {
                player->deactivate();
                CHECK(player->isActive() == false);
            } else {
                // Player once deactivated cannot be reactivated in current design
                CHECK(player->isActive() == false);
            }
        }
    }

    SUBCASE("Rapid coin operations") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Simulate rapid coin changes
        for (int i = 0; i < 1000; ++i) {
            player->addCoins(1);
            CHECK(player->getCoins() == i + 1);
        }

        for (int i = 999; i >= 0; --i) {
            player->removeCoins(1);
            CHECK(player->getCoins() == i);
        }
    }
}

TEST_CASE("Game - Memory and Performance Edge Cases") {
    SUBCASE("Large number of blocked actions") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);

        // Block many different actions
        for (int i = 0; i < 1000; ++i) {
            std::string action = "action" + std::to_string(i);
            player->blockAction(action, 1);
            CHECK(player->isBlocked(action) == true);
        }

        // Clear all blocks
        player->clearBlocks();

        // Check all are cleared
        for (int i = 0; i < 1000; ++i) {
            std::string action = "action" + std::to_string(i);
            CHECK(player->isBlocked(action) == false);
        }
    }

    SUBCASE("String handling edge cases") {
        Game game;

        // Test empty string names
        auto player1 = std::make_shared<Player>("", std::make_unique<DummyRole>());
        game.addPlayer(player1);
        CHECK(player1->getName() == "");

        // Test very long names
        std::string longName(10000, 'X');
        auto player2 = std::make_shared<Player>(longName, std::make_unique<DummyRole>());
        game.addPlayer(player2);
        CHECK(player2->getName() == longName);

        // Test special characters
        auto player3 = std::make_shared<Player>("测试", std::make_unique<DummyRole>());
        game.addPlayer(player3);
        CHECK(player3->getName() == "测试");
    }
}
