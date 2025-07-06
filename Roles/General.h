//
// Created by 12adi on 17/06/2025.
//
/**
 * @file General.h
 * @brief מחלקה המייצגת את התפקיד "גנרל" (General) במשחק.
 *
 * הגנרל הוא תפקיד עם יכולות הגנה ייחודיות:
 *  - מגן על שחקנים מהפיכה (coup) ומעצר (arrest) באמצעות מנגנונים ייחודיים.
 */

#ifndef GENERAL_H
#define GENERAL_H

#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

/**
 * @class General
 * @brief תפקיד "גנרל" במשחק. מיישם יכולות הגנה נגד coup ו־arrest.
 */
class General : public Role {
public:
  /**
   * @brief בנאי ברירת מחדל של General.
   */
  General();

  /**
   * @brief דסטרקטור.
   */
  ~General();

  /**
   * @brief מחזיר את שם התפקיד ("General").
   * @return מחרוזת "General"
   */
  std::string getName() const override;

  /**
   * @brief פעולה שמופעלת כאשר מתבצע arrest על שחקן עם תפקיד General.
   * במידה ולתוקף יש מטבע, הוא משלם אותו למגן.
   * @param attacker רפרנס לתוקף (מי שמנסה לעצור)
   * @param target רפרנס למתגונן (הגנרל עצמו)
   * @param game רפרנס למשחק (לא בשימוש כאן)
   */
  void onArrest(Player& attacker, Player& target, Game& game) override;

  /**
   * @brief פעולה שמופעלת כאשר מתבצע coup על שחקן עם תפקיד General.
   * אם למתגונן יש לפחות 5 מטבעות — הוא משלם אותם ומתגונן מההדחה.
   * @param target רפרנס למתגונן (הגנרל)
   * @param attacker רפרנס לתוקף (מי שמנסה להפיל)
   * @param game רפרנס למשחק
   */
  void onCoup(Player& target, Player& attacker, Game& game) override;

  /**
   * @brief בודק האם הגנרל יכול לחסום פעולה מסוימת.
   * גנרל יכול לחסום coup בלבד.
   * @param actionName שם הפעולה
   * @return true אם actionName הוא "coup", אחרת false
   */
  bool canBlock(const std::string &actionName) const override;
};

#endif //GENERAL_H
