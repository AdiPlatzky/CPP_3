#include "Player.h"
#include "Role.h"
#include <stdexcept>

/**
 * @brief בנאי של שחקן
 * בודק שהתפקיד שהועבר תקין (לא nullptr), מאתחל את מצב המטבעות והמעמד (פעיל).
 * @throws std::invalid_argument אם ה־Role שהועבר לא חוקי
 */
Player::Player(const std::string& name, std::unique_ptr<Role> role)
    : name(name), coins(0), active(true) {
  if(!role) {
    throw std::invalid_argument("Role cannot be null");
  }
  this->role = std::move(role); // לוקח בעלות על התפקיד
}

/**
 * @brief דסטרקטור — כרגע לא משחרר משאבים מיוחדים, רק ברירת מחדל.
 */
Player::~Player() {}

const std::string& Player::getName() const {
  // מחזיר את שם השחקן (קבוע)
  return name;
}

int Player::getCoins() const {
  // מחזיר את כמות המטבעות הנוכחית של השחקן
  return coins;
}

Role* Player::getRole() const {
  // מחזיר מצביע לתפקיד השחקן
  return role.get();
}

bool Player::isActive() const {
  // בודק האם השחקן עדיין פעיל במשחק
  return active;
}

/**
 * @brief מוסיף מטבעות לחשבון השחקן.
 * מונע הוספה שלילית של מטבעות (בדיקת תקינות).
 * @throws std::invalid_argument אם נשלח ערך שלילי
 */
void Player::addCoins(int amount) {
  if(amount < 0) {
    throw std::invalid_argument("Cannot add negative coins");
  }
  coins += amount;
}

/**
 * @brief מוריד מטבעות מהשחקן.
 * מוודא שיש מספיק לפני ההפחתה.
 * @throws std::runtime_error אם אין מספיק מטבעות
 */
void Player::removeCoins(int amount) {
  if(amount > coins) {
    throw std::runtime_error("Not enough coins.");
  }
  coins -= amount;
}

/**
 * @brief מדיח את השחקן מהמשחק — השחקן כבר לא משתתף.
 */
void Player::deactivate() {
  active = false;
}

/**
 * @brief חוסם פעולה מסוימת לשחקן, למספר תורות.
 * החסימה מתבצעת ע"י הכנסת הערך למפה blockedActions.
 */
void Player::blockAction(const std::string& actionName, int turns) {
  blockedActions[actionName]= turns;
}

/**
 * @brief בודק האם פעולה חסומה עבור השחקן.
 * שימושי במיוחד לפעולות כמו arrest/bribe.
 */
bool Player::isBlocked(const std::string& actionName) const {
  auto it = blockedActions.find(actionName);
  return it != blockedActions.end() && it->second > 0;
}

/**
 * @brief מנקה חסימות ישנות — מוריד 1 מכל חסימה קיימת.
 * אם הגיע ל־0 — מסיר מהמפה לגמרי.
 * קריאה טיפוסית בתחילת תור.
 */
void Player::clearBlocks() {
  for (auto it = blockedActions.begin(); it != blockedActions.end(); ) {
    if (--(it->second) <= 0) {
      it = blockedActions.erase(it); // מוחק חסימה שהסתיימה
    } else {
      ++it;
    }
  }
}

/**
 * @brief בודק האם יש בונוס (תור נוסף) שעדיין לא נוצל.
 */
bool Player::hasBonusAction() const {
  return bonusActionPending && !bonusActionUsed;
}

/**
 * @brief מסמן לשחקן שהוא קיבל (או איבד) בונוס תור.
 * איפוס הדגל של השימוש בונוס
 */
void Player::setBonusAction(bool active){
  bonusActionPending = active;
  bonusActionUsed = false;
}

/**
 * @brief מנצל את הבונוס, אם קיים. מחזיר true אם הצליח, false אחרת.
 */
bool Player::consumeBonusAction() {
  if(hasBonusAction()) {
    bonusActionUsed = true;
    return true;
  }
  return false;
}

/**
 * @brief בודק האם התפקיד של השחקן יכול לחסום פעולה מסוימת.
 */
bool Player::canBlock(const std::string& actionName) const {
  return role && role->canBlock(actionName);
}
