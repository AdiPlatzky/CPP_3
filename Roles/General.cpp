//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "General.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

General::General(){}
General::~General(){}

void General::onArrest(Player& self, Player& attacker, Game&){
  if(attacker.getCoins() > 0){
    attacker.removeCoins(1);
    self.addCoins(1);
  }
}

void General::onCoup(Player& target, Player&, Game& game){
  if(target.getCoins() >= 5){
    target.removeCoins(5);
    game.addToCoinPool(5);
    // לא עושים deactivate — הוא הצליח להתגונן
    std::cout << "Wow that was close you almost died! \n But, the general has defended on you form the coup!";
  }
}