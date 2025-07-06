//
// Created by 12adi on 17/06/2025.
//
/**
 * @file Baron.h
 * @brief מחלקה המייצגת את התפקיד "ברון" (Baron) במשחק.
 * מחלקה זו יורשת מ־Role ומגדירה יכולות מיוחדות לברון:
 *  - יכול להרוויח מסנקציה (sanction)
 *  - יכול לבצע פעולה ייחודית של השקעה (invest)
 */

#ifndef BARON_H
#define BARON_H

#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

/**
 * @class Baron
 * @brief תפקיד הברון — תפקיד עם יכולת השקעה ייחודית והרווח מסנקציות.
 */
class Baron : public Role {
public:
    /**
     * @brief בנאי ברירת מחדל של Baron.
     */
    Baron();

    /**
     * @brief דסטרקטור.
     */
    ~Baron();

    /**
     * @brief מחזיר את שם התפקיד ("Baron").
     * @return מחרוזת "Baron"
     */
    std::string getName() const override;

    /**
     * @brief פעולה שמופעלת כאשר הברון סופג סנקציה — מקבל מטבע כפיצוי.
     * @param player רפרנס לברון עצמו (הנפגע)
     * @param attacker רפרנס לתוקף (לא בשימוש כאן)
     * @param game רפרנס למשחק
     */
    void onSanction(Player& player, Player& attacker, Game& game) override;

    /**
     * @brief פעולה ייחודית לברון — השקעה: משלם 3 מטבעות ומקבל 6 מטבעות (אם יש מספיק בקופה).
     * אם אין מספיק מטבעות בקופה המרכזית, הפעולה נכשלת.
     * @param self רפרנס לברון
     * @param game רפרנס למשחק
     */
    void onInvest(Player& self, Game& game) override;
};

#endif //BARON_H
