#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Judge.h"
#include <memory>
using namespace std;

/**
 * @class DummyRole
 * @brief תפקיד דמה לבדיקות (ללא התנהגות מיוחדת), כדי לא לערב לוגיקת תפקידים אמיתיים.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test בדיקת הוספת שחקנים למשחק ושליפת שחקנים פעילים.
 */
TEST_CASE("Game - addPlayer and getActivePlayers") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);

    auto active = game.getActivePlayers();
    CHECK(active.size() == 2);       // אמור להחזיר 2 שחקנים פעילים
    CHECK(active[0] == "Alice");     // בדיקה לפי סדר
    CHECK(active[1] == "Bob");
}

/**
 * @test בדיקת שחקן נוכחי לפני ואחרי מעבר תור (nextTurn).
 */
TEST_CASE("Game - getCurrentPlayer before and after nextTurn") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);

    CHECK(game.getCurrentPlayer().getName() == "Alice");   // מתחילים מאליס
    game.nextTurn();
    CHECK(game.getCurrentPlayer().getName() == "Bob");     // תור של בוב
    game.nextTurn();
    CHECK(game.getCurrentPlayer().getName() == "Alice");   // חוזר לאליס (מעגלי)
}

/**
 * @test בדיקת משחק ריק – לא ניתן לשלוף שחקן נוכחי, אין שחקנים פעילים.
 */
TEST_CASE("Game - no players added") {
    Game game;
    CHECK_THROWS(game.getCurrentPlayer());          // אין שחקנים כלל
    CHECK(game.getActivePlayers().empty());         // רשימת פעילים ריקה
}

/**
 * @test בדיקת הוספת אותו שחקן פעמיים – מוודא שגם במקרה כזה יש 2 רשומות.
 */
TEST_CASE("Game - add same player twice") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p1);  // אותו אובייקט פעמיים
    CHECK(game.getActivePlayers().size() == 2);
}

/**
 * @test בדיקת שינוי ערך קופה ישירות והוספת ערכים.
 */
TEST_CASE("Game - manipulate coin pool directly") {
    Game game;
    int& pool = game.getCoinPool();
    pool = -100;                     // שינוי ישיר – ערך שלילי
    CHECK(game.getCoinPool() == -100);

    game.addToCoinPool(150);         // הוספת 150
    CHECK(game.getCoinPool() == 50);
}

/**
 * @test ניסיון לבצע gather עם שחקן שהוא לא בתורו הנוכחי – אמור לזרוק חריגה.
 */
TEST_CASE("Game - performGather by non-current player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alice", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Bob", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);

    CHECK_THROWS(game.performGather(*p2)); // בוב – לא תורו
}

/**
 * @test ניסיון לבצע tax על שחקן שמוגדר כ־inactive (מודח מהמשחק).
 */
TEST_CASE("Game - performTax by dead player") {
    Game game;
    auto p1 = std::make_shared<Player>("Alive", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    p1->deactivate();
    CHECK_THROWS(game.performTax(*p1));
}

/**
 * @test ניסיון לבצע bribe פעמיים באותו תור – אמור להיזרק חריג בפעם השנייה.
 */
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

/**
 * @test ניסיון לעצור (arrest) שחקן שאינו פעיל – אמור להיזרק חריג.
 */
TEST_CASE("Game - arrest a dead player") {
    Game game;
    auto p1 = std::make_shared<Player>("Cop", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Dead", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    game.addPlayer(p2);
    p2->deactivate();
    CHECK_THROWS(game.performArrest(*p1, *p2));
}

/**
 * @test בדיקות לקצוות של סנקציה והפיכה: ודא שסנקציה לא מדיחה, והפיכה כן.
 */
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
    CHECK(coupResult.success);         // הפיכה חוקית
    CHECK(coupResult.requiresBlocking); // אפשר חסימה
    CHECK(p2->isActive());             // עדיין פעיל
    game.applyCoup(*p1, *p2);          // רק כאן מפעילים הדחה
    CHECK_FALSE(p2->isActive());       // עכשיו מודח
}

/**
 * @test ניסיון לבצע invest עם שחקן בתורו – ודא שאין חריגות.
 */
TEST_CASE("Game - invest with current player") {
    Game game;
    auto p1 = std::make_shared<Player>("Investor", std::make_unique<DummyRole>());
    game.addPlayer(p1);
    CHECK_NOTHROW(game.performInvest(*p1));
}

/**
 * @test בדיקת מנגנון סיום המשחק וזיהוי המנצח.
 */
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

/**
 * @test בדיקות בניה והגדרות בסיסיות – הוספת שחקנים, שליפת השחקן הנוכחי, בדיקת סדר.
 */
TEST_CASE("Game - Basic Construction and Setup") {
    Game game;

    SUBCASE("Add single player") {
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        CHECK(game.getPlayer().size() == 1);
        CHECK(game.getActivePlayers().size() == 1);
        CHECK(game.getCurrentPlayer().getName() == "Player1");
    }

    SUBCASE("Add multiple players") {
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
        auto p3 = std::make_shared<Player>("Player3", std::make_unique<DummyRole>());

        game.addPlayer(p1);
        game.addPlayer(p2);
        game.addPlayer(p3);

        CHECK(game.getPlayer().size() == 3);
        CHECK(game.getActivePlayers().size() == 3);
        CHECK(game.getCurrentPlayer().getName() == "Player1");
    }

    SUBCASE("Add same player multiple times") {
        auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
        game.addPlayer(p1);
        game.addPlayer(p1);
        CHECK(game.getPlayer().size() == 2); // אותו שחקן פעמיים
    }
}

/**
 * @test בדיקת ניהול תורות – תור רגיל, דילוג על שחקנים מודחים, מעגליות תורים.
 */
TEST_CASE("Game - Turn Management") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());
    auto p3 = std::make_shared<Player>("Player3", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);
    game.addPlayer(p3);

    SUBCASE("Turn rotation") {
        CHECK(game.getCurrentPlayer().getName() == "Player1");
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player2");
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player3");
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player1"); // חוזר להתחלה
    }

    SUBCASE("Skip inactive players") {
        CHECK(game.getCurrentPlayer().getName() == "Player1");
        p2->deactivate();
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player3"); // דילוג על 2
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player1");
    }

    SUBCASE("All players inactive except one") {
        p2->deactivate();
        p3->deactivate();
        CHECK(game.getCurrentPlayer().getName() == "Player1");
        game.nextTurn();
        CHECK(game.getCurrentPlayer().getName() == "Player1"); // רק שחקן אחד פעיל
    }
}

/**
 * @test בדיקות לקופה המרכזית – ערך התחלתי גדול, הוספה/החסרה/אפס.
 */
TEST_CASE("Game - Coin Pool Management") {
    Game game;

    SUBCASE("Initial coin pool") {
        CHECK(game.getCoinPool() > 1000000); // ערך גדול מההתחלה
    }

    SUBCASE("Add to coin pool") {
        int initial = game.getCoinPool();
        game.addToCoinPool(100);
        CHECK(game.getCoinPool() == initial + 100);
    }

    SUBCASE("Add negative to coin pool") {
        int initial = game.getCoinPool();
        game.addToCoinPool(-50);
        CHECK(game.getCoinPool() == initial - 50);
    }

    SUBCASE("Add zero to coin pool") {
        int initial = game.getCoinPool();
        game.addToCoinPool(0);
        CHECK(game.getCoinPool() == initial);
    }
}

/**
 * @test בדיקת תנאי ניצחון – משחק לא הסתיים, משחק הסתיים, ללא מנצח.
 */
TEST_CASE("Game - Win Conditions") {
    Game game;
    auto p1 = std::make_shared<Player>("Player1", std::make_unique<DummyRole>());
    auto p2 = std::make_shared<Player>("Player2", std::make_unique<DummyRole>());

    game.addPlayer(p1);
    game.addPlayer(p2);

    SUBCASE("Game not over with multiple active players") {
        CHECK(game.isGameOver() == false);
        CHECK_THROWS(game.getWinner());
    }

    SUBCASE("Game over with one player left") {
        p2->deactivate();
        game.checkGameOver();
        CHECK(game.isGameOver() == true);
        CHECK(game.getWinner() == "Player1");
    }

    SUBCASE("Game over with no players") {
        p1->deactivate();
        p2->deactivate();
        game.checkGameOver();
        CHECK(game.isGameOver() == true);
        CHECK_THROWS(game.getWinner()); // אין מנצח בכלל
    }
}
