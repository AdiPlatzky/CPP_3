//
// Created by 12adi on 16/06/2025.
//
#include <iostream>
#include "Judge.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

/**
 * @brief בנאי ברירת מחדל של Judge.
 */
Judge::Judge(){}

/**
 * @brief דסטרקטור של Judge.
 */
Judge::~Judge(){}

/**
 * @brief מחזיר את שם התפקיד ("Judge").
 * @return מחרוזת "Judge"
 */
std::string Judge::getName() const {
  return "Judge";
}

/**
 * @brief פעולה שמופעלת כאשר שופט חוסם שוחד:
 * מבטל את תור הבונוס של השחקן שניסה לבצע שוחד.
 * @param actor השחקן שניסה לשחד
 * @param self רפרנס אל השופט (לא בשימוש)
 * @param game רפרנס למשחק (לא בשימוש)
 */
void Judge::onBribe(Player& actor, Player&, Game&){
  actor.setBonusAction(false); // מבטל את תור הבונוס
}

/**
 * @brief פעולה שמופעלת כאשר שופט מגיב לסנקציה:
 * מחייב את התוקף לשלם קנס של מטבע אחד לקופה המרכזית.
 * אם אין מספיק מטבעות – תיזרק חריגה (exception).
 * @param actor השחקן התוקף שישלם את הקנס
 * @param self רפרנס אל השופט (לא בשימוש)
 * @param game רפרנס למשחק (לקבלת קופה מרכזית)
 */
void Judge::onSanction(Player& actor, Player&, Game& game) {
  if (actor.getCoins() < 1) {
    throw std::runtime_error("Not enough coins.");
  }
  actor.removeCoins(1);
  game.addToCoinPool(1);
}

/**
 * @brief בודק אם השופט יכול לחסום פעולה נתונה.
 * Judge חוסם אך ורק "bribe".
 * @param actionName שם הפעולה לבדיקה
 * @return true אם הפעולה היא "bribe", אחרת false
 */
bool Judge::canBlock(const std::string &actionName) const {
  return actionName == "bribe";
}
