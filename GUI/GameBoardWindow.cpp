#include "GameBoardWindow.h"
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


// GameBoardWindow::GameBoardWindow(QWidget *parent) : QWidget(parent)
// {
//     setWindowTitle("New Game - COUP");
//     resize(1000, 700);
//
//     mainLayout = new QVBoxLayout(this);
//
//     homeButton = new QPushButton("🏠 Back to manu", this);
//     connect(homeButton, &QPushButton::clicked, [=]() {
//         auto *mainMenu = new MainWindow();
//         mainMenu->show();
//         this->close();
//     });
//     mainLayout->addWidget(homeButton);
//
//
//     turnLabel = new QLabel(this);
//     // highlightLayout = new QVBoxLayout();
//     // mainLayout->addLayout(highlightLayout);
//     turnLabel->setAlignment(Qt::AlignCenter);
//     QFont font = turnLabel->font();
//     font.setPointSize(18);
//     turnLabel->setFont(font);
//     mainLayout->addStretch(1);
//     mainLayout->addWidget(turnLabel, 0, Qt::AlignHCenter);
//     mainLayout->addStretch(1);
//
//
//
//     coinLabel = new QLabel(this);
//     coinLabel->setAlignment(Qt::AlignCenter);
//     QFont cFont = coinLabel->font();
//     font.setPointSize(14);
//     coinLabel->setFont(cFont);
//     mainLayout->addStretch(1);
//     //mainLayout->addWidget(coinLabel, 0, Qt::AlignHCenter);
//     mainLayout->addStretch(1);
//
//     actionResultLabel = new QLabel(this);
//     actionResultLabel->setAlignment(Qt::AlignCenter);
//     actionResultLabel->setText("Choose action");
//     mainLayout->addWidget(actionResultLabel);
//
//     // playerNames = {"Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6"};
//     // game = std::make_unique<Game>();
//     //
//     // for (const QString &name : playerNames) {
//     //     auto role = RoleFactory::createRandomRole();
//     //     auto player = std::make_shared<Player>(name.toStdString(), std::move(role));
//     //     game->addPlayer(player);
//     // }
//
//     setupPlayers();
//     setupActions();
//     updateTurnLabel();
//     highlightCurrentPlayer();
//     updateCoinLabel();
// }

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
    font.setPointSize(14);
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
        //playerLabels.append(player);
        playerLayout->addWidget(player);
        playerLabelMap[QString::fromStdString(name)] = player; // חדש

    }
    mainLayout->addLayout(playerLayout);
    highlightLayout = new QVBoxLayout();
    mainLayout->addLayout(highlightLayout);
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
    // for (int i = 0; i < game->getPlayer().size(); i++) {
    //     if (game->getPlayer()[i].get() == &currentPlayer) {
    //         highlightPlayer = playerLabels[i];
    //         highlightIndex = i;
    //         break;
    //     }
    // }

    if (highlightPlayer) {
        playerLayout->removeWidget(highlightPlayer);
        mainLayout->addStretch(2);
        mainLayout->addWidget(highlightPlayer, 0, Qt::AlignCenter);
        mainLayout->addStretch(2);
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

// void GameBoardWindow::setupCards() {
//     cardLayout = new QHBoxLayout();
//
//     for (int i = 0; i < 2; i++) {
//         QLabel *card = new QLabel("🔒", this);
//         card->setStyleSheet("font-size: 48px; margin: 10px;");
//         cardLabels.append(card);
//         cardLayout->addWidget(card);
//     }
//     mainLayout->addLayout(cardLayout);
// }

void GameBoardWindow::setupActions() {
    actionLayout = new QGridLayout();

    GatherButton = new QPushButton("Gather - איסוף משאבים", this);
    TaxButton = new QPushButton("Tax - מס", this);
    BribeButton = new QPushButton("Bribe - שוחד", this);
    ArrestButton = new QPushButton("Arrest - מעצר", this);
    SanctionButton = new QPushButton("Sanction - חרם", this);
    CoupButton = new QPushButton("Coup - הפיכה", this);
    InvestButton = new QPushButton("Invest - השקעה", this);


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



    actionLayout->addWidget(GatherButton, 0,0);
    actionLayout->addWidget(TaxButton, 0,1);
    actionLayout->addWidget(BribeButton, 0,2);
    actionLayout->addWidget(ArrestButton, 0,3);
    actionLayout->addWidget(SanctionButton, 0,4);
    actionLayout->addWidget(CoupButton, 0,5);
    actionLayout->addWidget(InvestButton, 1,0);
    mainLayout->addLayout(actionLayout);
    // actionButton = new QPushButton("Make actions", this);
    // connect(actionButton, &QPushButton::clicked, this, &GameBoardWindow::handleAction);
    //
    // actionLayout->addWidget(actionButton);
    // mainLayout->addLayout(actionLayout);
}

void GameBoardWindow::updateTurnLabel() {
    turnLabel->setText("Player turn: " + QString::fromStdString(game->getCurrentPlayer().getName()));
    // animateTurnLabel();
}
void GameBoardWindow::updateCoinLabel() {
    for (const auto& playerPtr : game->getPlayer())
    {
        QString name = QString::fromStdString(playerPtr->getName());
        if (!playerLabelMap.contains(name)) continue;

        QString role = QString::fromStdString(playerPtr->getRole()->getName());
        QString coins = QString::number(playerPtr->getCoins());

        QLabel *label = playerLabelMap[name];
        label->setText(name + " (" + role + ") - 💰" + coins);

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
//     const auto& playersList = game->getPlayer();
//     for (int i = 0; i < playersList.size(); i++) {
//         auto& player = playersList[i];
//         QString name = QString::fromStdString(player->getName());
//         QString role = QString::fromStdString(player->getRole()->getName());
//         QString coins = QString::number(player->getCoins());
//         playerLabels[i]->setText(name + " (" + role + ") - 💰" + coins);
//
//         if (!player->isActive()) {
//             playerLabels[i]->setStyleSheet("background: #DDEEFF; opacity: 0.5; border: 2px solid gray;");
//         }
//         else {
//             playerLabels[i]->setStyleSheet("background: #DDEEFF; border: 2px solid black;");
//         }
//     }
// }


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


// bool GameBoardWindow::eventFilter(QObject *watched, QEvent *event) {
//     if (awaitingTargetSelection && event->type() == QEvent::MouseButtonPress) {
//         for (auto it = playerLabelMap.begin(); it != playerLabelMap.end(); ++it) {
//             if (watched == it.value()) {
//                 try {
//                     QString name = it.key();
//                     Player& attacker = game->getCurrentPlayer();
//
//                     // חיפוש השחקן לפי שם (כי getPlayer() מחזיר מצביעים)
//                     Player* targetPtr = nullptr;
//                     for (const auto& p : game->getPlayer()) {
//                         if (QString::fromStdString(p->getName()) == name) {
//                             targetPtr = p.get();
//                             break;
//                         }
//                     }
//
//                     if (!targetPtr) throw std::runtime_error("Target player not found");
//
//                     Player& target = *targetPtr;
//
//                     QString result = QString::fromStdString(pendingActionFunction(attacker, target));
//                     actionResultLabel->setText(result);
//                     game->nextTurn();
//                     updateTurnLabel();
//                     highlightCurrentPlayer();
//                     updateCoinLabel();
//                     updatePlayerStatusVisuals();
//                 } catch (const std::exception& e) {
//                     QMessageBox::warning(this, "שגיאה", e.what());
//                 }
//                 resetPlayerHighlights();
//                 return true;
//             }
//         }
//     }
//     return QWidget::eventFilter(watched, event);
// }
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


// void GameBoardWindow::handleIncome() {
//     Player &attacker = game->getCurrentPlayer();
//     Player &target = game->getCurrentPlayer();

     //game->performGather(attacker);
     // game->performTax(attacker);
     //  game->performBribe(attacker);
     //  game->performArrest(attacker,target);
     //  game->performSanction(attacker, target);
     //  game->performCoup(attacker, target);
     //  game->performInvest(attacker);
     // game->performInvest(attacker);


    //QString result = QString::fromStdString(game->performInvest(attacker));

//     actionResultLabel->setText(result);
//     game->nextTurn();
//     updateTurnLabel();
//     updateCoinLabel();
// }

// void GameBoardWindow::handleGather() {
//     // QString message;
//     Player &attacker = game->getCurrentPlayer();
//     // פה אני בודקת מצב שהפעולה כבר חסומה
//     if (attacker.isBlocked("gather")) {
//         QMessageBox::information(this, "Try author action", "Gather action is blocked for you");
//         // updateTurnLabel();
//         // highlightCurrentPlayer();
//         // updateCoinLabel();
//         return;
//     }
//     try {
//         ActionResult result = game->performGather(attacker);
//         // פה אני בודקת מצב של האפשרות לחסום פעולה אחרי שניסיתי לעשות אותה
//         if (result.requiresBlocking) {
//             QStringList blockers = game->getPlayersWhoCanBlock("Gather", attacker); // פונקציה שתכף נבנה
//             bool wasBlocked = askForBlock(attacker.getName().data(), "Gather", blockers, "");
//
//             if (wasBlocked) {
//                 // message = QString()
//                 QMessageBox::information(this, "BLOCK MESSAGE",
//                                          QString("Gather is blocked for you from %1").arg(blockers));
//
//                 //showMessage("הפעולה Bribe נחסמה!");
//                 // return; // לא ממשיכים עם הפעולה
//                 game->nextTurn();
//             }
//         }
//         //QString result = QString::fromStdString(resultObj.message);
//         // actionResultLabel->setText(result);
//         actionResultLabel->setText(QString::fromStdString(result.message));
//
//         if (result.success) {
//             game->nextTurn();
//         }
//
//         //game->nextTurn();
//         updateTurnLabel();
//         highlightCurrentPlayer();
//         updateCoinLabel();
//     } catch (const std::exception& e) {
//         QMessageBox::warning(this, "This action is blocked try other action", e.what());
//     }
//     // updateTurnLabel();
//     // highlightCurrentPlayer();
//     // updateCoinLabel();
//     // game->nextTurn();
// }

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
                    game->applyTax(attacker);
                } catch (const std::exception& e) {
                    QMessageBox::warning(this, "שגיאה בביצוע הפעולה", e.what());
                }
            }

            // if (wasBlocked) {
            //     QMessageBox::information(this, "חסימה", "הפעולה Gather נחסמה!");

                // הצגת תוצאה
                actionResultLabel->setText(QString::fromStdString(result.message));
                game->nextTurn();
                updateTurnLabel();
                highlightCurrentPlayer();
                updateCoinLabel();
                // return;
            }
        }

        // // הצגת תוצאה
        // actionResultLabel->setText(QString::fromStdString(result.message));

        // if (result.success) {
        //     game->nextTurn();
        // }

        // updateTurnLabel();
        // highlightCurrentPlayer();
        // updateCoinLabel();
    // }
    catch (const std::exception& e) {
        QMessageBox::warning(this, "שגיאה", e.what());
    }
}
//
// void GameBoardWindow::handleTax() {
//     Player &attacker = game->getCurrentPlayer();
//     if (attacker.isBlocked("tax")) {
//         QMessageBox::information(this, "Try author action", "Tax action is blocked for you");
//         // updateTurnLabel();
//         // highlightCurrentPlayer();
//         // updateCoinLabel();
//         return;
//     }
//
//     try {
//         ActionResult resultObj = game->performTax(attacker);
//         // QString result = QString::fromStdString(resultObj.message);
//         // actionResultLabel->setText(result);
//         actionResultLabel->setText(QString::fromStdString(resultObj.message));
//
//         if (resultObj.success) {
//             game->nextTurn();
//         }
//
//         //game->nextTurn();
//         updateTurnLabel();
//         highlightCurrentPlayer();
//         updateCoinLabel();
//     } catch (const std::exception& e) {
//         QMessageBox::warning(this, "This action is blocked", e.what());
//     }
//     // updateTurnLabel();
//     // highlightCurrentPlayer();
//     // updateCoinLabel();
//     // game->nextTurn();
// }

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

        // QString result = QString::fromStdString(resultObj.message);
        // actionResultLabel->setText(result);

        // if (resultObj.success) {
        //     game->nextTurn();
        // }

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


// void GameBoardWindow::handleArrest() {
//     Player &attacker = game->getCurrentPlayer();
//     Player &target = game->getCurrentPlayer();
//
//     QString result = QString::fromStdString(game->performArrest(attacker, target));
//     actionResultLabel->setText(result);
//     game->nextTurn();
//     updateTurnLabel();
//     updateCoinLabel();
// }
//

// void GameBoardWindow::handleSanction() {
//     Player &attacker = game->getCurrentPlayer();
//     Player &target = game->getCurrentPlayer();
//
//     QString result = QString::fromStdString(game->performSanction(attacker, target));
//     actionResultLabel->setText(result);
//     game->nextTurn();
//     updateTurnLabel();
//     updateCoinLabel();
// }

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

// void GameBoardWindow::handleCoup() {
//     Player &attacker = game->getCurrentPlayer();
//     if (attacker.getCoins() < 7) {
//         QMessageBox::warning(this, "You have no coins to make coup (you need 7 or more coins)!", "ERROR:Coins");
//         return;
//     }
//     actionResultLabel->setText("Select the player you want to eliminate");
//     awaitingTarget = true;
//
//     for (int i = 0; i < playerLabels.size(); i++) {
//         Player &target = *game->getPlayer()[i];
//
//         if (!target.isActive() || &target == &attacker) {
//             continue;
//         }
//         playerLabels[i]->setStyleSheet("background-color: #FFCCCC; border: 3px solid red;");
//         playerLabels[i]->installEventFilter(this);
//     }
//
//     //QString result = QString::fromStdString(game->performCoup(attacker,target));
//     //actionResultLabel->setText(result);
//     game->nextTurn();
//     updateTurnLabel();
//     updateCoinLabel();
// }

void GameBoardWindow::handleInvest() {
    Player &attacker = game->getCurrentPlayer();
    if (attacker.isBlocked("invest")) {
        QMessageBox::information(this, "Try other action", "Invest action is blocked for you");
        return;
    }

    try {
        ActionResult resultObj = game->performInvest(attacker);
        // QString result = QString::fromStdString(resultObj.message);
        // actionResultLabel->setText(result);
        actionResultLabel->setText(QString::fromStdString(resultObj.message));

        if (resultObj.success) {
            game->nextTurn();
        }

        //game->nextTurn();
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




// void GameBoardWindow::closeGame() {
//     close();  // סוגר את החלון הזה
// }

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

    // for (int i = 0; i < playerLabels.size(); ++i) {
    //     if (playerLabelMap.contains(name)) {
    //         QWidget *toRemove = playerLabelMap[name];
    //         playerLayout->removeWidget(toRemove);
    //         toRemove->hide();
    //         playerLabelMap.remove(name);
    //     }
    //     // if (playerLabels[i]->text().contains(name)) {
    //     //     QWidget *toRemove = playerLabels[i];
    //     //     playerLayout->removeWidget(toRemove);
    //     //     toRemove->hide();
    //     //     playerLabels.remove(i);
    //     //     break;
    //     // }
    // }
}

bool GameBoardWindow::askForBlock(const QString &attackerName,
    const QString &actionName,
    const QStringList &blockers,
    const QString &targetName = "")
{

    for (const QString &blockerName : blockers) {
        BlockingDialog *dialog = new BlockingDialog(attackerName, actionName, blockers, targetName,this);
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
            return true;
        }
    }

    // אף אחד לא חסם
    return false;
}



