//
// Created by 12adi on 17/06/2025.
//

// גנרל

#ifndef GENERAL_H
#define GENERAL_H
#include <string>
#include "../Role.h"

class Player;
class Game;


class General : public Role {
  public:
    General();
    ~General();
  void onArrest(Player& self, Player& attacker, Game& game) override;
  void onCoup(Player& target, Player& attacker, Game& game) override;


};



#endif //GENERAL_H
