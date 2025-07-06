//
// Created by 12adi on 19/06/2025.
//
// קובץ בדיקות עבור תפקיד ה־Merchant (סוחר) במשחק.
// מטרת הבדיקות לוודא שכל יכולות התפקיד פועלות כמצופה:
// קבלת בונוס בתחילת תור, הגנה על Arrest, וטיפול במצבים גבוליים.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Merchant.h"
#include <memory>
using namespace std;

/**
 * @class DummyRole
 * @brief תפקיד דמה לצורך בדיקות ללא התנהגות ייחודית.
 */
class DummyRole : public Role {
public:
    std::string getName() const override {
        return "Dummy";
    }
};

/**
 * @test Merchant Role Tests
 * סדרת בדיקות המאמתות את התנהגות תפקיד Merchant במשחק.
 */
TEST_CASE("Merchant Role Tests") {
    Game game;
    auto merchant = std::make_shared<Player>("Merchant", std::make_unique<Merchant>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(merchant);
    game.addPlayer(other);

    /**
     * @subcase קבלת בונוס בתחילת תור
     * כאשר ל־Merchant יש 3 מטבעות או יותר, הוא אמור לקבל מטבע נוסף בתחילת תורו.
     */
    SUBCASE("Merchant gets bonus coin at turn start") {
        merchant->addCoins(3); // מוסיפים בדיוק 3 מטבעות
        int initial_coins = merchant->getCoins();
        merchant->getRole()->onTurnStart(*merchant, game); // הפעלת תחילת תור
        CHECK(merchant->getCoins() == initial_coins + 1); // צפוי לקבל בונוס של 1
    }

    /**
     * @subcase אין בונוס אם יש פחות מ־3 מטבעות
     * במצב זה הסוחר לא יקבל מטבע בתחילת תורו.
     */
    SUBCASE("Merchant no bonus with less than 3 coins") {
        merchant->addCoins(2);
        int initial_coins = merchant->getCoins();
        merchant->getRole()->onTurnStart(*merchant, game);
        CHECK(merchant->getCoins() == initial_coins); // אין שינוי במספר המטבעות
    }

    /**
     * @subcase הגנה על Arrest (מעצר)
     * כאשר ל־Merchant יש מספיק מטבעות, הוא יכול להגן על עצמו בפעולת Arrest על ידי תשלום 2 מטבעות.
     * נבדק שהמספר אכן יורד.
     */
    SUBCASE("Merchant defense against arrest") {
        merchant->addCoins(5);
        other->addCoins(3);
        game.nextTurn(); // מעבירים תור לשחקן השני

        int initial_merchant = merchant->getCoins();
        merchant->getRole()->onArrest(*merchant, *other, game);

        if (initial_merchant >= 2) {
            CHECK(merchant->getCoins() == initial_merchant - 2); // נגרעו שני מטבעות
        }
    }

    /**
     * @subcase התמודדות עם חוסר במטבעות בהגנה על Arrest
     * בודק מה קורה אם ל־Merchant אין מספיק מטבעות (פחות מ־2) – יש לצפות להתמודדות תקינה ללא זריקת חריגות.
     */
    SUBCASE("Merchant insufficient coins for arrest defense") {
        merchant->addCoins(1); // רק מטבע אחד
        other->addCoins(3);
        game.nextTurn();

        // אמור לעבור בלי לזרוק חריגות (CHECK_NOTHROW)
        CHECK_NOTHROW(merchant->getRole()->onArrest(*merchant, *other, game));
    }
}
