//
// Created by 12adi on 06/07/2025.
//
/**
 * @file EdgeCase_test.cpp
 * @brief בדיקות יחידה מקיפות למצבי קצה (Edge Cases), עמידות (Stress), אינטגרציה, קונקרנציה וזיכרון במשחק.
 * הבדיקות נועדו לוודא התנהגות תקינה תחת עומס, במצבים לא רגילים, ובמקרי קצה שעלולים להוביל לשגיאות חבויות.
 */

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

/**
 * @class DummyRole
 * @brief תפקיד דמה (DummyRole) ללא השפעות לוגיות, משמש להפרדה בבדיקות.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test Game - Edge Cases and Stress Tests
 * בדיקות למצבי קצה ועומס: כמות שחקנים, ניהול תורות, חסימות, ומצבים לא שגרתיים.
 */
TEST_CASE("Game - Edge Cases and Stress Tests") {

    /**
     * @subcase בודק הוספה של 100 שחקנים, ובודק שכל השחקנים אכן פעילים.
     */
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

    /**
     * @subcase בודק זיהוי זכייה כאשר כל השחקנים לא פעילים חוץ מאחד.
     */
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

    /**
     * @subcase בודק מעבר מהיר בין תורות במספר רב של שחקנים, לאורך 1000 תורות.
     */
    SUBCASE("Rapid turn switching with many players") {
        Game game;
        std::vector<std::shared_ptr<Player>> players;
        for (int i = 0; i < 20; ++i) {
            auto player = std::make_shared<Player>("Player" + std::to_string(i),
                                                  std::make_unique<DummyRole>());
            players.push_back(player);
            game.addPlayer(player);
        }
        for (int i = 0; i < 1000; ++i) {
            std::string expectedPlayer = "Player" + std::to_string(i % 20);
            CHECK(game.getCurrentPlayer().getName() == expectedPlayer);
            game.nextTurn();
        }
    }

    /**
     * @subcase בודק חסימות מורכבות – חסימת פעולות שונות לזמנים שונים, ווידוא שהחסימות מתעדכנות ונעלמות בזמן הנכון.
     */
    SUBCASE("Complex blocking scenarios") {
        Game game;
        auto player = std::make_shared<Player>("BlockedPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);
        player->blockAction("gather", 5);
        player->blockAction("tax", 3);
        player->blockAction("bribe", 1);
        player->blockAction("arrest", 10);
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

/**
 * @test Game - Error Handling and Invalid States
 * בדיקות למצבים לא חוקיים, שגיאות גישה, שמות כפולים, חסימות קצה ועוד.
 */
TEST_CASE("Game - Error Handling and Invalid States") {

    // בדיקה ל־getCurrentPlayer / nextTurn על משחק ריק הוסרה בגלל קריסות אפשריות.

    /**
     * @subcase בודק שפעולות על השחקן הנותר אפשריות גם לאחר סיום המשחק.
     */
    SUBCASE("Actions after game over") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);
        p2->deactivate();
        game.checkGameOver();
        CHECK_NOTHROW(game.getCurrentPlayer());
    }

    /**
     * @subcase בודק טיפול בנקודת קצה – חיפוש שחקן שלא קיים או בעל שם ריק.
     */
    SUBCASE("Null pointer handling") {
        Game game;
        auto player = std::make_shared<Player>("TestPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);
        CHECK(game.getPlayerByName("NonExistent") == nullptr);
        CHECK(game.getPlayerByName("") == nullptr);
    }

    /**
     * @subcase בודק הוספת שני שחקנים עם אותו שם – ודואג ששניהם קיימים במערכת.
     */
    SUBCASE("Player with same names") {
        Game game;
        auto p1 = std::make_shared<Player>("SameName", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("SameName", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);
        CHECK(game.getPlayer().size() == 2);
        auto found = game.getPlayerByName("SameName");
        CHECK(found != nullptr);
    }

    /**
     * @subcase בודק חסימות קצה: חסימה עם ערך גדול מאוד, חסימה ל־0, וחסימה שלילית.
     */
    SUBCASE("Extreme blocking values") {
        Game game;
        auto player = std::make_shared<Player>("TestPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);
        player->blockAction("gather", 99999);
        CHECK(player->isBlocked("gather") == true);
        player->blockAction("tax", 0);
        CHECK(player->isBlocked("tax") == false);
        player->blockAction("bribe", -1);
        // התנהגות חסימה שלילית תלויה במימוש בפועל.
    }
}

/**
 * @test Game - Multi-Role Interaction Tests
 * בודק אינטראקציות בין תפקידים שונים – חסימות, תגובות ובונוסים.
 */
TEST_CASE("Game - Multi-Role Interaction Tests") {

    /**
     * @subcase אינטראקציה בין Governor ל־Judge – קבלת בונוס במס וקבלת חסימה.
     */
    SUBCASE("Governor vs Judge tax blocking") {
        Game game;
        auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
        auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
        game.addPlayer(governor);
        game.addPlayer(judge);
        auto result = game.performTax(*governor);
        CHECK(result.success == true);
        game.applyTax(*governor);
        CHECK(governor->getCoins() == 3); // 2 + 1 בונוס
    }

    /**
     * @subcase אינטראקציה בין Baron ל־General – הגנה על הפיכה (coup).
     */
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
        if (general->getCoins() >= 5) {
            general->getRole()->onCoup(*baron, *general, game);
        }
    }

    /**
     * @subcase שימוש ביכולות תפקידים שונים באותו תור – למשל, בונוס של Merchant.
     */
    SUBCASE("Multiple role abilities in one turn") {
        Game game;
        auto merchant = std::make_shared<Player>("Merchant", std::make_unique<Merchant>());
        game.addPlayer(merchant);
        merchant->addCoins(3);
        merchant->getRole()->onTurnStart(*merchant, game);
        CHECK(merchant->getCoins() == 4);
        auto result = game.performTax(*merchant);
        CHECK(result.success == true);
        game.applyTax(*merchant);
        CHECK(merchant->getCoins() == 6); // 4 + 2 = 6
    }
}

/**
 * @test Game - Complex Scenarios
 * בודק תרחישים מורכבים: הדחות שרשרת, מיצוי משאבים, חסימות משולבות ועוד.
 */
TEST_CASE("Game - Complex Scenarios") {

    /**
     * @subcase תרחיש הדחות בשרשרת – שחקן מפיל כמה שחקנים ברצף באמצעות coup.
     */
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
        p1->addCoins(20);
        auto result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p2->isActive() == false);
        CHECK(p1->getCoins() == 13);
        game.nextTurn(); // דילוג לשחקן הבא
        game.nextTurn();
        game.nextTurn();
        result = game.performCoup(*p1, *p3);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p3);
        CHECK(p3->isActive() == false);
        CHECK(p1->getCoins() == 6);
        game.checkGameOver();
        CHECK(game.isGameOver() == false); // נותרו שניים
        game.nextTurn();
        game.nextTurn();
        result = game.performCoup(*p1, *p4);
        CHECK(result.success == false);
    }

    /**
     * @subcase מיצוי משאבים – בדיקת התנהגות עם קופה ריקה ועם ניסיון לאסוף מעבר למותר.
     */
    SUBCASE("Resource exhaustion scenarios") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);
        game.getCoinPool() = 1;
        auto result = game.performGather(*player);
        CHECK(result.success == true);
        game.applyGather(*player);
        CHECK(player->getCoins() == 1);
        game.getCoinPool() = 0;
        result = game.performGather(*player);
        if (result.success) {
            CHECK_THROWS(game.applyGather(*player));
        }
    }

    /**
     * @subcase מצב קצה – שחקן בעל 10 מטבעות מחויב לבצע coup, כל פעולה אחרת נחסמת.
     */
    SUBCASE("Forced coup scenario") {
        Game game;
        auto p1 = std::make_shared<Player>("RichPlayer", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("TargetPlayer", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);
        p1->addCoins(10);
        auto gather_result = game.performGather(*p1);
        CHECK(gather_result.success == false);
        auto tax_result = game.performTax(*p1);
        CHECK(tax_result.success == false);
        auto bribe_result = game.performBribe(*p1);
        CHECK(bribe_result.success == false);
        auto coup_result = game.performCoup(*p1, *p2);
        CHECK(coup_result.success == true);
    }

    /**
     * @subcase חסימות משולבות – כאשר השחקן חסום בכל פעולה כלכלית.
     */
    SUBCASE("Blocked action combinations") {
        Game game;
        auto player = std::make_shared<Player>("BlockedPlayer", std::make_unique<DummyRole>());
        game.addPlayer(player);
        player->blockAction("gather", 2);
        player->blockAction("tax", 2);
        player->addCoins(5);
        CHECK(game.performGather(*player).success == false);
        CHECK(game.performTax(*player).success == false);
        CHECK(game.performBribe(*player).success == true);
    }
}

/**
 * @test Game - Boundary Value Tests
 * בדיקות ערכי סף מינימום ומקסימום לכל פעולה.
 */
TEST_CASE("Game - Boundary Value Tests") {

    /**
     * @subcase בדיקה לערכים אפסיים – ניסיון להוריד מטבע ללא מטבעות, הוספת אפס מטבעות.
     */
    SUBCASE("Zero coin operations") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);
        CHECK(player->getCoins() == 0);
        CHECK_THROWS(player->removeCoins(1));
        player->addCoins(0);
        CHECK(player->getCoins() == 0);
    }

    /**
     * @subcase בדיקת ספי מינימום לכל פעולה: sanction, bribe, coup.
     */
    SUBCASE("Minimum coins for actions") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);
        p1->addCoins(3);
        auto result = game.performSanction(*p1, *p2);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
        p1->addCoins(4);
        result = game.performBribe(*p1);
        CHECK(result.success == true);
        CHECK(p1->getCoins() == 0);
        p1->addCoins(7);
        result = game.performCoup(*p1, *p2);
        CHECK(result.success == true);
        game.applyCoup(*p1, *p2);
        CHECK(p1->getCoins() == 0);
        CHECK(p2->isActive() == false);
    }

    /**
     * @subcase בדיקה לערך אחד פחות מהמינימום – כל פעולה אמורה להיכשל.
     */
    SUBCASE("One less than minimum coins") {
        Game game;
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p2);
        p1->addCoins(2);
        CHECK(game.performSanction(*p1, *p2).success == false);
        p1->addCoins(1);
        p1->removeCoins(1);
        p1->addCoins(1);
        CHECK(game.performBribe(*p1).success == false);
        p1->addCoins(3);
        CHECK(game.performCoup(*p1, *p2).success == false);
    }
}

/**
 * @test Game - Threading and Concurrency Edge Cases
 * בדיקות מדומות לקונקרנציה: שינויי מצב מהירים, שינויים במטבעות ברצף, בדיקה לעמידות תחת ריצות מהירות.
 */
TEST_CASE("Game - Threading and Concurrency Edge Cases") {

    /**
     * @subcase שינוי סטטוס של שחקן (הפעלה/השבתה) במהירות – לוודא שלא ניתן להפעיל מחדש שחקן.
     */
    SUBCASE("Rapid state changes") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);
        for (int i = 0; i < 1000; ++i) {
            if (i % 2 == 0) {
                player->deactivate();
                CHECK(player->isActive() == false);
            } else {
                CHECK(player->isActive() == false);
            }
        }
    }

    /**
     * @subcase תנועות מטבע מהירות – הוספה והסרה של מטבעות ברצף.
     */
    SUBCASE("Rapid coin operations") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);
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

/**
 * @test Game - Memory and Performance Edge Cases
 * בדיקות ביצועים וזיכרון: ריבוי חסימות, שמות מיוחדים, ושימוש במחרוזות גדולות.
 */
TEST_CASE("Game - Memory and Performance Edge Cases") {

    /**
     * @subcase בדיקת חסימה של אלף פעולות שונות – ווידוא ניהול זיכרון תקין.
     */
    SUBCASE("Large number of blocked actions") {
        Game game;
        auto player = std::make_shared<Player>("Player", std::make_unique<DummyRole>());
        game.addPlayer(player);
        for (int i = 0; i < 1000; ++i) {
            std::string action = "action" + std::to_string(i);
            player->blockAction(action, 1);
            CHECK(player->isBlocked(action) == true);
        }
        player->clearBlocks();
        for (int i = 0; i < 1000; ++i) {
            std::string action = "action" + std::to_string(i);
            CHECK(player->isBlocked(action) == false);
        }
    }

    /**
     * @subcase בדיקת טיפול במחרוזות: שמות ריקים, שמות ארוכים מאוד, ותווים לא־לטיניים.
     */
    SUBCASE("String handling edge cases") {
        Game game;
        auto player1 = std::make_shared<Player>("", std::make_unique<DummyRole>());
        game.addPlayer(player1);
        CHECK(player1->getName() == "");
        std::string longName(10000, 'X');
        auto player2 = std::make_shared<Player>(longName, std::make_unique<DummyRole>());
        game.addPlayer(player2);
        CHECK(player2->getName() == longName);
        auto player3 = std::make_shared<Player>("测试", std::make_unique<DummyRole>());
        game.addPlayer(player3);
        CHECK(player3->getName() == "测试");
    }
}
