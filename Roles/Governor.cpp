//
// Created by 12adi on 16/06/2025.
//
#include <iostream>
#include "Governor.h"
#include "../Player.h"
#include "../Game.h"

/**
 * @brief בנאי ברירת מחדל של Governor.
 */
Governor::Governor(){}

/**
 * @brief דסטרקטור של Governor.
 */
Governor::~Governor(){}

/**
 * @brief מחזיר את שם התפקיד ("Governor").
 * @return מחרוזת "Governor"
 */
std::string Governor::getName() const {
  return "Governor";
}

/**
 * @brief מימוש השפעת הניצב על tax:
 * אם השחקן שמבצע את הפעולה הוא הניצב עצמו (self) — מקבל בונוס של מטבע (אם יש במאגר).
 * אחרת, הוא חוסם את אותו שחקן מלעשות tax בתור הבא.
 * @param player רפרנס לשחקן שמבצע את הפעולה (יכול להיות Governor או אחר)
 * @param self רפרנס אל Governor (השחקן עצמו)
 * @param game רפרנס למשחק
 */
void Governor::onTax(Player& player, Player& self, Game& game){
  // אם השחקן המבצע הוא אני (הניצב) — קבל בונוס של מטבע
  if(&player == &self){
    if(game.getCoinPool() < 1){
      std::cout << "You don't have enough coins for Governor bonus.";
    }
    player.addCoins(1);
    game.getCoinPool()--;
  }
  else {
    // Governor עוצר/חוסם את הפעולה tax של שחקן אחר
    player.blockAction("tax",1);
  }
}

/**
 * @brief בודק האם Governor יכול לחסום פעולה מסוימת.
 * Governor חוסם אך ורק tax.
 * @param actionName שם הפעולה
 * @return true אם actionName הוא "tax", אחרת false
 */
bool Governor::canBlock(const std::string &actionName) const {
  return actionName == "tax";
}
