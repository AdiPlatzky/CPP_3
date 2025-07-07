
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
/**
 * בונה מופע חדש של מחלקת Game.
 * @param parent מצביע לאובייקט האב (משמש ל־GUI, ברירת מחדל nullptr)
 */
Game::Game(QObject *parent) : QObject(parent){}

/**
 * הורס את מופע המשחק. אין צורך בשחרור משאבים מיוחד כי כל השחקנים מנוהלים ע"י מצביעים חכמים.
 */
Game::~Game(){}

static std::map<std::pair<std::string, std::string>, int> arrestHistory;

/**
 * מוסיף שחקן למשחק.
 * זורק חריגה אם המשחק כבר נגמר.
 * @param player מצביע חכם לשחקן חדש
 * @throws std::runtime_error אם מנסים להוסיף שחקן אחרי שהמשחק הסתיים
 */
void Game::addPlayer(const std::shared_ptr<Player>& player){
  if(gameOver){
    throw std::runtime_error("Cannot add player after game has ended.");
  }
  this->players.push_back(player);
}

/**
 * מחזיר וקטור קבוע של כל השחקנים (פעילים ומודחים).
 */
const std::vector<std::shared_ptr<Player>>& Game::getPlayer() const {
  return players;
}

/**
 * מאתר שחקן לפי שם ומחזיר מצביע. אם לא נמצא, מחזיר nullptr.
 * @param name שם לחיפוש
 * @return מצביע לשחקן או nullptr אם לא נמצא
 */
Player* Game::getPlayerByName(const std::string& name) {
  for (auto& p : players) {
    if (p->getName() == name) {
      return p.get();
    }
  }
  return nullptr;
}

/**
 * מחזיר רשימה של כל השמות של השחקנים הפעילים במשחק.
 * @return וקטור שמות
 */
const std::vector<std::string> Game::getActivePlayers() const{
  std::vector<std::string> names;
  for(const auto& p : players){
    if(p->isActive()){
      names.push_back(p->getName());
    }
  }
  return names;
}

/**
 * מסמן חסימת arrest עתידית – כאשר שחקן מסוג Spy בוחר שחקן אחר לחסימה בתור הבא.
 * @param spy המרגל
 * @param target השחקן שייחסם
 */
void Game::markPlannedArrest(Player& spy, Player& target) {
  plannedArrests[target.getName()] = spy.getName();  // spy יחסום את arrest על target
}

/**
 * מחזיר רפרנס למפה של כל החסימות העתידיות (קריאה בלבד).
 * @return map של שם שחקן לחוסם
 */
const std::map<std::string, std::string>& Game::getPlannedArrests() const {
  return plannedArrests;
}

/**
 * בודק אם לשחקן יש חסימת arrest תקפה לתור הקרוב.
 * @param target שחקן לבדיקה
 * @return true אם יש חסימה, אחרת false
 */
bool Game::isArrestBlockedNextTurn(Player& target) const {
  return plannedArrests.count(target.getName()) > 0;
}

/**
 * מנקה חסימת arrest עתידית מהשחקן הנתון (ברגע שנגמרה/נוצלה).
 * @param target שחקן
 */
void Game::clearPlannedArrest(Player& target) {
  plannedArrests.erase(target.getName());
}

/**
 * מחזיר רפרנס לשחקן הנוכחי בתור.
 * מדלג על שחקנים מודחים עד שנמצא שחקן פעיל.
 * @return רפרנס לשחקן
 * @throws std::runtime_error אם אין שחקנים במשחק
 */
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

/**
 * עובר לתור הבא. מדלג על שחקנים מודחים.
 * מאפס חסימות/בונוסים של השחקן החדש ומבצע בדיקות אוטומטיות (כמו forced coup).
 */
void Game::nextTurn(){
  if(gameOver) return;

  // מעבר לתור הבא
  do {
    currentTurnIndex = (currentTurnIndex + 1) % players.size();
  } while(!players[currentTurnIndex]->isActive());

  checkGameOver();

  // ניקוי חסימות של שחקן נוכחי
  Player& current = getCurrentPlayer();
  current.clearBlocks();

  // ✨ בדיקה אם יש חסימת arrest מהמרגל
  if (isArrestBlockedNextTurn(current)) {
    clearPlannedArrest(current);  // מנקה את החסימה – היא תקפה רק לתור אחד
  }

  // הפעלת תכונות התחלת תור
  current.getRole()->onTurnStart(current, *this);

  if(current.getCoins() >= 10){
    std::cout << current.getName() << " must perform a coup this turn!\n";
  }

  current.setBribeUsedThisTurn(false);

}

/**
 * מחזיר את שם המנצח (הפעיל האחרון).
 * @throws std::runtime_error אם המשחק טרם הסתיים או אין מנצח.
 */
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

/**
 * מחזיר הפניה לערך הקופה המרכזית.
 * @return רפרנס למשתנה הקופה
 */
int& Game::getCoinPool(){
  return coinPool;
}

/**
 * מוסיף ערך למספר המטבעות בקופה המרכזית.
 * @param coin מספר המטבעות להוסיף
 */
void Game::addToCoinPool(int coin){
  this->coinPool += coin;
}

/**
 * בודק האם המשחק הסתיים (אם נשאר שחקן פעיל יחיד).
 * @return true אם הסתיים, אחרת false
 */
bool Game::isGameOver() const {
  int active = 0;
  for(const auto& p : players) {
    if(p->isActive()) {
      active++;
    }
  }
  return active <= 1;
}

/**
 * מבצע בדיקה האם המשחק נגמר – מסמן אם כן ומשדר סיגנל.
 */
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
/**
 * פונקציה בודקת האם שחקן יכול לבצע gather, ומחזירה תוצאה (כולל חסימות/שגיאות).
 * @param player שחקן
 * @return תוצאת הפעולה
 */
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
    return {false, player.getName() + "You are blocked from gather"};
  }

  return {true, player.getName() + "is attempting to collect gather (1 coins)", true}; // requiresBlocking = true
}


/**
 * מממש בפועל את פעולת gather – מוסיף מטבע לשחקן ומוריד מהקופה.
 * @param player השחקן המבצע
 * @throws std::runtime_error אם אין מטבעות בקופה
 */
void Game::applyGather(Player &player) {
  if(coinPool < 1){
    throw std::runtime_error("No coins left in the pool.");
  }
  player.addCoins(1);
  this->coinPool--;
  checkGameOver();
}


/**
 * בודק אם ניתן לבצע tax, ומחזיר תוצאת פעולה.
 * @param player שחקן
 * @return תוצאה
 */
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
  return {true, player.getName() + "is attempting to collect taxes (2 coins)", true}; // requiresBlocking = true
}

/**
 * מממש בפועל את גביית המיסים – מוסיף לשחקן ומוריד מהקופה, מפעיל אפקטים של תפקידים.
 * @param player שחקן
 * @throws std::runtime_error אם אין מספיק מטבעות בקופה
 */
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
  this->coinPool -= baseCoins;
  checkGameOver();
}

/**
 * בודק האם ניתן לבצע פעולה bribe, ומחזיר תוצאה (כולל שגיאות וחסימות).
 * @param player שחקן
 * @return תוצאת פעולה
 */
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

  if(player.isBlocked("bribe")){
    return {false, "You  are blocked from bribe."};
  }

  if (player.hasUsedBribeThisTurn()) {
    throw std::runtime_error("Cannot bribe twice in the same turn!");
  }
  player.setBribeUsedThisTurn(true);

  if(player.getCoins() < 4){
    return {false, "You don't have enough coins to bribe."};
  }

  player.removeCoins(4);
  coinPool += 4;
  checkGameOver();
  return {true, player.getName() + "איזה יופי קיבלת בשוחד עוד תור", true};
}

/**
 * מממש את פעולת השוחד בפועל – מפעיל אפשרות לבונוס, חסימות, עדכון תור.
 * @param player שחקן
 */
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
  checkGameOver();
}

/**
 * בודק אם ניתן לבצע arrest, כולל חסימות עתידיות, ומחזיר תוצאה.
 * @param attacker התוקף
 * @param target הנעצור
 * @return תוצאה
 */
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


  if (isArrestBlockedNextTurn(target)) {
    clearPlannedArrest(target);
    return {false, "המרגל חסם את פעולת המעצר על השחקן הזה בתורו!"};
  }


  if (!target.isActive()) {
    throw std::runtime_error("Target player is inactive! Try another player.");
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

}

/**
 * מממש את פעולת המעצר בפועל – לוקח מטבע, מעדכן היסטוריה.
 * @param attacker התוקף
 * @param target המותקף
 */
void Game::applyArrest(Player& attacker, Player& target) {
  auto key = std::make_pair(attacker.getName(), target.getName());

  for (auto& p : players) {
    if (p.get() != &attacker && p->isActive()) {
      p->getRole()->onArrest(attacker, target, *this);
    }
  }

  attacker.getRole()->onArrest(attacker, target, *this);
  // לוקח ממנו מטבע
  target.removeCoins(1);
  attacker.addCoins(1);

  // עדכון היסטוריית arrest
  arrestHistory[key] = currentTurnIndex;
  checkGameOver();
}

/**
 * בודק אם ניתן לבצע סנקציה (sanction).
 * @param attacker התוקף
 * @param target הקורבן
 * @return תוצאת פעולה
 */
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

  if (!target.isActive()) {
    throw std::runtime_error("Target player is inactive! Try another player.");
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

/**
 * בודק אם ניתן לבצע הפיכה (coup), כולל תנאי חוקי, ומחזיר תוצאה.
 * @param attacker תוקף
 * @param target קורבן
 * @return תוצאת פעולה
 */
ActionResult Game::performCoup(Player& attacker, Player& target){
  if(!attacker.isActive()){
    throw std::runtime_error("This player is Inactive!");
  }

  if (&attacker != &getCurrentPlayer()) {
    throw std::runtime_error("It's not your turn!");
  }

  if (!target.isActive()) {
    throw std::runtime_error("Target player is inactive! Try another player.");
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
  attacker.removeCoins(7);
  coinPool += 7;
  checkGameOver();
  return {true, attacker.getName() + " is attempting to collect coup (killing " + target.getName() + ")", true}; // requiresBlocking = true
}

/**
 * מממש את פעולת ההפיכה – מדיח שחקן מהמשחק ומפעיל סיגנל ל־GUI.
 * @param attacker תוקף
 * @param target מודח
 */
void Game::applyCoup(Player &attacker, Player &target) {
  for (auto& p : players) {
    if (p.get() != &attacker && p->isActive()) {
      p->getRole()->onCoup(attacker, target, *this);
    }
  }

  attacker.getRole()->onCoup(attacker, target, *this);

  // אם עדיין פעיל – לא נחסם, ולכן מדיחים אותו
  if(target.isActive()){
    target.deactivate();  // כאן הוא הופך ל־isActive = false
    emit playerEliminated(
      QString::fromStdString(target.getName()), "הודח בהפיכה על ידי " + QString::fromStdString(attacker.getName()));
  }
  checkGameOver();
}

/**
 * בודק אם ברון יכול להשקיע – ואם כן, מבצע את פעולת ההשקעה עבור הברון.
 * @param player הברון
 * @return תוצאה
 */
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

/**
 * מחזיר map של שמות שחקנים פעילים ומספר המטבעות שיש לכל אחד.
 * @return מפת שם→מטבעות
 */
std::map<std::string, int> Game::getPlayersCoinCounts() const {
  std::map<std::string, int> result;
  for (const auto& p : players) {
    if (p->isActive()) {
      result[p->getName()] = p->getCoins();
    }
  }
  return result;
}


/**
 * מחזיר רשימה של שמות שחקנים שיכולים לחסום פעולה מסוימת (לפי סוג הפעולה).
 * @param actionName שם הפעולה
 * @param attacker תוקף
 * @return וקטור שמות
 */
std::vector<std::string> Game::getPlayersWhoCanBlock(const std::string& actionName,
                                                     const Player* attacker) const {
  std::vector<std::string> result;
  if (actionName == "arrest") {
    for (const auto& entry : plannedArrests) {
      if (entry.first == attacker->getName()) {
        return {};
      }
    }
  }
  for (const auto& p : players) {
    if (!p->isActive() || p.get() == attacker) continue;
    if (p->canBlock(actionName)) {
      result.push_back(p->getName());
    }
  }
  return result;
}

