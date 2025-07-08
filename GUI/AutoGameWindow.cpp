// 12adi45@gmail.com

#include "AutoGameWindow.h"
#include "../Game.h"
#include "GameBoardWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QCheckBox>
#include <QSplitter>
#include <QHeaderView>
#include <QScrollArea>
#include <QDateTime>
#include <QDebug>
#include <algorithm>
#include <climits>

/**
 * @class SimpleGameBoard
 * @brief מחלקה פשוטה לניהול לוח Connect Four - 6 שורות ו-7 עמודות
 */
class SimpleGameBoard {
private:
    int board[6][7];      // לוח המשחק
    int currentPlayer;    // השחקן הנוכחי
    bool gameOver;        // האם המשחק הסתיים
    int winner;          // המנצח (0=תיקו, 1-2=שחקנים)

public:
    /** @brief בנאי - מאתחל לוח ריק */
    SimpleGameBoard() : currentPlayer(1), gameOver(false), winner(0) {
        resetBoard();
    }

    /** @brief מאפס את הלוח למצב התחלתי */
    void resetBoard() {
        for (int i = 0; i < 6; i++) {
            for (int j = 0; j < 7; j++) {
                board[i][j] = 0;
            }
        }
        currentPlayer = 1;
        gameOver = false;
        winner = 0;
    }

    /** @brief מבצע מהלך בעמודה נתונה ובודק ניצחון */
    bool makeMove(int col, int player) {
        if (col < 0 || col >= 7 || gameOver) return false;

        for (int row = 5; row >= 0; row--) {
            if (board[row][col] == 0) {
                board[row][col] = player;
                checkWin(row, col, player);
                return true;
            }
        }
        return false; // Column full
    }

    /** @brief בודק אם מהלך בעמודה נתונה חוקי */
    bool isValidMove(int col) const {
        return col >= 0 && col < 7 && !gameOver && board[0][col] == 0;
    }

    /** @brief מחזיר האם המשחק הסתיים */
    bool isGameOver() const { return gameOver; }

    /** @brief מחזיר את המנצח */
    int getWinner() const { return winner; }

    /** @brief מחזיר את לוח המשחק */
    const int (*getBoard() const)[7] { return board; }

private:
    /** @brief בודק ניצחון לאחר מהלך - אופקי, אנכי ואלכסוני */
    void checkWin(int row, int col, int player) {
        // Check horizontal
        int count = 1;
        for (int i = col - 1; i >= 0 && board[row][i] == player; i--) count++;
        for (int i = col + 1; i < 7 && board[row][i] == player; i++) count++;
        if (count >= 4) { gameOver = true; winner = player; return; }

        // Check vertical
        count = 1;
        for (int i = row + 1; i < 6 && board[i][col] == player; i++) count++;
        if (count >= 4) { gameOver = true; winner = player; return; }

        // Check diagonal
        count = 1;
        for (int i = 1; row - i >= 0 && col - i >= 0 && board[row - i][col - i] == player; i++) count++;
        for (int i = 1; row + i < 6 && col + i < 7 && board[row + i][col + i] == player; i++) count++;
        if (count >= 4) { gameOver = true; winner = player; return; }

        count = 1;
        for (int i = 1; row - i >= 0 && col + i < 7 && board[row - i][col + i] == player; i++) count++;
        for (int i = 1; row + i < 6 && col - i >= 0 && board[row + i][col - i] == player; i++) count++;
        if (count >= 4) { gameOver = true; winner = player; return; }

        // Check for draw
        bool boardFull = true;
        for (int j = 0; j < 7; j++) {
            if (board[0][j] == 0) {
                boardFull = false;
                break;
            }
        }
        if (boardFull) {
            gameOver = true;
            winner = 0; // Draw
        }
    }
};

/**
 * @brief בנאי - יוצר חלון משחק אוטומטי עם שני שחקנים ואלגוריתמים
 */
AutoGameWindow::AutoGameWindow(const QString& player1Name, const QString& player1Algorithm,
                               const QString& player2Name, const QString& player2Algorithm,
                               QWidget *parent)
    : QMainWindow(parent)
    , m_player1Name(player1Name)
    , m_player1Algorithm(player1Algorithm)
    , m_player2Name(player2Name)
    , m_player2Algorithm(player2Algorithm)
    , m_currentPlayer(1)
    , m_moveCount(0)
    , m_gameSpeed(1000)
    , m_gameRunning(false)
    , m_gamePaused(false)
    , m_autoMoveEnabled(true)
    , m_player1Score(0)
    , m_player2Score(0)
    , m_draws(0)
    , m_player1Color(Qt::red)
    , m_player2Color(Qt::yellow)
    , m_emptyColor(Qt::white)
    , m_highlightColor(Qt::green)
{
    setWindowTitle("משחק אוטומטי - Connect Four");
    setMinimumSize(1000, 700);

    m_gameBoard = new SimpleGameBoard();
    m_gameTimer = new QTimer(this);
    connect(m_gameTimer, &QTimer::timeout, this, &AutoGameWindow::nextMove);

    setupUI();
    updatePlayerInfo();
    updateGameBoard();
    updateGameStatus();
}

/**
 * @brief מחסל - מנקה את לוח המשחק
 */
AutoGameWindow::~AutoGameWindow()
{
    delete m_gameBoard;
}

/**
 * @brief בונה את כל ממשק המשתמש - תצוגת משחק, בקרה ומידע שחקנים
 */
void AutoGameWindow::setupUI()
{
    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);
    m_mainLayout = new QHBoxLayout(m_centralWidget);

    setupGameDisplay();
    setupControlPanel();
    setupPlayersInfo();
}

/**
 * @brief בונה תצוגת לוח המשחק עם סטטוס ופס התקדמות
 */
void AutoGameWindow::setupGameDisplay()
{
    m_gameFrame = new QFrame();
    m_gameFrame->setFrameStyle(QFrame::Box);
    m_gameFrame->setLineWidth(2);
    m_gameFrame->setMinimumSize(500, 400);

    QVBoxLayout* gameLayout = new QVBoxLayout(m_gameFrame);

    // Game status display
    m_gameStatusLabel = new QLabel("מוכן להתחיל משחק");
    m_gameStatusLabel->setAlignment(Qt::AlignCenter);
    m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: blue;");
    gameLayout->addWidget(m_gameStatusLabel);

    // Game board
    QWidget* boardWidget = new QWidget();
    m_boardLayout = new QGridLayout(boardWidget);
    m_boardLayout->setSpacing(2);

    // Initialize board labels
    for (int row = 0; row < 6; row++) {
        for (int col = 0; col < 7; col++) {
            m_boardLabels[row][col] = new QLabel();
            m_boardLabels[row][col]->setFixedSize(50, 50);
            m_boardLabels[row][col]->setAlignment(Qt::AlignCenter);
            m_boardLabels[row][col]->setStyleSheet("border: 1px solid black; background-color: white;");
            m_boardLabels[row][col]->setText("");
            m_boardLayout->addWidget(m_boardLabels[row][col], row, col);
        }
    }

    gameLayout->addWidget(boardWidget);

    // Current player and move count
    QHBoxLayout* statusLayout = new QHBoxLayout();
    m_currentPlayerLabel = new QLabel("תור שחקן: " + m_player1Name);
    m_currentPlayerLabel->setStyleSheet("font-size: 14px; font-weight: bold;");
    m_moveCountLabel = new QLabel("מספר מהלכים: 0");
    m_moveCountLabel->setStyleSheet("font-size: 14px;");

    statusLayout->addWidget(m_currentPlayerLabel);
    statusLayout->addStretch();
    statusLayout->addWidget(m_moveCountLabel);
    gameLayout->addLayout(statusLayout);

    // Progress bar
    m_gameProgressBar = new QProgressBar();
    m_gameProgressBar->setRange(0, 42); // Maximum moves in Connect Four
    m_gameProgressBar->setValue(0);
    gameLayout->addWidget(m_gameProgressBar);

    m_mainLayout->addWidget(m_gameFrame, 2);
}

/**
 * @brief בונה פאנל בקרה עם כפתורים, מהירות והיסטוריית מהלכים
 */
void AutoGameWindow::setupControlPanel()
{
    m_controlFrame = new QFrame();
    m_controlFrame->setFrameStyle(QFrame::Box);
    m_controlFrame->setLineWidth(1);
    m_controlFrame->setMaximumWidth(300);

    m_controlLayout = new QVBoxLayout(m_controlFrame);

    // Control buttons
    QGroupBox* controlGroup = new QGroupBox("בקרת משחק");
    QVBoxLayout* controlButtonsLayout = new QVBoxLayout(controlGroup);

    m_startButton = new QPushButton("התחל משחק");
    m_startButton->setStyleSheet("QPushButton { background-color: green; color: white; font-weight: bold; }");
    connect(m_startButton, &QPushButton::clicked, this, &AutoGameWindow::startGame);

    m_pauseButton = new QPushButton("השהה");
    m_pauseButton->setEnabled(false);
    m_pauseButton->setStyleSheet("QPushButton { background-color: orange; color: white; font-weight: bold; }");
    connect(m_pauseButton, &QPushButton::clicked, this, &AutoGameWindow::pauseGame);

    m_resetButton = new QPushButton("איפוס משחק");
    m_resetButton->setStyleSheet("QPushButton { background-color: red; color: white; font-weight: bold; }");
    connect(m_resetButton, &QPushButton::clicked, this, &AutoGameWindow::resetGame);

    m_nextMoveButton = new QPushButton("מהלך הבא");
    m_nextMoveButton->setEnabled(false);
    connect(m_nextMoveButton, &QPushButton::clicked, this, &AutoGameWindow::nextMove);

    controlButtonsLayout->addWidget(m_startButton);
    controlButtonsLayout->addWidget(m_pauseButton);
    controlButtonsLayout->addWidget(m_resetButton);
    controlButtonsLayout->addWidget(m_nextMoveButton);

    // Auto move checkbox
    m_autoMoveCheckBox = new QCheckBox("מהלכים אוטומטיים");
    m_autoMoveCheckBox->setChecked(true);
    connect(m_autoMoveCheckBox, &QCheckBox::toggled, this, &AutoGameWindow::onAutoMoveToggled);
    controlButtonsLayout->addWidget(m_autoMoveCheckBox);

    // Speed control
    QHBoxLayout* speedLayout = new QHBoxLayout();
    m_speedLabel = new QLabel("מהירות (מילישניות):");
    m_speedSpinBox = new QSpinBox();
    m_speedSpinBox->setRange(100, 5000);
    m_speedSpinBox->setValue(1000);
    m_speedSpinBox->setSuffix(" ms");
    connect(m_speedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &AutoGameWindow::onSpeedChanged);

    speedLayout->addWidget(m_speedLabel);
    speedLayout->addWidget(m_speedSpinBox);
    controlButtonsLayout->addLayout(speedLayout);

    m_controlLayout->addWidget(controlGroup);

    // Move history
    QGroupBox* historyGroup = new QGroupBox("היסטוריית מהלכים");
    QVBoxLayout* historyLayout = new QVBoxLayout(historyGroup);

    m_moveHistoryTextEdit = new QTextEdit();
    m_moveHistoryTextEdit->setMaximumHeight(200);
    m_moveHistoryTextEdit->setReadOnly(true);
    historyLayout->addWidget(m_moveHistoryTextEdit);

    m_controlLayout->addWidget(historyGroup);
    m_controlLayout->addStretch();
    m_mainLayout->addWidget(m_controlFrame, 1);
}

/**
 * @brief בונה אזור מידע השחקנים עם שמות, אלגוריתמים וניקוד
 */
void AutoGameWindow::setupPlayersInfo()
{
    QVBoxLayout* playersLayout = new QVBoxLayout();

    // Player 1 info
    m_player1Group = new QGroupBox("שחקן 1");
    m_player1Group->setStyleSheet("QGroupBox { color: red; font-weight: bold; }");
    QVBoxLayout* player1Layout = new QVBoxLayout(m_player1Group);

    m_player1NameLabel = new QLabel("שם: " + m_player1Name);
    m_player1AlgorithmLabel = new QLabel("אלגוריתם: " + m_player1Algorithm);
    m_player1ScoreLabel = new QLabel("ניצחונות: 0");

    player1Layout->addWidget(m_player1NameLabel);
    player1Layout->addWidget(m_player1AlgorithmLabel);
    player1Layout->addWidget(m_player1ScoreLabel);

    // Player 2 info
    m_player2Group = new QGroupBox("שחקן 2");
    m_player2Group->setStyleSheet("QGroupBox { color: orange; font-weight: bold; }");
    QVBoxLayout* player2Layout = new QVBoxLayout(m_player2Group);

    m_player2NameLabel = new QLabel("שם: " + m_player2Name);
    m_player2AlgorithmLabel = new QLabel("אלגוריתם: " + m_player2Algorithm);
    m_player2ScoreLabel = new QLabel("ניצחונות: 0");

    player2Layout->addWidget(m_player2NameLabel);
    player2Layout->addWidget(m_player2AlgorithmLabel);
    player2Layout->addWidget(m_player2ScoreLabel);

    // Game statistics
    QGroupBox* statsGroup = new QGroupBox("סטטיסטיקות");
    QVBoxLayout* statsLayout = new QVBoxLayout(statsGroup);

    QLabel* drawsLabel = new QLabel("תיקו: 0");
    statsLayout->addWidget(drawsLabel);

    playersLayout->addWidget(m_player1Group);
    playersLayout->addWidget(m_player2Group);
    playersLayout->addWidget(statsGroup);
    playersLayout->addStretch();

    QWidget* playersWidget = new QWidget();
    playersWidget->setLayout(playersLayout);
    playersWidget->setMaximumWidth(250);

    m_mainLayout->addWidget(playersWidget, 1);
}

/**
 * @brief מתחיל משחק חדש ומפעיל טיימר אוטומטי אם נדרש
 */
void AutoGameWindow::startGame()
{
    if (!m_gameRunning) {
        m_gameRunning = true;
        m_gamePaused = false;
        m_startButton->setText("משחק פועל...");
        m_startButton->setEnabled(false);
        m_pauseButton->setEnabled(true);
        m_nextMoveButton->setEnabled(!m_autoMoveEnabled);

        if (m_autoMoveEnabled) {
            m_gameTimer->start(m_gameSpeed);
        }

        updateGameStatus();
        m_moveHistoryTextEdit->append("--- משחק חדש התחיל ---");
    }
}

/**
 * @brief משהה או ממשיך את המשחק בהתאם למצב הנוכחי
 */
void AutoGameWindow::pauseGame()
{
    if (m_gameRunning) {
        if (m_gamePaused) {
            // Resume game
            m_gamePaused = false;
            m_pauseButton->setText("השהה");
            m_nextMoveButton->setEnabled(!m_autoMoveEnabled);

            if (m_autoMoveEnabled) {
                m_gameTimer->start(m_gameSpeed);
            }
        } else {
            // Pause game
            m_gamePaused = true;
            m_pauseButton->setText("המשך");
            m_nextMoveButton->setEnabled(true);
            m_gameTimer->stop();
        }

        updateGameStatus();
    }
}

/**
 * @brief מאפס את המשחק למצב התחלתי
 */
void AutoGameWindow::resetGame()
{
    m_gameRunning = false;
    m_gamePaused = false;
    m_gameTimer->stop();

    m_gameBoard->resetBoard();
    m_currentPlayer = 1;
    m_moveCount = 0;
    m_moveHistory.clear();

    m_startButton->setText("התחל משחק");
    m_startButton->setEnabled(true);
    m_pauseButton->setText("השהה");
    m_pauseButton->setEnabled(false);
    m_nextMoveButton->setEnabled(false);

    updateGameBoard();
    updateGameStatus();
    m_moveHistoryTextEdit->clear();
}

/**
 * @brief מבצע מהלך הבא אם המשחק פועל
 */
void AutoGameWindow::nextMove()
{
    if (m_gameRunning && !m_gamePaused && !m_gameBoard->isGameOver()) {
        makeAutoMove();
    }
}

/**
 * @brief מבצע מהלך אוטומטי לפי האלגוריתם של השחקן הנוכחי
 */
void AutoGameWindow::makeAutoMove()
{
    QString currentAlgorithm = (m_currentPlayer == 1) ? m_player1Algorithm : m_player2Algorithm;
    QString currentPlayerName = (m_currentPlayer == 1) ? m_player1Name : m_player2Name;

    int move = getComputerMove(currentAlgorithm, m_currentPlayer);

    if (move != -1) {
        executeMove(move);

        // Add to move history
        QString moveText = QString("מהלך %1: %2 (%3) בחר עמודה %4")
                          .arg(m_moveCount)
                          .arg(currentPlayerName)
                          .arg(currentAlgorithm)
                          .arg(move + 1);
        m_moveHistory.append(moveText);
        updateMoveHistory();

        // Check if game is over
        checkGameEnd();
    }
}

/**
 * @brief מבצע מהלך נתון ומעדכן תצוגה
 */
void AutoGameWindow::executeMove(int move)
{
    if (m_gameBoard->makeMove(move, m_currentPlayer)) {
        m_moveCount++;
        m_currentPlayer = (m_currentPlayer == 1) ? 2 : 1;

        updateGameBoard();
        updateGameStatus();
        m_gameProgressBar->setValue(m_moveCount);
    }
}

/**
 * @brief בודק אם המשחק הסתיים ומעדכן ניקוד
 */
void AutoGameWindow::checkGameEnd()
{
    if (m_gameBoard->isGameOver()) {
        int winner = m_gameBoard->getWinner();
        onGameFinished();

        if (winner != 0) {
            if (winner == 1) {
                m_player1Score++;
            } else {
                m_player2Score++;
            }
        } else {
            m_draws++;
        }

        updatePlayerInfo();
        showGameResults();
    }
}

/**
 * @brief מטפל בסיום משחק - עוצר טיימר ומעדכן כפתורים
 */
void AutoGameWindow::onGameFinished()
{
    m_gameRunning = false;
    m_gameTimer->stop();

    m_startButton->setText("התחל משחק");
    m_startButton->setEnabled(true);
    m_pauseButton->setEnabled(false);
    m_nextMoveButton->setEnabled(false);

    updateGameStatus();
}

/**
 * @brief מציג תוצאות המשחק בדיאלוג
 */
void AutoGameWindow::showGameResults()
{
    int winner = m_gameBoard->getWinner();
    QString result;

    if (winner == 1) {
        result = QString("שחקן 1 (%1) ניצח!").arg(m_player1Name);
    } else if (winner == 2) {
        result = QString("שחקן 2 (%1) ניצח!").arg(m_player2Name);
    } else {
        result = "המשחק הסתיים בתיקו!";
    }

    m_moveHistoryTextEdit->append("--- " + result + " ---");
    QMessageBox::information(this, "תוצאות המשחק", result);
}

/**
 * @brief מעדכן תוויות ניקוד השחקנים
 */
void AutoGameWindow::updatePlayerInfo()
{
    m_player1ScoreLabel->setText(QString("ניצחונות: %1").arg(m_player1Score));
    m_player2ScoreLabel->setText(QString("ניצחונות: %1").arg(m_player2Score));
}

/**
 * @brief מעדכן תצוגת לוח המשחק עם צבעים לפי השחקנים
 */
void AutoGameWindow::updateGameBoard()
{
    const int rows = 6;
    const int cols = 7;

    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            int piece = m_gameBoard->getBoard()[row][col];

            if (piece == 1) {
                m_boardLabels[row][col]->setStyleSheet("border: 1px solid black; background-color: red; border-radius: 20px;");
                m_boardLabels[row][col]->setText("●");
            } else if (piece == 2) {
                m_boardLabels[row][col]->setStyleSheet("border: 1px solid black; background-color: yellow; border-radius: 20px;");
                m_boardLabels[row][col]->setText("●");
            } else {
                m_boardLabels[row][col]->setStyleSheet("border: 1px solid black; background-color: white; border-radius: 20px;");
                m_boardLabels[row][col]->setText("");
            }
        }
    }
}

/**
 * @brief מעדכן תווית סטטוס המשחק לפי המצב הנוכחי
 */
void AutoGameWindow::updateGameStatus()
{
    if (!m_gameRunning) {
        m_gameStatusLabel->setText("מוכן להתחיל משחק");
        m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: blue;");
    } else if (m_gamePaused) {
        m_gameStatusLabel->setText("המשחק מושהה");
        m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: orange;");
    } else if (m_gameBoard->isGameOver()) {
        int winner = m_gameBoard->getWinner();
        if (winner == 1) {
            m_gameStatusLabel->setText("שחקן 1 ניצח!");
            m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: red;");
        } else if (winner == 2) {
            m_gameStatusLabel->setText("שחקן 2 ניצח!");
            m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: orange;");
        } else {
            m_gameStatusLabel->setText("תיקו!");
            m_gameStatusLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: gray;");
        }
    } else {
        QString currentPlayerName = (m_currentPlayer == 1) ? m_player1Name : m_player2Name;
        m_gameStatusLabel->setText("תור של: " + currentPlayerName);
        QString color = (m_currentPlayer == 1) ? "red" : "orange";
        m_gameStatusLabel->setStyleSheet(QString("font-size: 16px; font-weight: bold; color: %1;").arg(color));
    }

    QString currentPlayerName = (m_currentPlayer == 1) ? m_player1Name : m_player2Name;
    m_currentPlayerLabel->setText("תור שחקן: " + currentPlayerName);
    m_moveCountLabel->setText(QString("מספר מהלכים: %1").arg(m_moveCount));
}

/**
 * @brief מעדכן שדה היסטוריית המהלכים ומגלל לסוף
 */
void AutoGameWindow::updateMoveHistory()
{
    m_moveHistoryTextEdit->clear();
    for (const QString& move : m_moveHistory) {
        m_moveHistoryTextEdit->append(move);
    }

    // Scroll to bottom
    QTextCursor cursor = m_moveHistoryTextEdit->textCursor();
    cursor.movePosition(QTextCursor::End);
    m_moveHistoryTextEdit->setTextCursor(cursor);
}

/**
 * @brief מעדכן מהירות המשחק ומחדש את הטיימר אם פועל
 */
void AutoGameWindow::onSpeedChanged(int speed)
{
    m_gameSpeed = speed;
    if (m_gameTimer->isActive()) {
        m_gameTimer->setInterval(m_gameSpeed);
    }
}

/**
 * @brief מעדכן מצב מהלכים אוטומטיים ומתאים את הטיימר והכפתורים
 */
void AutoGameWindow::onAutoMoveToggled(bool enabled)
{
    m_autoMoveEnabled = enabled;

    if (m_gameRunning && !m_gamePaused) {
        if (enabled) {
            m_gameTimer->start(m_gameSpeed);
            m_nextMoveButton->setEnabled(false);
        } else {
            m_gameTimer->stop();
            m_nextMoveButton->setEnabled(true);
        }
    }
}

// AI Algorithms Implementation

/**
 * @brief מחזיר מהלך מחושב לפי האלגוריתם הנבחר
 */
int AutoGameWindow::getComputerMove(const QString& algorithm, int player)
{
    if (algorithm == "אקראי") {
        return getRandomMove();
    } else if (algorithm == "מיני-מקס") {
        return getMinimaxMove(player);
    } else if (algorithm == "אלפא-בטא") {
        return getAlphaBetaMove(player);
    } else if (algorithm == "הכי טוב") {
        return getBestMove(player);
    }

    return getRandomMove(); // Default fallback
}

/**
 * @brief מחזיר מהלך אקראי מהעמודות הזמינות
 */
int AutoGameWindow::getRandomMove()
{
    QList<int> validMoves;

    for (int col = 0; col < 7; col++) {
        if (m_gameBoard->isValidMove(col)) {
            validMoves.append(col);
        }
    }

    if (validMoves.isEmpty()) {
        return -1;
    }

    int randomIndex = rand() % validMoves.size();
    return validMoves[randomIndex];
}

/**
 * @brief מחזיר מהלך לפי אלגוריתם minimax פשוט
 */
int AutoGameWindow::getMinimaxMove(int player, int depth)
{
    // Simple minimax implementation
    int bestMove = -1;
    int bestScore = (player == 1) ? INT_MIN : INT_MAX;

    for (int col = 0; col < 7; col++) {
        if (m_gameBoard->isValidMove(col)) {
            // Make temporary move
            SimpleGameBoard* tempBoard = new SimpleGameBoard(*m_gameBoard);
            tempBoard->makeMove(col, player);

            // Calculate score for this move
            int score = 0;
            if (tempBoard->isGameOver()) {
                int winner = tempBoard->getWinner();
                if (winner == player) {
                    score = 1000;
                } else if (winner != 0) {
                    score = -1000;
                } else {
                    score = 0;
                }
            } else {
                // Simple evaluation: count potential wins
                score = evaluateBoard(tempBoard, player);
            }

            if (player == 1 && score > bestScore) {
                bestScore = score;
                bestMove = col;
            } else if (player == 2 && score < bestScore) {
                bestScore = score;
                bestMove = col;
            }

            delete tempBoard;
        }
    }

    return bestMove != -1 ? bestMove : getRandomMove();
}

/**
 * @brief מחזיר מהלך לפי אלגוריתם alpha-beta (כרגע זהה ל-minimax)
 */
int AutoGameWindow::getAlphaBetaMove(int player, int depth)
{
    // For now, use minimax (can be enhanced later)
    return getMinimaxMove(player, depth);
}

/**
 * @brief מחזיר מהלך הטוב ביותר עם אסטרטגיה מתקדמת - ניצחון, חסימה, מרכז
 */
int AutoGameWindow::getBestMove(int player)
{
    // Enhanced strategy: prioritize center, check for wins/blocks

    // First, check if we can win
    for (int col = 0; col < 7; col++) {
        if (m_gameBoard->isValidMove(col)) {
            SimpleGameBoard* tempBoard = new SimpleGameBoard(*m_gameBoard);
            tempBoard->makeMove(col, player);
            if (tempBoard->getWinner() == player) {
                delete tempBoard;
                return col;
            }
            delete tempBoard;
        }
    }

    // Second, check if we need to block opponent
    int opponent = (player == 1) ? 2 : 1;
    for (int col = 0; col < 7; col++) {
        if (m_gameBoard->isValidMove(col)) {
            SimpleGameBoard* tempBoard = new SimpleGameBoard(*m_gameBoard);
            tempBoard->makeMove(col, opponent);
            if (tempBoard->getWinner() == opponent) {
                delete tempBoard;
                return col;
            }
            delete tempBoard;
        }
    }

    // Third, prefer center columns
    QList<int> centerColumns = {3, 2, 4, 1, 5, 0, 6};
    for (int col : centerColumns) {
        if (m_gameBoard->isValidMove(col)) {
            return col;
        }
    }

    return getRandomMove();
}

/**
 * @brief מעריך את איכות מצב הלוח עבור שחקן - מרכז ופוטנציאל ניצחון
 */
int AutoGameWindow::evaluateBoard(SimpleGameBoard* board, int player)
{
    // Simple board evaluation
    int score = 0;
    const int rows = 6;
    const int cols = 7;
    
    // Count pieces in center column (preferred)
    for (int row = 0; row < rows; row++) {
        if (board->getBoard()[row][3] == player) {
            score += 3;
        }
    }
    
    // Evaluate horizontal, vertical, and diagonal potential
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (board->getBoard()[row][col] == player) {
                // Check horizontal potential
                if (col <= cols - 4) {
                    int count = 0;
                    for (int i = 0; i < 4; i++) {
                        if (board->getBoard()[row][col + i] == player) count++;
                        else if (board->getBoard()[row][col + i] != 0) count = -1;
                    }
                    if (count > 0) score += count * count;
                }
                
                // Check vertical potential
                if (row <= rows - 4) {
                    int count = 0;
                    for (int i = 0; i < 4; i++) {
                        if (board->getBoard()[row + i][col] == player) count++;
                        else if (board->getBoard()[row + i][col] != 0) count = -1;
                    }
                    if (count > 0) score += count * count;
                }
                
                // Check diagonal potential (both directions)
                if (row <= rows - 4 && col <= cols - 4) {
                    int count = 0;
                    for (int i = 0; i < 4; i++) {
                        if (board->getBoard()[row + i][col + i] == player) count++;
                        else if (board->getBoard()[row + i][col + i] != 0) count = -1;
                    }
                    if (count > 0) score += count * count;
                }
                
                if (row >= 3 && col <= cols - 4) {
                    int count = 0;
                    for (int i = 0; i < 4; i++) {
                        if (board->getBoard()[row - i][col + i] == player) count++;
                        else if (board->getBoard()[row - i][col + i] != 0) count = -1;
                    }
                    if (count > 0) score += count * count;
                }
            }
        }
    }
    
    return score;
}