//
// Created by 12adi on 26/06/2025.
//

#ifndef PLAYERREGISTRATIONSCREEN_H
#define PLAYERREGISTRATIONSCREEN_H
#include <QWidget>
#include <vector>
#include <memory>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
# include <QCheckBox>
#include "../Player.h"
#include "../Roles/RoleFactory.h"


class PlayerRegistrationScreen : public QWidget {
  Q_OBJECT

public:
  explicit PlayerRegistrationScreen(QWidget *parent = nullptr);

  signals:
    void playerRegistered(const std::vector<std::shared_ptr<Player>>& players);

  private slots:
    void updateNameFields(int playerCount);
  void startGame();
  void goBack();


private:
  QSpinBox *playerCountSpin;
  QPushButton *startButton;
  QVBoxLayout *mainLayout;
  QVBoxLayout *formLayout;
  std::vector<QLineEdit*> nameFields;
  RoleFactory roleFactory;
  QCheckBox *useRandomNamesCheckBox;

  QCheckBox *showActionsCheckBox;

  QPushButton *backButton;
};

#endif //PLAYERREGISTRATIONSCREEN_H
