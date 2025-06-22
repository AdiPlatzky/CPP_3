#include "MainWindow.h"
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
  auto *central = new QWidget(this);
  auto *layout = new QVBoxLayout();

  newGameButton = new QPushButton("משחק חדש", this);
  instructionsButton = new QPushButton("הוראות משחק", this);

  layout->addWidget(newGameButton);
  layout->addWidget(instructionsButton);

  central->setLayout(layout);
  setCentralWidget(central);
  setWindowTitle("תפריט ראשי - Coup");

  connect(newGameButton, &QPushButton::clicked, this, &MainWindow::startNewGame);
  connect(instructionsButton, &QPushButton::clicked, this, &MainWindow::showInstructions);
}

MainWindow::~MainWindow() {}

void MainWindow::startNewGame() {
  QMessageBox::information(this, "משחק חדש", "כאן יופיע לוח המשחק 🎮");
}

void MainWindow::showInstructions() {
  QMessageBox::information(this, "הוראות", "כל שחקן מקבל תפקיד סודי... המטרה: לשרוד אחרון!");
}
