//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "General.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

/**
 * @brief בנאי ברירת מחדל של General.
 */
General::General(){}

/**
 * @brief דסטרקטור של General.
 */
General::~General(){}

/**
 * @brief מחזיר את שם התפקיד ("General").
 * @return מחרוזת "General"
 */
std::string General::getName() const {
  return "General";
}

/**
 * @brief פעולה שמופעלת כאשר מנסים לעצור (arrest) את השחקן הגנרל.
 * אם לתוקף יש לפחות מטבע אחד, הוא מעביר אותו למתגונן.
 * @param attacker רפרנס לתוקף (השחקן שמנסה לעצור)
 * @param target רפרנס למתגונן (הגנרל)
 * @param game רפרנס למשחק (לא בשימוש כאן)
 */
void General::onArrest(Player& attacker, Player& target, Game&){
  if(attacker.getCoins() > 0){
    attacker.removeCoins(1);
    target.addCoins(1);
  }
}

/**
 * @brief פעולה שמופעלת כאשר מנסים לבצע coup על שחקן גנרל.
 * אם למתגונן יש לפחות 5 מטבעות — הוא משלם אותם ומתגונן (לא מודח!).
 * @param target רפרנס למתגונן (הגנרל)
 * @param attacker רפרנס לתוקף
 * @param game רפרנס למשחק
 */
void General::onCoup(Player&, Player& target, Game& game){
  if(target.getCoins() >= 5){
    target.removeCoins(5);
    game.addToCoinPool(5);
    // לא מדיחים את השחקן — הצליח להתגונן
    std::cout << "Wow that was close you almost died! \n But, the general has defended on you form the coup!";
  }
}

/**
 * @brief בודק האם הגנרל יכול לחסום פעולה מסוימת.
 * גנרל חוסם אך ורק coup.
 * @param actionName שם הפעולה
 * @return true אם actionName הוא "coup", אחרת false
 */
bool General::canBlock(const std::string &actionName) const {
  return actionName == "coup";
}
