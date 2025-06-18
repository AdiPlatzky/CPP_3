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

void General::onArrest(Player& self, Player& attacker, Game& game){
  if(attacker.getCoins() > 0){
    attacker.removeCoins(1);
    self.addCoins(1);
  }
}

void General::onCoup(Player& target, Player& attacker, Game& game){
  if(target.getCoins() >= 5){
    target.removeCoins(5);
    game.addToCoinPool(5);
    // לא עושים deactivate — הוא הצליח להתגונן
    std::cout << "General defended against the coup!\n";
  }
}