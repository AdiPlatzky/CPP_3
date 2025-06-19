
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

Game::Game(){}
Game::~Game(){}

static std::map<std::pair<std::string, std::string>, int> arrestHistory;

void Game::addPlayer(const std::shared_ptr<Player>& player){
  if(gameOver){
    throw std::runtime_error("Cannot add player after game has ended.");
  }
  this->players.push_back(player);
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
  }
}

// הגדרת פעולות
void Game::performGather(Player& player){
  if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if (player.getCoins() >= 10) {
    throw std::runtime_error("Player must perform a coup with 10 or more coins.");
  }

  if(player.isBlocked("gather")){
    throw std::runtime_error("Player is blocked from gathering.");
  }

  if(coinPool < 1){
    throw std::runtime_error("No coins left in the pool.");
  }
  player.addCoins(1);
  coinPool--;
  checkGameOver();
}

void Game::performTax(Player& player){
 if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if (player.getCoins() >= 10) {
    throw std::runtime_error("Player must perform a coup with 10 or more coins.");
  }

  if(player.isBlocked("tax")){
    throw std::runtime_error("Player is blocked from performing tax.");
  }

  for(auto& p : players){
    if(p.get() != &player && p->isActive()){
      p->getRole()->onTax(player, *p, *this);
    }
  }

  player.getRole()->onTax(player, player, *this);

  int baseCoins = 2;
  if(coinPool < baseCoins){
    throw std::runtime_error("Not enuogh coins in the pool.");
  }

  player.addCoins(baseCoins);
  coinPool -= baseCoins;
  checkGameOver();
}

void Game::performBribe(Player& player){
 if(!player.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if (player.getCoins() >= 10) {
    throw std::runtime_error("Player must perform a coup with 10 or more coins.");
  }

  if(player.getCoins() < 4){
    throw std::runtime_error("Not enough coins to bribe.");
  }
  player.removeCoins(4);
  coinPool += 4;

  // כאן ניתן לכל שחקן להזדמנות לחסום את השוחד
  for(auto& otherPlayer : players){
    if(otherPlayer.get() != &player && otherPlayer->isActive()){
      otherPlayer->getRole()->onBribe(player, *otherPlayer, *this);
    }
  }
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
  checkGameOver();
}

void Game::performArrest(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("The attacker player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if (attacker.getCoins() >= 10) {
    throw std::runtime_error("The attacker player must perform a coup with 10 or more coins.");
  }

  if(!target.isActive()){
    throw std::runtime_error("The target player is inactive! \n Try another player from the game.");
  }

  if(attacker.isBlocked("arrest")){
    throw std::runtime_error("This player's is blocked this turn.");
  }

  if(&attacker == &target){
    throw std::runtime_error("Seriously?! Were you planning on arresting yourself? Its not possible to make sanction yourself in this game...");
  }
  // בדיקה אם arrest חוזר על עצמו
  auto key = std::make_pair(attacker.getName(), target.getName());
  if(arrestHistory[key] == currentTurnIndex){
    throw std::runtime_error("Cannot arrest same player twice in a row.");
  }

  if(target.getCoins() == 0){
    throw std::runtime_error("Target has no coins.");
  }
  // לוקח ממנו מטבע
  target.removeCoins(1);
  attacker.addCoins(1);
  // הפעלת תגובת תפקידים
  target.getRole()->onArrest(target, attacker, *this);

  // עדכון היסטוריית arrest
  arrestHistory[key] = currentTurnIndex;
  checkGameOver();
}

void Game::performSanction(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("The attacker player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if (attacker.getCoins() >= 10) {
    throw std::runtime_error("The attacker player must perform a coup with 10 or more coins.");
  }

  if(!target.isActive()){
    throw std::runtime_error("The target player is inactive! \n Try another player from the game.");
  }

  if(&attacker == &target){
    throw std::runtime_error("Seriously?! Were you planning on attacking yourself? Its not possible to make sanction yourself in this game...");
  }
  if(attacker.getCoins() < 3){
    throw std::runtime_error("Not enough coins to perform sanction.");
  }
  attacker.removeCoins(3);
  coinPool += 3;

  // חסימה של פעולות כלכליות
  target.blockAction("gather");
  target.blockAction("tax");

  // הפעלת תגובת התפקיד (של הנפגע)
  target.getRole()->onSanction(target, attacker, *this);
  checkGameOver();
}

void Game::performCoup(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("The attacker player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

  if(!target.isActive()){
    throw std::runtime_error("The target player is inactive! \n Try another player from the game.");
  }

  if(&attacker == &target){
    throw std::runtime_error("Seriously?! Were you planning on kill yourself? Dont worry, It's not possible in this game...you are protected!");
  }

  if(attacker.getCoins() < 7){
    throw std::runtime_error("Not enough coins to perform coup.");
  }
  attacker.removeCoins(7);
  coinPool += 7;

  // הפעלת תגובת הגנה של ה־target (למשל General)
  target.getRole()->onCoup(target, attacker, *this);

  // אם עדיין פעיל – לא נחסם, ולכן מדיחים אותו
  if(target.isActive()){
    target.deactivate();
  }
  checkGameOver();
}

void Game::performInvest(Player& player) {
  if(!player.isActive()){
    throw std::runtime_error("Dear Baron you are Inactive - yours game is over!");
  }

  if (&player != &getCurrentPlayer()) {
    throw std::runtime_error("Not your turn.");
  }

    player.getRole()->onInvest(player, *this);
    checkGameOver();
}
