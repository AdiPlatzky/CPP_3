//
// Created by 12adi on 19/06/2025.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include <memory>
using namespace std;


class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

TEST_CASE("Merchant Role Tests") {
    Game game;
    auto merchant = std::make_shared<Player>("Merchant", std::make_unique<Merchant>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(merchant);
    game.addPlayer(other);

    SUBCASE("Merchant gets bonus coin at turn start") {
        merchant->addCoins(3);
        int initial_coins = merchant->getCoins();
        merchant->getRole()->onTurnStart(*merchant, game);
        CHECK(merchant->getCoins() == initial_coins + 1);
    }

    SUBCASE("Merchant no bonus with less than 3 coins") {
        merchant->addCoins(2);
        int initial_coins = merchant->getCoins();
        merchant->getRole()->onTurnStart(*merchant, game);
        CHECK(merchant->getCoins() == initial_coins); // No bonus
    }

    SUBCASE("Merchant defense against arrest") {
        merchant->addCoins(5);
        other->addCoins(3);
        game.nextTurn(); // Switch to other player

        int initial_merchant = merchant->getCoins();
        merchant->getRole()->onArrest(*merchant, *other, game);

        if (initial_merchant >= 2) {
            CHECK(merchant->getCoins() == initial_merchant - 2);
        }
    }

    SUBCASE("Merchant insufficient coins for arrest defense") {
        merchant->addCoins(1);
        other->addCoins(3);
        game.nextTurn(); // Switch to other player

        // Should handle gracefully when merchant can't pay
        CHECK_NOTHROW(merchant->getRole()->onArrest(*merchant, *other, game));
    }
}