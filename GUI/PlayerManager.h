//
// 12adi45@gmail.com
//

#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H
#include "../Roles/RoleFactory.h"
#include <QString>
#include <iostream>

/**
 * @class PlayerManager
 * @brief מנהל יצירת שחקנים עם תפקידים רנדומליים
 */
class PlayerManager {
public:
    /**
     * @brief בנאי - יוצר מנהל שחקנים ריק
     */
    PlayerManager();

    /**
     * @brief יוצר שחקנים חדשים עם תפקידים רנדומליים לפי רשימת שמות
     * @param names רשימת שמות השחקנים ליצירה
     */
    void createPlayers(const QVector<QString> &names);

    /**
     * @brief מחזיר גישה לקריאה לרשימת השחקנים שנוצרו
     * @return const reference לוקטור השחקנים
     */
    const std::vector<std::shared_ptr<Player>> &getPlayers() const;

private:
    /** @brief רשימת כל השחקנים שנוצרו */
    std::vector<std::shared_ptr<Player>> players;

    /** @brief יצרן התפקידים לבחירה רנדומלית */
    RoleFactory roleFactory;
};

#endif //PLAYERMANAGER_H