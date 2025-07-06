//
// Created by 12adi on 17/06/2025.
//
/**
 * @file Spy.h
 * @brief מחלקה המייצגת את דמות המרגל (Spy) במשחק.
 *
 * המרגל יכול לחסום פעולת מעצר (arrest) ולבצע חסימה למבצע הפעולה.
 */

#ifndef SPY_H
#define SPY_H
#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

/**
 * @class Spy
 * @brief מחלקה זו מממשת את תפקיד המרגל במשחק.
 *
 * כאשר מנסים לעצור את המרגל, הוא חוסם את האפשרות לעצור אותו בתור הבא.
 */
class Spy : public Role {
public:
    /**
     * @brief בנאי ברירת מחדל של Spy.
     */
    Spy();

    /**
     * @brief דסטרקטור של Spy.
     */
    ~Spy();

    /**
     * @brief מחזיר את שם התפקיד ("Spy").
     * @return מחרוזת "Spy"
     */
    std::string getName() const override;

    /**
     * @brief מופעל כאשר מנסים לעצור את המרגל (arrest):
     * המרגל חוסם את התוקף מלעצור אותו שוב בתור הבא, ומדפיס הודעה לחשיפת כמות המטבעות של התוקף.
     * @param self רפרנס למרגל (המותקף)
     * @param attacker רפרנס לתוקף
     * @param game רפרנס למשחק
     */
    void onArrest(Player& self, Player& attacker, Game& game) override;

    /**
     * @brief בודק האם המרגל יכול לחסום פעולה מסוימת.
     * @param actionName שם הפעולה לבדיקה
     * @return true אם הפעולה היא arrest, אחרת false
     */
    bool canBlock(const std::string &actionName) const override;
};

#endif //SPY_H
