//
// Created by 12adi on 16/06/2025.

// ניצב

#ifndef GOVERNOR_H
#define GOVERNOR_H
#include <string>
#include "../Role.h"

class QString;
class Player;
class Game;

class Governor : public Role {
  public:
    Governor();
    ~Governor();

    std::string getName() const override;
    void onTax(Player& player, Player& self, Game& game) override;
    bool canBlock(const std::string &actionName) const override;

};



#endif //GOVERNOR_H
