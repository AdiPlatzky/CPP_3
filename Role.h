//
// Created by 12adi on 11/06/2025.
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
    virtual void onTurnStart(Player& player, Game& game){}
    // השפעה על tax – למשל Governor
    virtual void onTax(Player& player, Player& self, Game& game){}
    // תגובה ל־bribe – למשל Judge
    virtual void onBribe(Player& actor, Player& self, Game& game){}
    // תגובה ל־arrest – למשל General או Merchant
    virtual void onArrest(Player& self, Player& attacker, Game& game){}
    // תגובה ל־sanction – למשל Baron או Judge
    virtual void onSanction(Player& player, Player& attacker, Game& game){}
    // תגובה ל־coup – למשל General
    virtual void onCoup(Player& target, Player& attacker, Game& game){}


};

#endif //ROLE_H
