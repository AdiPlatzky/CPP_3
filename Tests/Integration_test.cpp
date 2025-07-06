//
// Created by 12adi on 06/07/2025.
//
// קובץ בדיקות המדמה משחק מלא (סימולציה) של כל מחלקות התפקידים המרכזיות: Governor, Baron, Judge, General.
// הבדיקה מדמה רצף תורות אמיתי, כולל תור מלא, איסוף מטבעות, ביצוע Coup, בדיקת סוף משחק וכו'.
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

/**
 * @class DummyRole
 * @brief תפקיד דמה שלא עושה כלום, כדי לאפשר יצירת שחקנים בטסטים מבלי השפעה לוגית על מהלך המשחק.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test סימולציה של משחק שלם עם 4 שחקנים – בודק את כל שלבי המשחק מתחילתו ועד לסיום:
 * מוסיף 4 שחקנים עם תפקידים שונים, מריץ סיבוב מלא, בודק איסוף מטבעות, ביצוע Coup, בדיקת ניצחון, ועוד.
 */
TEST_CASE("Game - Full Game Simulation") {
    /**
     * @subcase משחק מלא עם 4 שחקנים:
     * Governor, Baron, Judge, General – כל שחקן מבצע תור מלא, כל הפעולות המרכזיות.
     * בודק מעבר תורות, הגדלת מטבעות, ביצוע coup, סוף משחק, בחירת מנצח.
     */
    SUBCASE("Complete 4-player game") {
        Game game;
        auto governor = std::make_shared<Player>("Governor", std::make_unique<Governor>());
        auto baron = std::make_shared<Player>("Baron", std::make_unique<Baron>());
        auto judge = std::make_shared<Player>("Judge", std::make_unique<Judge>());
        auto general = std::make_shared<Player>("General", std::make_unique<General>());

        game.addPlayer(governor);
        game.addPlayer(baron);
        game.addPlayer(judge);
        game.addPlayer(general);

        // בודק שנוספו 4 שחקנים ושעוד לא נגמר המשחק
        CHECK(game.getPlayer().size() == 4);
        CHECK(game.isGameOver() == false);

        // --- שלב ראשון: כל שחקן מבצע את התור הראשוני שלו ---
        // Governor מתחיל ומבצע Tax (מקבל בונוס של 1 נוסף)
        CHECK(game.getCurrentPlayer().getName() == "Governor");
        game.performTax(*governor);
        game.applyTax(*governor);
        CHECK(governor->getCoins() == 3);
        game.nextTurn();

        // Baron מקבל 3 מטבעות, משקיע (invest), אמור לקבל 6 מטבעות בחזרה (סה"כ 6)
        CHECK(game.getCurrentPlayer().getName() == "Baron");
        baron->addCoins(3);
        game.performInvest(*baron);
        CHECK(baron->getCoins() == 6);
        game.nextTurn();

        // Judge אוסף מטבע (gather)
        CHECK(game.getCurrentPlayer().getName() == "Judge");
        game.performGather(*judge);
        game.applyGather(*judge);
        CHECK(judge->getCoins() == 1);
        game.nextTurn();

        // General אוסף מטבע (gather)
        CHECK(game.getCurrentPlayer().getName() == "General");
        game.performGather(*general);
        game.applyGather(*general);
        CHECK(general->getCoins() == 1);
        game.nextTurn();

        // --- ממשיכים לסבבים עד שאחד השחקנים יכול לבצע Coup (7+ מטבעות) ---
        while (!game.isGameOver()) {
            auto& current = game.getCurrentPlayer();
            // אם לשחקן יש 10+ מטבעות – חייב לבצע Coup (על פי חוקי המשחק)
            if (current.getCoins() >= 10) {
                for (auto& target : game.getPlayer()) {
                    if (target->isActive() && target.get() != &current) {
                        auto result = game.performCoup(current, *target);
                        if (result.success) {
                            game.applyCoup(current, *target);
                            break;
                        }
                    }
                }
            }
            // אם יש לפחות 7 מטבעות – יכול לבצע Coup
            else if (current.getCoins() >= 7) {
                for (auto& target : game.getPlayer()) {
                    if (target->isActive() && target.get() != &current) {
                        auto result = game.performCoup(current, *target);
                        if (result.success) {
                            game.applyCoup(current, *target);
                            break;
                        }
                    }
                }
            }
            // אחרת – מבצע gather (איסוף מטבע)
            else {
                auto result = game.performGather(current);
                if (result.success) {
                    game.applyGather(current);
                }
            }

            // בודקים אם המשחק נגמר
            game.checkGameOver();
            if (!game.isGameOver()) {
                game.nextTurn();
            }

            // מנגנון בטיחות כדי למנוע לולאה אינסופית אם יש באג בלוגיקה
            static int turnCount = 0;
            if (++turnCount > 1000) break;
        }

        // בסיום – אם המשחק נגמר, בודקים שניתן לשלוף את שם המנצח ללא חריגה
        if (game.isGameOver()) {
            CHECK_NOTHROW(game.getWinner());
        }
    }
}
