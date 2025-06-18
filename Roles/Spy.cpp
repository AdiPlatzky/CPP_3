//
// Created by 12adi on 17/06/2025.
//
#include <iostream>
#include "Spy.h"
#include "../Player.h"
#include "../Game.h"
#include "../Role.h"

Spy::Spy(){}
Spy::~Spy(){}

std::string Spy::getName() const {
    return "Spy";
}

void Spy::onArrest(Player& self, Player& attacker, Game& game){
  std::cout << "Spy sees " << attacker.getName() << " has " << attacker.getCoins() << " coins\n";
  attacker.blockAction("arrest");
}
