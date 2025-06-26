//
// Created by 12adi on 16/06/2025.
//
#include <iostream>
#include "Governor.h"
#include "../Player.h"
#include "../Game.h"

Governor::Governor(){}
Governor::~Governor(){}

std::string Governor::getName() const {
  return "Governor";
}

void Governor::onTax(Player& player, Player& self, Game& game){
  // אם השחקן המבצע זה אני – תוספת של 1
  if(&player == &self){
    if(game.getCoinPool() < 1){
      std::cout << "You don't have enough coins for Governor bonus.";
    }
    player.addCoins(1);
    game.getCoinPool()--;
  }
  else {
    // Governor עוצר את הפעולה של מישהו אחר
    player.blockAction("tax",1);
  }
}
