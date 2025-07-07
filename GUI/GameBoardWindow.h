#ifndef GAMEBOARDWINDOW_H
#define GAMEBOARDWINDOW_H

#include <QMap>
#include <QListWidget>
#include <QDockWidget>
#include <QVector>
#include <QString>
#include <memory>
#include "../Game.h"
#include "../Player.h"
#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <vector>
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
    explicit GameBoardWindow(const std::vector<std::shared_ptr<Player>>& players, QWidget *parent = nullptr);
    QString getRoleColor(const QString &roleName); // הוסיפי כאן

private:
    QLabel *turnLabel;
    QLabel *coinLabel;
    QLabel *actionResultLabel;
    QLabel *highlightPlayer = nullptr;
    QLabel* lastBlockedLabel = nullptr;

    QVBoxLayout *mainLayout;
    QHBoxLayout *playerLayout;
    QGridLayout *actionLayout;
    QHBoxLayout *cardLayout;
    QVBoxLayout *highlightLayout;
    QVBoxLayout *lastBlockedLayout;

    QVector<QLabel*> actionCards;
    QVector<QString> playerNames;

    QMap<QString, QLabel*> playerLabelMap; // map name → label

    QPushButton *NextTurnButton;
    QPushButton *GatherButton;
    QPushButton *TaxButton;
    QPushButton *BribeButton;
    QPushButton *ArrestButton;
    QPushButton *SanctionButton;
    QPushButton *CoupButton;
    QPushButton *InvestButton;
    QPushButton *homeButton;
    QPushButton *PeekButton;

    // תוספות חדשות לבית הקברות:
    QDockWidget *graveyardDock;
    QListWidget *graveyardList;

    QMap<QString, QWidget*> playerWidgets; // קישור בין שם שחקן לווידג'ט שלו

    std::unique_ptr<Game> game;
    std::function<ActionResult(Player&, Player&)> pendingActionFunction;
    bool awaitingTargetSelection = false;
    int highlightIndex = -1;

    void handleSpyCoins();

    void setupPlayers();
    void setupActions();
    void setupActionCards();
    void updateTurnLabel();
    void updateCoinLabel();
    void animateTurnLabel();
    void animateCardToCenter(QPushButton* card);
    void chooseAndExecuteTargetAction(const QString& action);
    void requestTargetForAction(std::function<ActionResult(Player&, Player&)> actionFunc);
    void resetPlayerHighlights();
    void highlightCurrentPlayer();
    void updatePlayerStatusVisuals();

    void requestTargetPlayer(const QString& title,
                         std::function<void(Player&)> callback);

    bool eventFilter(QObject *watched, QEvent *event) override;
    void addPlayerToGraveyard(const QString &name,  const QString &reason);
    bool askForBlock(const QString &attackerName, const QString &actionName, const QStringList &blockers, const QString &targetName = "");
    void showLastBlockedPlayer(const Player& player);
    void updateBlockedArrestLabel();

private slots:
    void handleGather();
    void handleTax();
    void handleBribe();
    void handleArrest();
    void handleCoup();
    void handleInvest();
    void handleGameEnd(const QString& winnerName);
};

#endif // GAMEBOARDWINDOW_H