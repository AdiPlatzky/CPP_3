#ifndef AUTOGAMEWINDOW_H
#define AUTOGAMEWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QTimer>
#include <QProgressBar>
#include <QTextEdit>
#include <QFrame>
#include <QGroupBox>
#include <QSpinBox>
#include <QComboBox>
#include <QCheckBox>
#include <QTextCursor>

// Forward declaration
class SimpleGameBoard;

class AutoGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AutoGameWindow(const QString& player1Name, const QString& player1Algorithm,
                           const QString& player2Name, const QString& player2Algorithm,
                           QWidget *parent = nullptr);
    ~AutoGameWindow();

private slots:
    void startGame();
    void pauseGame();
    void resetGame();
    void nextMove();
    void onGameFinished();
    void onSpeedChanged(int speed);
    void onAutoMoveToggled(bool enabled);

private:
    void setupUI();
    void setupControlPanel();
    void setupGameDisplay();
    void setupPlayersInfo();
    void updatePlayerInfo();
    void updateGameBoard();
    void updateGameStatus();
    void updateMoveHistory();
    void makeAutoMove();
    void executeMove(int move);
    void checkGameEnd();
    void showGameResults();

    // Game logic
    int getComputerMove(const QString& algorithm, int player);
    int getRandomMove();
    int getMinimaxMove(int player, int depth = 4);
    int getAlphaBetaMove(int player, int depth = 4);
    int getBestMove(int player);
    int evaluateBoard(SimpleGameBoard* board, int player);

    // UI Components
    QWidget* m_centralWidget;
    QHBoxLayout* m_mainLayout;

    // Game Board Display
    QFrame* m_gameFrame;
    QGridLayout* m_boardLayout;
    QLabel* m_boardLabels[6][7];  // 6 rows, 7 columns

    // Control Panel
    QFrame* m_controlFrame;
    QVBoxLayout* m_controlLayout;
    QPushButton* m_startButton;
    QPushButton* m_pauseButton;
    QPushButton* m_resetButton;
    QPushButton* m_nextMoveButton;
    QCheckBox* m_autoMoveCheckBox;
    QSpinBox* m_speedSpinBox;
    QLabel* m_speedLabel;

    // Players Info
    QGroupBox* m_player1Group;
    QGroupBox* m_player2Group;
    QLabel* m_player1NameLabel;
    QLabel* m_player1AlgorithmLabel;
    QLabel* m_player1ScoreLabel;
    QLabel* m_player2NameLabel;
    QLabel* m_player2AlgorithmLabel;
    QLabel* m_player2ScoreLabel;

    // Game Status
    QLabel* m_currentPlayerLabel;
    QLabel* m_moveCountLabel;
    QLabel* m_gameStatusLabel;
    QProgressBar* m_gameProgressBar;

    // Move History
    QTextEdit* m_moveHistoryTextEdit;

    // Game State
    SimpleGameBoard* m_gameBoard;
    QTimer* m_gameTimer;
    QString m_player1Name;
    QString m_player1Algorithm;
    QString m_player2Name;
    QString m_player2Algorithm;
    int m_currentPlayer;
    int m_moveCount;
    int m_gameSpeed;
    bool m_gameRunning;
    bool m_gamePaused;
    bool m_autoMoveEnabled;
    QStringList m_moveHistory;

    // Game Results
    int m_player1Score;
    int m_player2Score;
    int m_draws;

    // Colors for pieces
    QColor m_player1Color;
    QColor m_player2Color;
    QColor m_emptyColor;
    QColor m_highlightColor;
};

#endif // AUTOGAMEWINDOW_H