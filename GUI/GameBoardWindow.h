// 12adi45@gmail.com

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

// הכרזות מוקדמות למחלקות Qt
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPropertyAnimation;

/**
 * @class GameBoardWindow
 * @brief החלון המרכזי של משחק COUP - מנהל את הממשק הגרפי והאינטראקציה עם השחקנים
 * @author 12adi45@gmail.com
 */
class GameBoardWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר חלון משחק חדש עם השחקנים שניתנו
     * @param players רשימת שחקנים למשחק
     * @param parent חלון אב
     */
    explicit GameBoardWindow(const std::vector<std::shared_ptr<Player>>& players, QWidget *parent = nullptr);

    /**
     * @brief מחזיר צבע מתאים לתפקיד נתון לעיצוב הממשק
     * @param roleName שם התפקיד
     * @return קוד צבע HEX
     */
    QString getRoleColor(const QString &roleName);

private:
    // ===== רכיבי תצוגת מידע =====

    /** @brief מציג את השחקן שבתורו הנוכחי */
    QLabel *turnLabel;

    /** @brief מציג מידע מטבעות - מלא לשחקן נוכחי, מוסתר לאחרים */
    QLabel *coinLabel;

    /** @brief מציג תוצאות הפעולה האחרונה והודעות למשתמש */
    QLabel *actionResultLabel;

    /** @brief מצביע לשחקן המודגש כעת (השחקן שבתורו) */
    QLabel *highlightPlayer = nullptr;

    /** @brief מציג מידע על חסימות פעולות ומעצרים מתוכננים */
    QLabel* lastBlockedLabel = nullptr;

    // ===== פריסות (Layouts) =====

    /** @brief הפריסה הראשית האנכית של כל החלון */
    QVBoxLayout *mainLayout;

    /** @brief פריסה אופקית לתצוגת השחקנים */
    QHBoxLayout *playerLayout;

    /** @brief פריסה רשתית לכפתורי הפעולות (2x7) */
    QGridLayout *actionLayout;

    /** @brief פריסה לקלפי פעולות (לשימוש עתידי) */
    QHBoxLayout *cardLayout;

    /** @brief פריסה למיקום מרכזי של השחקן המודגש */
    QVBoxLayout *highlightLayout;

    /** @brief פריסה להודעות חסימות (לשימוש עתידי) */
    QVBoxLayout *lastBlockedLayout;

    // ===== רכיבי תצוגה נוספים =====

    /** @brief רשימת תוויות לקלפי פעולות (לשימוש עתידי) */
    QVector<QLabel*> actionCards;

    /** @brief רשימת שמות השחקנים (לשימוש עתידי) */
    QVector<QString> playerNames;

    /** @brief מיפוי בין שמות שחקנים לתוויות הגרפיות - חיוני לעדכון תצוגה */
    QMap<QString, QLabel*> playerLabelMap;

    // ===== כפתורי פעולות =====

    /** @brief מעבר לתור הבא ללא פעולה */
    QPushButton *NextTurnButton;

    /** @brief איסוף משאבים - מוסיף 1 מטבע, ניתן לחסימה */
    QPushButton *GatherButton;

    /** @brief גביית מס - מוסיף 2-3 מטבעות, ניתן לחסימה על ידי Governor */
    QPushButton *TaxButton;

    /** @brief שוחד - נותן תור נוסף תמורת 4 מטבעות, ניתן לחסימה על ידי Judge */
    QPushButton *BribeButton;

    /** @brief מעצר - גונב מטבע מיריב, ניתן לחסימה על ידי Spy */
    QPushButton *ArrestButton;

    /** @brief חרם - חוסם פעולות מסוימות של יריב בתור הבא */
    QPushButton *SanctionButton;

    /** @brief הפיכה - מודיח יריב תמורת 7 מטבעות, ניתן לחסימה על ידי General */
    QPushButton *CoupButton;

    /** @brief השקעה - ייחודי לBaron, משקיע 3 מטבעות ומקבל 6 */
    QPushButton *InvestButton;

    /** @brief חזרה לתפריט הראשי */
    QPushButton *homeButton;

    /** @brief הצצה - ייחודי לSpy, רואה מטבעות כל השחקנים ויכול לתכנן מעצר */
    QPushButton *PeekButton;

    // ===== מערכת "בית הקברות" =====

    /** @brief פאנל צדדי המציג שחקנים שהודחו מהמשחק */
    QDockWidget *graveyardDock;

    /** @brief רשימת השחקנים המודחים עם סיבת ההדחה */
    QListWidget *graveyardList;

    /** @brief מיפוי בין שמות שחקנים לווידג'טים (לשימוש עתידי) */
    QMap<QString, QWidget*> playerWidgets;

    // ===== מערכת המשחק =====

    /** @brief אובייקט המשחק המרכזי - מכיל את כל לוגיקת המשחק */
    std::unique_ptr<Game> game;

    /** @brief פונקציה ממתינה לביצוע על מטרה שתיבחר */
    std::function<ActionResult(Player&, Player&)> pendingActionFunction;

    /** @brief דגל - האם המערכת ממתינה לבחירת מטרה */
    bool awaitingTargetSelection = false;

    /** @brief אינדקס המיקום הקודם של השחקן המודגש */
    int highlightIndex = -1;

    // ===== פונקציות עזר פרטיות =====

    /** @brief מטפלת בפעולה הייחודית של Spy - הצצה למטבעות ותכנון מעצר */
    void handleSpyCoins();

    /** @brief בונה את תצוגת השחקנים הגרפית */
    void setupPlayers();

    /** @brief בונה את כפתורי הפעולות ומחברת אותם לפונקציות */
    void setupActions();

    /** @brief בונה קלפי פעולות (לשימוש עתידי) */
    void setupActionCards();

    /** @brief מעדכנת את התווית המציגה את השחקן הנוכחי */
    void updateTurnLabel();

    /** @brief מעדכנת תצוגת מטבעות - מלא לנוכחי, מוסתר לאחרים */
    void updateCoinLabel();

    /** @brief מבצעת אנימציה קלה על תווית התור */
    void animateTurnLabel();

    /** @brief מבצעת אנימציה של קלף למרכז (לשימוש עתידי) */
    void animateCardToCenter(QPushButton* card);

    /** @brief בוחרת ומבצעת פעולה אוטומטית (לשימוש עתידי) */
    void chooseAndExecuteTargetAction(const QString& action);

    /** @brief מפעילה מצב בחירת מטרה - מדגישה שחקנים וממתינה ללחיצה */
    void requestTargetForAction(std::function<ActionResult(Player&, Player&)> actionFunc);

    /** @brief מאפסת הדגשות שחקנים ומחזירה למצב רגיל */
    void resetPlayerHighlights();

    /** @brief מדגישה ויזואלית את השחקן הנוכחי ומציגה כפתורים ייחודיים */
    void highlightCurrentPlayer();

    /** @brief מעדכנת תצוגה של שחקנים מודחים - הופכת לשקופים ומעבירה לבית קברות */
    void updatePlayerStatusVisuals();

    /** @brief פותחת דיאלוג לבחירת שחקן מטרה מרשימה */
    void requestTargetPlayer(const QString& title, std::function<void(Player&)> callback);

    /** @brief מטפלת בלחיצות על שחקנים כאשר ממתינים לבחירת מטרה */
    bool eventFilter(QObject *watched, QEvent *event) override;

    /** @brief מוסיפה שחקן לבית הקברות עם סיבת ההדחה */
    void addPlayerToGraveyard(const QString &name, const QString &reason);

    /** @brief שואלת שחקנים אם רוצים לחסום פעולה עם דיאלוג */
    bool askForBlock(const QString &attackerName, const QString &actionName, const QStringList &blockers, const QString &targetName = "");

    /** @brief מציגה הודעה על שחקן שנחסם */
    void showLastBlockedPlayer(const Player& player);

    /** @brief מעדכנת תווית חסימות מעצר מתוכננות על ידי Spy */
    void updateBlockedArrestLabel();

private slots:
    /** @brief מטפלת בפעולת Gather - איסוף משאבים (+1 מטבע) */
    void handleGather();

    /** @brief מטפלת בפעולת Tax - גביית מס (+2-3 מטבעות) */
    void handleTax();

    /** @brief מטפלת בפעולת Bribe - שוחד (תור נוסף תמורת 4 מטבעות) */
    void handleBribe();

    /** @brief מטפלת בפעולת Arrest - מעצר (גניבת מטבע מיריב) */
    void handleArrest();

    /** @brief מטפלת בפעולת Coup - הפיכה (הדחת יריב תמורת 7 מטבעות) */
    void handleCoup();

    /** @brief מטפלת בפעולת Invest - השקעה ייחודית לBaron */
    void handleInvest();

    /** @brief מטפלת בסיום המשחק - מציגה מנצח וחוזרת לתפריט */
    void handleGameEnd(const QString& winnerName);
};

#endif // GAMEBOARDWINDOW_H