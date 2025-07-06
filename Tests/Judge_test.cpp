//
// Created by 12adi on 19/06/2025.
//
// קובץ בדיקות לוגיקה של תפקיד ה־Judge (שופט) במשחק.
// הבדיקות כוללות בדיקת יכולות החסימה, חסימת תור בונוס, והפעלת קנס בפעולת סנקציה.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Judge.h"
#include <memory>

using namespace std;

/**
 * @class DummyRole
 * @brief תפקיד דמה לצורך בדיקות, ללא השפעה על המשחק.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test בדיקות עבור תפקיד ה־Judge (שופט).
 * נבדקות כל היכולות: חסימת שוחד, ביטול תור בונוס, קנס בפעולת סנקציה.
 */
TEST_CASE("Judge Role Tests") {
    Game game;
    auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(judge);
    game.addPlayer(other);

    /**
     * @subcase בדיקת יכולת החסימה של השופט
     * בודק שה־Judge יכול לחסום פעולה מסוג bribe (שוחד), אבל לא פעולה מסוג tax (מס).
     */
    SUBCASE("Judge can block bribe") {
        CHECK(judge->canBlock("bribe") == true);
        CHECK(judge->canBlock("tax") == false);
    }

    /**
     * @subcase בדיקת חסימת תור בונוס בשוחד
     * בודק שאם שחקן אחר מקבל תור בונוס (בעקבות bribe), השופט מבטל לו את הבונוס מיד כשה־onBribe מופעל.
     */
    SUBCASE("Judge blocks bribe action") {
        other->addCoins(10);
        game.nextTurn(); // העברה לשחקן השני
        other->setBonusAction(true); // סימון שיש בונוס
        // Judge מבצע את הפעולה שמבטלת את הבונוס
        judge->getRole()->onBribe(*other, *other, game);
        CHECK(other->hasBonusAction() == false);
    }

    /**
     * @subcase בדיקת קנס בפעולת סנקציה (אם יש מספיק מטבעות)
     * מוודא שהשחקן התוקף משלם קנס כאשר Judge הוא המטרה בפעולת סנקציה, ורק אם יש לו מספיק מטבעות.
     *
     * בתהליך:
     * - מוסיפים למשתמש השני 10 מטבעות (מספיק לשלם קנס).
     * - מעבירים לו את התור.
     * - מפעילים את סנקציה (performSanction) כלפי ה־Judge.
     * - מוודאים שהתוצאה נכונה (תלוי בלוגיקת הסנקציה – אם היא מצליחה או לא, ולמה).
     * - סופרים את מספר המטבעות של השחקן לפני ואחרי קנס, ומוודאים שמספר המטבעות ירד.
     */
    SUBCASE("Judge penalty on sanction - מספיק מטבעות") {
        other->addCoins(10); // ודאו שיש מספיק לשלם קנס
        game.nextTurn();
        auto result = game.performSanction(*other, *judge);
        CHECK(result.success == false); // ייתכן שהמשחק שלך מונע תקיפה על Judge (תלוי בלוגיקה)
        int initial_coins = other->getCoins();
        judge->getRole()->onSanction(*other, *judge, game);
        CHECK(other->getCoins() < initial_coins); // וידוא שהשחקן שילם קנס
    }
}
