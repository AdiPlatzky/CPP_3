#include "MainWindow.h"
#include "InstructionsWindow.h"
#include "GameBoardWindow.h"
#include "PlayerRegistrationScreen.h"
#include <QPushButton>
#include <QVBoxLayout>

#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
  setWindowTitle("תפריט ראשי - Coup");
  resize(300, 200);

  layout = new QVBoxLayout(this);

  newGameButton = new QPushButton("New Game", this);
  instructionsButton = new QPushButton("הוראות משחק", this);

  layout->addWidget(newGameButton);
  layout->addWidget(instructionsButton);

  connect(newGameButton, &QPushButton::clicked, this, &MainWindow::openPlayerRegistration);
  connect(instructionsButton, &QPushButton::clicked, this, &MainWindow::openInstructions);
  //
  // auto *central = new QWidget(this);
  // auto *layout = new QVBoxLayout();

  //
  // central->setLayout(layout);
  // setCentralWidget(central);
}

//MainWindow::~MainWindow() {}

void MainWindow::openPlayerRegistration() {
    auto *registrationScreen = new PlayerRegistrationScreen();
    registrationScreen->show();
    this->hide(); // נסתיר את התפריט הראשי (לא נסגור עדיין)

    connect(registrationScreen, &PlayerRegistrationScreen::playerRegistered, this, &MainWindow::startGameWithPlayers);
}

void MainWindow::startGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players) {
  auto *gameBoardWindow = new GameBoardWindow(players);
  gameBoardWindow->show();
  this->close();
}

// void MainWindow::openNewGame() {
//   auto *gameBoardWindow = new GameBoardWindow();
//   gameBoardWindow->show();
//   this->close();
//   // QMessageBox::information(this, "משחק חדש", "כאן יופיע לוח המשחק 🎮");
// }

void MainWindow::openInstructions() {
  auto *instructionsWindow = new InstructionsWindow();
  instructionsWindow->show();
  this->close();

  // QMessageBox::information(this, "הוראות", "כל שחקן מקבל תפקיד סודי... המטרה: לשרוד אחרון!");
}
