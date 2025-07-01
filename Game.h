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
#include <limits>

struct ActionResult {
  bool success;
  std::string message;
  bool requiresBlocking = false;
};

class Player;
class Game : public QObject {
    Q_OBJECT


  private:
    std::vector<std::shared_ptr<Player>> players;
    int currentTurnIndex = 0;
    int coinPool = std::numeric_limits<int>::max();
    bool gameOver = false;

    public:
      Game(QObject* parent = nullptr);
      ~Game();
      // ניהול שחקנים
      void addPlayer(const std::shared_ptr<Player>& player);
      Player* getPlayerByName(const std::string& name);
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
    ActionResult performGather(Player& player);
    void applyGather(Player& player);
    std::map<std::string, int> getPlayersCoinCounts() const;
    std::map<std::string, std::string> plannedArrests;
    void markPlannedArrest(Player& spy, Player& target);
    bool isArrestBlockedNextTurn(Player& target) const;
    void clearPlannedArrest(Player& target);
    const std::map<std::string, std::string>& getPlannedArrests() const;



    ActionResult performTax(Player& player);
    void applyTax(Player& player);

    ActionResult performBribe(Player& player);
    void applyBribe(Player& player);

    ActionResult performArrest(Player& attacker, Player& terget);
    void applyArrest(Player& attacker, Player& terget);

    ActionResult performSanction(Player& attacker, Player& target);

    ActionResult performCoup(Player& attacker, Player& target);
    void applyCoup(Player& attacker, Player& target);

    ActionResult performInvest(Player& player);

    std::vector<std::string> getPlayersWhoCanBlock(const std::string &actionName, const Player *attacker) const;

    // std::string performGather(Player& player);
      // std::string performTax(Player& player);
      // std::string performBribe(Player& player);
      // std::string performArrest(Player& attacker, Player& terget);
      // std::string performSanction(Player& attacker, Player& target);
      // std::string performCoup(Player& attacker, Player& target);
      // std::string performInvest(Player& player);

    signals:
    void gameOverSignal(const QString& winnerName);
    void playerEliminated(const QString& playerName, const QString& reason);

};

#endif //GAME_H
