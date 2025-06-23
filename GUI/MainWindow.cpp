#include "MainWindow.h"
#include "InstructionsWindow.h"
#include "GameBoardWindow.h"
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

  newGameButton = new QPushButton("משחק חדש", this);
  instructionsButton = new QPushButton("הוראות משחק", this);

  layout->addWidget(newGameButton);
  layout->addWidget(instructionsButton);

  connect(newGameButton, &QPushButton::clicked, this, &MainWindow::openNewGame);
  connect(instructionsButton, &QPushButton::clicked, this, &MainWindow::openInstructions);
  //
  // auto *central = new QWidget(this);
  // auto *layout = new QVBoxLayout();

  //
  // central->setLayout(layout);
  // setCentralWidget(central);
}

MainWindow::~MainWindow() {}

void MainWindow::openNewGame() {
  auto *gameBoardWindow = new GameBoardWindow();
  gameBoardWindow->show();
  this->close();
  // QMessageBox::information(this, "משחק חדש", "כאן יופיע לוח המשחק 🎮");
}

void MainWindow::openInstructions() {
  auto *instructionsWindow = new InstructionsWindow();
  instructionsWindow->show();
  this->close();

  // QMessageBox::information(this, "הוראות", "כל שחקן מקבל תפקיד סודי... המטרה: לשרוד אחרון!");
}
