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
#include <QObject>

class Player;
class Game : public QObject {
    Q_OBJECT


  private:
    std::vector<std::shared_ptr<Player>> players;
    int currentTurnIndex = 0;
    int coinPool = INFINITY;
    bool gameOver = false;

    public:
      Game(QObject* parent = nullptr);
      ~Game();
      // ניהול שחקנים
      void addPlayer(const std::shared_ptr<Player>& player);
      const std::vector<std::shared_ptr<Player>>& getPlayer() const;
      const std::vector<std::string> getActivePlayers() const;

      // תור
      Player& getCurrentPlayer();
      void nextTurn();
      bool isGameOver() const;
      std::string getWinner() const; // זכייה
      // תוספת לשימוש פנימי
      int& getCoinPool();  // קופה מרכזית
      void addToCoinPool(int coinPool); // להוסיף לקופה המרכזית
      void checkGameOver();


      // פעולות
      std::string performGather(Player& player);
      std::string performTax(Player& player);
      std::string performBribe(Player& player);
      std::string performArrest(Player& attacker, Player& terget);
      std::string performSanction(Player& attacker, Player& target);
      std::string performCoup(Player& attacker, Player& target);
      std::string performInvest(Player& player);

    signals:
    void gameOverSignal(const QString& winnerName);
    void playerEliminated(const QString& playerName, const QString& reason);

};

#endif //GAME_H
