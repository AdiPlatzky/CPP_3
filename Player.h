//
// 12adi45@gmail.com
//

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <memory>
#include <unordered_map>

/**
 * @brief מחלקת שחקן - מייצגת שחקן יחיד במשחק.
 *        כל שחקן מכיל שם, תפקיד (Role), כמות מטבעות, מצב (פעיל/מודח),
 *        מערכת לחסימת פעולות, וניהול בונוסים מיוחדים בתור.
 */
class Role;
class Game;

class Player {
  private:
    std::string name; ///< שם השחקן (למשל "Alice")
    int coins; ///< כמות המטבעות הנוכחית של השחקן
    std::unique_ptr<Role> role; ///< מצביע לתפקיד (Role) הייחודי של השחקן
    bool active; ///< האם השחקן עדיין פעיל במשחק (true) או מודח (false)
    std::unordered_map<std::string, int> blockedActions; ///< פעולות חסומות - שם פעולה -> לכמה תורות החסימה
    bool bonusActionPending = false; ///< האם השחקן זכאי לבונוס תור נוסף
    bool bonusActionUsed = false; ///< האם הבונוס נוצל כבר בתור הנוכחי

  public:
    /**
     * @brief בנאי - יוצר שחקן חדש בשם מסוים ותפקיד נתון.
     * @param name שם השחקן
     * @param role תפקיד ראשוני (אסור שיהיה nullptr)
     * @throws std::invalid_argument אם role הוא nullptr
     */
    Player(const std::string& name, std::unique_ptr<Role> role);

    /**
     * @brief דסטרקטור - מנקה משאבים של השחקן (כמו התפקיד)
     */
    ~Player();

    // =========== גישה למידע בסיסי ===========
    /**
     * @return שם השחקן
     */
    const std::string& getName() const;

    /**
     * @return כמות המטבעות שיש לשחקן
     */
    int getCoins() const;

    /**
     * @return מצביע לתפקיד (Role*) של השחקן
     */
    Role* getRole() const;

    /**
     * @return true אם השחקן פעיל במשחק (עדיין לא הודח)
     */
    bool isActive() const;

    // =========== שינוי מצב ===========
    /**
     * @brief מוסיף מטבעות לשחקן (לא ניתן להוסיף שלילי)
     * @param amount כמות להוספה (חייבת להיות אי-שלילית)
     * @throws std::invalid_argument אם amount שלילי
     */
    void addCoins(int amount);

    /**
     * @brief מוריד מטבעות מהשחקן (אם יש מספיק)
     * @param amount כמות להורדה
     * @throws std::runtime_error אם אין מספיק מטבעות
     */
    void removeCoins(int amount);

    /**
     * @brief מדיח את השחקן (מסמן כלא פעיל)
     */
    void deactivate();

    // =========== חסימת פעולות (לדוג' חסימה ע"י תפקיד אחר) ===========
    /**
     * @brief חוסם פעולה בשם מסוים למספר תורות (ברירת מחדל: 1)
     * @param actionName שם הפעולה לחסימה (למשל "arrest")
     * @param turns כמה תורות לחסום (ברירת מחדל: 1)
     */
    void blockAction(const std::string& actionName, int turns = 1);

    /**
     * @brief בודק האם פעולה חסומה עבור השחקן
     * @param actionName שם פעולה לבדיקה
     * @return true אם הפעולה חסומה (turns>0)
     */
    bool isBlocked(const std::string& actionName) const;

    /**
     * @brief מנקה חסימות ישנות (מוריד turn אחד מכל פעולה חסומה, ומוחק אם הגיע ל־0)
     *        נקרא בדרך כלל בתחילת תור
     */
    void clearBlocks();

    // =========== ניהול בונוס תור חוזר ===========
    /**
     * @brief בודק אם יש בונוס תור נוסף (שניתן ע"י פעולה מסוימת)
     * @return true אם יש בונוס תור נוסף שטרם נוצל
     */
    bool hasBonusAction() const;

    /**
     * @brief מסמן לשחקן שהוא קיבל/לא קיבל בונוס תור נוסף
     * @param active האם להפעיל את הבונוס
     */
    void setBonusAction(bool active);

    /**
     * @brief מנסה לנצל את הבונוס, ואם הצליח מחזיר true
     * @return true אם בונוס נוצל, false אם לא היה בונוס
     */
    bool consumeBonusAction();

    // =========== ממשק תפקיד ===========
    /**
     * @brief בודק אם התפקיד הנוכחי יכול לחסום פעולה בשם מסוים
     * @param actionName שם הפעולה (למשל "arrest")
     * @return true אם התפקיד יודע לחסום
     */
    bool canBlock(const std::string& actionName) const;

    // =========== בקרה על שוחד ===========
    /**
     * @brief דגל - האם השחקן השתמש בשוחד בתור הנוכחי
     */
    bool bribeUsedThisTurn = false;

    /**
     * @brief מסמן אם בוצע שוחד בתור זה
     */
    void setBribeUsedThisTurn(bool val) { bribeUsedThisTurn = val; }

    /**
     * @brief בודק אם השחקן כבר השתמש בשוחד בתור הנוכחי
     * @return true אם השתמש
     */
    bool hasUsedBribeThisTurn() const { return bribeUsedThisTurn; }
};

#endif //PLAYER_H
