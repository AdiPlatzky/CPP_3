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

/*!
 * @class AutoDemoWindow
 * @brief מנהלת הדגמה אוטומטית (AI) של משחק Coup כולל ממשק משתמש גרפי, יומן פעולות, תצוגת סטטוס לכל שלב ושליטה מלאה על מהלך המשחק.
 *        מאפשרת להריץ משחק בין שחקנים ממוחשבים (ללא מגע משתמש), עם עדכון תצוגה אוטומטי וצפייה מהצד.
 *
 * מאפיינים:
 * - כל שחקן הוא מצביע חכם (shared_ptr<Player>).
 * - AI מבצע החלטות רנדומליות/אסטרטגיות עבור כל שחקן בתורו.
 * - יומן מפורט של כל פעולה.
 * - שליטה על מהירות/השהיה/הפסקה/שלב בודד.
 * - תצוגה דינמית של סטטוס, תור, הדחות ושחקנים.
 */
class AutoDemoWindow : public QWidget {
    Q_OBJECT

public:
    /*!
  * @brief בנאי — בונה את חלון המשחק האוטומטי, יוצר את כל רכיבי הממשק ומאתחל את המשחק.
  * @param players רשימת שחקנים (shared_ptr) שמועברים למשחק.
  * @param showDetailedActions האם להציג את הפעולות בפירוט (מגדיר את אופן הדיווח ביומן).
  * @param parent מצביע ל־QWidget האב (ברירת מחדל nullptr).
  */
    explicit AutoDemoWindow(const std::vector<std::shared_ptr<Player>>& players,
                            bool showDetailedActions = true,
                            QWidget *parent = nullptr);

private slots:
    /*!
    * @brief התחלת דמו חדש או המשך משחק שהופסק. מאתחל מחדש את השחקנים והמשחק, מפעיל את הטיימר של ה־AI.
    */
    void startDemo();
    /*!
     * @brief השהיית המשחק — עוצר את לולאת הטיימר של ההדגמה.
     */
    void pauseDemo();
    /*!
     * @brief עוצר את המשחק לחלוטין ומחזיר למצב התחלתי.
     */
    void stopDemo();
    /*!
     * @brief מבצע שלב אחד של משחק (עבור debugging או משחק איטי מאוד).
     */
    void stepDemo();
    /*!
     * @brief קריאה פנימית: מפעילה תור של AI, מעדכנת תצוגה, ומבצעת בדיקת סיום.
     */
    void onDemoStep();
    /*!
     * @brief משנה את מהירות הדמו (באמצעות הסליידר).
     * @param value ערך בין 1 ל־10, קובע את מהירות ההרצה.
     */
    void adjustSpeed(int value);
    /*!
     * @brief מטפל בסיום משחק — מציג חלונית סיום, מציג מנצח, מאתחל את החלון.
     * @param winnerName שם המנצח במשחק.
     */
    void onGameEnd(const QString& winnerName);
    /*!
     * @brief מטפל בהדחת שחקן מהמשחק (עדכון יומן ותצוגה).
     * @param playerName שם השחקן שהודח.
     * @param reason סיבה להדחה.
     */
    void onPlayerEliminated(const QString& playerName, const QString& reason);

private:
    // ---------------------  UI Components ---------------------

    QVBoxLayout *mainLayout;
    QHBoxLayout *playerLayout;
    QGridLayout *controlLayout;

    // תצוגת שחקנים
    QMap<QString, QLabel*> playerLabelMap;
    QLabel *currentPlayerHighlight = nullptr;

    // סטטוס המשחק
    QLabel *turnLabel;
    QLabel *statusLabel;
    QLabel *actionResultLabel;
    QLabel *speedLabel;

    // כפתורים לשליטה
    QPushButton *startButton;
    QPushButton *pauseButton;
    QPushButton *stopButton;
    QPushButton *stepButton;
    QPushButton *homeButton;
    QSlider *speedSlider;

    // יומן פעולות
    QTextEdit *actionLog;

    // בית קברות (שחקנים מודחים)
    QDockWidget *graveyardDock;
    QListWidget *graveyardList;

    // -------------------- Game Components ---------------------

    std::unique_ptr<Game> game;
    std::vector<std::shared_ptr<Player>> players;
    std::mt19937 rng;
    QTimer *demoTimer;

    // --------------------- Demo State -------------------------

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

    /*!
    * @brief בונה את כל ממשק המשתמש (labels, לחצנים, סליידר, יומן, תצוגות).
    * הערה: מותר להקצות mainLayout רק פעם אחת בקונסטרקטור! אין להקצות מחדש בפונקציה זו.
    */
    void setupUI();
    /*!
     * @brief אתחול לוגיקת המשחק — יצירת מופע Game, חיבור סיגנלים, הוספת שחקנים.
     */
    void setupGame();
    /*!
     * @brief בונה תצוגת קלפי השחקנים (labels + מיפוי לפי שם).
     */
    void setupPlayerDisplay();
    /*!
     * @brief בונה את קופסת הבקרה למשחק (play/pause/stop/step + סליידר מהירות).
     */
    void setupControls();
    /*!
     * @brief בונה את אזור יומן הפעולות — תצוגה מתגלגלת של כל אירוע במשחק.
     */
    void setupActionLog();
    /*!
     * @brief בונה את תצוגת "בית הקברות" — רשימת שחקנים שהודחו (בינתיים יומן בלבד).
     */
    void setupGraveyard();
    /*!
     * @brief מעדכן תצוגת השחקנים (כולל צבע, סטטוס, מטבעות).
     */
    void updatePlayerDisplay();
    /*!
     * @brief מדגיש גרפית את השחקן הנוכחי (זה שבתורו).
     */
    void highlightCurrentPlayer();
    /*!
     * @brief מעדכן את הלייבלים של סטטוס המשחק והתור הנוכחי.
     */
    void updateGameStatus();
    /*!
     * @brief מוסיף שורה ליומן הפעולות, עם צבע והדגשה.
     * @param message טקסט האירוע.
     * @param color צבע הקו (HEX).
     */
    void logAction(const QString &message, const QString &color = "#ecf0f1");
    /*!
     * @brief מבצע מהלך של AI לשחקן הנוכחי (בחירת פעולה, ביצוע, עדכון תצוגה).
     */
    void performAIAction();
    /*!
     * @brief בוחר רנדומלית פעולה מותרת לשחקן (בהתאם לכללים).
     * @param player רפרנס לשחקן.
     * @return שם הפעולה.
     */
    QString getRandomAction(const Player& player);
    /*!
     * @brief בוחר רנדומלית שחקן יעד מתאים (לפעולות שמצריכות יעד).
     * @param attacker תוקף/שחקן פועל.
     * @return מצביע חכם ל־Player היעד.
     */
    std::shared_ptr<Player> getRandomTarget(const Player& attacker);
    /*!
     * @brief מבצע פעולה מסוימת לשחקן (כולל הדמיית חסימות, תוצאות).
     * @param action שם הפעולה.
     * @param player רפרנס לשחקן.
     */
    void simulatePlayerDecision(const QString& action, Player& player);
    /*!
     * @brief מחזיר צבע HEX לפי שם תפקיד (להדגשה גרפית של דמות).
     * @param roleName שם התפקיד.
     */
    QString getRoleColor(const QString &roleName);
    /*!
     * @brief מחזיר מחרוזת תיאור של שחקן (לצורך דוחות/יומן).
     * @param player רפרנס לשחקן.
     */
    QString getPlayerInfo(const Player &player);
    /*!
     * @brief מוסיף שחקן לרשימת המודחים (בינתיים רק ליומן, אפשר להרחיב).
     * @param name שם השחקן.
     * @param reason סיבה.
     */
    void addPlayerToGraveyard(const QString &name, const QString &reason);
    /*!
     * @brief פותח דיאלוג סיום משחק — מציג ניצחון ומציע להפעיל משחק חדש.
     * @param winner שם המנצח.
     */
    void showGameOverDialog(const QString& winner);
};

#endif // AUTODEMOWINDOW_H
