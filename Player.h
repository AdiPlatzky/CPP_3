//
// 12adi45@gmail.com
//

#ifndef PLAYER_H
#define PLAYER_H

#include <string>
#include <memory>
#include <set>

class Role;
class Game;

class Player{
  private:
    std::string name;
    int coins;
    std::unique_ptr<Role> role;
    bool active;
    std::set<std::string> blockedActions;
    bool bonusActionPending = false;
    bool bonusActionUsed = false;

    public:
      // בנאי
      Player(const std::string& name, std::unique_ptr<Role> role);
      ~Player();

      // גישה למידע
      const std::string& getName() const;
      int getCoins() const;
      Role* getRole() const;
      bool isActive() const;

      //שינוי מצב
     void addCoins(int amount);
     void removeCoins(int amount);
     void deactivate(); // הדחה מהמשחק

    // פעולות חסימה – תומך בדרישות של "אפשר לחסום פעולה בתור הבא"
    void blockAction(const std::string& actionName);
    bool isBlocked(const std::string& actionName) const;
    void clearBlocks(); //מתאפס בתחילת תור

    // פעולות ולידוא תור חוזר יחיד
    bool hasBonusAction() const; // בודק האם השחקן קיבל בונוס ועדיין לא השתמש בו
    void setBonusAction(bool active); // מסמן לשחקן שהוא קיבל בונוס
    bool consumeBonusAction(); // מנסה לנצל את הבונוס (אם קיים) ומחזיר true אם הצליח
};
#endif //PLAYER_H
