#include "AutoDemoWindow.h"
#include "MainWindow.h"
#include "../Roles/Baron.h"
#include "../Roles/Governor.h"
#include "../Roles/Judge.h"
#include "../Roles/General.h"
#include "../Roles/Spy.h"
#include "../Roles/Merchant.h"
#include <QHBoxLayout>
#include <QSlider>
#include <QSplitter>
#include <QGroupBox>
#include <QFont>
#include <QMessageBox>
#include <QApplication>
#include <QScrollBar>
#include <algorithm>

AutoDemoWindow::AutoDemoWindow(QWidget *parent)
    : QWidget(parent), currentState(STOPPED), currentStep(0), maxTurns(50), rng(std::random_device{}())
{
    setWindowTitle("🎮 COUP - Auto Demo");
    resize(1200, 800);
    
    setupUI();
    setupGame();
    
    demoTimer = new QTimer(this);
    connect(demoTimer, &QTimer::timeout, this, &AutoDemoWindow::onDemoStep);
    demoTimer->setInterval(2000); // 2 seconds default
}

void AutoDemoWindow::setupUI() {
    mainLayout = new QVBoxLayout(this);
    
    // Header
    QLabel *titleLabel = new QLabel("🎭 COUP - משחק אוטומטי מלא", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: #2c3e50; margin: 10px; padding: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Control Panel
    QGroupBox *controlGroup = new QGroupBox("🎮 בקרות המשחק", this);
    QHBoxLayout *controlLayout = new QHBoxLayout(controlGroup);
    
    startButton = new QPushButton("▶️ התחל", this);
    startButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; font-weight: bold; padding: 8px; }");
    
    pauseButton = new QPushButton("⏸️ השהה", this);
    pauseButton->setStyleSheet("QPushButton { background-color: #f39c12; color: white; font-weight: bold; padding: 8px; }");
    pauseButton->setEnabled(false);
    
    stopButton = new QPushButton("⏹️ עצור", this);
    stopButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; font-weight: bold; padding: 8px; }");
    stopButton->setEnabled(false);
    
    stepButton = new QPushButton("⏭️ צעד אחד", this);
    stepButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; font-weight: bold; padding: 8px; }");
    
    homeButton = new QPushButton("🏠 חזרה לתפריט", this);
    homeButton->setStyleSheet("QPushButton { background-color: #95a5a6; color: white; font-weight: bold; padding: 8px; }");
    
    controlLayout->addWidget(startButton);
    controlLayout->addWidget(pauseButton);
    controlLayout->addWidget(stopButton);
    controlLayout->addWidget(stepButton);
    controlLayout->addStretch();
    controlLayout->addWidget(homeButton);
    
    mainLayout->addWidget(controlGroup);
    
    // Speed Control
    QGroupBox *speedGroup = new QGroupBox("⚡ מהירות המשחק", this);
    QHBoxLayout *speedLayout = new QHBoxLayout(speedGroup);
    
    speedLabel = new QLabel("מהירות: רגילה", this);
    QSlider *speedSlider = new QSlider(Qt::Horizontal, this);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(5);
    speedSlider->setTickPosition(QSlider::TicksBelow);
    speedSlider->setTickInterval(1);
    
    speedLayout->addWidget(new QLabel("איטי"));
    speedLayout->addWidget(speedSlider);
    speedLayout->addWidget(new QLabel("מהיר"));
    speedLayout->addWidget(speedLabel);
    
    mainLayout->addWidget(speedGroup);
    
    // Status
    statusLabel = new QLabel("📊 סטטוס: מוכן להתחלה", this);
    statusLabel->setStyleSheet("background-color: #ecf0f1; padding: 5px; border: 1px solid #bdc3c7;");
    mainLayout->addWidget(statusLabel);
    
    turnLabel = new QLabel("🎯 תור: טרם החל", this);
    turnLabel->setStyleSheet("background-color: #e8f6ff; padding: 5px; border: 1px solid #3498db; font-weight: bold;");
    mainLayout->addWidget(turnLabel);
    
    // Log Display
    logDisplay = new QTextEdit(this);
    logDisplay->setReadOnly(true);
    logDisplay->setFont(QFont("Consolas", 10));
    logDisplay->setStyleSheet("background-color: #2c3e50; color: #ecf0f1; border: 2px solid #34495e;");
    mainLayout->addWidget(logDisplay);
    
    // Connect signals
    connect(startButton, &QPushButton::clicked, this, &AutoDemoWindow::startDemo);
    connect(pauseButton, &QPushButton::clicked, this, &AutoDemoWindow::pauseDemo);
    connect(stopButton, &QPushButton::clicked, this, &AutoDemoWindow::stopDemo);
    connect(stepButton, &QPushButton::clicked, this, &AutoDemoWindow::stepDemo);
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });
    connect(speedSlider, &QSlider::valueChanged, this, &AutoDemoWindow::adjustSpeed);
}

void AutoDemoWindow::setupGame() {
    game = std::make_unique<Game>(this);
    
    connect(game.get(), &Game::gameOverSignal, this, [this](const QString& winner) {
        logMessage("🏆🏆🏆 המשחק הסתיים! 🏆🏆🏆", "#f1c40f");
        logMessage("👑 המנצח: " + winner + " 👑", "#e74c3c");
        currentState = FINISHED;
        demoTimer->stop();
        showFinalResults();
        updateStatus();
    });
    
    connect(game.get(), &Game::playerEliminated, this, [this](const QString& player, const QString& reason) {
        logMessage("💀 " + player + " הודח מהמשחק!", "#e74c3c");
        logMessage("📝 סיבה: " + reason, "#95a5a6");
    });
}

void AutoDemoWindow::createPlayers() {
    logHeader("🎭 יצירת שחקנים והקצאת תפקידים");
    
    QStringList names = {"Alice", "Bob", "Charlie", "Diana", "Eva", "Frank"};
    QStringList roleNames = {"Governor", "Baron", "Judge", "General", "Spy", "Merchant"};
    
    for (int i = 0; i < names.size(); ++i) {
        std::unique_ptr<Role> role;
        
        if (roleNames[i] == "Governor") role = std::make_unique<Governor>();
        else if (roleNames[i] == "Baron") role = std::make_unique<Baron>();
        else if (roleNames[i] == "Judge") role = std::make_unique<Judge>();
        else if (roleNames[i] == "General") role = std::make_unique<General>();
        else if (roleNames[i] == "Spy") role = std::make_unique<Spy>();
        else if (roleNames[i] == "Merchant") role = std::make_unique<Merchant>();
        
        auto player = std::make_shared<Player>(names[i].toStdString(), std::move(role));
        
        // Give starting coins
        int startingCoins = std::uniform_int_distribution<int>(2, 5)(rng);
        player->addCoins(startingCoins);
        
        players.push_back(player);
        game->addPlayer(player);
        
        QString roleColor = getRoleColor(roleNames[i]);
        logMessage("✓ " + names[i] + " קיבל את התפקיד: " + roleNames[i] + 
                  " (" + QString::number(startingCoins) + " מטבעות)", roleColor);
    }
    
    logMessage("🎲 כל השחקנים מוכנים למשחק!", "#27ae60");
}

void AutoDemoWindow::demonstrateRoleAbilities() {
    logHeader("🎪 הדגמת יכולות התפקידים");
    
    for (const auto& player : players) {
        if (!player->isActive()) continue;
        
        QString playerName = QString::fromStdString(player->getName());
        QString roleName = QString::fromStdString(player->getRole()->getName());
        QString roleColor = getRoleColor(roleName);
        
        logSubHeader(playerName + " (" + roleName + ") - יכולות מיוחדות");
        
        if (roleName == "Governor") {
            logMessage("🏛️ נגיד:", roleColor);
            logMessage("   • מקבל 3 מטבעות במקום 2 ממס", "#3498db");
            logMessage("   • יכול לחסום פעולות מס של אחרים", "#3498db");
        }
        else if (roleName == "Baron") {
            logMessage("💼 ברון:", roleColor);
            logMessage("   • יכול להשקיע 3 מטבעות כדי לקבל 6 בחזרה", "#9b59b6");
            logMessage("   • מקבל פיצוי כשמוטל עליו חרם", "#9b59b6");
        }
        else if (roleName == "Judge") {
            logMessage("⚖️ שופט:", roleColor);
            logMessage("   • יכול לחסום פעולות שוחד", "#e74c3c");
            logMessage("   • גובה קנס כשמוטל עליו חרם", "#e74c3c");
        }
        else if (roleName == "General") {
            logMessage("🛡️ גנרל:", roleColor);
            logMessage("   • יכול להתגונן מפני הפיכה (עולה 5 מטבעות)", "#27ae60");
            logMessage("   • מקבל מטבע בחזרה כשנעצר", "#27ae60");
        }
        else if (roleName == "Spy") {
            logMessage("🕵️ מרגל:", roleColor);
            logMessage("   • יכול לראות כמה מטבעות יש לשחקנים אחרים", "#f1c40f");
            logMessage("   • יכול לחסום פעולות מעצר", "#f1c40f");
        }
        else if (roleName == "Merchant") {
            logMessage("🏪 סוחר:", roleColor);
            logMessage("   • מקבל מטבע בונוס בתחילת התור (אם יש לו 3+ מטבעות)", "#1abc9c");
            logMessage("   • הגנה מיוחדת נגד מעצר", "#1abc9c");
        }
    }
}

void AutoDemoWindow::showGameState() {
    logSubHeader("📊 מצב המשחק הנוכחי");
    
    logMessage("┌─────────────────────────────────────────────────────────┐", "#7f8c8d");
    logMessage("│ שחקן          │ תפקיד     │ מטבעות │ סטטוס            │", "#34495e");
    logMessage("├─────────────────────────────────────────────────────────┤", "#7f8c8d");
    
    for (const auto& player : players) {
        QString name = QString::fromStdString(player->getName());
        QString role = QString::fromStdString(player->getRole()->getName());
        QString coins = QString::number(player->getCoins());
        QString status = player->isActive() ? "✓ פעיל" : "✗ הודח";
        QString statusColor = player->isActive() ? "#27ae60" : "#e74c3c";
        
        bool isCurrentTurn = false;
        try {
            isCurrentTurn = (player->getName() == game->getCurrentPlayer().getName());
        } catch (...) {}
        
        QString turnMarker = isCurrentTurn ? ">>> " : "    ";
        QString roleColor = getRoleColor(role);
        
        QString line = QString("│ %1%-12s │ %-10s │   💰%-3s  │ %-17s │")
                      .arg(turnMarker)
                      .arg(name.toStdString().c_str())
                      .arg(role.toStdString().c_str())
                      .arg(coins.toStdString().c_str())
                      .arg(status.toStdString().c_str());
        
        logMessage(line, isCurrentTurn ? "#3498db" : "#ecf0f1");
    }
    
    logMessage("└─────────────────────────────────────────────────────────┘", "#7f8c8d");
    logMessage("🏦 קופת המשחק: " + QString::number(game->getCoinPool()) + " מטבעות", "#f39c12");
}

void AutoDemoWindow::performRandomAction() {
    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());
        QString roleName = QString::fromStdString(currentPlayer.getRole()->getName());
        
        logMessage("🎯 " + playerName + " (" + roleName + ") חושב על הפעולה הבאה...", 
                  getRoleColor(roleName));
        
        // Check for forced coup
        if (currentPlayer.getCoins() >= 10) {
            logMessage("⚠️ " + playerName + " חייב לבצע הפיכה כי יש לו 10+ מטבעות!", "#e74c3c");
        }
        
        // Merchant turn start bonus
        if (roleName == "Merchant" && currentPlayer.getCoins() >= 3) {
            int beforeCoins = currentPlayer.getCoins();
            currentPlayer.getRole()->onTurnStart(currentPlayer, *game);
            if (currentPlayer.getCoins() > beforeCoins) {
                logMessage("💼 סוחר קיבל בונוס: +1 מטבע!", "#1abc9c");
            }
        }
        
        // Choose random action
        std::vector<QString> possibleActions;
        
        if (currentPlayer.getCoins() < 10) {
            possibleActions.push_back("gather");
            possibleActions.push_back("tax");
        }
        
        if (currentPlayer.getCoins() >= 4 && !currentPlayer.hasUsedBribeThisTurn()) {
            possibleActions.push_back("bribe");
        }
        
        if (currentPlayer.getCoins() >= 3) {
            possibleActions.push_back("sanction");
        }
        
        if (currentPlayer.getCoins() >= 7) {
            possibleActions.push_back("coup");
        }
        
        // Find valid targets
        std::vector<std::shared_ptr<Player>> validTargets;
        for (const auto& p : players) {
            if (p->isActive() && p->getName() != currentPlayer.getName()) {
                validTargets.push_back(p);
            }
        }
        
        if (!validTargets.empty()) {
            possibleActions.push_back("arrest");
        }
        
        // Special role actions
        if (roleName == "Baron" && currentPlayer.getCoins() >= 3) {
            possibleActions.push_back("invest");
        }
        
        if (possibleActions.empty()) {
            logMessage("❌ " + playerName + " אין לו פעולות זמינות!", "#e74c3c");
            return;
        }
        
        // Choose random action
        QString action = possibleActions[std::uniform_int_distribution<size_t>(0, possibleActions.size() - 1)(rng)];
        
        ActionResult result;
        
        if (action == "gather") {
            result = game->performGather(currentPlayer);
            logMessage("💰 " + playerName + " מנסה לאסוף משאבים (+1 מטבע)", "#f39c12");
            if (result.success) {
                if (std::uniform_int_distribution<int>(1, 3)(rng) == 1) {
                    logMessage("🛡️ הפעולה נחסמה על ידי שחקן אחר!", "#e74c3c");
                } else {
                    game->applyGather(currentPlayer);
                    logMessage("✅ " + result.message.c_str(), "#27ae60");
                }
            }
        }
        else if (action == "tax") {
            result = game->performTax(currentPlayer);
            logMessage("💰💰 " + playerName + " מנסה לגבות מס (+2+ מטבעות)", "#3498db");
            if (result.success) {
                if (std::uniform_int_distribution<int>(1, 3)(rng) == 1) {
                    logMessage("🛡️ הפעולה נחסמה על ידי שחקן אחר!", "#e74c3c");
                } else {
                    game->applyTax(currentPlayer);
                    logMessage("✅ " + result.message.c_str(), "#27ae60");
                }
            }
        }
        else if (action == "bribe") {
            result = game->performBribe(currentPlayer);
            logMessage("💸 " + playerName + " מנסה לתת שוחד לתור נוסף", "#f39c12");
            if (result.success) {
                if (std::uniform_int_distribution<int>(1, 4)(rng) == 1) {
                    logMessage("🛡️ השופט חסם את השוחד!", "#e74c3c");
                } else {
                    game->applyBribe(currentPlayer);
                    logMessage("✅ " + result.message.c_str(), "#27ae60");
                }
            }
        }
        else if (action == "arrest" && !validTargets.empty()) {
            auto target = validTargets[std::uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
            QString targetName = QString::fromStdString(target->getName());
            
            if (target->getCoins() > 0) {
                result = game->performArrest(currentPlayer, *target);
                logMessage("🚔 " + playerName + " מנסה לעצור את " + targetName, "#e67e22");
                if (result.success) {
                    if (std::uniform_int_distribution<int>(1, 3)(rng) == 1) {
                        logMessage("🛡️ המרגל חסם את המעצר!", "#e74c3c");
                    } else {
                        game->applyArrest(currentPlayer, *target);
                        logMessage("✅ " + result.message.c_str(), "#27ae60");
                    }
                }
            }
        }
        else if (action == "sanction" && !validTargets.empty()) {
            auto target = validTargets[std::uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
            QString targetName = QString::fromStdString(target->getName());
            
            result = game->performSanction(currentPlayer, *target);
            logMessage("🚫 " + playerName + " מטיל חרם על " + targetName, "#8e44ad");
            logMessage("✅ " + result.message.c_str(), "#27ae60");
        }
        else if (action == "coup" && !validTargets.empty()) {
            auto target = validTargets[std::uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
            QString targetName = QString::fromStdString(target->getName());
            
            result = game->performCoup(currentPlayer, *target);
            logMessage("💥 " + playerName + " מבצע הפיכה נגד " + targetName, "#c0392b");
            if (result.success) {
                if (target->getRole()->getName() == "General" && target->getCoins() >= 5) {
                    logMessage("🛡️ הגנרל התגונן מפני ההפיכה!", "#27ae60");
                } else {
                    game->applyCoup(currentPlayer, *target);
                    logMessage("💀 " + targetName + " הודח מהמשחק!", "#e74c3c");
                }
            }
        }
        else if (action == "invest") {
            result = game->performInvest(currentPlayer);
            logMessage("📈 " + playerName + " (ברון) מבצע השקעה", "#9b59b6");
            logMessage("✅ " + result.message.c_str(), "#27ae60");
        }
        
        if (!result.success) {
            logMessage("❌ " + result.message.c_str(), "#e74c3c");
        }
        
    } catch (const std::exception& e) {
        logMessage("❌ שגיאה: " + QString(e.what()), "#e74c3c");
    }
}

void AutoDemoWindow::showFinalResults() {
    logHeader("🏆 תוצאות סופיות");
    
    // Show winner
    try {
        QString winner = QString::fromStdString(game->getWinner());
        logMessage("🎉🎉🎉 מזל טוב! 🎉🎉🎉", "#f1c40f");
        logMessage("👑 המנצח: " + winner + " 👑", "#e74c3c");
        
        // Find winner's details
        for (const auto& player : players) {
            if (QString::fromStdString(player->getName()) == winner) {
                QString roleName = QString::fromStdString(player->getRole()->getName());
                logMessage("🎭 תפקיד: " + roleName, getRoleColor(roleName));
                logMessage("💰 מטבעות: " + QString::number(player->getCoins()), "#f39c12");
                break;
            }
        }
    } catch (const std::exception& e) {
        logMessage("❌ שגיאה בקביעת המנצח: " + QString(e.what()), "#e74c3c");
    }
    
    // Show final standings
    logSubHeader("📊 דירוג סופי");
    
    auto standings = players;
    std::sort(standings.begin(), standings.end(), [](const auto& a, const auto& b) {
        if (a->isActive() != b->isActive()) return a->isActive();
        return a->getCoins() > b->getCoins();
    });
    
    for (size_t i = 0; i < standings.size(); ++i) {
        QString medal;
        if (i == 0 && standings[i]->isActive()) medal = "🥇";
        else if (i == 1) medal = "🥈";
        else if (i == 2) medal = "🥉";
        else medal = "  ";
        
        QString name = QString::fromStdString(standings[i]->getName());
        QString role = QString::fromStdString(standings[i]->getRole()->getName());
        QString coins = QString::number(standings[i]->getCoins());
        QString status = standings[i]->isActive() ? "פעיל" : "הודח";
        
        logMessage(medal + " " + QString::number(i + 1) + ". " + name + 
                  " (" + role + ") - " + coins + " מטבעות - " + status, 
                  getRoleColor(role));
    }
    
    // Game statistics
    logSubHeader("📈 סטטיסטיקות משחק");
    
    int totalCoins = 0;
    int activePlayers = 0;
    
    for (const auto& player : players) {
        totalCoins += player->getCoins();
        if (player->isActive()) activePlayers++;
    }
    
    logMessage("🔢 סך השחקנים: " + QString::number(players.size()), "#34495e");
    logMessage("✅ שחקנים פעילים: " + QString::number(activePlayers), "#27ae60");
    logMessage("💰 סך המטבעות במשחק: " + QString::number(totalCoins), "#f39c12");
    logMessage("🏦 מטבעות בקופה: " + QString::number(game->getCoinPool()), "#3498db");
    logMessage("🎯 מספר תורות: " + QString::number(currentStep), "#9b59b6");
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

void AutoDemoWindow::logMessage(const QString &message, const QString &color) {
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString coloredMessage = QString("<span style='color: %1'>[%2] %3</span>")
                            .arg(color)
                            .arg(timestamp)
                            .arg(message);
    
    logDisplay->append(coloredMessage);
    
    // Auto-scroll to bottom
    QScrollBar *scrollBar = logDisplay->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}

void AutoDemoWindow::logHeader(const QString &title) {
    logMessage("", "#ecf0f1");
    logMessage("═══════════════════════════════════════════════════════════", "#3498db");
    logMessage("  " + title, "#3498db");
    logMessage("═══════════════════════════════════════════════════════════", "#3498db");
}

void AutoDemoWindow::logSubHeader(const QString &subtitle) {
    logMessage("", "#ecf0f1");
    logMessage("─── " + subtitle + " ───", "#f39c12");
}

void AutoDemoWindow::startDemo() {
    if (currentState == STOPPED || currentState == FINISHED) {
        logMessage("🎮 מתחיל דמונסטרציה חדשה...", "#3498db");
        
        // Reset game
        game = std::make_unique<Game>(this);
        connect(game.get(), &Game::gameOverSignal, this, [this](const QString& winner) {
            logMessage("🏆🏆🏆 המשחק הסתיים! 🏆🏆🏆", "#f1c40f");
            logMessage("👑 המנצח: " + winner + " 👑", "#e74c3c");
            currentState = FINISHED;
            demoTimer->stop();
            showFinalResults();
            updateStatus();
        });
        
        connect(game.get(), &Game::playerEliminated, this, [this](const QString& player, const QString& reason) {
            logMessage("💀 " + player + " הודח מהמשחק!", "#e74c3c");
            logMessage("📝 סיבה: " + reason, "#95a5a6");
        });
        
        players.clear();
        currentStep = 0;
        
        createPlayers();
        demonstrateRoleAbilities();
        showGameState();
        
        logHeader("🎮 התחלת המשחק האוטומטי");
    }
    
    currentState = RUNNING;
    demoTimer->start();
    updateStatus();
}

void AutoDemoWindow::pauseDemo() {
    if (currentState == RUNNING) {
        currentState = PAUSED;
        demoTimer->stop();
        logMessage("⏸️ המשחק הושהה", "#f39c12");
        updateStatus();
    }
}

void AutoDemoWindow::stopDemo() {
    currentState = STOPPED;
    demoTimer->stop();
    logMessage("⏹️ המשחק נעצר", "#e74c3c");
    updateStatus();
}

void AutoDemoWindow::stepDemo() {
    if (currentState != FINISHED) {
        onDemoStep();
    }
}

void AutoDemoWindow::onDemoStep() {
    if (currentState == FINISHED || game->isGameOver() || currentStep >= maxTurns) {
        if (!game->isGameOver() && currentStep >= maxTurns) {
            logMessage("⏰ המשחק הסתיים עקב מגבלת תורות!", "#f39c12");
        }
        currentState = FINISHED;
        demoTimer->stop();
        if (!game->isGameOver()) {
            showFinalResults();
        }
        updateStatus();
        return;
    }
    
    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());
        
        currentStep++;
        
        logSubHeader("תור " + QString::number(currentStep) + ": " + playerName);
        
        showGameState();
        
        performRandomAction();
        
        game->checkGameOver();
        if (!game->isGameOver()) {
            game->nextTurn();
        }
        
        updateStatus();
        
    } catch (const std::exception& e) {
        logMessage("❌ שגיאה בתור: " + QString(e.what()), "#e74c3c");
        currentState = FINISHED;
        demoTimer->stop();
        updateStatus();
    }
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
    
    speedLabel->setText("מהירות: " + speedText);
}

void AutoDemoWindow::updateStatus() {
    QString status;
    QString turnText = "🎯 תור: ";
    
    switch (currentState) {
        case STOPPED:
            status = "📊 סטטוס: עצור";
            turnText += "טרם החל";
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(true);
            break;
            
        case RUNNING:
            status = "📊 סטטוס: רץ";
            try {
                turnText += QString::fromStdString(game->getCurrentPlayer().getName()) + 
                           " (תור " + QString::number(currentStep) + ")";
            } catch (...) {
                turnText += "לא ידוע";
            }
            startButton->setEnabled(false);
            pauseButton->setEnabled(true);
            stopButton->setEnabled(true);
            stepButton->setEnabled(false);
            break;
            
        case PAUSED:
            status = "📊 סטטוס: מושהה";
            try {
                turnText += QString::fromStdString(game->getCurrentPlayer().getName()) + 
                           " (תור " + QString::number(currentStep) + ")";
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
            status = "📊 סטטוס: הסתיים";
            turnText += "המשחק הסתיים";
            startButton->setText("▶️ התחל");
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(false);
            break;
    }
    
    statusLabel->setText(status);
    turnLabel->setText(turnText);
}