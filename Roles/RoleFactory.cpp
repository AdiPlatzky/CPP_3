//
// Created by 12adi on 23/06/2025.
//

#include "RoleFactory.h"
#include "Baron.h"
#include "General.h"
#include "Governor.h"
#include "Judge.h"
#include "Merchant.h"
#include "Spy.h"
#include <cstdlib>
#include <ctime>


std::unique_ptr<Role> RoleFactory::createRandomRole(){
  static bool seeded = false;
  if(!seeded){
    std::srand(std::time(nullptr));
    seeded = true;
  }
  int randomNumber = std::rand() % 6;
  switch(randomNumber){
    case 0: return std::make_unique<Baron>();
    case 1: return std::make_unique<General>();
    case 2: return std::make_unique<Governor>();
    case 3: return std::make_unique<Judge>();
    case 4: return std::make_unique<Merchant>();
    case 5: return std::make_unique<Spy>();
    default: std::make_unique<Baron>();
  }
}
