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

GameBoardWindow::GameBoardWindow(const std::vector<std::shared_ptr<Player>>& players, QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("New Game - COUP");
    resize(1000, 700);

    mainLayout = new QVBoxLayout(this);

    homeButton = new QPushButton("🏠 Back to menu", this);
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });
    mainLayout->addWidget(homeButton);

    // יצירת פאנל בית קברות
    graveyardDock = new QDockWidget("🪦 בית הקברות", this);
    graveyardDock->setAllowedAreas(Qt::RightDockWidgetArea);
    graveyardDock->setFeatures(QDockWidget::DockWidgetClosable | QDockWidget::DockWidgetMovable);

    // רשימת שחקנים מודחים
    graveyardList = new QListWidget(graveyardDock);
    graveyardList->setStyleSheet("background-color: #f4f4f4; font-size: 14px;");
    graveyardDock->setWidget(graveyardList);

    // מאפייני עיצוב ומיקום
    graveyardDock->setMinimumWidth(150);
    graveyardDock->setMaximumWidth(200);
    graveyardDock->setFloating(false);
    graveyardDock->setVisible(true);

    // הוספה לפריסת המסך
    mainLayout->addWidget(graveyardDock);

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

    // ניצור את האובייקט של Game ונכניס לתוכו את השחקנים שקיבלנו
    game = std::make_unique<Game>(this);
    connect(game.get(), &Game::gameOverSignal, this, &GameBoardWindow::handleGameEnd);
    connect(game.get(), &Game::playerEliminated, this, &GameBoardWindow::addPlayerToGraveyard);
    for (const auto& player : players) {
        game->addPlayer(player);
    }

    connect(game.get(), &Game::playerEliminated, this, &GameBoardWindow::addPlayerToGraveyard);

    setupPlayers();
    setupActions();
    updateTurnLabel();
    highlightCurrentPlayer();
    updateCoinLabel();
}

void GameBoardWindow::setupPlayers() {
    playerLayout = new QHBoxLayout();
    const auto& playersList = game->getPlayer();

    for (const auto& playerPtr  : playersList) {
        if (!playerPtr->isActive()){continue;}// דלג על שחקנים מודחים
        std::string name = playerPtr->getName();
        std::string role = playerPtr->getRole()->getName();
        QString label = QString::fromStdString(name + " (" + role + ")");

        QLabel *player = new QLabel(label, this);
        player->setStyleSheet("border: 2px solid black; padding: 15px; background: #DDEEFF;" "font-size: 10px;");
        player->setMinimumSize(120, 80);
        player->setAlignment(Qt::AlignCenter);
        playerLayout->addWidget(player);
        playerLabelMap[QString::fromStdString(name)] = player; // חדש
    }
    mainLayout->addLayout(playerLayout);
    highlightLayout = new QVBoxLayout();
    mainLayout->addLayout(highlightLayout);
}

void GameBoardWindow::updateBlockedArrestLabel() {
    QString blockedText = "🔒 חסימת מעצר: ";
    bool found = false;

    for (const auto& entry : game->getPlannedArrests()) {
        blockedText += QString::fromStdString(entry.first) + " ע\"י " + QString::fromStdString(entry.second);
        found = true;
        break;  // יש רק חסימה אחת בזמן נתון
    }

    if (!found)
        blockedText += "אין שחקן חסום";

    lastBlockedLabel->setText(blockedText);
}

void GameBoardWindow::highlightCurrentPlayer() {
    mainLayout->addStretch(2);

    highlightLayout = new QVBoxLayout();
    mainLayout->addLayout(highlightLayout);

    if (highlightPlayer) {
        highlightLayout->removeWidget(highlightPlayer);

        mainLayout->addStretch(1);
        mainLayout->addWidget(highlightPlayer, 0, Qt::AlignCenter);
        mainLayout->addStretch(1);

        playerLayout->insertWidget(highlightIndex, highlightPlayer);
        highlightPlayer = nullptr;
    }

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

    // בדיקה אם השחקן הוא מרגל
    if (game->getCurrentPlayer().getRole()->getName() == "Spy") {
        if (PeekButton) PeekButton->show();
    } else {
        if (PeekButton) PeekButton->hide();
    }

    // בדיקה אם השחקן הוא ברון
    if (game->getCurrentPlayer().getRole()->getName() == "Baron") {
        if (InvestButton) InvestButton->show();
    } else {
        if (InvestButton) InvestButton->hide();
    }
}

void GameBoardWindow::updatePlayerStatusVisuals() {
    const auto& playersList = game->getPlayer();

    for (const auto& player : playersList) {
        if (!player->isActive()) {
            QString name = QString::fromStdString(player->getName());

            if (!playerLabelMap.contains(name))
                continue;

            QLabel* label = playerLabelMap[name];

            if (!label->isVisible())
                continue;

            // 1. הפוך לחצי שקוף
            auto *effect = new QGraphicsOpacityEffect(this);
            effect->setOpacity(0.4);
            label->setGraphicsEffect(effect);

            // 2. הסר מהמסך
            playerLayout->removeWidget(label);
            label->hide();

            // 3. הוסף לבית הקברות
            addPlayerToGraveyard(label->text(), "הודח מהמשחק");

            // 4. הסרה מהמפה
            playerLabelMap.remove(name);
        }
    }
}

void GameBoardWindow::setupActions() {
    actionLayout = new QGridLayout();

    NextTurnButton = new QPushButton("Next Turn", this);
    GatherButton = new QPushButton("Gather - איסוף משאבים", this);
    TaxButton = new QPushButton("Tax - מס", this);
    BribeButton = new QPushButton("Bribe - שוחד", this);
    ArrestButton = new QPushButton("Arrest - מעצר", this);
    SanctionButton = new QPushButton("Sanction - חרם", this);
    CoupButton = new QPushButton("Coup - הפיכה", this);
    InvestButton = new QPushButton("Invest - השקעה", this);
    PeekButton = new QPushButton("Peek - להציץ", this);

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

    actionLayout->addWidget(GatherButton, 0,0);
    actionLayout->addWidget(TaxButton, 0,1);
    actionLayout->addWidget(BribeButton, 0,2);
    actionLayout->addWidget(ArrestButton, 0,3);
    actionLayout->addWidget(SanctionButton, 0,4);
    actionLayout->addWidget(CoupButton, 0,5);
    actionLayout->addWidget(NextTurnButton, 0,6);
    actionLayout->addWidget(InvestButton, 1,0);
    InvestButton->hide();
    actionLayout->addWidget(PeekButton, 1, 1); // או מיקום אחר שתרצי
    PeekButton->hide();

    mainLayout->addLayout(actionLayout);
}

void GameBoardWindow::showLastBlockedPlayer(const Player& player) {
    // עדכון הלייבל של השחקן החסום
    QString blockedText = "🔒 שחקן חסום: " + QString::fromStdString(player.getName());
    lastBlockedLabel->setText(blockedText);
}

void GameBoardWindow::handleSpyCoins() {
    Player& spy = game->getCurrentPlayer();
    if (spy.getRole()->getName() != "Spy") {
        QMessageBox::warning(this, "שגיאה", "רק מרגל יכול לבצע פעולה זו.");
        return;
    }

    PeekButton->show();
    std::map<std::string, int> coinCounts = game->getPlayersCoinCounts();
    QString message;
    for (const auto& [name, coins] : coinCounts) {
        message += QString::fromStdString(name) + ": " + QString::number(coins) + " מטבעות\n";
    }

    QMessageBox::information(this, "מודיעין מרגל", message);
    // בלי nextTurn – לא מבזבז תור!

    // שלב ב: אפשרות להטיל arrest לתור הבא
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
    PeekButton->hide();
}

void GameBoardWindow::updateTurnLabel() {
    turnLabel->setText("Player turn: " + QString::fromStdString(game->getCurrentPlayer().getName()));
}

void GameBoardWindow::updateCoinLabel() {
    const std::string& currentPlayerName = game->getCurrentPlayer().getName();

    for (const auto& playerPtr : game->getPlayer())
    {
        QString name = QString::fromStdString(playerPtr->getName());

        if (!playerLabelMap.contains(name)) continue;

        QLabel *label = playerLabelMap[name];

        if (playerPtr->getName() == currentPlayerName) {
            // זה השחקן שבתורו – נחשוף את המידע המלא
            QString role = QString::fromStdString(playerPtr->getRole()->getName());
            QString coins = QString::number(playerPtr->getCoins());
            label->setText(name + " (" + role + ") - 💰" + coins);
        }
        else {
            // שחקן אחר – נסתיר את המידע
            label->setText(name + " (?) - 🔒 ");
        }

        // עיצוב
        if (!playerPtr->isActive())
        {
            label->setStyleSheet("background: #DDEEFF; opacity: 0.5; border: 2px solid gray;");
        }
        else
        {
            label->setStyleSheet("background: #DDEEFF; border: 2px solid black;");
        }
    }
}

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

void GameBoardWindow::resetPlayerHighlights() {
    awaitingTargetSelection = false;
    pendingActionFunction = nullptr;

    for (QLabel* label : playerLabelMap.values()) {
        label->setStyleSheet("border: 2px solid black; padding: 10px; background: #DDEEFF;");
        label->removeEventFilter(this);
    }
}

bool GameBoardWindow::eventFilter(QObject *watched, QEvent *event) {
    if (awaitingTargetSelection && event->type() == QEvent::MouseButtonPress) {
        for (auto it = playerLabelMap.begin(); it != playerLabelMap.end(); ++it) {
            if (watched == it.value()) {
                try {
                    QString name = it.key();
                    Player& attacker = game->getCurrentPlayer();

                    // מציאת השחקן לפי שם
                    Player* targetPtr = nullptr;
                    for (const auto& p : game->getPlayer()) {
                        if (QString::fromStdString(p->getName()) == name) {
                            targetPtr = p.get();
                            break;
                        }
                    }

                    if (!targetPtr)
                        throw std::runtime_error("Target player not found");

                    Player& target = *targetPtr;

                    // קריאה לפונקציית פעולה שמחזירה ActionResult
                    ActionResult result = pendingActionFunction(attacker, target);
                    actionResultLabel->setText(QString::fromStdString(result.message));

                    // רק אם הצליח → ממשיכים לתור הבא
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

void GameBoardWindow::requestTargetForAction(std::function<ActionResult(Player&, Player&)> actionFunc) {
    awaitingTargetSelection = true;
    pendingActionFunction = actionFunc;

    for (QLabel* label : playerLabelMap.values()) {
        label->setStyleSheet("background: yellow; border: 2px solid red; padding: 10px;");
        label->installEventFilter(this);
    }

    actionResultLabel->setText("בחר שחקן לביצוע הפעולה");
}

void GameBoardWindow::requestTargetPlayer(const QString& title,
                                          std::function<void(Player&)> callback) {
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
    QString selectedName = QInputDialog::getItem(this, title,
                                                 "בחר שחקן:", playerNames, 0, false, &ok);

    if (ok) {
        for (Player* p : alivePlayers) {
            if (QString::fromStdString(p->getName()) == selectedName) {
                callback(*p);  // שולחת את השחקן שנבחר חזרה לפעולה
                return;
            }
        }
    }
}

void GameBoardWindow::handleGather() {
    Player& attacker = game->getCurrentPlayer();  // שימוש ב־Player& כמו בשאר הקוד

    // בדיקה אם הפעולה כבר חסומה
    if (attacker.isBlocked("gather")) {
        QMessageBox::information(this, "פעולה חסומה", "Gather חסומה עבורך בתור הזה.");
        return;
    }

    try {
        qDebug() << "DEBUG: מבצע performGather...";
        ActionResult result = game->performGather(attacker);
        qDebug() << "DEBUG: result.success =" << result.success;
        qDebug() << "DEBUG: result.requiresBlocking =" << result.requiresBlocking;

        bool wasBlocked = false;

        // אם הפעולה דורשת חסימה מצד שחקנים אחרים
        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("gather", &attacker);

            // המרת vector<string> ל־QStringList
            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            qDebug() << "DEBUG: blockers =" << blockers;

            wasBlocked = askForBlock(
                QString::fromStdString(attacker.getName()),
                "Gather",
                blockers,
                ""  // אין מטרה
            );

            // אם הפעולה לא נחסמה – מבצעים אותה בפועל
            if (result.success && !wasBlocked) {
                try {
                    game->applyGather(attacker);
                } catch (const std::exception& e) {
                    QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
                }
            }

            // הצגת תוצאה
            actionResultLabel->setText(QString::fromStdString(result.message));
            game->nextTurn();
            updateTurnLabel();
            highlightCurrentPlayer();
            updateCoinLabel();
        }

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

void GameBoardWindow::handleTax() {
    Player& attacker = game->getCurrentPlayer();  // עדיין עובדים עם Player& כמו שסיכמנו

    // בדיקה אם הפעולה כבר חסומה
    if (attacker.isBlocked("tax")) {
        QMessageBox::information(this, "פעולה חסומה", "Tax חסומה עבורך בתור הזה.");
        return;
    }

    try {
        qDebug() << "DEBUG: מבצע performTax...";
        ActionResult result = game->performTax(attacker);

        qDebug() << "DEBUG: result.success =" << result.success;
        qDebug() << "DEBUG: result.message =" << QString::fromStdString(result.message);
        qDebug() << "DEBUG: result.requiresBlocking =" << result.requiresBlocking;

        bool wasBlocked = false;

        // אם הפעולה דורשת חסימה מצד שחקנים אחרים
        if (result.requiresBlocking) {
            qDebug() << "DEBUG: הפעולה דורשת חסימה – מחפש חוסמים...";
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("tax", &attacker);

            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            qDebug() << "DEBUG: blockers = " << blockers;

            wasBlocked = askForBlock(
                QString::fromStdString(attacker.getName()),
                "Tax",
                blockers,
                ""  // אין מטרה
            );

            if (wasBlocked) {
                QMessageBox::information(this, "חסימה", "הפעולה Tax נחסמה!");
                game->nextTurn();
                updateTurnLabel();
                highlightCurrentPlayer();
                updateCoinLabel();
                return;
            }
        }

        // אם הפעולה לא נחסמה – מבצעים אותה בפועל
        if (result.success && !wasBlocked) {
            try {
                game->applyTax(attacker);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
            }
        }

        // הצגת תוצאה
        actionResultLabel->setText(QString::fromStdString(result.message));
        game->nextTurn();
        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();

    } catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

void GameBoardWindow::handleBribe() {
    Player &attacker = game->getCurrentPlayer();
    if (attacker.isBlocked("bribe")) {
        QMessageBox::information(this, "Try other action", "Bribe action is blocked for you");
        return;
    }
    try {
        qDebug() << "DEBUG: מבצע performBribe...";
        ActionResult result = game->performBribe(attacker);

        qDebug() << "DEBUG: result.success =" << result.success;
        qDebug() << "DEBUG: result.requiresBlocking =" << result.requiresBlocking;

        bool wasBlocked = false;

        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("bribe", &attacker);
            QStringList blockers;
            for (const std::string& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }

            qDebug() << "DEBUG: blockers =" << blockers;

            wasBlocked = askForBlock(
                QString::fromStdString(attacker.getName()),
                "Bribe",
                blockers,
                ""
            );

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
            try {
                game->applyBribe(attacker);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
            }
        }

        actionResultLabel->setText(QString::fromStdString(result.message));

        if (!attacker.hasBonusAction()) {
            game->nextTurn();
        }
        updateTurnLabel();
        highlightCurrentPlayer();
        updateCoinLabel();
    }catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

void GameBoardWindow::handleArrest() {
    Player& attacker = game->getCurrentPlayer();

    if (attacker.isBlocked("arrest")) {
        QMessageBox::information(this, "Try other action", "Arrest action is blocked for you");
        return;
    }
    requestTargetPlayer("בחר שחקן למעצר", [this, &attacker](Player& target) {
        try {
            qDebug() << "DEBUG: מבצע performArrest...";
            ActionResult result = game->performArrest(attacker, target);
            qDebug() << "DEBUG: result.success =" << result.success;
            qDebug() << "DEBUG: result.requiresBlocking =" << result.requiresBlocking;

            bool wasBlocked = false;

            if (result.requiresBlocking) {
                std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("arrest", &attacker);
                QStringList blockers;
                for (const auto& name : blockerNames) {
                    blockers << QString::fromStdString(name);
                }

                qDebug() << "DEBUG: blockers =" << blockers;

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
                try {
                    game->applyArrest(attacker, target);
                }
                catch (const std::exception& e) {
                    QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
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
    });
}

void GameBoardWindow::handleCoup() {
    Player& attacker = game->getCurrentPlayer();
    requestTargetPlayer("בחר שחקן למעצר", [this, &attacker](Player& target) {

    if (attacker.isBlocked("coup")) {
        QMessageBox::information(this, "Try other action", "Coup action is blocked for you");
        return;
    }

    try {
        qDebug() << "DEBUG: מבצע performCoup...";
        ActionResult result = game->performCoup(attacker, target);

        qDebug() << "DEBUG: result.success =" << result.success;
        qDebug() << "DEBUG: result.requiresBlocking =" << result.requiresBlocking;

        bool wasBlocked = false;

        if (result.requiresBlocking) {
            std::vector<std::string> blockerNames = game->getPlayersWhoCanBlock("coup", &attacker);
            QStringList blockers;
            for (const auto& name : blockerNames) {
                blockers << QString::fromStdString(name);
            }
            qDebug() << "DEBUG: blockers =" << blockers;

            wasBlocked = askForBlock(
                QString::fromStdString(attacker.getName()),
                "Coup",
                blockers,
                QString::fromStdString(target.getName())
                );

                if (wasBlocked) {
                    Player* blocker = game->getPlayerByName(blockers[0].toStdString());  // נניח שיש רק חוסם אחד שבחר לחסום

                    if (blocker != nullptr && blocker->getCoins() >= 5) {
                        blocker->removeCoins(5);
                        game->addToCoinPool(5);  // אם יש לך פונקציה כזו
                    }
                    else {
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
            try {
            game->applyCoup(attacker, target);
            } catch (const std::exception& e) {
                QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
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
    });
}

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

    }catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}

void GameBoardWindow::handleGameEnd(const QString& winnerName) {
    QMessageBox::information(this, "Winner!!", winnerName + "Is winner in the game!!");
    auto *mainMenu = new MainWindow();
    mainMenu->show();
    this->close();
}

void GameBoardWindow::addPlayerToGraveyard(const QString &name,  const QString &reason) {
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

bool GameBoardWindow::askForBlock(const QString &attackerName,
    const QString &actionName,
    const QStringList &blockers,
    const QString &targetName)
{
    for (const QString &blockerName : blockers) {
        BlockingDialog *dialog = new BlockingDialog(attackerName, actionName,  QStringList() << blockerName, targetName,this);
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
        loop.exec();  // מחכה עד שהשחקן יבחר

        dialog->deleteLater();

        if (blocked) {
            // מישהו חסם – לא צריך להמשיך לבדוק
            // עדכון הלייבל עם שם הפעולה ושם החוסם
            lastBlockedLabel->setText(QString("🔒 %1 blocked! the action was %2 and %3 saved")
                                      .arg(attackerName, actionName, targetName));
            return true;
        }
    }

    // אף אחד לא חסם
    return false;
}

// פונקציות dummy לתמיכה בפונקציונליות שלא הייתה בקוד המקורי
void GameBoardWindow::setupActionCards() {
    // פונקציה ריקה - הקוד המקורי לא השתמש בה
}

void GameBoardWindow::animateCardToCenter(QPushButton* card) {
    // פונקציה ריקה - הקוד המקורי לא השתמש בה
    Q_UNUSED(card)
}

void GameBoardWindow::chooseAndExecuteTargetAction(const QString& action) {
    // פונקציה ריקה - הקוד המקורי לא השתמש בה
    Q_UNUSED(action)
}
