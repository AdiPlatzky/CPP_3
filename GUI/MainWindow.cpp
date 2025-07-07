#include "MainWindow.h"
#include "InstructionsWindow.h"
#include "GameBoardWindow.h"
#include "PlayerRegistrationScreen.h"
#include "AutoPlayerSetupWindow.h"
#include "AutoDemoWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QFont>
#include <QWidget>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QWidget(parent)
{
  setWindowTitle("🎭 COUP - תפריט ראשי");
  resize(400, 350);

  // Set background color
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

  autoDemoButton = new QPushButton("🤖 משחק אוטומטי (בינה מלאכותית)", this);
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

  // Add some spacing at the bottom
  layout->addStretch();

  // Add info label
  QLabel *infoLabel = new QLabel("💡 במשחק האוטומטי - הבינה המלאכותית משחקת במקומך", this);
  infoLabel->setWordWrap(true);
  infoLabel->setStyleSheet("color: #7f8c8d; font-size: 12px; margin: 10px; padding: 10px; background-color: #f8f9fa; border-radius: 5px;");
  layout->addWidget(infoLabel);

  // Connect signals - only once!
  connect(newGameButton, &QPushButton::clicked, this, &MainWindow::openPlayerRegistration);
  connect(autoDemoButton, &QPushButton::clicked, this, &MainWindow::openAutoPlayerSetup);
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

void MainWindow::openAutoPlayerSetup() {
  auto *autoSetupWindow = new AutoPlayerSetupWindow();
  autoSetupWindow->show();
  this->hide();

  connect(autoSetupWindow, &AutoPlayerSetupWindow::playersConfigured,
          this, &MainWindow::startAutoGameWithPlayers);
}

void MainWindow::startAutoGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players, bool showActions) {
  auto *autoDemoWindow = new AutoDemoWindow(players, showActions, nullptr);
  autoDemoWindow->show();
  this->hide();

  connect(autoDemoWindow, &AutoDemoWindow::destroyed, this, &MainWindow::show);

}

void MainWindow::openInstructions() {
  auto *instructionsWindow = new InstructionsWindow();
  instructionsWindow->show();
  this->close();
}