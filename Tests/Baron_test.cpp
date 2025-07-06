//
// Created by 12adi on 19/06/2025.
//
/**
 * @file Baron_test.cpp
 * @brief בדיקות יחידה (unit tests) עבור תפקיד ה־Baron (ברון) במשחק.
 * מתמקד בפעולות הייחודיות של הברון – השקעה (invest) ופיצוי (compensation) בסנקציה.
 */

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Baron.h"
#include <memory>

using namespace std;

/**
 * @class DummyRole
 * @brief רול דמה בסיסי עבור בדיקות – אין לו השפעות על לוגיקת המשחק.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test Baron Role Tests
 * בדיקות עבור כל יכולות ותסריטים הקשורים לתפקיד ה־Baron.
 */
TEST_CASE("Baron Role Tests") {
    Game game;
    auto baron = std::make_shared<Player>("Baron", std::make_unique<Baron>());
    game.addPlayer(baron);

    /**
     * @subcase בדיקה: פעולה תקינה של השקעה (invest) לברון, כאשר יש לו 5 מטבעות.
     * מצפה שהברון יוריד 3 מטבעות, יקבל 6, והתוצאה הסופית תהיה 8.
     */
    SUBCASE("Baron invest action") {
        baron->addCoins(5);
        auto result = game.performInvest(*baron);
        CHECK(result.success == true);
        CHECK(baron->getCoins() == 8); // 5 - 3 + 6 = 8
    }

    /**
     * @subcase בדיקה: פעולה של השקעה (invest) בדיוק עם 3 מטבעות.
     * מצפה שהברון יקבל 6 מטבעות וישאר עם 6.
     */
    SUBCASE("Baron invest with exactly 3 coins") {
        baron->addCoins(3);
        auto result = game.performInvest(*baron);
        CHECK(result.success == true);
        CHECK(baron->getCoins() == 6); // 3 - 3 + 6 = 6
    }

    /**
     * @subcase בדיקה: הברון מקבל פיצוי כאשר הוא נפגע בפעולת סנקציה (sanction).
     * נוצר שחקן נוסף, מבוצעת נגד הברון סנקציה, נבדק שהוא קיבל מטבע אחד כפיצוי.
     */
    SUBCASE("Baron compensation on sanction") {
        auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
        game.addPlayer(other);

        other->addCoins(5);
        game.nextTurn(); // מעבר לשחקן השני
        auto result = game.performSanction(*other, *baron);
        CHECK(result.success == true);
        // בודקים את מספר המטבעות לפני הפיצוי
        int initial_coins = baron->getCoins();
        // הקריאה ל־onSanction אמורה להעניק לברון מטבע נוסף
        baron->getRole()->onSanction(*baron, *other, game);
        CHECK(baron->getCoins() == initial_coins + 1);
    }
}
