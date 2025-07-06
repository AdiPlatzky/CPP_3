//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "Merchant.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

/**
 * @brief בנאי ברירת מחדל של Merchant.
 */
Merchant::Merchant(){}

/**
 * @brief דסטרקטור של Merchant.
 */
Merchant::~Merchant(){}

/**
 * @brief מחזיר את שם התפקיד ("Merchant").
 * @return מחרוזת "Merchant"
 */
std::string Merchant::getName() const {
  return "Merchant";
}

/**
 * @brief פעולה שמופעלת בתחילת כל תור של הסוחר:
 * אם לשחקן יש לפחות 3 מטבעות ויש מטבעות בקופה המרכזית – יתווסף לו מטבע בונוס (על חשבון הקופה).
 * @param player רפרנס לשחקן הסוחר
 * @param game רפרנס למשחק
 */
void Merchant::onTurnStart(Player& player, Game& game){
  if(player.getCoins() >= 3 && game.getCoinPool() > 0){
    player.addCoins(1);
    game.addToCoinPool(-1);
  }
}

/**
 * @brief תגובה כאשר הסוחר נבחר למעצר (arrest):
 * אם יש לסוחר לפחות 2 מטבעות – משלם אותם לקופה, ומבטל את הרווח של התוקף (מוריד לו מטבע).
 * אחרת – לא מתבצע תשלום ולא מתבטל הרווח.
 * @param self רפרנס לסוחר (המותקף)
 * @param attacker רפרנס לתוקף
 * @param game רפרנס למשחק (להעברת המטבעות)
 */
void Merchant::onArrest(Player& self, Player& attacker, Game& game){
  if(self.getCoins() >= 2){
    self.removeCoins(2);
    game.addToCoinPool(2);
    // התוקף קיבל מטבע קודם – נבטל את זה
    attacker.removeCoins(1);
  }
  else {
    std::cout << "Merchant doesn't have enough coins to be arrested";
  }
}
