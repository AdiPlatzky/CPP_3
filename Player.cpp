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

void Player::blockAction(const std::string& actionName) {
  blockedActions.insert(actionName);
}

bool Player::isBlocked(const std::string& actionName) const {
  return blockedActions.count(actionName) > 0;
}

void Player::clearBlocks() {
  blockedActions.clear();
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