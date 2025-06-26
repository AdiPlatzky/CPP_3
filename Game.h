//
// 12adi45@gmail.com
//

#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <optional>

class Player;
class Game {
  private:
    std::vector<std::shared_ptr<Player>> players;
    int currentTurnIndex = 0;
    int coinPool = INFINITY;
    bool gameOver = false;

    public:
      Game();
      ~Game();
      // ניהול שחקנים
      void addPlayer(const std::shared_ptr<Player>& player);
      const std::vector<std::shared_ptr<Player>>& getPlayer() const;
      const std::vector<std::string> getActivePlayers() const;

      // תור
      Player& getCurrentPlayer();
      void nextTurn();

      // פעולות
      std::string performGather(Player& player);
      std::string performTax(Player& player);
      std::string performBribe(Player& player);
      std::string performArrest(Player& attacker, Player& terget);
      std::string performSanction(Player& attacker, Player& target);
      std::string performCoup(Player& attacker, Player& target);
      std::string performInvest(Player& player);

      // זכייה
      std::string getWinner() const;

     // תוספת לשימוש פנימי
      int& getCoinPool();  // קופה מרכזית
      void addToCoinPool(int coinPool); // להוסיף לקופה המרכזית
      bool isGameOver() const;
      void checkGameOver();


};

#endif //GAME_H
