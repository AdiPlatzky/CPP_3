//
// Created by 12adi on 16/06/2025.
//
/**
 * @file Judge.h
 * @brief מחלקה המייצגת את תפקיד השופט (Judge) במשחק.
 *
 * תפקיד השופט נותן חסימה לפעולת שוחד (bribe) ויכול להעניש תוקף בפעולת סנקציה (sanction).
 */

#ifndef JUDGE_H
#define JUDGE_H

#include "../Role.h"
#include <string>

class QString;
class Player;
class Game;

/**
 * @class Judge
 * @brief מימוש של תפקיד "שופט" (Judge) במשחק.
 *
 * תפקיד זה יודע לחסום פעולת שוחד, ולחייב את התוקף בתשלום קנס בסנקציה.
 */
class Judge : public Role {
public:
    /**
     * @brief בנאי ברירת מחדל של Judge.
     */
    Judge();

    /**
     * @brief דסטרקטור של Judge.
     */
    ~Judge();

    /**
     * @brief מחזיר את שם התפקיד ("Judge").
     * @return מחרוזת "Judge"
     */
    std::string getName() const override;

    /**
     * @brief תגובה לפעולת שוחד: מבטל את תור הבונוס של מי שביצע שוחד.
     * @param actor השחקן שניסה לבצע שוחד
     * @param self רפרנס אל Judge (לא בשימוש כאן)
     * @param game רפרנס למשחק
     */
    void onBribe(Player& actor, Player& self, Game& game) override;

    /**
     * @brief תגובה לפעולת סנקציה: מחייב את השחקן התוקף לשלם קנס של מטבע אחד.
     * במידה ואין מספיק מטבעות, תיזרק חריגה.
     * @param actor השחקן שתקף (זה שישלם את הקנס)
     * @param self רפרנס אל Judge (לא בשימוש כאן)
     * @param game רפרנס למשחק (להוספת המטבע לקופה)
     */
    void onSanction(Player& actor, Player& self, Game& game) override;

    /**
     * @brief בודק אם Judge יכול לחסום פעולה.
     * Judge יכול לחסום רק פעולה בשם "bribe" (שוחד).
     * @param actionName שם הפעולה
     * @return true אם זו פעולה "bribe", אחרת false
     */
    bool canBlock(const std::string &actionName) const override;
};
#endif //JUDGE_H
