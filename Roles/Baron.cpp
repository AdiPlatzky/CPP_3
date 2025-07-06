//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "Baron.h"
#include "../Player.h"
#include "../Game.h"

Baron::Baron(){}
Baron::~Baron(){}

std::string Baron::getName() const {
    return "Baron";
}

void Baron::onSanction(Player& player, Player&, Game&){
  player.addCoins(1);
}

void Baron::onInvest(Player& self, Game& game){
    if(!self.isActive()){
        std::cout <<"Dear baron: Your activity has been blocked \n Your game over! Bye - Bye";
    }

    if(self.getCoins() < 3){
        std::cout << "Dear baron you do not enough coins to invest.";
    }

    if (self.getCoins() >= 10) {
        std::cout << "Dear baron you have 10 or more coins.\n Must perform a coup before... See you later";
    }

    if(game.getCoinPool() < 3){
      throw std::runtime_error("Dear baron the government in crisis and it not enough coins in pool coins to return profit - we sorry about that.");
    }

    self.removeCoins(3);
    self.addCoins(6);
    game.addToCoinPool(-3);
}


