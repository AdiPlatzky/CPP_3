//
// Created by 12adi on 23/06/2025.
//

#ifndef ROLEFACTORY_H
#define ROLEFACTORY_H

#include <memory>
#include "../Role.h"

class RoleFactory {
  public:
    static std::unique_ptr<Role> createRandomRole();
    static std::unique_ptr<Role> createRole(Player &player);


};



#endif //ROLEFACTORY_H
