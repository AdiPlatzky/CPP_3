//
// Created by 12adi on 19/06/2025.
//
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "Roles/Baron.h"
#include "Roles/Governor.h"
#include "Roles/Judge.h"
#include "Roles/General.h"
#include "Roles/Spy.h"
#include "Roles/Merchant.h"
#include <memory>
using namespace std;

int main() {
  Game game;

  auto player1 = std::make_shared<Player>("Alice", std::make_unique<Governor>());
  auto player2 = std::make_shared<Player>("Bob", std::make_unique<Spy>());
  auto player3 = std::make_shared<Player>("Carol", std::make_unique<Judge>());
  auto player4 = std::make_shared<Player>("Dan", std::make_unique<General>());

  game.addPlayer(player1);
  game.addPlayer(player2);
  game.addPlayer(player3);
  game.addPlayer(player4);

  try {
    // Turn 1 - Alice (Governor)
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl << std::endl;
    game.performTax(*player1); // Governor should get 3 coins
    game.nextTurn();

    // Turn 2 - Bob (Spy)
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl << std::endl;
    game.performGather(*player2);
    game.nextTurn();

    // Turn 3 - Carol (Judge)
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl << std::endl;
    game.performBribe(*player3);
    game.nextTurn();

    // Turn 4 - Dan (General)
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl << std::endl;
    game.performArrest(*player4, *player1);
    game.nextTurn();

    // Turn 1 again - Alice
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl << std::endl;
    game.performSanction(*player1, *player3);
    game.nextTurn();

    // Fake: Pump Dan to 10 coins, test forced coup
    player4->addCoins(10);
    std::cout << "Turn: " << game.getCurrentPlayer().getName() << std::endl;
    game.performCoup(*player4, *player2);  // Dan coups Bob
    game.nextTurn();

    std::cout << "Winner: " << game.getWinner() << std::endl;
  }
  catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
  }
  return 0;

}