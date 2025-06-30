//
// Created by 12adi on 17/06/2025.
//

// ברון

#ifndef BARON_H
#define BARON_H
#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

class Baron : public Role {
  public:
    Baron();
    ~Baron();

    std::string getName() const override;
    void onSanction(Player& player, Player& attacker, Game& game)override;
    void onInvest(Player& self, Game& game) override;
};



#endif //BARON_H
