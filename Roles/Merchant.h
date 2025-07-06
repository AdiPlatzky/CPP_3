//
// Created by 12adi on 17/06/2025.
//
/**
 * @file Merchant.h
 * @brief מחלקה המייצגת את תפקיד הסוחר (Merchant) במשחק.
 *
 * תפקיד הסוחר מעניק בונוס מטבע בתחילת תורו (אם עומד בתנאים), ומאפשר לו להתגונן ממעצר (arrest) על־ידי תשלום.
 */

#ifndef MERCHANT_H
#define MERCHANT_H
#include <string>
#include "../Role.h"

class Player;
class Game;

/**
 * @class Merchant
 * @brief מימוש של תפקיד "סוחר" (Merchant) במשחק.
 *
 * סוחר מקבל מטבע נוסף בתחילת התור אם יש לו לפחות 3 מטבעות, ויכול לבטל מעצר על־ידי תשלום.
 */
class Merchant : public Role {
public:
    /**
     * @brief בנאי ברירת מחדל של Merchant.
     */
    Merchant();

    /**
     * @brief דסטרקטור של Merchant.
     */
    ~Merchant();

    /**
     * @brief מחזיר את שם התפקיד ("Merchant").
     * @return מחרוזת "Merchant"
     */
    std::string getName() const override;

    /**
     * @brief פעולה שמופעלת בתחילת כל תור של הסוחר:
     * אם יש לסוחר לפחות 3 מטבעות ויש מטבעות בקופה – יקבל בונוס של מטבע אחד.
     * @param player רפרנס לשחקן (הסוחר)
     * @param game רפרנס למשחק (לקופה)
     */
    void onTurnStart(Player& player, Game& game) override;

    /**
     * @brief תגובה כאשר הסוחר נבחר למעצר (arrest):
     * אם יש לו לפחות 2 מטבעות – משלם 2 מטבעות לקופה ומבטל את הרווח של התוקף.
     * אחרת – ההגנה לא מתבצעת.
     * @param self רפרנס לסוחר
     * @param attacker רפרנס לתוקף
     * @param game רפרנס למשחק
     */
    void onArrest(Player& self, Player& attacker, Game& game) override;
};

#endif //MERCHANT_H
