#ifndef AUTODEMOWINDOW_H
#define AUTODEMOWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <QSlider>
#include <QGroupBox>
#include <QMap>
#include <QListWidget>
#include <QDockWidget>
#include <memory>
#include <random>
#include "../Game.h"
#include "../Player.h"
#include "../Roles/RoleFactory.h"

class AutoDemoWindow : public QWidget {
    Q_OBJECT

public:
    explicit AutoDemoWindow(const std::vector<std::shared_ptr<Player>>& players,
                           bool showDetailedActions = true,
                           QWidget *parent = nullptr);

private slots:
    void startDemo();
    void pauseDemo();
    void stopDemo();
    void stepDemo();
    void onDemoStep();
    void adjustSpeed(int value);
    void onGameEnd(const QString& winnerName);
    void onPlayerEliminated(const QString& playerName, const QString& reason);

private:
    // UI Components - similar to GameBoardWindow
    QVBoxLayout *mainLayout;
    QHBoxLayout *playerLayout;
    QGridLayout *controlLayout;

    // Player display
    QMap<QString, QLabel*> playerLabelMap;
    QLabel *currentPlayerHighlight = nullptr;

    // Game status
    QLabel *turnLabel;
    QLabel *statusLabel;
    QLabel *actionResultLabel;
    QLabel *speedLabel;

    // Controls
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *stepButton;
    QPushButton *homeButton;
    QSlider *speedSlider;

    // Action log
    QTextEdit *actionLog;

    // Graveyard (eliminated players)
    QDockWidget *graveyardDock;
    QListWidget *graveyardList;

    // Game Components
    std::unique_ptr<Game> game;
    std::vector<std::shared_ptr<Player>> players;
    std::mt19937 rng;
    QTimer *demoTimer;

    // Demo State
    enum DemoState {
        STOPPED,
        RUNNING,
        PAUSED,
        FINISHED
    };
    DemoState currentState;
    int currentStep;
    int maxTurns;
    bool showDetailedActions;

    // Helper Methods
    void setupUI();
    void setupGame();
    void setupPlayerDisplay();
    void setupControls();
    void setupActionLog();
    void setupGraveyard();

    void updatePlayerDisplay();
    void highlightCurrentPlayer();
    void updateGameStatus();
    void logAction(const QString &message, const QString &color = "#ecf0f1");

    void performAIAction();
    QString getRandomAction(const Player& player);
    std::shared_ptr<Player> getRandomTarget(const Player& attacker);
    void simulatePlayerDecision(const QString& action, Player& player);

    QString getRoleColor(const QString &roleName);
    QString getPlayerInfo(const Player &player);
    void addPlayerToGraveyard(const QString &name, const QString &reason);

    void showGameOverDialog(const QString& winner);
};

#endif // AUTODEMOWINDOW_H