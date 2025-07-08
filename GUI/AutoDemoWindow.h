//12adi45@gmail.com
#ifndef AUTODEMOWINDOW_H
#define AUTODEMOWINDOW_H

#include <QGroupBox>
#include <QTextEdit>
#include <QPushButton>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QTimer>
#include <QLabel>
#include <QMap>
#include <QListWidget>
#include <QDockWidget>
#include <memory>
#include <random>
#include "../Game.h"
#include "../Player.h"

/**
 * @class AutoDemoWindow
 * @brief חלון הדגמה אוטומטית של משחק Coup עם שחקני AI וממשק ניהול מלא
 */
class AutoDemoWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר חלון דמו עם רשימת שחקנים וממשק בקרה
     * @param players רשימת השחקנים למשחק
     * @param showDetailedActions האם להציג פעולות מפורטות ביומן
     * @param parent ווידג'ט אב
     */
    explicit AutoDemoWindow(const std::vector<std::shared_ptr<Player>>& players,
                            bool showDetailedActions = true,
                            QWidget *parent = nullptr);

private slots:
    /** @brief מתחיל או ממשיך את הדמו האוטומטי */
    void startDemo();

    /** @brief משהה את הדמו הרץ */
    void pauseDemo();

    /** @brief עוצר את הדמו ומחזיר למצב התחלתי */
    void stopDemo();

    /** @brief מבצע צעד בודד במשחק (למצב דיבוג) */
    void stepDemo();

    /** @brief מבצע תור AI אחד ומעדכן את התצוגה */
    void onDemoStep();

    /** @brief משנה את מהירות הדמו לפי ערך הסליידר */
    void adjustSpeed(int value);

    /** @brief מטפל בסיום המשחק ומציג דיאלוג ניצחון */
    void onGameEnd(const QString& winnerName);

    /** @brief מטפל בהדחת שחקן ומוסיף אותו לבית הקברות */
    void onPlayerEliminated(const QString& playerName, const QString& reason);

private:
    // ---------------------  UI Components ---------------------

    QVBoxLayout *mainLayout;
    QHBoxLayout *playerLayout;
    QGridLayout *controlLayout;

    /** @brief מיפוי שמות שחקנים לתוויות התצוגה שלהם */
    QMap<QString, QLabel*> playerLabelMap;
    QLabel *currentPlayerHighlight = nullptr;

    /** @brief תווית המציגה את מצב המשחק הנוכחי */
    QLabel *turnLabel;
    QLabel *statusLabel;
    QLabel *actionResultLabel;
    QLabel *speedLabel;

    /** @brief כפתורי בקרת המשחק */
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *stepButton;
    QPushButton *homeButton;
    QSlider *speedSlider;

    /** @brief יומן פעולות המשחק */
    QTextEdit *actionLog;

    /** @brief רשימת שחקנים שהודחו */
    QDockWidget *graveyardDock;
    QListWidget *graveyardList;

    // -------------------- Game Components ---------------------

    std::unique_ptr<Game> game;
    std::vector<std::shared_ptr<Player>> players;
    std::mt19937 rng;
    QTimer *demoTimer;

    // --------------------- Demo State -------------------------

    /** @brief מצבי הדמו האפשריים */
    enum DemoState {
        STOPPED,   ///< מצב התחלתי – המשחק לא התחיל
        RUNNING,   ///< המשחק רץ
        PAUSED,    ///< מושהה
        FINISHED   ///< הסתיים
    };
    DemoState currentState;
    int currentStep;
    int maxTurns;
    bool showDetailedActions;

    // ---------------------- Helper Methods --------------------

    /** @brief בונה את כל רכיבי ממשק המשתמש */
    void setupUI();

    /** @brief מאתחל את אובייקט המשחק ומחבר סיגנלים */
    void setupGame();

    /** @brief יוצר תצוגת קלפי השחקנים */
    void setupPlayerDisplay();

    /** @brief יוצר את פאנל כפתורי הבקרה */
    void setupControls();

    /** @brief יוצר את תיבת יומן הפעולות */
    void setupActionLog();

    /** @brief יוצר את רשימת השחקנים המודחים */
    void setupGraveyard();

    /** @brief מעדכן את תצוגת כל השחקנים */
    void updatePlayerDisplay();

    /** @brief מדגיש את השחקן שבתורו */
    void highlightCurrentPlayer();

    /** @brief מעדכן את תוויות הסטטוס והתור */
    void updateGameStatus();

    /** @brief מוסיף הודעה ליומן עם צבע וחותמת זמן */
    void logAction(const QString &message, const QString &color = "#ecf0f1");

    /** @brief מבצע מהלך AI לשחקן הנוכחי */
    void performAIAction();

    /** @brief בוחר פעולה אקראית חוקית לשחקן */
    QString getRandomAction(const Player& player);

    /** @brief בוחר שחקן יעד אקראי מתאים */
    std::shared_ptr<Player> getRandomTarget(const Player& attacker);

    /** @brief מבצע פעולה נבחרת עם הדמיית חסימות */
    void simulatePlayerDecision(const QString& action, Player& player);

    /** @brief מחזיר צבע ייחודי לכל תפקיד */
    QString getRoleColor(const QString &roleName);

    /** @brief מחזיר מחרוזת מידע מלאה על שחקן */
    QString getPlayerInfo(const Player &player);

    /** @brief מוסיף שחקן לרשימת המודחים */
    void addPlayerToGraveyard(const QString &name, const QString &reason);

    /** @brief מציג דיאלוג סיום משחק עם אפשרות להתחיל מחדש */
    void showGameOverDialog(const QString& winner);
};

#endif // AUTODEMOWINDOW_H