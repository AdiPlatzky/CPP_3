//
// Created by 12adi on 16/06/2025.
//
#include <iostream>
#include "Judge.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role"

Judge::Judge(){}
Judge::~Judge(){}

std::string Judge::getName() const {
  return "Judge";
}

void Judge::onBribe(Player& actor, Player& self, Game& game){
  actor.setBonusAction(false); // מבטל את תור הבונוס
}

void Judge::onSanction(Player& actor, Player& self, Game& game){
  if(actor.getCoins() < 1){
    throw std::runtime_error("Attacker cannot pay Judge penalty")
  }
  actor.removeCoins(1);
  game.addToCoinPool(1);
}
