//
// Created by 12adi on 23/06/2025.
//
/**
 * @file RoleFactory.h
 * @brief מחלקה סטטית ליצירת אובייקטים של Role (תפקידים) עבור שחקנים במשחק.
 *
 * RoleFactory מספקת פונקציות ליצירת תפקידים רנדומליים או מותאמים אישית לכל שחקן.
 */

#ifndef ROLEFACTORY_H
#define ROLEFACTORY_H

#include <memory>
#include "../Role.h"

class Player;

/**
 * @class RoleFactory
 * @brief מחלקה סטטית האחראית ליצירת מופעי Role חדשים (תפקידים) בהתאם לדרישות המשחק.
 *
 * מספקת יצירה רנדומלית של תפקידים, או יצירת תפקיד לפי לוגיקה מותאמת לשחקן.
 */
class RoleFactory {
public:
    /**
     * @brief יוצר תפקיד רנדומלי חדש מסוג Role.
     * בכל קריאה, נבחר תפקיד אקראי מתוך: Baron, General, Governor, Judge, Merchant, Spy.
     * @return מצביע ייחודי (unique_ptr) לאובייקט Role חדש
     */
    static std::unique_ptr<Role> createRandomRole();

    /**
     * @brief יוצר תפקיד חדש לפי שחקן (ניתן להרחבה ללוגיקת בחירה מותאמת).
     * כרגע לא ממומש, אך ניתן להוסיף לוגיקה בהתאם לפרטי השחקן.
     * @param player רפרנס לשחקן עבורו נוצר התפקיד
     * @return מצביע ייחודי (unique_ptr) לאובייקט Role חדש
     */
    static std::unique_ptr<Role> createRole(Player &player);
};

#endif //ROLEFACTORY_H
