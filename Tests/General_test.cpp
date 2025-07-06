//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/General.h"
#include <memory>

using namespace std;

/**
 * @class DummyRole
 * @brief תפקיד דמה (DummyRole) – משמש לטסטים בלבד, אינו משפיע על פעולות המשחק.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test בדיקות לתפקיד הגנרל (General) – בדיקת יכולות חסימה, הגנה, ופיצוי.
 */
TEST_CASE("General Role Tests") {
    Game game;
    auto general = std::make_shared<Player>("General", std::make_unique<General>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(general);
    game.addPlayer(other);

    /**
     * @subcase בדיקה האם הגנרל מסוגל לחסום פעולת Coup (הפיכה), ואינו חוסם arrest.
     */
    SUBCASE("General can block coup") {
        CHECK(general->canBlock("coup") == true);    // גנרל אמור לחסום הפיכה
        CHECK(general->canBlock("arrest") == false); // לא אמור לחסום arrest
    }

    /**
     * @subcase בדיקה האם הגנרל באמת מגן על עצמו כאשר יש לו מספיק מטבעות בעת Coup.
     * מדמה תרחיש בו שחקן אחר מנסה לבצע הפיכה, ולגנרל יש בדיוק או יותר מ-5 מטבעות.
     * במצב כזה, אמור להיות קיזוז של 5 מטבעות והגנרל שורד.
     */
    SUBCASE("General defends against coup") {
        other->addCoins(10);      // השחקן התוקף עם מספיק כסף להפיכה
        general->addCoins(5);     // לגנרל מספיק כדי להגן על עצמו
        game.nextTurn();          // עוברים לשחקן השני (other)

        int initial_coins = general->getCoins();
        general->getRole()->onCoup(*other, *general, game); // מפעילים תגובת גנרל
        if (initial_coins >= 5) {
            CHECK(general->getCoins() == initial_coins - 5); // ציפייה שירדו 5 מטבעות
        }
    }

    /**
     * @subcase בדיקה שהגנרל מקבל פיצוי כאשר נעשתה עליו פעולת arrest:
     * התוקף (other) מפסיד מטבע, הגנרל מקבל מטבע.
     */
    SUBCASE("General compensation on arrest") {
        other->addCoins(5);     // לתוקף יש כסף
        general->addCoins(5);   // לגנרל גם יש כסף
        game.nextTurn();        // תור התוקף

        int initial_attacker = other->getCoins();
        int initial_general = general->getCoins();

        general->getRole()->onArrest(*other, *general, game);
        // בודקים שהמטבע עבר מהתוקף לגנרל
        CHECK(other->getCoins() < initial_attacker);   // לתוקף נגרע מטבע
        CHECK(general->getCoins() > initial_general);  // לגנרל נוסף מטבע
    }
}
