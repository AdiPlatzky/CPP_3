//
// Created by 12adi on 11/06/2025.
//

#ifndef GAME_H
#define GAME_H

#include <vector>
#include <memory>
#include <string>
#include <optional>

class Player;
class Game {
  private:
    std::vector<std::shared_ptr<Player>> players;
    int currentTurnIndex = 0;
    int coinPool = 50;
    bool gameOver = false;

    public:
      Game();
      ~Game();
      // ניהול שחקנים
      void addPlayer(const std::shared_ptr<Player>& player);
      const std::vector<std::string> getActivePlayers() const;

      // תור
      Player& getCurrentPlayer();
      void nextTurn();

      // פעולות
      void performGather(Player& player);
      void performTax(Player& player);
      void performBribe(Player& player);
      void performArrest(Player& attacker, Player& terget);
      void performSanction(Player& attacker, Player& target);
      void performCoup(Player& attacker, Player& target);

      // זכייה
      std::string getWinner() const;

     // תוספת לשימוש פנימי
      int& getCoinPool();  // קופה מרכזית
      void addToCoinPool(int coinPool); // להוסיף לקופה המרכזית
      bool isGameOver() const;

};

#endif //GAME_H
