//
// 12adi45@gmail.com
//

#ifndef ROLE_H
#define ROLE_H
#include <string>

class Player;
class Game;

class Role {
  public:
    virtual ~Role() = default;

    virtual std::string getName() const = 0;
    // תוספת או שינוי בתחילת תור (כמו מטבע נוסף למרצ'נט)
    virtual void onTurnStart(Player&, Game&){}
    // השפעה על tax – למשל Governor
    virtual void onTax(Player&, Player&, Game&){}
    // תגובה ל־bribe – למשל Judge
    virtual void onBribe(Player&, Player&, Game&){}
    // תגובה ל־arrest – למשל General או Merchant
    virtual void onArrest(Player&, Player&, Game&){}
     // תגובה ל־sanction – למשל Baron או Judge
    virtual void onSanction(Player&, Player&, Game&){}
    // תגובה ל־coup – למשל General
    virtual void onCoup(Player&, Player&, Game&){}

    virtual void onInvest(Player&, Game&){}

    virtual bool canBlock(const std::string &actionName) const {
        return false;
    }
};

#endif //ROLE_H
