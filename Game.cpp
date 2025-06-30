
//
// 12adi45@gmail.com
//
#include <iostream>
#include "Game.h"
#include "Player.h"
#include "Role.h"
#include <stdexcept>
#include <string>
#include <map>

#include "Roles/RoleFactory.h"

Game::Game(QObject *parent) : QObject(parent){}
Game::~Game(){}

static std::map<std::pair<std::string, std::string>, int> arrestHistory;

void Game::addPlayer(const std::shared_ptr<Player>& player){
  if(gameOver){
    throw std::runtime_error("Cannot add player after game has ended.");
  }
  this->players.push_back(player);
}

const std::vector<std::shared_ptr<Player>>& Game::getPlayer() const {
  return players;
}

const std::vector<std::string> Game::getActivePlayers() const{
  std::vector<std::string> names;
  for(const auto& p : players){
    if(p->isActive()){
      names.push_back(p->getName());
    }
  }
  return names;
}

Player& Game::getCurrentPlayer(){
  if(players.empty()){
    throw std::runtime_error("No player in the game.");
  }
  // לדלג על שחקנים לא פעילים
  while(!players[currentTurnIndex]->isActive()){
    currentTurnIndex = (currentTurnIndex + 1) % players.size();
  }
  return *players[currentTurnIndex];
}

void Game::nextTurn(){
  if(gameOver) return;
  // מעבר לתור הבא
  do{
    currentTurnIndex = (currentTurnIndex + 1) % players.size();
  } while(!players[currentTurnIndex]->isActive());

  checkGameOver();
  // ניקוי חסימות של שחקן נוכחי
  Player& current = getCurrentPlayer();
  current.clearBlocks();
  current.getRole()->onTurnStart(current, *this);


  if(current.getCoins() >= 10){
    std::cout << current.getName() << "must perform a coup this turn!\n";
  }
}

std::string Game::getWinner() const{
  if(gameOver == false){
    throw std::runtime_error("Game is not over yet.");
  }
  for(const auto& p : players){
    if(p->isActive()){
      return p->getName();
    }
  }
  throw std::runtime_error("No active player found.");
}

int& Game::getCoinPool(){
  return coinPool;
}

void Game::addToCoinPool(int coin){
  this->coinPool += coin;
}

bool Game::isGameOver() const{
  if(players.size() > 1){
     return false;
  }
  return true;
}

void Game::checkGameOver(){
  int activeCount = 0;
  for(const auto& p : players){
    if(p->isActive()){
      activeCount++;
    }
  }
  if(activeCount <= 1){
    gameOver = true;

    for (const auto& p : players) {
      if (p->isActive()) {
        emit gameOverSignal(QString::fromStdString(p->getName()));
        break;
      }
    }
  }
}

// הגדרת פעולות
ActionResult Game::performGather(Player& player){
  if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (player.getCoins() >= 10) {
    return {false, "You must perform coup because you have a 10 or more coins...remember?"};
  }

  if(player.isBlocked("gather")){
    return {false, player.getName() + "You are bloced from gather"};
  }
  return {true, player.getName() + "is attempting to collect gather (1 coins)", true}; // requiresBlocking = true

  if(coinPool < 1){
    throw std::runtime_error("No coins left in the pool.");
  }
  player.addCoins(1);
  coinPool--;
  checkGameOver();
  return {true, player.getName() + " עוד יום עוד אגורה"};
}

void Game::applyGather(Player &player) {
  if(coinPool < 1){
    throw std::runtime_error("No coins left in the pool.");
  }
  player.addCoins(1);
  coinPool--;
  checkGameOver();
  // return {true, player.getName() + " עוד יום עוד אגורה"};

}



ActionResult Game::performTax(Player& player){
 if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (player.getCoins() >= 10) {
    return {false, "You must perform coup because you have a 10 or more coins...remember?"};
  }

  if(player.isBlocked("tax")){
    return {false, "You  are blocked from takes."};
  }

  // for(auto& p : players){
  //   if(p.get() != &player && p->isActive()){
  //     p->getRole()->onTax(player, *p, *this);
  //   }
  // }
  //
  // player.getRole()->onTax(player, player, *this);
  //
  // int baseCoins = 2;
  // if(coinPool < baseCoins){
  //   return {false, "Not enough coins in the pool."};
  // }
  //
  // player.addCoins(baseCoins);
  // coinPool -= baseCoins;
  // checkGameOver();
  //
  // return {true, player.getName() + " עוד יום עוד 2 אגורות ", true};
  return {true, player.getName() + "is attempting to collect taxes (2 coins)", true}; // requiresBlocking = true
}

void Game::applyTax(Player& player) {
  int baseCoins = 2;

  for(auto& p : players){
    if(p.get() != &player && p->isActive()){
      p->getRole()->onTax(player, *p, *this);
    }
  }

  player.getRole()->onTax(player, player, *this);

  if(coinPool < baseCoins){
    std::string("Not enough coins in the pool.");
  }

  player.addCoins(baseCoins);
  coinPool -= baseCoins;
  checkGameOver();

  // return {true, player.getName() + " עוד יום עוד 2 אגורות ", true};
}


ActionResult Game::performBribe(Player& player){
 if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (player.getCoins() >= 10) {
    return {false, "You must perform coup because you have a 10 or more coins...remember?"};
  }

  if(player.getCoins() < 4){
    return {false, "You don't have enough coins to bribe."};
  }
  // player.removeCoins(4);
  // coinPool += 4;
  //
  // // כאן ניתן לכל שחקן להזדמנות לחסום את השוחד
  // for(auto& otherPlayer : players){
  //   if(otherPlayer.get() != &player && otherPlayer->isActive()){
  //     otherPlayer->getRole()->onBribe(player, *otherPlayer, *this);
  //   }
  // }
  // // אם עדיין פעיל, מסמן שהשחקן קיבל תור נוסף
  // if(player.isActive()){
  //   player.setBonusAction(true);  // זכאי לבונוס רק אם לא נחסם
  // }
  //
  // if(player.hasBonusAction()){
  //   // תור נוסף מותר
  //   player.consumeBonusAction(); // מנצל אותו
  // }
  // else{
  //   nextTurn(); // אין בונוס או כבר נוצל
  // }
  checkGameOver();
  return {true, player.getName() + "איזה יופי קיבלת בשוחד עוד תור", true};
}

void Game::applyBribe(Player& player) {
  // כאן ניתן לכל שחקן להזדמנות לחסום את השוחד
  for (auto& p : players) {
    if (p.get() != &player && p->isActive()) {
      p->getRole()->onBribe(player, player, *this);
    }
  }

  player.getRole()->onBribe(player, player, *this);

  // אם עדיין פעיל, מסמן שהשחקן קיבל תור נוסף
  if(player.isActive()){
    player.setBonusAction(true);  // זכאי לבונוס רק אם לא נחסם
  }

  if(player.hasBonusAction()){
    // תור נוסף מותר
    player.consumeBonusAction(); // מנצל אותו
  }
  else{
    nextTurn(); // אין בונוס או כבר נוצל
  }
  player.removeCoins(4);
  coinPool += 4;
  checkGameOver();
}

ActionResult Game::performArrest(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (attacker.getCoins() >= 10) {
    return {false, "You must perform coup because you have a 10 or more coins...remember?"};
  }

  if (attacker.isBlocked("arrest")) {
    return {false, "You are blocked from making arrest."};
  }

  if(!target.isActive()){
    return {false, "The target player is inactive! \n Try another player from the game."};
  }

  if(attacker.getName() == target.getName() && &attacker == &target){
    return {false, "Seriously?! Were you planning on arresting yourself? Its not possible to make sanction yourself in this game..."};
  }

  // בדיקה אם arrest חוזר על עצמו
  auto key = std::make_pair(attacker.getName(), target.getName());
  if(arrestHistory[key] == currentTurnIndex){
    return {false, "Cannot arrest same player twice in a row."};
  }

  if(target.getCoins() == 0){
    return {false, "Target has no coins."};
  }
  checkGameOver();
  return {true, attacker.getName() + " is attempting to collect (1 coin) from" + target.getName() + "."};
  //
  // if(!target.isActive()){
  //   return {false, "The target player is inactive! \n Try another player from the game."};
  // }
  //
  // if(attacker.isBlocked("arrest")){
  //   return {false, "You are blocked in this turn, try other active"};
  // }
  //
  // if(attacker.getName() == target.getName() && &attacker == &target){
  //   return {false, "Seriously?! Were you planning on arresting yourself? Its not possible to make sanction yourself in this game..."};
  // }
  // // בדיקה אם arrest חוזר על עצמו
  // auto key = std::make_pair(attacker.getName(), target.getName());
  // if(arrestHistory[key] == currentTurnIndex){
  //   return {false, "Cannot arrest same player twice in a row."};
  // }
  //
  // if(target.getCoins() == 0){
  //   return {false, "Target has no coins."};
  // }
  // // לוקח ממנו מטבע
  // target.removeCoins(1);
  // attacker.addCoins(1);
  // // הפעלת תגובת תפקידים
  // target.getRole()->onArrest(target, attacker, *this);
  //
  // // עדכון היסטוריית arrest
  // arrestHistory[key] = currentTurnIndex;
  // checkGameOver();
  // return {true, attacker.getName() + "שיחקת אותה קיבלת מטבע מ - " + target.getName(), true};
}

void Game::applyArrest(Player& attacker, Player& target) {
  auto key = std::make_pair(attacker.getName(), target.getName());

  // if(!target.isActive()){
  //   return {false, "The target player is inactive! \n Try another player from the game."};
  // }

  // if(attacker.isBlocked("arrest")){
  //   return {false, "You are blocked in this turn, try other active"};
  // }

  // if(attacker.getName() == target.getName() && &attacker == &target){
  //   return {false, "Seriously?! Were you planning on arresting yourself? Its not possible to make sanction yourself in this game..."};
  // }
  // // בדיקה אם arrest חוזר על עצמו
  // auto key = std::make_pair(attacker.getName(), target.getName());
  // if(arrestHistory[key] == currentTurnIndex){
  //   return {false, "Cannot arrest same player twice in a row."};
  // }
  //
  // if(target.getCoins() == 0){
  //   return {false, "Target has no coins."};
  // }

  for (auto& p : players) {
    if (p.get() != &attacker && p->isActive()) {
      p->getRole()->onArrest(attacker, target, *this);
    }
  }

  attacker.getRole()->onArrest(attacker, target, *this);
  // לוקח ממנו מטבע
  target.removeCoins(1);
  attacker.addCoins(1);
  // הפעלת תגובת תפקידים
  // target.getRole()->onArrest(target, attacker, *this);

  // עדכון היסטוריית arrest
  arrestHistory[key] = currentTurnIndex;
  checkGameOver();
  // return {true, attacker.getName() + "שיחקת אותה קיבלת מטבע מ - " + target.getName(), true};

}

ActionResult Game::performSanction(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (attacker.getCoins() >= 10) {
    return {false, "You must perform coup because you have a 10 or more coins...remember?"};
  }

  if(!target.isActive()){
    return {false, "The target player is inactive! \n Try another player from the game."};
  }

  if(attacker.getName() == target.getName() && &attacker == &target){
    return {false, "Seriously?! Were you planning on attacking yourself? Its not possible to make sanction yourself in this game..."};
  }
  if(attacker.getCoins() < 3){
    return {false, "You don't have enough coins to perform sanction."};
  }
  attacker.removeCoins(3);
  coinPool += 3;

  // חסימה של פעולות כלכליות
  target.blockAction("gather",2);
  target.blockAction("tax",2);

  // הפעלת תגובת התפקיד (של הנפגע)
  target.getRole()->onSanction(target, attacker, *this);
  checkGameOver();
  return {true, attacker.getName() + "is attempting to collect sanction (the " + target.getName() + " is blocked from taking Gather/Tax for 1 turn)", true};
}

ActionResult Game::performCoup(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if(!target.isActive()){
    return {false, "The target player is inactive! \n Try another player from the game."};
  }

  if(attacker.getName() == target.getName() && &attacker == &target){
    return {false, "Seriously?! Were you planning on kill yourself? Dont worry, It's not possible in this game...you are protected!"};
  }

  if (attacker.isBlocked("coup")) {
    return {false, "You are blocked from coup action."};
  }

  if(attacker.getCoins() < 7){
    return {false, "You don't have enough coins to perform coup."};
  }
  checkGameOver();
  return {true, attacker.getName() + " is attempting to collect coup (killing " + target.getName() + ")", true}; // requiresBlocking = true

  // if(attacker.getCoins() < 7){
  //   return {false, "You don't have enough coins to perform coup."};
  // }
  // attacker.removeCoins(7);
  // coinPool += 7;
  //
  //
  // // הפעלת תגובת הגנה של ה־target (למשל General)
  // target.getRole()->onCoup(target, attacker, *this);
  //
  // // אם עדיין פעיל – לא נחסם, ולכן מדיחים אותו
  // if(target.isActive()){
  //   target.deactivate();  // כאן הוא הופך ל־isActive = false
  //   emit playerEliminated(
  //     QString::fromStdString(target.getName()), "הודח בהפיכה על ידי " + QString::fromStdString(attacker.getName()));
  // }
  // checkGameOver();
  // return {true, attacker.getName() + "You killed: " + target.getName(), true};
}

void Game::applyCoup(Player &attacker, Player &target) {
  // if(attacker.getCoins() < 7){
  //   return {false, "You don't have enough coins to perform coup."};
  // }


  for (auto& p : players) {
    if (p.get() != &attacker && p->isActive()) {
      p->getRole()->onCoup(attacker, target, *this);
    }
  }

  attacker.getRole()->onCoup(attacker, target, *this);


  // // הפעלת תגובת הגנה של ה־target (למשל General)
  // target.getRole()->onCoup(target, attacker, *this);

  // אם עדיין פעיל – לא נחסם, ולכן מדיחים אותו
  if(target.isActive()){
    target.deactivate();  // כאן הוא הופך ל־isActive = false
    emit playerEliminated(
      QString::fromStdString(target.getName()), "הודח בהפיכה על ידי " + QString::fromStdString(attacker.getName()));
  }
  attacker.removeCoins(7);
  coinPool += 7;
  checkGameOver();
  // return {true, attacker.getName() + "You killed: " + target.getName(), true};
}


ActionResult Game::performInvest(Player& player) {
  if(!player.isActive()){
    throw std::runtime_error("Dear Baron you are Inactive - yours game is over!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("Dear Baron: It's not your turn!");
  }

  player.getRole()->onInvest(player, *this);
  checkGameOver();
  return {true, player.getName() + "השקעה טובה ברון"};
}

std::vector<std::string> Game::getPlayersWhoCanBlock(const std::string& actionName,
                                                     const Player* attacker) const {
  std::vector<std::string> result;
for (const auto& p : players) {
    if (!p->isActive() || p.get() == attacker) continue;
    if (p->canBlock(actionName)) {
      result.push_back(p->getName());
    }
  }
  return result;
}


