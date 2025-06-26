#ifndef GAMEBOARDWINDOW_H
#define GAMEBOARDWINDOW_H

#include <QWidget>
#include <QVector>
#include <QString>
#include <memory>
#include "../Game.h"
#include "../Player.h"
#include "../Roles/RoleFactory.h"

class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPropertyAnimation;

class GameBoardWindow : public QWidget {
    Q_OBJECT

public:
    explicit GameBoardWindow(QWidget *parent = nullptr);

    // private slots:
    //     void closeGame();

private:
    QLabel *turnLabel;
    QLabel *coinLabel;
    QLabel *actionResultLabel;
    QVBoxLayout *mainLayout;
    QHBoxLayout *playerLayout;
    QGridLayout *actionLayout;
    QHBoxLayout *cardLayout;

    QVector<QLabel*> playerLabels;
    QVector<QLabel*> actionCards;
    QVector<QString> playerNames;


    QPushButton *GatherButton;
    QPushButton *TaxButton;
    QPushButton *BribeButton;
    QPushButton *ArrestButton;
    QPushButton *SanctionButton;
    QPushButton *CoupButton;
    QPushButton *InvestButton;
    QPushButton *homeButton;

    std::unique_ptr<Game> game;
    std::function<std::string(Player&, Player&)> pendingActionFunction;
    bool awaitingTargetSelection = false;

    void setupPlayers();
    void setupActions();
    void setupActionCards();
    void updateTurnLabel();
    void updateCoinLabel();
    void animateTurnLabel();
    void animateCardToCenter(QPushButton* card);
    void chooseAndExecuteTargetAction(const QString& action);
    void requestTargetForAction(std::function<std::string(Player&, Player&)> actionFunc);
    void resetPlayerHighlights();
    bool eventFilter(QObject *watched, QEvent *event)override;

private slots:
    //void handleIncome();
    void handleGather();
    void handleTax();
    void handleBribe();
    // void handleArrest();
    // void handleSanction();
    // void handleCoup();
    void handleInvest();
};

#endif // GAMEBOARDWINDOW_H
