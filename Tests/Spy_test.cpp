//
// Created by 12adi on 19/06/2025.
//

#include <iostream>
#include "doctest.h"
#include "../Game.h"
#include "../Player.h"
#include "../Roles/Spy.h"
#include <memory>

using namespace std;

/**
 * @brief תפקיד דמה (DummyRole) – משמש לבדיקה בלבד, לא משפיע על לוגיקת המשחק.
 */
class DummyRole : public Role {
public:
    std::string getName() const override { return "Dummy"; }
};

/**
 * @test בדיקות לתפקיד "מרגל" (Spy)
 */
TEST_CASE("Spy Role Tests") {
    Game game;
    auto spy = std::make_shared<Player>("Spy", std::make_unique<Spy>());
    auto other = std::make_shared<Player>("Other", std::make_unique<DummyRole>());
    game.addPlayer(spy);
    game.addPlayer(other);

    /**
     * @subcase בדיקת יכולת חסימה של המרגל – האם יכול לחסום arrest והאם לא יכול לחסום coup.
     */
    SUBCASE("Spy can block arrest") {
        CHECK(spy->canBlock("arrest") == true);
        CHECK(spy->canBlock("coup") == false);
    }

    /**
     * @subcase בדיקת שליפת מידע על כמות המטבעות – ודאו שהמרגל והאחרים מקבלים את הערך הנכון.
     */
    SUBCASE("Spy gets coin information") {
        spy->addCoins(3);
        other->addCoins(7);
        auto coinCounts = game.getPlayersCoinCounts();
        CHECK(coinCounts["Spy"] == 3);
        CHECK(coinCounts["Other"] == 7);
    }

    /**
     * @subcase בדיקת חסימת arrest – המרגל חוסם פעולה ורואה את כמות המטבעות של התוקף.
     * ודאו שהתוקף נחסם.
     */
    SUBCASE("Spy blocks arrest and sees attacker coins") {
        other->addCoins(5);
        game.nextTurn(); // מעבר לתור של Other

        // המרגל מפעיל את היכולת – התוקף אמור להיחסם מ-arrest
        spy->getRole()->onArrest(*spy, *other, game);
        CHECK(other->isBlocked("arrest") == true);
    }

    /**
     * @subcase בדיקת לוגיקת arrest עתידי – בדיקת סימון וביטול חסימה מתוכננת ע״י המרגל.
     */
    SUBCASE("Spy planned arrest functionality") {
        game.markPlannedArrest(*spy, *other);
        CHECK(game.isArrestBlockedNextTurn(*other) == true);

        game.clearPlannedArrest(*other);
        CHECK(game.isArrestBlockedNextTurn(*other) == false);
    }
}
