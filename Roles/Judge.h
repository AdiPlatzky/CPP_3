//
// Created by 12adi on 16/06/2025.
//

// שופט

#ifndef JUDGE_H
#define JUDGE_H

#include "../Role.h"
#include <string>

class QString;
class Player;
class Game;


class Judge : public Role {
  public:
    Judge();
    ~Judge();

    std::string getName() const override;
    // תגובה לפעולת שוחד
    void onBribe(Player& actor, Player& self, Game& game) override;
    void onSanction(Player& actor, Player& self, Game& game) override;
    bool canBlock(const std::string &actionName) const override;

};
#endif //JUDGE_H
