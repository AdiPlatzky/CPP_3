// 12adi45@gmail.com

#include "GameBoardWindow.h"
#include "../Game.h"
#include "MainWindow.h"
#include "BlockingDialog.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QLabel>
#include <QGuiApplication>
#include <QScreen>
#include <QGridLayout>
#include <QGraphicsOpacityEffect>
#include <QPixmap>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include <QMouseEvent>
#include <QDebug>

/**
 * @brief בנאי - מאתחל את חלון המשחק עם השחקנים הנתונים
 */
GameBoardWindow::GameBoardWindow(const std::vector<std::shared_ptr<Player>>& players, QWidget *parent)
    : QWidget(parent)
{
    // הגדרת חלון בסיסי
    setWindowTitle("New Game - COUP");
    resize(1000, 700);
    mainLayout = new QVBoxLayout(this);

    // כפתור חזרה לתפריט
    homeButton = new QPushButton("🏠 Back to menu", this);
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });
    mainLayout->addWidget(homeButton);

    // יצירת בית קברות
    graveyardDock = new QDockWidget("🪦 בית הקברות", this);
    graveyardDock->setAllowedAreas(Qt::RightDockWidgetArea);
    graveyardDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);
    graveyardList = new QListWidget(graveyardDock);
    graveyardList->setStyleSheet("background-color: #f4f4f4; font-size: 14px;");
    graveyardDock->setWidget(graveyardList);
    graveyardDock->setMinimumWidth(150);
    graveyardDock->setMaximumWidth(200);
    graveyardDock->setFloating(false);
    graveyardDock->setVisible(true);
    mainLayout->addWidget(graveyardDock);

    // תוויות מידע
    lastBlockedLabel = new QLabel("🔒 חסימות : אין שחקן חסום");
    lastBlockedLabel->setStyleSheet("font-weight: bold; color: darkred;");
    mainLayout->addWidget(lastBlockedLabel, 0, Qt::AlignCenter);

    turnLabel = new QLabel(this);
    turnLabel->setAlignment(Qt::AlignCenter);
    QFont font = turnLabel->font();
    font.setPointSize(18);
    turnLabel->setFont(font);
    mainLayout->addStretch(1);
    mainLayout->addWidget(turnLabel, 0, Qt::AlignHCenter);
    mainLayout->addStretch(1);

    coinLabel = new QLabel(this);
    coinLabel->setAlignment(Qt::AlignCenter);
    QFont cFont = coinLabel->font();
    cFont.setPointSize(14);
    coinLabel->setFont(cFont);
    mainLayout->addStretch(1);
    mainLayout->addStretch(1);

    actionResultLabel = new QLabel(this);
    actionResultLabel->setAlignment(Qt::AlignCenter);
    actionResultLabel->setText("Choose action");
    mainLayout->addWidget(actionResultLabel);

    // אתחול משחק
    game = std::make_unique<Game>(this);
    connect(game.get(), &Game::gameOverSignal, this, &GameBoardWindow::handleGameEnd);
    connect(game.get(), &Game::playerEliminated, this, &GameBoardWindow::addPlayerToGraveyard);
    for (const auto& player : players) {
        game->addPlayer(player);
    }

    // בניית ממשק
    setupPlayers();
    setupActions();
    updateTurnLabel();
    highlightCurrentPlayer();
    updateCoinLabel();
}

/**
 * @brief בונה תצוגת השחקנים - יוצר תווית לכל שחקן פעיל
 */
void GameBoardWindow::setupPlayers() {
    playerLayout = new QHBoxLayout();
    const auto& playersList = game->getPlayer();

    for (const auto& playerPtr : playersList) {
        if (!playerPtr->isActive()) { continue; }

        std::string name = playerPtr->getName();
        std::string role = playerPtr->getRole()->getName();
        QString label = QString::fromStdString(name + " (" + role + ")");

        QLabel *player = new QLabel(label, this);
        player->setStyleSheet("border: 2px solid black; padding: 15px; background: #DDEEFF; font-size: 10px;");
        player->setMinimumSize(120, 80);
        player->setAlignment(Qt::AlignCenter);
        playerLayout->addWidget(player);
        playerLabelMap[QString::fromStdString(name)] = player;
    }

    mainLayout->addLayout(playerLayout);
    highlightLayout = new QVBoxLayout();
    mainLayout->addLayout(highlightLayout);
}

/**
 * @brief מעדכן תווית חסימות מעצר מתוכננות
 */
void GameBoardWindow::updateBlockedArrestLabel() {
    QString blockedText = "🔒 חסימת מעצר: ";
    bool found = false;

    for (const auto& entry : game->getPlannedArrests()) {
        blockedText += QString::fromStdString(entry.first) + " ע\"י " + QString::fromStdString(entry.second);
        found = true;
        break;
    }

    if (!found)
        blockedText += "אין שחקן חסום";

    lastBlockedLabel->setText(blockedText);
}

/**
 * @brief מדגיש את השחקן הנוכחי ומציג כפתורים ייחודיים לתפקיד
 */
void GameBoardWindow::highlightCurrentPlayer() {
    mainLayout->addStretch(2);
    highlightLayout = new QVBoxLayout();
    mainLayout->addLayout(highlightLayout);

    // החזרת שחקן קודם למקום
    if (highlightPlayer) {
        highlightLayout->removeWidget(highlightPlayer);
        mainLayout->addStretch(1);
        mainLayout->addWidget(highlightPlayer, 0, Qt::AlignCenter);
        mainLayout->addStretch(1);
        playerLayout->insertWidget(highlightIndex, highlightPlayer);
        highlightPlayer = nullptr;
    }

    // הדגשת שחקן נוכחי
    const Player &currentPlayer = game->getCurrentPlayer();
    QString name = QString::fromStdString(currentPlayer.getName());
    if (playerLabelMap.contains(name)) {
        highlightPlayer = playerLabelMap[name];
    }

    if (highlightPlayer) {
        playerLayout->removeWidget(highlightPlayer);
        mainLayout->addStretch(2);
        mainLayout->addWidget(highlightPlayer, 0, Qt::AlignCenter);
        mainLayout->addStretch(2);
    }

    // הצגת כפתורים ייחודיים לתפקיד
    if (game->getCurrentPlayer().getRole()->getName() == "Spy") {
        if (PeekButton) PeekButton->show();
    } else {
        if (PeekButton) PeekButton->hide();
    }

    if (game->getCurrentPlayer().getRole()->getName() == "Baron") {
        if (InvestButton) InvestButton->show();
    } else {
        if (InvestButton) InvestButton->hide();
    }
}

/**
 * @brief מעדכן תצוגת שחקנים מודחים - מעביר לבית קברות
 */
void GameBoardWindow::updatePlayerStatusVisuals() {
    const auto& playersList = game->getPlayer();

    for (const auto& player : playersList) {
        if (!player->isActive()) {
            QString name = QString::fromStdString(player->getName());
            if (!playerLabelMap.contains(name)) continue;

            QLabel* label = playerLabelMap[name];
            if (!label->isVisible()) continue;

            // אפקט שקיפות
            auto *effect = new QGraphicsOpacityEffect(this);
            effect->setOpacity(0.4);
            label->setGraphicsEffect(effect);

            // הסרה מהמסך
            playerLayout->removeWidget(label);
            label->hide();

            // הוספה לבית קברות
            addPlayerToGraveyard(label->text(), "הודח מהמשחק");
            playerLabelMap.remove(name);
        }
    }
}

/**
 * @brief בונה כפתורי פעולות ומחבר אותם לפונקציות הטיפול
 */
void GameBoardWindow::setupActions() {
    actionLayout = new QGridLayout();

    // יצירת כפתורים
    NextTurnButton = new QPushButton("Next Turn", this);
    GatherButton = new QPushButton("Gather - איסוף משאבים", this);
    TaxButton = new QPushButton("Tax - מס", this);
    BribeButton = new QPushButton("Bribe - שוחד", this);
    ArrestButton = new QPushButton("Arrest - מעצר", this);
    SanctionButton = new QPushButton("Sanction - חרם", this);
    CoupButton = new QPushButton("Coup - הפיכה", this);
    InvestButton = new QPushButton("Invest - השקעה", this);
    PeekButton = new QPushButton("Peek - להציץ", this);

    // חיבור סיגנלים
    connect(NextTurnButton, &QPushButton::clicked, this, [this](){game->nextTurn();});
    connect(GatherButton, &QPushButton::clicked, this, &GameBoardWindow::handleGather);
    connect(TaxButton, &QPushButton::clicked, this, &GameBoardWindow::handleTax);
    connect(BribeButton, &QPushButton::clicked, this, &GameBoardWindow::handleBribe);
    connect(ArrestButton, &QPushButton::clicked, this, &GameBoardWindow::handleArrest);
    connect(SanctionButton, &QPushButton::clicked, [this]() {
        requestTargetForAction([this](Player &attacker, Player &target) {
            return game->performSanction(attacker, target);
        });
    });
    connect(CoupButton, &QPushButton::clicked, this, &GameBoardWindow::handleCoup);
    connect(InvestButton, &QPushButton::clicked, this, &GameBoardWindow::handleInvest);
    connect(PeekButton, &QPushButton::clicked, this, &GameBoardWindow::handleSpyCoins);

    // פריסה ברשת
    actionLayout->addWidget(GatherButton, 0, 0);
    actionLayout->addWidget(TaxButton, 0, 1);
    actionLayout->addWidget(BribeButton, 0, 2);
    actionLayout->addWidget(ArrestButton, 0, 3);
    actionLayout->addWidget(SanctionButton, 0, 4);
    actionLayout->addWidget(CoupButton, 0, 5);
    actionLayout->addWidget(NextTurnButton, 0, 6);
    actionLayout->addWidget(InvestButton, 1, 0);
    InvestButton->hide();
    actionLayout->addWidget(PeekButton, 1, 1);
    PeekButton->hide();

    mainLayout->addLayout(actionLayout);
}

/**
 * @brief מציג הודעה על שחקן חסום
 */
void GameBoardWindow::showLastBlockedPlayer(const Player& player) {
    QString blockedText = "🔒 שחקן חסום: " + QString::fromStdString(player.getName());
    lastBlockedLabel->setText(blockedText);
}

/**
 * @brief מטפל בפעולת Spy - הצצה למטבעות ותכנון מעצר
 */
void GameBoardWindow::handleSpyCoins() {
    Player& spy = game->getCurrentPlayer();
    if (spy.getRole()->getName() != "Spy") {
        QMessageBox::warning(this, "שגיאה", "רק מרגל יכול לבצע פעולה זו.");
        return;
    }

    // הצגת מטבעות כל השחקנים
    std::map<std::string, int> coinCounts = game->getPlayersCoinCounts();
    QString message;
    for (const auto& [name, coins] : coinCounts) {
        message += QString::fromStdString(name) + ": " + QString::number(coins) + " מטבעות\n";
    }
    QMessageBox::information(this, "מודיעין מרגל", message);

    // אפשרות תכנון מעצר
    int choice = QMessageBox::question(this, "מעצר מתוכנן",
        "האם ברצונך לתכנן מעצר של שחקן לתור הבא?",
        QMessageBox::Yes | QMessageBox::No);

    if (choice == QMessageBox::Yes) {
        requestTargetPlayer("בחר שחקן למעצר עתידי", [this, &spy](Player& target) {
            game->markPlannedArrest(spy, target);
            updateBlockedArrestLabel();
            QMessageBox::information(this, "הוראה התקבלה",
                QString::fromStdString(spy.getName()) + " יתנגד למעצר של " +
                QString::fromStdString(target.getName()) + " בתור הבא.");
        });
    }
}

/**
 * @brief מעדכן תווית השחקן הנוכחי
 */
void GameBoardWindow::updateTurnLabel() {
    turnLabel->setText("Player turn: " + QString::fromStdString(game->getCurrentPlayer().getName()));
}

/**
 * @brief מעדכן תצוגת מטבעות - מלא לנוכחי, מוסתר לאחרים
 */
void GameBoardWindow::updateCoinLabel() {
    const std::string& currentPlayerName = game->getCurrentPlayer().getName();

    for (const auto& playerPtr : game->getPlayer()) {
        QString name = QString::fromStdString(playerPtr->getName());
        if (!playerLabelMap.contains(name)) continue;

        QLabel *label = playerLabelMap[name];

        if (playerPtr->getName() == currentPlayerName) {
            // שחקן נוכחי - מידע מלא
            QString role = QString::fromStdString(playerPtr->getRole()->getName());
            QString coins = QString::number(playerPtr->getCoins());
            label->setText(name + " (" + role + ") - 💰" + coins);
        } else {
            // שחקן אחר - מידע מוסתר
            label->setText(name + " (?) - 🔒 ");
        }

        // עיצוב לפי מצב
        if (!playerPtr->isActive()) {
            label->setStyleSheet("background: #DDEEFF; opacity: 0.5; border: 2px solid gray;");
        } else {
            label->setStyleSheet("background: #DDEEFF; border: 2px solid black;");
        }
    }
}

/**
 * @brief מבצע אנימציה קלה על תווית התור
 */
void GameBoardWindow::animateTurnLabel() {
    auto *animation = new QPropertyAnimation(turnLabel, "geometry");
    QRect start = turnLabel->geometry();
    QRect end = QRect(start.x(), start.y() + 15, start.width(), start.height());
    animation->setDuration(200);
    animation->setStartValue(start);
    animation->setEndValue(end);
    animation->setEasingCurve(QEasingCurve::OutBounce);
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

/**
 * @brief מאפס הדגשות שחקנים וחוזר למצב רגיל
 */
void GameBoardWindow::resetPlayerHighlights() {
    awaitingTargetSelection = false;
    pendingActionFunction = nullptr;

    for (QLabel* label : playerLabelMap.values()) {
        label->setStyleSheet("border: 2px solid black; padding: 10px; background: #DDEEFF;");
        label->removeEventFilter(this);
    }
}

/**
 * @brief מטפל בלחיצות על שחקנים בעת בחירת מטרה
 */
bool GameBoardWindow::eventFilter(QObject *watched, QEvent *event) {
    if (awaitingTargetSelection && event->type() == QEvent::MouseButtonPress) {
        for (auto it = playerLabelMap.begin(); it != playerLabelMap.end(); ++it) {
            if (watched == it.value()) {
                try {
                    QString name = it.key();
                    Player& attacker = game->getCurrentPlayer();

                    // מציאת השחקן המטרה
                    Player* targetPtr = nullptr;
                    for (const auto& p : game->getPlayer()) {
                        if (QString::fromStdString(p->getName()) == name) {
                            targetPtr = p.get();
                            break;
                        }
                    }

                    if (!targetPtr) throw std::runtime_error("Target player not found");

                    // ביצוע הפעולה
                    ActionResult result = pendingActionFunction(attacker, *targetPtr);
                    actionResultLabel->setText(QString::fromStdString(result.message));

                    // עדכון תצוגה אם הצליח
                    if (result.success) {
                        game->nextTurn();
                        updateTurnLabel();
                        highlightCurrentPlayer();
                        updateCoinLabel();
                        updatePlayerStatusVisuals();
                    }

                } catch (const std::exception& e) {
                    QMessageBox::warning(this, "שגיאה", e.what());
                }

                resetPlayerHighlights();
                return true;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

/**
 * @brief מפעיל מצב בחירת מטרה - מדגיש שחקנים וממתין ללחיצה
 */
void GameBoardWindow::requestTargetForAction(std::function<ActionResult(Player&, Player&)> actionFunc) {
    awaitingTargetSelection = true;
    pendingActionFunction = actionFunc;

    for (QLabel* label : playerLabelMap.values()) {
        label->setStyleSheet("background: yellow; border: 2px solid red; padding: 10px;");
        label->installEventFilter(this);
    }

    actionResultLabel->setText("בחר שחקן לביצוע הפעולה");
}

/**
 * @brief פותח דיאלוג לבחירת שחקן מטרה
 */
void GameBoardWindow::requestTargetPlayer(const QString& title, std::function<void(Player&)> callback) {
    QStringList playerNames;
    std::vector<Player*> alivePlayers;

    for (const auto& p : game->getPlayer()) {
        if (p->isActive() && p.get() != &game->getCurrentPlayer()) {
            playerNames << QString::fromStdString(p->getName());
            alivePlayers.push_back(p.get());
        }
    }

    if (playerNames.isEmpty()) {
        QMessageBox::information(this, "אין מטרות", "אין שחקנים אחרים לבחור בהם.");
        return;
    }

    bool ok = false;
    QString selectedName = QInputDialog::getItem(this, title, "בחר שחקן:", playerNames, 0, false, &ok);

    if (ok) {
        for (Player* p : alivePlayers) {
            if (QString::fromStdString(p->getName()) == selectedName) {
                callback(*p);
                return;
            }
        }
    }
}

/**
 * @brief מטפל בפעולת Gather - בדיקת חסימה, ביצוע, עדכון תצוגה
 */
void GameBoardWindow::handleGather() {
    Player& attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("gather")) {
        QMessageBox::information(this, "פעולה חסומה", "Gather חסומה עבורך בתור הזה.");
        return;
    }

    try {
        ActionResult result = game->performGather(attacker);
        bool wasBlocked = false;

        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("gather", &attacker);
            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            wasBlocked = askForBlock(QString::fromStdString(attacker.getName()), "Gather", blockers, "");

            if (result.success && !wasBlocked) {
                game->applyGather(attacker);
            }
        }

        actionResultLabel->setText(QString::fromStdString(result.message));
        game->nextTurn();
        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

/**
 * @brief מטפל בפעולת Tax - דומה ל-Gather אך עם תשואה גבוהה יותר
 */
void GameBoardWindow::handleTax() {
    Player& attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("tax")) {
        QMessageBox::information(this, "פעולה חסומה", "Tax חסומה עבורך בתור הזה.");
        return;
    }

    try {
        ActionResult result = game->performTax(attacker);
        bool wasBlocked = false;

        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("tax", &attacker);
            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            wasBlocked = askForBlock(QString::fromStdString(attacker.getName()), "Tax", blockers, "");

            if (wasBlocked) {
                QMessageBox::information(this, "חסימה", "הפעולה Tax נחסמה!");
                game->nextTurn();
                updateTurnLabel();
                highlightCurrentPlayer();
                updateCoinLabel();
                return;
            }
        }

        if (result.success && !wasBlocked) {
            game->applyTax(attacker);
        }

        actionResultLabel->setText(QString::fromStdString(result.message));
        game->nextTurn();
        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

/**
 * @brief מטפל בפעולת Bribe - נותן תור נוסף אם לא נחסם
 */
void GameBoardWindow::handleBribe() {
    Player &attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("bribe")) {
        QMessageBox::information(this, "Try other action", "Bribe action is blocked for you");
        return;
    }

    try {
        ActionResult result = game->performBribe(attacker);
        bool wasBlocked = false;

        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("bribe", &attacker);
            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            wasBlocked = askForBlock(QString::fromStdString(attacker.getName()), "Bribe", blockers, "");

            if (wasBlocked) {
                QMessageBox::information(this, "חסימה", "הפעולה Bribe נחסמה!");
                game->nextTurn();
                updateTurnLabel();
                highlightCurrentPlayer();
                updateCoinLabel();
                return;
            }
        }

        if (result.success && !wasBlocked) {
            game->applyBribe(attacker);
        }

        actionResultLabel->setText(QString::fromStdString(result.message));

        // אם אין תור נוסף - עבור לתור הבא
        if (!attacker.hasBonusAction()) {
            game->nextTurn();
        }
        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

/**
 * @brief מטפל בפעולת Arrest - גניבת מטבע מיריב
 */
void GameBoardWindow::handleArrest() {
    Player& attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("arrest")) {
        QMessageBox::information(this, "Try other action", "Arrest action is blocked for you");
        return;
    }

    requestTargetPlayer("בחר שחקן למעצר", [this, &attacker](Player& target) {
        try {
            ActionResult result = game->performArrest(attacker, target);
            bool wasBlocked = false;

            if (result.requiresBlocking) {
                std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("arrest", &attacker);
                QStringList blockers;
                for (const auto& name : blockerNames) {
                    blockers << QString::fromStdString(name);
                }

                wasBlocked = askForBlock(
                    QString::fromStdString(attacker.getName()),
                    "Arrest",
                    blockers,
                    QString::fromStdString(target.getName())
                );

                if (wasBlocked) {
                    QMessageBox::information(this, "חסימה", "הפעולה Arrest נחסמה!");
                    game->nextTurn();
                    updateTurnLabel();
                    highlightCurrentPlayer();
                    updateCoinLabel();
                    return;
                }
            }

            if (result.success && !wasBlocked) {
                game->applyArrest(attacker, target);
            }

            actionResultLabel->setText(QString::fromStdString(result.message));
            game->nextTurn();
            updateTurnLabel();
            highlightCurrentPlayer();
            updateCoinLabel();

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "שגיאה", e.what());
        }
    });
}

/**
 * @brief מטפל בפעולת Coup - הדחת יריב תמורת 7 מטבעות
 */
void GameBoardWindow::handleCoup() {
    Player& attacker = game->getCurrentPlayer();

    requestTargetPlayer("בחר שחקן למעצר", [this, &attacker](Player& target) {
        if (attacker.isBlocked("coup")) {
            QMessageBox::information(this, "Try other action", "Coup action is blocked for you");
            return;
        }

        try {
            ActionResult result = game->performCoup(attacker, target);
            bool wasBlocked = false;

            if (result.requiresBlocking) {
                std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("coup", &attacker);
                QStringList blockers;
                for (const auto& name : blockerNames) {
                    blockers << QString::fromStdString(name);
                }

                wasBlocked = askForBlock(
                    QString::fromStdString(attacker.getName()),
                    "Coup",
                    blockers,
                    QString::fromStdString(target.getName())
                );

                if (wasBlocked) {
                    // החוסם משלם 5 מטבעות
                    Player* blocker = game->getPlayerByName(blockers[0].toStdString());
                    if (blocker != nullptr && blocker->getCoins() >= 5) {
                        blocker->removeCoins(5);
                        game->addToCoinPool(5);
                    } else {
                        QMessageBox::warning(this, "שגיאה", "החוסם לא יכול לשלם 5 מטבעות!");
                    }

                    QMessageBox::information(this, "חסימה", "הפעולה Coup נחסמה!");
                    game->nextTurn();
                    updateTurnLabel();
                    highlightCurrentPlayer();
                    updateCoinLabel();
                    return;
                }
            }

            if (result.success && !wasBlocked) {
                game->applyCoup(attacker, target);
            }

            actionResultLabel->setText(QString::fromStdString(result.message));
            game->nextTurn();
            updateTurnLabel();
            highlightCurrentPlayer();
            updateCoinLabel();

        } catch (const std::exception& e) {
            QMessageBox::warning(this, "שגיאה", e.what());
        }
    });
}

/**
 * @brief מטפל בפעולת Invest - ייחודי לBaron, לא ניתן לחסימה
 */
void GameBoardWindow::handleInvest() {
    Player &attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("invest")) {
        QMessageBox::information(this, "Try other action", "Invest action is blocked for you");
        return;
    }

    try {
        ActionResult resultObj = game->performInvest(attacker);
        actionResultLabel->setText(QString::fromStdString(resultObj.message));

        if (resultObj.success) {
            game->nextTurn();
        }

        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

/**
 * @brief מטפל בסיום המשחק - מציג מנצח וחוזר לתפריט
 */
void GameBoardWindow::handleGameEnd(const QString& winnerName) {
    QMessageBox::information(this, "Winner!!", winnerName + "Is winner in the game!!");
    auto *mainMenu = new MainWindow();
    mainMenu->show();
    this->close();
}

/**
 * @brief מוסיף שחקן לבית קברות עם סיבת הדחה
 */
void GameBoardWindow::addPlayerToGraveyard(const QString &name, const QString &reason) {
    QLabel *graveLabel = new QLabel(name, this);
    graveLabel->setStyleSheet(
        "border: 2px solid gray; padding: 10px; background: #CCCCCC;"
        "font-size: 10px; opacity: 0.5;"
    );
    graveLabel->setMinimumSize(100, 60);
    graveLabel->setAlignment(Qt::AlignCenter);
    graveLabel->setToolTip(reason);

    QListWidgetItem *item = new QListWidgetItem();
    graveyardList->addItem(item);
    graveyardList->setItemWidget(item, graveLabel);

    if (playerLabelMap.contains(name)) {
        QWidget *toRemove = playerLabelMap[name];
        playerLayout->removeWidget(toRemove);
        toRemove->hide();
        playerLabelMap.remove(name);
    }
}

/**
 * @brief שואל שחקנים אם רוצים לחסום פעולה עם דיאלוג
 */
bool GameBoardWindow::askForBlock(const QString &attackerName, const QString &actionName, const QStringList &blockers, const QString &targetName) {
    for (const QString &blockerName : blockers) {
        BlockingDialog *dialog = new BlockingDialog(attackerName, actionName, QStringList() << blockerName, targetName, this);
        dialog->setWindowTitle(QString(" %1  You can make a blocking").arg(blockerName));

        bool blocked = false;
        QEventLoop loop;

        connect(dialog, &BlockingDialog::blockConfirmed, this, [&]() {
            blocked = true;
            loop.quit();
        });

        connect(dialog, &BlockingDialog::skipDeclined, this, [&]() {
            loop.quit();
        });

        dialog->show();
        loop.exec();
        dialog->deleteLater();

        if (blocked) {
            lastBlockedLabel->setText(QString("🔒 %1 blocked! the action was %2 and %3 saved")
                                      .arg(attackerName, actionName, targetName));
            return true;
        }
    }
    return false;
}

// פונקציות ריקות לשימוש עתידי
void GameBoardWindow::setupActionCards() {}
void GameBoardWindow::animateCardToCenter(QPushButton* card) { Q_UNUSED(card) }
void GameBoardWindow::chooseAndExecuteTargetAction(const QString& action) { Q_UNUSED(action) }