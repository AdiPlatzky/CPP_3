//
// Created by 12adi on 06/07/2025.
//
/**
 * @file Game_test.cpp
 * @brief קובץ בדיקות יחידה (unit tests) עבור המחלקות המרכזיות: Game, Player, Role.
 * מבוסס על ספריית Doctest.
 *
 * בקובץ זה נבדקות פעולות המשחק המרכזיות, באמצעות יצירת שחקנים ב־DummyRole
 * (רול בסיסי ללא לוגיקה מיוחדת) ובדיקת כללי המשחק, קצוות וחריגות.
 */

#include <stdexcept>
#include "doctest.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include "../Roles/Judge.h"
#include "../Role.h"
#include "../Game.h"

/**
 * @class DummyRole
 * @brief רול דמה – תפקיד בסיסי למשחק, ללא לוגיקת השפעה.
 * משמש להבטחת אי־תלות בבדיקות תפקידים.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test Game - Gather Action
 * בדיקות עבור הפעולה 'gather' (איסוף מטבע).
 */
TEST_CASE("Game - Gather Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    /**
     * @subcase בדיקה: ביצוע תקין של gather
     */
    SUBCASE("Valid gather action") {
        auto result = game.performGather(*p1);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyGather(*p1);
        CHECK(p1->getCoins() == 1);
    }

    /**
     * @subcase בדיקה: ניסיון איסוף ע"י שחקן שלא בתורו
     */
    SUBCASE("Gather by wrong player") {
        CHECK_THROWS(game.performGather(*p2)); // Not current player's turn
    }

    /**
     * @subcase בדיקה: ניסיון איסוף ע"י שחקן מודח (לא פעיל)
     */
    SUBCASE("Gather by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performGather(*p1));
    }

    /**
     * @subcase בדיקה: איסוף כאשר פעולה חסומה (blocked)
     */
    SUBCASE("Gather when blocked") {
        p1->blockAction("gather", 1);
        auto result = game.performGather(*p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: שחקן עם 10 מטבעות חייב לבצע coup (לא אמור לאסוף)
     */
    SUBCASE("Gather with 10+ coins") {
        p1->addCoins(10);
        auto result = game.performGather(*p1);
        CHECK(result.success == false); // Must coup instead
    }

    /**
     * @subcase בדיקה: אין מספיק מטבעות בקופה – אמור להיזרק חריג
     */
    SUBCASE("Gather with insufficient coin pool") {
        game.getCoinPool() = 0;
        auto result = game.performGather(*p1);
        if (result.success) {
            CHECK_THROWS(game.applyGather(*p1));
        }
    }
}

/**
 * @test Game - Tax Action
 * בדיקות עבור הפעולה 'tax' (גביית מיסים).
 */
TEST_CASE("Game - Tax Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    /**
     * @subcase בדיקה: פעולה תקינה של tax
     */
    SUBCASE("Valid tax action") {
        auto result = game.performTax(*p1);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyTax(*p1);
        CHECK(p1->getCoins() == 2);
    }

    /**
     * @subcase בדיקה: שחקן שאינו בתורו
     */
    SUBCASE("Tax by wrong player") {
        CHECK_THROWS(game.performTax(*p2));
    }

    /**
     * @subcase בדיקה: שחקן לא פעיל
     */
    SUBCASE("Tax by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performTax(*p1));
    }

    /**
     * @subcase בדיקה: פעולה חסומה
     */
    SUBCASE("Tax when blocked") {
        p1->blockAction("tax", 1);
        auto result = game.performTax(*p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: ניסיון גביית מס כאשר יש 10+ מטבעות
     */
    SUBCASE("Tax with 10+ coins") {
        p1->addCoins(10);
        auto result = game.performTax(*p1);
        CHECK(result.success == false);
    }
}

/**
 * @test Game - Bribe Action
 * בדיקות עבור פעולה 'bribe' (שוחד).
 */
TEST_CASE("Game - Bribe Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    game.addPlayer(p1);

    /**
     * @subcase בדיקה: שוחד תקין
     */
    SUBCASE("Valid bribe action") {
        p1->addCoins(5);
        auto result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 1); // 5 - 4 = 1
        CHECK(p1->hasUsedBribeThisTurn() == true);
    }

    /**
     * @subcase בדיקה: שוחד עם פחות מדי מטבעות
     */
    SUBCASE("Bribe with insufficient coins") {
        p1->addCoins(3);
        auto result = game.performBribe(*p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: שחקן חסום מהשוחד
     */
    SUBCASE("Bribe when blocked") {
        p1->addCoins(5);
        p1->blockAction("bribe", 1);
        auto result = game.performBribe(*p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: שוחד בדיוק עם 4 מטבעות
     */
    SUBCASE("Bribe with exactly 4 coins") {
        p1->addCoins(4);
        auto result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
    }
}

/**
 * @test Game - Arrest Action
 * בדיקות עבור פעולה 'arrest' (מעצר).
 */
TEST_CASE("Game - Arrest Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    /**
     * @subcase בדיקה: ניסיון לעצור את עצמך – אמור להיכשל
     */
    SUBCASE("Arrest self") {
        auto result = game.performArrest(*p1, *p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: ניסיון לעצור שחקן לא פעיל
     */
    SUBCASE("Arrest inactive player") {
        p2->deactivate();
        CHECK_THROWS(game.performArrest(*p1, *p2));
    }

    /**
     * @subcase בדיקה: ניסיון לעצור שחקן בלי מטבעות
     */
    SUBCASE("Arrest player with no coins") {
        auto result = game.performArrest(*p1, *p2);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: ניסיון מעצר על־ידי שחקן לא פעיל
     */
    SUBCASE("Arrest by inactive player") {
        p1->deactivate();
        CHECK_THROWS(game.performArrest(*p1, *p2));
    }

    /**
     * @subcase בדיקה: ניסיון מעצר עם 10+ מטבעות (חייב להפוך ל-coup)
     */
    SUBCASE("Arrest with 10+ coins") {
        p1->addCoins(10);
        p2->addCoins(5);
        auto result = game.performArrest(*p1, *p2);
        CHECK(result.success == false);
    }
}

/**
 * @test Game - Sanction Action
 * בדיקות לפעולת 'sanction' (סנקציה).
 */
TEST_CASE("Game - Sanction Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    /**
     * @subcase בדיקה: פעולה תקינה של סנקציה
     */
    SUBCASE("Valid sanction action") {
        p1->addCoins(5);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 2); // 5 - 3 = 2
        CHECK(p2->isBlocked("gather") == true);
        CHECK(p2->isBlocked("tax") == true);
    }

    /**
     * @subcase בדיקה: סנקציה עם פחות מדי מטבעות
     */
    SUBCASE("Sanction with insufficient coins") {
        p1->addCoins(2);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: סנקציה על עצמך
     */
    SUBCASE("Sanction self") {
        p1->addCoins(5);
        auto result = game.performSanction(*p1, *p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: סנקציה על שחקן לא פעיל
     */
    SUBCASE("Sanction inactive player") {
        p1->addCoins(5);
        p2->deactivate();
        CHECK_THROWS(game.performSanction(*p1, *p2));
    }

    /**
     * @subcase בדיקה: סנקציה בדיוק עם 3 מטבעות
     */
    SUBCASE("Sanction with exactly 3 coins") {
        p1->addCoins(3);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
    }
}

/**
 * @test Game - Coup Action
 * בדיקות עבור פעולה 'coup' (הפיכה).
 */
TEST_CASE("Game - Coup Action") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    /**
     * @subcase בדיקה: הפיכה תקינה
     */
    SUBCASE("Valid coup action") {
        p1->addCoins(10);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        CHECK(result.requiresBlocking == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 3); // 10 - 7 = 3
        CHECK(p2->isActive() == false);
    }

    /**
     * @subcase בדיקה: הפיכה עם פחות מדי מטבעות
     */
    SUBCASE("Coup with insufficient coins") {
        p1->addCoins(6);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: ניסיון הפיכה על עצמך
     */
    SUBCASE("Coup self") {
        p1->addCoins(10);
        auto result = game.performCoup(*p1, *p1);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: ניסיון הפיכה על שחקן לא פעיל
     */
    SUBCASE("Coup inactive player") {
        p1->addCoins(10);
        p2->deactivate();
        CHECK_THROWS(game.performCoup(*p1, *p2));
    }

    /**
     * @subcase בדיקה: שחקן חסום מ־coup
     */
    SUBCASE("Coup when blocked") {
        p1->addCoins(10);
        p1->blockAction("coup", 1);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == false);
    }

    /**
     * @subcase בדיקה: בדיוק עם 7 מטבעות
     */
    SUBCASE("Coup with exactly 7 coins") {
        p1->addCoins(7);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 0);
        CHECK(p2->isActive() == false);
    }
}
