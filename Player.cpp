//
// 12adi45@gmail.com
//

#include "Player.h"
#include "Role.h"
#include <stdexcept>

Player::Player(const std::string& name, std::unique_ptr<Role> role)
    : name(name), coins(0), active(true) {
  if(!role) {
    throw std::invalid_argument("Role cannot be null");
  }
  this->role = std::move(role);
}

Player::~Player() {}

const std::string& Player::getName() const {
  return name;
}


int Player::getCoins() const {
  return coins;
}

Role* Player::getRole() const {
  return role.get();
}

bool Player::isActive() const {
  return active;
}
// זור לזה -אני לא בטוחה בבדיקה שעשיתי!!
void Player::addCoins(int amount) {
  if(amount < 0) {
    throw std::invalid_argument("Cannot add negative coins");
  }
  coins += amount;
}


void Player::removeCoins(int amount) {
  if(amount > coins) {
    throw std::runtime_error("Not enough coins.");
  }
  coins -= amount;
}

void Player::deactivate() {
  active = false;
}

void Player::blockAction(const std::string& actionName, int turns = 1) {
  blockedActions[actionName]= turns;
}

bool Player::isBlocked(const std::string& actionName) const {
  auto it = blockedActions.find(actionName);
  return it != blockedActions.end() && it->second > 0;
}

void Player::clearBlocks() {
  for (auto it = blockedActions.begin(); it != blockedActions.end(); ) {
    if (--(it->second) <= 0) {
      it = blockedActions.erase(it);
    } else {
      ++it;
    }
  }
}

bool Player::hasBonusAction() const {
  return bonusActionPending && !bonusActionUsed;
}

void Player::setBonusAction(bool active){
  bonusActionPending = active;
  bonusActionUsed = false;
}

bool Player::consumeBonusAction() {
  if(hasBonusAction()) {
    bonusActionUsed = true;
    return true;
  }
  return false;
}

bool Player::canBlock(const std::string& actionName) const {
  return role && role->canBlock(actionName);
}


