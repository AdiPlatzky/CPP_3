//
// 12adi45@gmail.com
//

#include "PlayerManager.h"
#include "../Player.h"
#include <QList>

/**
 * @brief בנאי - יוצר מנהל שחקנים ריק
 */
PlayerManager::PlayerManager() {
    // הבנאי ריק - כל האתחולים מתבצעים אוטומטית
}

/**
 * @brief יוצר שחקנים חדשים עם תפקידים רנדומליים לפי רשימת שמות
 * @param names רשימת שמות השחקנים ליצירה
 */
void PlayerManager::createPlayers(const QVector<QString> &names) {
    // ניקוי רשימת השחקנים הקיימת
    players.clear();
    players.reserve(names.size());

    // יצירת שחקן לכל שם ברשימה
    for (const QString& name : names) {
        try {
            // המרת השם ויצירת תפקיד רנדומלי
            std::string playerName = name.toStdString();
            auto role = roleFactory.createRandomRole();

            // יצירת השחקן והוספתו לרשימה
            auto player = std::make_shared<Player>(playerName, std::move(role));
            players.push_back(player);

        } catch (const std::exception& e) {
            /** @brief טיפול בשגיאות ביצירת שחקן */
            std::cerr << "שגיאה ביצירת שחקן עם השם " << name.toStdString()
                      << ": " << e.what() << std::endl;
        }
    }

    std::cout << "נוצרו " << players.size() << " שחקנים בהצלחה" << std::endl;
}

/**
 * @brief מחזיר גישה לקריאה לרשימת השחקנים שנוצרו
 */
const std::vector<std::shared_ptr<Player>>& PlayerManager::getPlayers() const {
    return players;
}