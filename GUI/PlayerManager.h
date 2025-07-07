//
// Created by 12adi on 26/06/2025.
//

#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H
#include "../Roles/RoleFactory.h"
#include <QString>
#include <iostream>



class PlayerManager {
public:
  PlayerManager();
  void createPlayers(const QVector<QString> &names);
  const std::vector<std::shared_ptr<Player>> &getPlayers() const;

  private:
    std::vector<std::shared_ptr<Player>> players;
    RoleFactory roleFactory;

};



#endif //PLAYERMANAGER_H
