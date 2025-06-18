//
// Created by 12adi on 17/06/2025.
//

// סוחר

#ifndef MERCHANT_H
#define MERCHANT_H
#include <string>
#include "../Role.h"

class Player;
class Game;

class Merchant : public Role {
  public:
    Merchant();
    ~Merchant();

    std::string getName() const override;
    void onArrest(Player& self, Player& attacker, Game& game)override;
    void onTurnStart(Player& player, Game& game) override;


};



#endif //MERCHANT_H
