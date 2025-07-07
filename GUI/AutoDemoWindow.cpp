#include "AutoDemoWindow.h"
#include "MainWindow.h"
#include "../Roles/Baron.h"
#include "../Roles/Governor.h"
#include "../Roles/Judge.h"
#include "../Roles/General.h"
#include "../Roles/Spy.h"
#include "../Roles/Merchant.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <QSplitter>
#include <QFrame>
#include <algorithm>
#include <random>

AutoDemoWindow::AutoDemoWindow(const std::vector<std::shared_ptr<Player>>& inputPlayers,
                               bool showDetailedActions, QWidget *parent)
    : QWidget(parent), players(inputPlayers), showDetailedActions(showDetailedActions),
      rng(std::random_device{}()), currentState(STOPPED), currentStep(0), maxTurns(100)
{
    mainLayout = new QVBoxLayout(this); // רק כאן!

    setWindowTitle("🤖 COUP - משחק אוטומטי");
    resize(1200, 800);

    setupUI();
    setupGame();

    demoTimer = new QTimer(this);
    connect(demoTimer, &QTimer::timeout, this, &AutoDemoWindow::onDemoStep);
    demoTimer->setInterval(3000); // 3 seconds default
}

void AutoDemoWindow::setupUI() {
    // Title and back button
    if (!mainLayout)
        mainLayout = new QVBoxLayout(this);
    QHBoxLayout *headerLayout = new QHBoxLayout();

    homeButton = new QPushButton("🏠 חזרה לתפריט", this);
    homeButton->setStyleSheet(
        "QPushButton { "
        "background-color: #95a5a6; "
        "color: white; "
        "font-weight: bold; "
        "padding: 8px 15px; "
        "border: none; "
        "border-radius: 5px; "
        "}"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });

    QLabel *titleLabel = new QLabel("🤖 משחק אוטומטי - COUP", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(homeButton);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    setupPlayerDisplay();
    setupControls();
    setupActionLog();
    setupGraveyard();

    QHBoxLayout* statusLayout = new QHBoxLayout();

    statusLabel = new QLabel("📊 סטטוס: עצור", this);
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    statusLabel->setFont(statusFont);

    turnLabel = new QLabel("🎯 תור: טרם החל", this);
    QFont turnFont = turnLabel->font();
    turnFont.setPointSize(12);
    turnFont.setBold(true);
    turnLabel->setFont(turnFont);

    statusLayout->addWidget(statusLabel);
    statusLayout->addSpacing(20);
    statusLayout->addWidget(turnLabel);

    mainLayout->addLayout(statusLayout);
}

void AutoDemoWindow::setupPlayerDisplay() {
    // Player cards display (similar to GameBoardWindow)
    QLabel *playersLabel = new QLabel("👥 שחקנים:", this);
    playersLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 10px 0px 5px 0px;");
    mainLayout->addWidget(playersLabel);

    playerLayout = new QHBoxLayout();

    for (const auto& player : players) {
        QString name = QString::fromStdString(player->getName());
        QString role = QString::fromStdString(player->getRole()->getName());
        QString label = name + "\n(" + role + ")";

        QLabel *playerLabel = new QLabel(label, this);
        playerLabel->setStyleSheet(
            "border: 2px solid #3498db; "
            "padding: 15px; "
            "background: #ecf0f1; "
            "font-size: 12px; "
            "font-weight: bold; "
            "border-radius: 8px; "
            "min-height: 80px; "
            "max-width: 150px;"
        );
        playerLabel->setAlignment(Qt::AlignCenter);
        playerLabel->setWordWrap(true);

        playerLabelMap[name] = playerLabel;
        playerLayout->addWidget(playerLabel);
    }

    mainLayout->addLayout(playerLayout);

    // Current turn display
    turnLabel = new QLabel("🎯 תור: טרם החל", this);
    turnLabel->setAlignment(Qt::AlignCenter);
    QFont turnFont = turnLabel->font();
    turnFont.setPointSize(14);
    turnFont.setBold(true);
    turnLabel->setFont(turnFont);
    turnLabel->setStyleSheet(
        "background-color: #3498db; "
        "color: white; "
        "padding: 10px; "
        "border-radius: 8px; "
        "margin: 10px;"
    );
    mainLayout->addWidget(turnLabel);

    // Action result display
    actionResultLabel = new QLabel("בחר פעולה או התחל את המשחק", this);
    actionResultLabel->setAlignment(Qt::AlignCenter);
    actionResultLabel->setStyleSheet(
        "background-color: #f8f9fa; "
        "padding: 8px; "
        "border: 1px solid #dee2e6; "
        "border-radius: 5px; "
        "margin: 5px;"
    );
    mainLayout->addWidget(actionResultLabel);
}

void AutoDemoWindow::setupControls() {
    QGroupBox *controlGroup = new QGroupBox("🎮 בקרות משחק", this);
    controlLayout = new QGridLayout(controlGroup);

    startButton = new QPushButton("▶️ התחל", this);
    startButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");

    pauseButton = new QPushButton("⏸️ השהה", this);
    pauseButton->setStyleSheet("QPushButton { background-color: #f39c12; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    pauseButton->setEnabled(false);

    stopButton = new QPushButton("⏹️ עצור", this);
    stopButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    stopButton->setEnabled(false);

    stepButton = new QPushButton("⏭️ צעד אחד", this);
    stepButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");

    // Speed control
    QLabel *speedLabelText = new QLabel("מהירות:", this);
    speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(5);
    speedLabel = new QLabel("רגילה", this);

    controlLayout->addWidget(startButton, 0, 0);
    controlLayout->addWidget(pauseButton, 0, 1);
    controlLayout->addWidget(stopButton, 0, 2);
    controlLayout->addWidget(stepButton, 0, 3);
    controlLayout->addWidget(speedLabelText, 1, 0);
    controlLayout->addWidget(speedSlider, 1, 1, 1, 2);
    controlLayout->addWidget(speedLabel, 1, 3);

    mainLayout->addWidget(controlGroup);

    // Connect signals
    connect(startButton, &QPushButton::clicked, this, &AutoDemoWindow::startDemo);
    connect(pauseButton, &QPushButton::clicked, this, &AutoDemoWindow::pauseDemo);
    connect(stopButton, &QPushButton::clicked, this, &AutoDemoWindow::stopDemo);
    connect(stepButton, &QPushButton::clicked, this, &AutoDemoWindow::stepDemo);
    connect(speedSlider, &QSlider::valueChanged, this, &AutoDemoWindow::adjustSpeed);
}

void AutoDemoWindow::setupActionLog() {
    QLabel *logLabel = new QLabel("📋 יומן פעולות:", this);
    logLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 10px 0px 5px 0px;");
    mainLayout->addWidget(logLabel);

    actionLog = new QTextEdit(this);
    actionLog->setReadOnly(true);
    actionLog->setMaximumHeight(200);
    actionLog->setFont(QFont("Consolas", 10));
    actionLog->setStyleSheet(
        "background-color: #2c3e50; "
        "color: #ecf0f1; "
        "border: 2px solid #34495e; "
        "border-radius: 5px;"
    );

    mainLayout->addWidget(actionLog);
}

void AutoDemoWindow::setupGraveyard() {
    graveyardDock = new QDockWidget("🪦 שחקנים שהודחו", this);
    graveyardDock->setAllowedAreas(Qt::RightDockWidgetArea);
    graveyardDock->setFeatures(QDockWidget::DockWidgetMovable);

    graveyardList = new QListWidget(graveyardDock);
    graveyardList->setStyleSheet("background-color: #f4f4f4; font-size: 12px;");
    graveyardDock->setWidget(graveyardList);

    graveyardDock->setMinimumWidth(150);
    graveyardDock->setMaximumWidth(200);
    graveyardDock->hide(); // Hidden initially

    // Note: In a real implementation, you'd add this to a main window with dock areas
    // For now, we'll just show eliminated players in the action log
}

void AutoDemoWindow::setupGame() {
    game = std::make_unique<Game>(this);

    // Add players to game
    for (const auto& player : players) {
        game->addPlayer(player);
    }

    connect(game.get(), &Game::gameOverSignal, this, &AutoDemoWindow::onGameEnd);
    connect(game.get(), &Game::playerEliminated, this, &AutoDemoWindow::onPlayerEliminated);

    updatePlayerDisplay();
    updateGameStatus();
}

void AutoDemoWindow::updatePlayerDisplay() {
    for (const auto& player : players) {
        QString name = QString::fromStdString(player->getName());
        QString role = QString::fromStdString(player->getRole()->getName());
        QString coins = QString::number(player->getCoins());

        if (playerLabelMap.contains(name)) {
            QLabel* label = playerLabelMap[name];

            // Update display
            QString displayText = name + "\n(" + role + ")\n💰 " + coins;
            if (!player->isActive()) {
                displayText += "\n❌ הודח";
            }
            label->setText(displayText);

            // Update colors
            QString roleColor = getRoleColor(role);
            QString bgColor = player->isActive() ? "#ecf0f1" : "#bdc3c7";
            QString borderColor = player->isActive() ? roleColor : "#7f8c8d";

            label->setStyleSheet(
                QString("border: 3px solid %1; "
                        "padding: 15px; "
                        "background: %2; "
                        "font-size: 12px; "
                        "font-weight: bold; "
                        "border-radius: 8px; "
                        "min-height: 80px; "
                        "max-width: 150px;")
                .arg(borderColor)
                .arg(bgColor)
            );
        }
    }

    highlightCurrentPlayer();
}

void AutoDemoWindow::highlightCurrentPlayer() {
    // Reset all highlights
    for (auto& label : playerLabelMap) {
        // Remove highlight (already handled in updatePlayerDisplay)
    }

    // Highlight current player
    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString currentName = QString::fromStdString(currentPlayer.getName());

        if (playerLabelMap.contains(currentName)) {
            QLabel* label = playerLabelMap[currentName];
            QString role = QString::fromStdString(currentPlayer.getRole()->getName());
            QString roleColor = getRoleColor(role);

            label->setStyleSheet(
                QString("border: 4px solid %1; "
                        "padding: 15px; "
                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 #ecf0f1); "
                        "font-size: 12px; "
                        "font-weight: bold; "
                        "border-radius: 8px; "
                        "min-height: 80px; "
                        "max-width: 150px; "
                        "animation: pulse 1s infinite;")
                .arg(roleColor)
            );
        }
    } catch (...) {
        // Game might be over or not started
    }
}

void AutoDemoWindow::updateGameStatus() {
    QString status = "📊 סטטוס: ";
    QString turnText = "🎯 תור: ";

    switch (currentState) {
        case STOPPED:
            status += "עצור";
            turnText += "טרם החל";
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(true);
            break;

        case RUNNING:
            status += "רץ";
            try {
                QString playerName = QString::fromStdString(game->getCurrentPlayer().getName());
                QString roleName = QString::fromStdString(game->getCurrentPlayer().getRole()->getName());
                turnText += playerName + " (" + roleName + ") - תור " + QString::number(currentStep);
            } catch (...) {
                turnText += "לא ידוע";
            }
            startButton->setEnabled(false);
            pauseButton->setEnabled(true);
            stopButton->setEnabled(true);
            stepButton->setEnabled(false);
            break;

        case PAUSED:
            status += "מושהה";
            try {
                QString playerName = QString::fromStdString(game->getCurrentPlayer().getName());
                turnText += playerName + " - תור " + QString::number(currentStep);
            } catch (...) {
                turnText += "לא ידוע";
            }
            startButton->setText("▶️ המשך");
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(true);
            stepButton->setEnabled(true);
            break;

        case FINISHED:
            status += "הסתיים";
            turnText += "המשחק הסתיים";
            startButton->setText("🔄 משחק חדש");
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(false);
            break;
    }

    if (statusLabel) statusLabel->setText(status);
    if (turnLabel) turnLabel->setText(turnText);
}

void AutoDemoWindow::logAction(const QString &message, const QString &color) {
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    QString coloredMessage = QString("<span style='color: %1'>[%2] %3</span>")
                            .arg(color)
                            .arg(timestamp)
                            .arg(message);

    actionLog->append(coloredMessage);

    // Auto-scroll to bottom
    QScrollBar *scrollBar = actionLog->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void AutoDemoWindow::startDemo() {
    if (currentState == STOPPED || currentState == FINISHED) {
        // Reset for new game
        currentStep = 0;

        // Reset all players
        for (auto& player : players) {
            player->addCoins(2); // Reset to starting coins
            // Re-activate if needed
        }

        logAction("🎮 התחלת משחק אוטומטי חדש!", "#27ae60");
        logAction("🎭 " + QString::number(players.size()) + " שחקנים נכנסו למשחק", "#3498db");

        // Show initial game state
        for (const auto& player : players) {
            QString name = QString::fromStdString(player->getName());
            QString role = QString::fromStdString(player->getRole()->getName());
            QString coins = QString::number(player->getCoins());
            logAction("👤 " + name + " (" + role + ") מתחיל עם " + coins + " מטבעות",
                     getRoleColor(role));
        }
    }

    currentState = RUNNING;
    demoTimer->start();
    updateGameStatus();
    updatePlayerDisplay();
}

void AutoDemoWindow::pauseDemo() {
    if (currentState == RUNNING) {
        currentState = PAUSED;
        demoTimer->stop();
        logAction("⏸️ המשחק הושהה", "#f39c12");
        updateGameStatus();
    }
}

void AutoDemoWindow::stopDemo() {
    currentState = STOPPED;
    demoTimer->stop();
    logAction("⏹️ המשחק נעצר", "#e74c3c");
    updateGameStatus();
}

void AutoDemoWindow::stepDemo() {
    if (currentState != FINISHED) {
        onDemoStep();
    }
}

void AutoDemoWindow::onDemoStep() {
    if (currentState == FINISHED || game->isGameOver() || currentStep >= maxTurns) {
        if (!game->isGameOver() && currentStep >= maxTurns) {
            logAction("⏰ המשחק הסתיים עקב מגבלת תורות!", "#f39c12");
        }
        currentState = FINISHED;
        demoTimer->stop();
        updateGameStatus();
        return;
    }

    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());
        QString roleName = QString::fromStdString(currentPlayer.getRole()->getName());

        currentStep++;

        logAction("═══ תור " + QString::number(currentStep) + ": " + playerName + " (" + roleName + ") ═══",
                 getRoleColor(roleName));

        // Show current player's coins
        logAction("💰 " + playerName + " יש לו " + QString::number(currentPlayer.getCoins()) + " מטבעות",
                 "#f39c12");

        // Perform AI action
        performAIAction();

        // Update displays
        updatePlayerDisplay();
        updateGameStatus();

        // Check for game over
        game->checkGameOver();
        if (!game->isGameOver()) {
            game->nextTurn();
        }

    } catch (const std::exception& e) {
        logAction("❌ שגיאה בתור: " + QString(e.what()), "#e74c3c");
        currentState = FINISHED;
        demoTimer->stop();
        updateGameStatus();
    }
}

void AutoDemoWindow::performAIAction() {
    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());

        // Check for forced coup
        if (currentPlayer.getCoins() >= 10) {
            logAction("⚠️ " + playerName + " חייב לבצע הפיכה (10+ מטבעות)!", "#e74c3c");
            simulatePlayerDecision("coup", currentPlayer);
            return;
        }

        // Get random action based on AI logic
        QString action = getRandomAction(currentPlayer);
        logAction("🤔 " + playerName + " בחר לבצע: " + action, "#9b59b6");

        simulatePlayerDecision(action, currentPlayer);

    } catch (const std::exception& e) {
        logAction("❌ שגיאה בביצוע פעולה: " + QString(e.what()), "#e74c3c");
    }
}

QString AutoDemoWindow::getRandomAction(const Player& player) {
    std::vector<QString> possibleActions;

    // Basic actions
    if (player.getCoins() < 10) {
        possibleActions.push_back("gather");
        possibleActions.push_back("tax");
    }

    // Bribe (if has enough coins and not used this turn)
    if (player.getCoins() >= 4 && !player.hasUsedBribeThisTurn()) {
        possibleActions.push_back("bribe");
    }

    // Sanction
    if (player.getCoins() >= 3) {
        possibleActions.push_back("sanction");
    }

    // Coup
    if (player.getCoins() >= 7) {
        possibleActions.push_back("coup");
    }

    // Arrest
    if (!players.empty()) {
        possibleActions.push_back("arrest");
    }

    // Special role actions
    if (player.getRole()->getName() == "Baron" && player.getCoins() >= 3) {
        possibleActions.push_back("invest");
    }

    if (possibleActions.empty()) {
        return "gather"; // Default fallback
    }

    // Random selection
    std::uniform_int_distribution<size_t> dis(0, possibleActions.size() - 1);
    return possibleActions[dis(rng)];
}

std::shared_ptr<Player> AutoDemoWindow::getRandomTarget(const Player& attacker) {
    std::vector<std::shared_ptr<Player>> validTargets;

    for (const auto& player : players) {
        if (player->isActive() && player->getName() != attacker.getName()) {
            validTargets.push_back(player);
        }
    }

    if (validTargets.empty()) {
        return nullptr;
    }

    std::uniform_int_distribution<size_t> dis(0, validTargets.size() - 1);
    return validTargets[dis(rng)];
}

void AutoDemoWindow::simulatePlayerDecision(const QString& action, Player& player) {
    QString playerName = QString::fromStdString(player.getName());

    try {
        ActionResult result;

        if (action == "gather") {
            result = game->performGather(player);
            actionResultLabel->setText("💰 " + playerName + " אוסף משאבים");
            if (result.success) {
                // Simulate possible blocking (30% chance)
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 3) {
                    logAction("🛡️ הפעולה נחסמה על ידי שחקן אחר!", "#e74c3c");
                } else {
                    game->applyGather(player);
                    logAction("✅ " + playerName + " אסף 1 מטבע", "#27ae60");
                }
            } else {
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        else if (action == "tax") {
            result = game->performTax(player);
            actionResultLabel->setText("💰💰 " + playerName + " גובה מס");
            if (result.success) {
                // Simulate possible blocking
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                    logAction("🛡️ נגיד חסם את גביית המס!", "#e74c3c");
                } else {
                    game->applyTax(player);
                    int bonus = (player.getRole()->getName() == "Governor") ? 1 : 0;
                    logAction("✅ " + playerName + " גבה " + QString::number(2 + bonus) + " מטבעות", "#27ae60");
                }
            } else {
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        else if (action == "bribe") {
            result = game->performBribe(player);
            actionResultLabel->setText("💸 " + playerName + " נותן שוחד");
            if (result.success) {
                // Simulate judge blocking (20% chance)
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                    logAction("⚖️ השופט חסם את השוחד!", "#e74c3c");
                } else {
                    game->applyBribe(player);
                    logAction("✅ " + playerName + " קיבל תור נוסף בשוחד!", "#27ae60");
                }
            } else {
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        else if (action == "arrest") {
            auto target = getRandomTarget(player);
            if (target && target->getCoins() > 0) {
                QString targetName = QString::fromStdString(target->getName());
                result = game->performArrest(player, *target);
                actionResultLabel->setText("🚔 " + playerName + " עוצר את " + targetName);
                if (result.success) {
                    // Simulate spy blocking (25% chance)
                    if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                        logAction("🕵️ המרגל חסם את המעצר!", "#e74c3c");
                    } else {
                        game->applyArrest(player, *target);
                        logAction("✅ " + playerName + " עצר את " + targetName + " (גנב 1 מטבע)", "#27ae60");
                    }
                } else {
                    logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
                }
            }
        }
        else if (action == "sanction") {
            auto target = getRandomTarget(player);
            if (target) {
                QString targetName = QString::fromStdString(target->getName());
                result = game->performSanction(player, *target);
                actionResultLabel->setText("🚫 " + playerName + " מטיל חרם על " + targetName);
                logAction("✅ " + playerName + " הטיל חרם על " + targetName, "#8e44ad");
            }
        }
        else if (action == "coup") {
            auto target = getRandomTarget(player);
            if (target) {
                QString targetName = QString::fromStdString(target->getName());
                result = game->performCoup(player, *target);
                actionResultLabel->setText("💥 " + playerName + " מבצע הפיכה נגד " + targetName);
                if (result.success) {
                    // Simulate general defense (if target is general with 5+ coins)
                    if (target->getRole()->getName() == "General" && target->getCoins() >= 5 &&
                        std::uniform_int_distribution<int>(1, 10)(rng) <= 7) { // 70% chance general defends
                        logAction("🛡️ " + targetName + " (גנרל) הגן מפני ההפיכה!", "#27ae60");
                    } else {
                        game->applyCoup(player, *target);
                        logAction("💀 " + targetName + " הודח מהמשחק בהפיכה!", "#e74c3c");
                    }
                } else {
                    logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
                }
            }
        }
        else if (action == "invest") {
            result = game->performInvest(player);
            actionResultLabel->setText("📈 " + playerName + " (ברון) משקיע");
            logAction("✅ " + playerName + " השקיע 3 מטבעות וקיבל 6 בחזרה!", "#9b59b6");
        }

    } catch (const std::exception& e) {
        logAction("❌ שגיאה: " + QString(e.what()), "#e74c3c");
    }
}

QString AutoDemoWindow::getRoleColor(const QString &roleName) {
    if (roleName == "Governor") return "#3498db";
    else if (roleName == "Baron") return "#9b59b6";
    else if (roleName == "Judge") return "#e74c3c";
    else if (roleName == "General") return "#27ae60";
    else if (roleName == "Spy") return "#f1c40f";
    else if (roleName == "Merchant") return "#1abc9c";
    return "#34495e";
}

QString AutoDemoWindow::getPlayerInfo(const Player &player) {
    QString name = QString::fromStdString(player.getName());
    QString role = QString::fromStdString(player.getRole()->getName());
    QString coins = QString::number(player.getCoins());
    QString status = player.isActive() ? "פעיל" : "הודח";

    return name + " (" + role + ") - " + coins + " מטבעות - " + status;
}

void AutoDemoWindow::addPlayerToGraveyard(const QString &name, const QString &reason) {
    // For now, just log it since we don't have a proper dock widget setup
    logAction("⚰️ " + name + " נוסף לבית הקברות: " + reason, "#7f8c8d");
}

void AutoDemoWindow::adjustSpeed(int value) {
    // Convert slider value to timer interval
    // Value 1 = 5000ms (slow), Value 10 = 500ms (fast)
    int interval = 5500 - (value * 500);
    demoTimer->setInterval(interval);

    QString speedText;
    if (value <= 3) speedText = "איטי";
    else if (value <= 7) speedText = "רגיל";
    else speedText = "מהיר";

    speedLabel->setText(speedText);
}

void AutoDemoWindow::onGameEnd(const QString& winnerName) {
    logAction("🏆🏆🏆 המשחק הסתיים! 🏆🏆🏆", "#f1c40f");
    logAction("👑 המנצח: " + winnerName + " 👑", "#e74c3c");

    currentState = FINISHED;
    demoTimer->stop();
    updateGameStatus();

    showGameOverDialog(winnerName);
}

void AutoDemoWindow::onPlayerEliminated(const QString& playerName, const QString& reason) {
    logAction("💀 " + playerName + " הודח מהמשחק!", "#e74c3c");
    logAction("📝 סיבה: " + reason, "#95a5a6");
    addPlayerToGraveyard(playerName, reason);
    updatePlayerDisplay();
}

void AutoDemoWindow::showGameOverDialog(const QString& winner) {
    // Find winner details
    QString winnerRole = "";
    int winnerCoins = 0;

    for (const auto& player : players) {
        if (QString::fromStdString(player->getName()) == winner) {
            winnerRole = QString::fromStdString(player->getRole()->getName());
            winnerCoins = player->getCoins();
            break;
        }
    }

    QString message = QString("🎉 מזל טוב! 🎉\n\n"
                             "👑 המנצח: %1\n"
                             "🎭 תפקיד: %2\n"
                             "💰 מטבעות: %3\n"
                             "🎯 תורות: %4\n\n"
                             "האם תרצה להתחיל משחק חדש?")
                     .arg(winner)
                     .arg(winnerRole)
                     .arg(winnerCoins)
                     .arg(currentStep);

    int reply = QMessageBox::question(this, "🏆 סיום המשחק", message,
                                     QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Reset for new game
        currentState = STOPPED;
        currentStep = 0;

        // Reset players
        for (auto& player : players) {
            // Reset coins
            player->addCoins(2 - player->getCoins()); // Reset to 2 coins
            // Reactivate if needed (you might need to add this method to Player class)
        }

        updatePlayerDisplay();
        updateGameStatus();

        logAction("🔄 התחלת משחק חדש!", "#3498db");
    }
}