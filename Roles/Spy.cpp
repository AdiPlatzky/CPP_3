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

void Spy::onArrest(Player&, Player& attacker, Game&){
  std::cout << "Spy sees " << attacker.getName() << " has " << attacker.getCoins() << " coins\n";
  attacker.blockAction("arrest",1);
}

bool Spy::canBlock(const std::string &actionName) const {
    return actionName == "arrest";
}

