//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Governor.h"
#include <memory>

using namespace std;

/**
 * @class DummyRole
 * @brief מחלקת תפקיד דמה – תפקיד שלא עושה כלום. משמש לכתיבת טסטים בקלות בלי השפעה על הלוגיקה.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test בדיקות ליכולת של דמות הניצב (Governor) במשחק:
 * כולל בונוס במס על עצמו, חסימת tax לשחקנים אחרים, ויכולת החסימה.
 */
TEST_CASE("Governor Role Tests") {
    Game game;
    auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(governor);
    game.addPlayer(other);

    /**
     * @subcase בדיקה: כאשר Governor מבצע Tax הוא מקבל 3 מטבעות (2 רגיל + 1 בונוס)
     */
    SUBCASE("Governor gets extra coin from tax") {
        auto result = game.performTax(*governor); // Governor מנסה לבצע Tax
        CHECK(result.success == true);            // מצפים שהפעולה מצליחה
        game.applyTax(*governor);                 // הפעלת הפעולה בפועל
        CHECK(governor->getCoins() == 3);         // בודקים שהתקבלו 3 מטבעות (בונוס מיוחד)
    }

    /**
     * @subcase בדיקה: Governor מסוגל לחסום פעולה מסוג Tax, אבל לא מסוג Gather.
     */
    SUBCASE("Governor can block tax") {
        CHECK(governor->canBlock("tax") == true);      // בודקים שהוא כן חוסם tax
        CHECK(governor->canBlock("gather") == false);  // ולא חוסם gather
    }

    /**
     * @subcase בדיקה: כאשר שחקן אחר (other) מבצע Tax, Governor חוסם לו את הפעולה, כלומר הוא חסום לתור הבא.
     */
    SUBCASE("Governor blocks other player's tax") {
        game.nextTurn();                        // מעבירים תור ל-other
        auto result = game.performTax(*other);  // other מנסה לבצע Tax
        CHECK(result.success == true);          // מצפים שהפעולה מאושרת עקרונית (לפני חסימות)
        game.applyTax(*other);                  // ביצוע בפועל
        CHECK(other->isBlocked("tax") == true); // בודקים ש-other נחסם לתור הבא ב-tax
    }
}
