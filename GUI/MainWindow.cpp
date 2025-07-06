#include "MainWindow.h"
#include "InstructionsWindow.h"
#include "GameBoardWindow.h"
#include "PlayerRegistrationScreen.h"
#include "AutoDemoWindow.h"
#include <QPushButton>
#include <QVBoxLayout>

#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
  setWindowTitle("תפריט ראשי - Coup");
    resize(400, 350);

  setStyleSheet("background-color: #ecf0f1;");

  layout = new QVBoxLayout(this);

  // Add title
  QLabel *titleLabel = new QLabel("🎭 COUP GAME 🎭", this);
  QFont titleFont = titleLabel->font();
  titleFont.setPointSize(24);
  titleFont.setBold(true);
  titleLabel->setFont(titleFont);
  titleLabel->setAlignment(Qt::AlignCenter);
  titleLabel->setStyleSheet("color: #2c3e50; margin: 20px; padding: 15px; background-color: #3498db; border-radius: 10px; color: white;");
  layout->addWidget(titleLabel);

  // Add subtitle
  QLabel *subtitleLabel = new QLabel("בחר את סוג המשחק:", this);
  QFont subtitleFont = subtitleLabel->font();
  subtitleFont.setPointSize(14);
  subtitleLabel->setFont(subtitleFont);
  subtitleLabel->setAlignment(Qt::AlignCenter);
  subtitleLabel->setStyleSheet("color: #34495e; margin: 10px;");
  layout->addWidget(subtitleLabel);

  newGameButton = new QPushButton("🎮 משחק חדש (שחקנים אמיתיים)", this);
  newGameButton->setStyleSheet(
     "QPushButton {"
     "  background-color: #27ae60;"
     "  color: white;"
     "  font-size: 16px;"
     "  font-weight: bold;"
     "  padding: 15px;"
     "  border: none;"
     "  border-radius: 8px;"
     "  margin: 5px;"
     "}"
     "QPushButton:hover {"
     "  background-color: #2ecc71;"
     "}"
     "QPushButton:pressed {"
     "  background-color: #229954;"
     "}"
   );

  autoDemoButton = new QPushButton("🤖 דמונסטרציה אוטומטית", this);
  autoDemoButton->setStyleSheet(
    "QPushButton {"
    "  background-color: #3498db;"
    "  color: white;"
    "  font-size: 16px;"
    "  font-weight: bold;"
    "  padding: 15px;"
    "  border: none;"
    "  border-radius: 8px;"
    "  margin: 5px;"
    "}"
    "QPushButton:hover {"
    "  background-color: #5dade2;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #2980b9;"
    "}"
  );


  instructionsButton = new QPushButton("📖 הוראות משחק", this);
  instructionsButton->setStyleSheet(
    "QPushButton {"
    "  background-color: #f39c12;"
    "  color: white;"
    "  font-size: 16px;"
    "  font-weight: bold;"
    "  padding: 15px;"
    "  border: none;"
    "  border-radius: 8px;"
    "  margin: 5px;"
    "}"
    "QPushButton:hover {"
    "  background-color: #f4d03f;"
    "}"
    "QPushButton:pressed {"
    "  background-color: #d68910;"
    "}"
  );

  layout->addWidget(newGameButton);
  layout->addWidget(autoDemoButton);
  layout->addWidget(instructionsButton);

  layout->addStretch();

  QLabel *infoLabel = new QLabel("💡 הדמונסטרציה האוטומטית מציגה משחק מלא עם הסברים", this);
  infoLabel->setWordWrap(true);
  infoLabel->setStyleSheet("color: #7f8c8d; font-size: 12px; margin: 10px; padding: 10px; background-color: #f8f9fa; border-radius: 5px;");
  layout->addWidget(infoLabel);

  connect(newGameButton, &QPushButton::clicked, this, &MainWindow::openPlayerRegistration);
  connect(autoDemoButton, &QPushButton::clicked, this, &MainWindow::openAutoDemo);
  connect(instructionsButton, &QPushButton::clicked, this, &MainWindow::openInstructions);
}


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

void MainWindow::openAutoDemo() {
  auto *autoDemoWindow = new AutoDemoWindow();
  autoDemoWindow->show();
  this->close();
}

void MainWindow::openInstructions() {
  auto *instructionsWindow = new InstructionsWindow();
  instructionsWindow->show();
  this->close();
}
