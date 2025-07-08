// 12adi45@gmail.com

#ifndef AUTOGAMEWINDOW_H
#define AUTOGAMEWINDOW_H

#include "MainWindow.h"
#include "../Player.h"
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
#include <vector>

// Forward declaration
class SimpleGameBoard;

/**
 * @class AutoGameWindow
 * @brief חלון משחק אוטומטי Connect Four עם AI - מאפשר צפייה במשחק בין שני אלגוריתמים
 * @author 12adi45@gmail.com
 */
class AutoGameWindow : public QMainWindow
{
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר חלון משחק אוטומטי עם שני שחקנים ואלגוריתמים
     * @param player1Name שם שחקן 1
     * @param player1Algorithm אלגוריתם שחקן 1
     * @param player2Name שם שחקן 2
     * @param player2Algorithm אלגוריתם שחקן 2
     * @param parent חלון אב
     */
    explicit AutoGameWindow(const QString& player1Name, const QString& player1Algorithm,
                           const QString& player2Name, const QString& player2Algorithm,
                           QWidget *parent = nullptr);

    /**
     * @brief בנאי אלטרנטיבי - יוצר משחק אוטומטי מרשימת שחקנים
     * @param vector רשימת שחקנים
     * @param show_actions האם להציג פעולות
     * @param main_window חלון ראשי
     */
    AutoGameWindow(const std::vector<std::shared_ptr<Player>> & vector, bool show_actions, MainWindow * main_window);

    /** @brief מחסל - מנקה את לוח המשחק */
    ~AutoGameWindow();

private slots:
    /** @brief מתחיל משחק חדש */
    void startGame();

    /** @brief משהה/ממשיך את המשחק */
    void pauseGame();

    /** @brief מאפס את המשחק למצב התחלתי */
    void resetGame();

    /** @brief מבצע מהלך הבא */
    void nextMove();

    /** @brief מטפל בסיום משחק */
    void onGameFinished();

    /** @brief מעדכן מהירות המשחק */
    void onSpeedChanged(int speed);

    /** @brief מעדכן מצב מהלכים אוטומטיים */
    void onAutoMoveToggled(bool enabled);

private:
    /** @brief בונה את ממשק המשתמש */
    void setupUI();

    /** @brief בונה פאנל הבקרה */
    void setupControlPanel();

    /** @brief בונה תצוגת לוח המשחק */
    void setupGameDisplay();

    /** @brief בונה מידע השחקנים */
    void setupPlayersInfo();

    /** @brief מעדכן מידע השחקנים וניקוד */
    void updatePlayerInfo();

    /** @brief מעדכן תצוגת לוח המשחק */
    void updateGameBoard();

    /** @brief מעדכן סטטוס המשחק */
    void updateGameStatus();

    /** @brief מעדכן היסטוריית מהלכים */
    void updateMoveHistory();

    /** @brief מבצע מהלך אוטומטי לפי האלגוריתם */
    void makeAutoMove();

    /** @brief מבצע מהלך נתון */
    void executeMove(int move);

    /** @brief בודק אם המשחק הסתיים */
    void checkGameEnd();

    /** @brief מציג תוצאות המשחק */
    void showGameResults();

    // Game logic
    /** @brief מחזיר מהלך מחושב לפי האלגוריתם */
    int getComputerMove(const QString& algorithm, int player);

    /** @brief מחזיר מהלך אקראי */
    int getRandomMove();

    /** @brief מחזיר מהלך לפי אלגוריתם minimax */
    int getMinimaxMove(int player, int depth = 4);

    /** @brief מחזיר מהלך לפי אלגוריתם alpha-beta */
    int getAlphaBetaMove(int player, int depth = 4);

    /** @brief מחזיר מהלך הטוב ביותר עם אסטרטגיה מתקדמת */
    int getBestMove(int player);

    /** @brief מעריך את איכות מצב הלוח עבור שחקן */
    int evaluateBoard(SimpleGameBoard* board, int player);

    // UI Components
    /** @brief הווידג'ט המרכזי */
    QWidget* m_centralWidget;

    /** @brief הפריסה הראשית האופקית */
    QHBoxLayout* m_mainLayout;

    // Game Board Display
    /** @brief מסגרת לוח המשחק */
    QFrame* m_gameFrame;

    /** @brief פריסה רשתית ללוח המשחק */
    QGridLayout* m_boardLayout;

    /** @brief מערך תוויות עבור לוח המשחק (6 שורות, 7 עמודות) */
    QLabel* m_boardLabels[6][7];

    // Control Panel
    /** @brief מסגרת פאנל הבקרה */
    QFrame* m_controlFrame;

    /** @brief פריסה אנכית לפאנל הבקרה */
    QVBoxLayout* m_controlLayout;

    /** @brief כפתור התחלת משחק */
    QPushButton* m_startButton;

    /** @brief כפתור השהיה */
    QPushButton* m_pauseButton;

    /** @brief כפתור איפוס משחק */
    QPushButton* m_resetButton;

    /** @brief כפתור מהלך הבא */
    QPushButton* m_nextMoveButton;

    /** @brief תיבת סימון למהלכים אוטומטיים */
    QCheckBox* m_autoMoveCheckBox;

    /** @brief בורר מהירות המשחק */
    QSpinBox* m_speedSpinBox;

    /** @brief תווית מהירות */
    QLabel* m_speedLabel;

    // Players Info
    /** @brief קבוצת מידע שחקן 1 */
    QGroupBox* m_player1Group;

    /** @brief קבוצת מידע שחקן 2 */
    QGroupBox* m_player2Group;

    /** @brief תווית שם שחקן 1 */
    QLabel* m_player1NameLabel;

    /** @brief תווית אלגוריתם שחקן 1 */
    QLabel* m_player1AlgorithmLabel;

    /** @brief תווית ניקוד שחקן 1 */
    QLabel* m_player1ScoreLabel;

    /** @brief תווית שם שחקן 2 */
    QLabel* m_player2NameLabel;

    /** @brief תווית אלגוריתם שחקן 2 */
    QLabel* m_player2AlgorithmLabel;

    /** @brief תווית ניקוד שחקן 2 */
    QLabel* m_player2ScoreLabel;

    // Game Status
    /** @brief תווית השחקן הנוכחי */
    QLabel* m_currentPlayerLabel;

    /** @brief תווית מספר מהלכים */
    QLabel* m_moveCountLabel;

    /** @brief תווית סטטוס המשחק */
    QLabel* m_gameStatusLabel;

    /** @brief פס התקדמות המשחק */
    QProgressBar* m_gameProgressBar;

    // Move History
    /** @brief שדה טקסט להיסטוריית מהלכים */
    QTextEdit* m_moveHistoryTextEdit;

    // Game State
    /** @brief אובייקט לוח המשחק */
    SimpleGameBoard* m_gameBoard;

    /** @brief טיימר למהלכים אוטומטיים */
    QTimer* m_gameTimer;

    /** @brief שם שחקן 1 */
    QString m_player1Name;

    /** @brief אלגוריתם שחקן 1 */
    QString m_player1Algorithm;

    /** @brief שם שחקן 2 */
    QString m_player2Name;

    /** @brief אלגוריתם שחקן 2 */
    QString m_player2Algorithm;

    /** @brief השחקן הנוכחי (1 או 2) */
    int m_currentPlayer;

    /** @brief מספר המהלכים הכולל */
    int m_moveCount;

    /** @brief מהירות המשחק במילישניות */
    int m_gameSpeed;

    /** @brief האם המשחק פועל */
    bool m_gameRunning;

    /** @brief האם המשחק מושהה */
    bool m_gamePaused;

    /** @brief האם מהלכים אוטומטיים מופעלים */
    bool m_autoMoveEnabled;

    /** @brief רשימת היסטוריית מהלכים */
    QStringList m_moveHistory;

    // Game Results
    /** @brief ניקוד שחקן 1 */
    int m_player1Score;

    /** @brief ניקוד שחקן 2 */
    int m_player2Score;

    /** @brief מספר תיקו */
    int m_draws;

    // Colors for pieces
    /** @brief צבע שחקן 1 */
    QColor m_player1Color;

    /** @brief צבע שחקן 2 */
    QColor m_player2Color;

    /** @brief צבע משבצת ריקה */
    QColor m_emptyColor;

    /** @brief צבע הדגשה */
    QColor m_highlightColor;
};

#endif // AUTOGAMEWINDOW_H