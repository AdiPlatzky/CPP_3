//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "Spy.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

/**
 * @brief בנאי ברירת מחדל של Spy.
 */
Spy::Spy(){}

/**
 * @brief דסטרקטור של Spy.
 */
Spy::~Spy(){}

/**
 * @brief מחזיר את שם התפקיד ("Spy").
 * @return מחרוזת "Spy"
 */
std::string Spy::getName() const {
    return "Spy";
}

/**
 * @brief פעולה שמופעלת כאשר מנסים לבצע arrest על המרגל:
 * - המרגל "רואה" כמה מטבעות יש לתוקף (מדפיס למסך).
 * - המרגל חוסם את התוקף מלעצור אותו שוב בתור הבא ע״י קריאה ל־blockAction("arrest",1) על התוקף.
 * @param self רפרנס למרגל (לא בשימוש כאן)
 * @param attacker רפרנס לשחקן שמנסה לעצור
 * @param game רפרנס למשחק (לא בשימוש כאן)
 */
void Spy::onArrest(Player&, Player& attacker, Game&){
    std::cout << "Spy sees " << attacker.getName() << " has " << attacker.getCoins() << " coins\n";
    attacker.blockAction("arrest",1);
}

/**
 * @brief בודק האם המרגל יכול לחסום פעולה.
 * המרגל מסוגל לחסום רק arrest.
 * @param actionName שם הפעולה
 * @return true אם actionName הוא "arrest", אחרת false
 */
bool Spy::canBlock(const std::string &actionName) const {
    return actionName == "arrest";
}
