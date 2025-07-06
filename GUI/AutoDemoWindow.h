#ifndef AUTODEMOWINDOW_H
#define AUTODEMOWINDOW_H

#include <QWidget>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QTimer>
#include <QLabel>
#include <QProgressBar>
#include <QScrollArea>
#include <memory>
#include <random>
#include "../Game.h"
#include "../Player.h"
#include "../Roles/RoleFactory.h"

class AutoDemoWindow : public QWidget {
    Q_OBJECT

public:
    explicit AutoDemoWindow(QWidget *parent = nullptr);

    private slots:
        void startDemo();
    void pauseDemo();
    void stopDemo();
    void stepDemo();
    void onDemoStep();
    void adjustSpeed(int value);

private:
    // UI Components
    QVBoxLayout *mainLayout;
    QTextEdit *logDisplay;
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *stepButton;
    QPushButton *homeButton;
    QLabel *statusLabel;
    QLabel *turnLabel;
    QLabel *speedLabel;
    QProgressBar *speedBar;
    QScrollArea *scrollArea;

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

    // Helper Methods
    void setupUI();
    void setupGame();
    void createPlayers();
    void logMessage(const QString &message, const QString &color = "black");
    void logHeader(const QString &title);
    void logSubHeader(const QString &subtitle);
    void performRandomAction();
    void updateStatus();
    void demonstrateRoleAbilities();
    void showGameState();
    void showFinalResults();
    QString getRoleColor(const QString &roleName);
    QString getPlayerInfo(const Player &player);
};

#endif // AUTODEMOWINDOW_H