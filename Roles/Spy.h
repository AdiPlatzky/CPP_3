//
// Created by 12adi on 17/06/2025.
//

// מרגל

#ifndef SPY_H
#define SPY_H
#include <string>
#include "../Role.h"

class Player;
class Game;

class Spy : public Role {
  public:
    Spy();
    ~Spy();

    std::string getName() const override;
    void onArrest(Player& self, Player& attacker, Game& game)override;
};



#endif //SPY_H
