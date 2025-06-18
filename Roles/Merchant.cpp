//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "Merchant.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

Merchant::Merchant(){}
Merchant::~Merchant(){}

std::string Merchant::getName() const {
  return "Merchant";
}

void Merchant::onTurnStart(Player& player, Game& game){
  if(player.getCoins() >= 3 && game.getCoinPool() > 0){
    player.addCoins(1);
    game.addToCoinPool(-1);
  }
}

void Merchant::onArrest(Player& self, Player& attacker, Game& game){
  if(self.getCoins() >= 2){
    self.removeCoins(2);
    game.addToCoinPool(2);
    // התוקף קיבל מטבע קודם – נבטל את זה
    attacker.removeCoins(1);
  }
  else {
    throw std::runtime_error("Merchant doesn't have enough coins to be arrested");
  }
}
