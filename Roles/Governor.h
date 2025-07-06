//
// Created by 12adi on 16/06/2025.
//
/**
 * @file Governor.h
 * @brief מחלקה המייצגת את התפקיד "ניצב" (Governor) במשחק.
 *
 * התפקיד Governor נותן לשחקן אפשרות לקבל בונוס במיסים (tax) ולחסום פעולת tax של שחקנים אחרים.
 */

#ifndef GOVERNOR_H
#define GOVERNOR_H

#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

/**
 * @class Governor
 * @brief מימוש של תפקיד "ניצב" במשחק, מעניק בונוס מס וחוסם tax של אחרים.
 */
class Governor : public Role {
public:
    /**
     * @brief בנאי ברירת מחדל של Governor.
     */
    Governor();

    /**
     * @brief דסטרקטור של Governor.
     */
    ~Governor();

    /**
     * @brief מחזיר את שם התפקיד ("Governor").
     * @return מחרוזת "Governor"
     */
    std::string getName() const override;

    /**
     * @brief פעולה שמופעלת כאשר מתבצעת פעולה tax במשחק.
     * - אם זה השחקן עצמו, מקבל בונוס מטבע.
     * - אם זה שחקן אחר, חוסם אותו מ־tax בתור הבא.
     * @param player רפרנס לשחקן שמבצע את הפעולה (יכול להיות אני או אחר)
     * @param self רפרנס אל השחקן (Governor)
     * @param game רפרנס למשחק
     */
    void onTax(Player& player, Player& self, Game& game) override;

    /**
     * @brief בודק האם Governor יכול לחסום פעולה מסוימת.
     * Governor חוסם רק tax.
     * @param actionName שם הפעולה
     * @return true אם מדובר ב־"tax", אחרת false
     */
    bool canBlock(const std::string &actionName) const override;
};

#endif //GOVERNOR_H
