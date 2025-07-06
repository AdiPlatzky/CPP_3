//
// Created by 12adi on 23/06/2025.
//
/**
 * @file RoleFactory.cpp
 * @brief מימוש פונקציות יצירת תפקידים עבור המשחק.
 */

#include "RoleFactory.h"
#include "Baron.h"
#include "General.h"
#include "Governor.h"
#include "Judge.h"
#include "Merchant.h"
#include "Spy.h"
#include <cstdlib>
#include <ctime>
#include <stdexcept>
#include "../Player.h"

/**
 * @brief יוצר תפקיד רנדומלי חדש מסוג Role.
 *
 * מבצע אתחול seed למנוע האקראיות (רק פעם אחת במהלך חיי התוכנית).
 * בוחר מספר אקראי בין 0 ל־5, ומחזיר תפקיד בהתאם:
 *   0 – Baron
 *   1 – General
 *   2 – Governor
 *   3 – Judge
 *   4 – Merchant
 *   5 – Spy
 * במקרה של ערך חריג – זורק חריגה.
 *
 * @return מצביע ייחודי לאובייקט Role שנבחר אקראית
 * @throws std::runtime_error אם נוצר ערך לא תקני
 */
std::unique_ptr<Role> RoleFactory::createRandomRole() {
  static bool seeded = false;
  if(!seeded){
    std::srand(std::time(nullptr));
    seeded = true;
  }
  int randomNumber = std::rand() % 6;
  switch(randomNumber){
    case 0: return std::make_unique<Baron>();
    case 1: return std::make_unique<General>();
    case 2: return std::make_unique<Governor>();
    case 3: return std::make_unique<Judge>();
    case 4: return std::make_unique<Merchant>();
    case 5: return std::make_unique<Spy>();
    default:
      throw std::runtime_error("Unknown role generated in RoleFactory!");
  }
}

/**
 * @brief יוצר תפקיד לפי השחקן (לפי צורך לוגי).
 *
 * כרגע אינו ממומש – יש להוסיף מימוש מותאם לדרישות בהמשך.
 * @param player רפרנס לשחקן
 * @return nullptr (יש לעדכן לפונקציונליות רצויה)
 */
std::unique_ptr<Role> RoleFactory::createRole(Player &player) {
  // כאן אפשר להוסיף לוגיקה בהתבסס על השחקן
  return nullptr;
}
