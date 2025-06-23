#include "GameBoardWindow.h"
#include "MainWindow.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPropertyAnimation>
#include <QSequentialAnimationGroup>
#include <QLabel>
#include <QGuiApplication>
#include <QScreen>
#include <QGridLayout>

GameBoardWindow::GameBoardWindow(QWidget *parent) : QWidget(parent)
{
    setWindowTitle("New Game - COUP");
    resize(1000, 700);

    mainLayout = new QVBoxLayout(this);

    homeButton = new QPushButton("🏠 Back to manu", this);
    connect(homeButton, &QPushButton::clicked, [=]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });
    mainLayout->addWidget(homeButton);

    turnLabel = new QLabel(this);
    turnLabel->setAlignment(Qt::AlignCenter);
    QFont font = turnLabel->font();
    font.setPointSize(18);
    turnLabel->setFont(font);
    mainLayout->addWidget(turnLabel);

    coinLabel = new QLabel(this);
    coinLabel->setAlignment(Qt::AlignCenter);
    QFont cFont = coinLabel->font();
    font.setPointSize(14);
    turnLabel->setFont(cFont);
    mainLayout->addWidget(coinLabel);

    actionResultLabel = new QLabel(this);
    actionResultLabel->setAlignment(Qt::AlignCenter);
    actionResultLabel->setText("Choose action");
    mainLayout->addWidget(actionResultLabel);

    playerNames = {"Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6"};
    game = std::make_unique<Game>();

    for (const QString &name : playerNames) {
        auto role = RoleFactory::createRandomRole();
        auto player = std::make_shared<Player>(name.toStdString(), std::move(role));
        game->addPlayer(player);
    }

    setupPlayers();
    setupActions();
    updateTurnLabel();
    updateCoinLabel();

    // mainLayout->addWidget(turnLabel);
    //
    // turnLabel = new QLabel(this);
    // setWindowTitle("לוח המשחק");
    // setMinimumSize(800, 600);
    //
    //
    // mainLayout = new QVBoxLayout(this);
    // turnLabel = new QLabel("Player turn: Player 1",this);
    // turnLabel->setAlignment(Qt::AlignCenter);
    // QFont font = turnLabel->font();
    // font.setPointSize(16);
    // turnLabel->setFont(font);
    // mainLayout->addWidget(turnLabel);
    //
    // setupPlayers();
    // setupActions();
    //

    // auto *layout = new QVBoxLayout(this);
    //
    // titleLabel = new QLabel("כאן יוצג לוח המשחק 🎮");
    // titleLabel->setAlignment(Qt::AlignCenter);
    //
    // closeButton = new QPushButton("סגור משחק");
    //
    // layout->addWidget(titleLabel);
    // layout->addWidget(closeButton);
    //
    // setLayout(layout);
    //
    //
    // move(QGuiApplication::primaryScreen()->geometry().center() - frameGeometry().center());
    //
    // connect(closeButton, &QPushButton::clicked, this, &GameBoardWindow::closeGame);
}

void GameBoardWindow::setupPlayers() {
    playerLayout = new QHBoxLayout();
    const auto& playersList = game->getPlayer();

    // QStringList players = {"Player 1", "Player 2", "Player 3", "Player 4", "Player 5", "Player 6"};

    for (const auto& playerPtr  : playersList) {
        std::string name = playerPtr->getName();
        std::string role = playerPtr->getRole()->getName();
        QString label = QString::fromStdString(name + " (" + role + ")");

        QLabel *player = new QLabel(label, this);
        player->setStyleSheet("border: 2px solid black; padding: 10px; background: #DDEEFF;");
        player->setAlignment(Qt::AlignCenter);
        playerLabels.append(player);
        playerLayout->addWidget(player);
    }
    mainLayout->addLayout(playerLayout);
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
    connect(SanctionButton, &QPushButton::clicked, this, &GameBoardWindow::handleSanction);
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
    animateTurnLabel();
}
void GameBoardWindow::updateCoinLabel() {
    coinLabel->setText("Your coins: " + QString::number(game->getCurrentPlayer().getCoins()));
}
// void GameBoardWindow::handleAction() {
//     static int turn = 0;
//     turn = (turn + 1) % playerLabels.size();
//     turnLabel->setText("Player turn: " + playerLabels[turn]->text());
//     animateTurnLabels();
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

void GameBoardWindow::handleGather() {
    Player &attacker = game->getCurrentPlayer();
    QString result = QString::fromStdString(game->performGather(attacker));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}

void GameBoardWindow::handleTax() {
    Player &attacker = game->getCurrentPlayer();
    QString result = QString::fromStdString(game->performTax(attacker));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}

void GameBoardWindow::handleBribe() {
    Player &attacker = game->getCurrentPlayer();
    QString result = QString::fromStdString(game->performBribe(attacker));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}

void GameBoardWindow::handleArrest() {
    Player &attacker = game->getCurrentPlayer();
    Player &target = game->getCurrentPlayer();

    QString result = QString::fromStdString(game->performArrest(attacker, target));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}

void GameBoardWindow::handleSanction() {
    Player &attacker = game->getCurrentPlayer();
    Player &target = game->getCurrentPlayer();

    QString result = QString::fromStdString(game->performSanction(attacker, target));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}
void GameBoardWindow::handleCoup() {
    Player &attacker = game->getCurrentPlayer();
    Player &target = game->getCurrentPlayer();

    QString result = QString::fromStdString(game->performCoup(attacker,target));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}

void GameBoardWindow::handleInvest() {
    Player &attacker = game->getCurrentPlayer();

    QString result = QString::fromStdString(game->performInvest(attacker));
    actionResultLabel->setText(result);
    game->nextTurn();
    updateTurnLabel();
    updateCoinLabel();
}




// void GameBoardWindow::closeGame() {
//     close();  // סוגר את החלון הזה
// }
