#ifndef GAME_H
#define GAME_H

#include <cmath>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <QObject>
#include <limits>
#include <map>

/**
 * תוצאה של ביצוע פעולה במשחק (האם הצליח, הודעה, האם נדרש חסימה)
 */
struct ActionResult {
  bool success; ///< האם הפעולה הצליחה
  std::string message; ///< הודעה טקסטואלית לתוצאה
  bool requiresBlocking = false; ///< האם דרושה אפשרות חסימה
};

class Player;

/**
 * מחלקה זו מנהלת את כל לוגיקת המשחק – תורות, שחקנים, פעולות, קופה, חסימות, סיום משחק ועוד.
 * כל פונקציה, פעולה ושדה מתועדים במדויק בהתאם להנחיות.
 */
class Game : public QObject {
    Q_OBJECT

  private:
    /**
     * וקטור של כל השחקנים במשחק (פעילים ומודחים)
     */
    std::vector<std::shared_ptr<Player>> players;

    /**
     * אינדקס של השחקן שבתור הנוכחי
     */
    int currentTurnIndex = 0;

    /**
     * ערך קופת המטבעות המרכזית של המשחק
     */
    int coinPool = std::numeric_limits<int>::max();

    /**
     * האם המשחק נגמר (true כשנשאר שחקן אחד)
     */
    bool gameOver = false;

  public:
    /**
     * בונה משחק חדש. פרמטר parent אופציונלי – משמש ל־GUI.
     */
    Game(QObject* parent = nullptr);

    /**
     * הורס את המשחק. לא נדרש טיפול מיוחד.
     */
    ~Game();

    /**
     * מוסיף שחקן למשחק.
     * @param player מצביע חכם לשחקן להוספה
     * @throws std::runtime_error אם מנסים להוסיף שחקן אחרי שהמשחק נגמר
     */
    void addPlayer(const std::shared_ptr<Player>& player);

    /**
     * מחפש שחקן לפי שם.
     * @param name שם השחקן
     * @return מצביע לשחקן או nullptr אם לא קיים
     */
    Player* getPlayerByName(const std::string& name);

    /**
     * מחזיר וקטור של כל השחקנים (כולל מודחים)
     * @return וקטור מצביעים חכמים לשחקנים
     */
    const std::vector<std::shared_ptr<Player>>& getPlayer() const;

    /**
     * מחזיר וקטור של שמות כל השחקנים הפעילים (שלא הודחו).
     * @return וקטור שמות של שחקנים פעילים
     */
    const std::vector<std::string> getActivePlayers() const;

    /**
     * מחזיר הפניה לשחקן שבתור הנוכחי (כלומר, זה שיכול לפעול עכשיו).
     * @return רפרנס לאובייקט Player של השחקן הנוכחי
     * @throws std::runtime_error אם אין שחקנים במשחק
     */
    Player& getCurrentPlayer();

    /**
     * עובר לשחקן הבא (הבא בתור שהוא פעיל).
     * @note אם לא נשארו שחקנים פעילים – לא עושה כלום
     */
    void nextTurn();

    /**
     * בודק האם המשחק נגמר (כלומר נשאר שחקן פעיל יחיד).
     * @return true אם המשחק נגמר, אחרת false
     */
    bool isGameOver() const;

    /**
     * מחזיר את שם המנצח (הפעיל האחרון) במשחק.
     * @return שם המנצח
     * @throws std::runtime_error אם המשחק טרם הסתיים
     */
    std::string getWinner() const;

    /**
     * מחזיר הפניה לערך הקופה המרכזית (מאפשר גישה/שינוי למטבעות).
     * @return רפרנס למשתנה הקופה
     */
    int& getCoinPool();

    /**
     * מוסיף ערך לקופה המרכזית.
     * @param coinPool מספר המטבעות להוסיף
     */
    void addToCoinPool(int coinPool);

    /**
     * מבצע בדיקה האם המשחק נגמר – אם כן, מסמן ויוצר אות מתאים.
     */
    void checkGameOver();
    void validateBasicAction(const Player& player, const std::string& action);

    /**
     * פעולה: איסוף (gather). בודק אם מותר לבצע ומחזיר תוצאה.
     * @param player השחקן שמנסה לבצע פעולה
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performGather(Player& player);

    /**
     * מממש את פעולת האיסוף בפועל (מוסיף מטבע, מוריד מהקופה).
     * @param player השחקן שמבצע את האיסוף
     */
    void applyGather(Player& player);

    /**
     * פעולה: גביית מיסים (tax).
     * @param player השחקן שמנסה לבצע פעולה
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performTax(Player& player);

    /**
     * מממש את פעולת גביית המיסים בפועל (מוסיף מטבעות, בודק חסימות).
     * @param player השחקן שמבצע את הפעולה
     */
    void applyTax(Player& player);

    /**
     * פעולה: שוחד (bribe).
     * @param player השחקן שמנסה לבצע פעולה
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performBribe(Player& player);

    /**
     * מממש את פעולת השוחד בפועל (מחלק בונוס/תור נוסף).
     * @param player השחקן שמבצע את השוחד
     */
    void applyBribe(Player& player);

    /**
     * פעולה: מעצר (arrest).
     * @param attacker התוקף
     * @param target הנעצור
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performArrest(Player& attacker, Player& target);

    /**
     * מממש את פעולת המעצר בפועל (מעביר מטבע, רושם היסטוריה).
     * @param attacker התוקף
     * @param target הנעצור
     */
    void applyArrest(Player& attacker, Player& target);

    /**
     * פעולה: סנקציה (sanction).
     * @param attacker התוקף
     * @param target הנפגע
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performSanction(Player& attacker, Player& target);

    /**
     * פעולה: הפיכה (coup).
     * @param attacker התוקף
     * @param target הקורבן
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performCoup(Player& attacker, Player& target);

    /**
     * מממש את פעולת ההפיכה בפועל (מודח שחקן מהמשחק).
     * @param attacker התוקף
     * @param target הקורבן
     */
    void applyCoup(Player& attacker, Player& target);

    /**
     * פעולה: השקעה (ברון).
     * @param player הברון שמבצע השקעה
     * @return תוצאה הכוללת האם הצליח, הודעה, האם ניתן לחסום
     */
    ActionResult performInvest(Player& player);

    /**
     * מחזיר שמות כל השחקנים שיכולים לחסום פעולה מסוימת.
     * @param actionName שם הפעולה
     * @param attacker השחקן המפעיל
     * @return וקטור שמות של שחקנים שיכולים לחסום
     */
    std::vector<std::string> getPlayersWhoCanBlock(const std::string &actionName, const Player *attacker) const;

    /**
     * מחזיר מפת שמות שחקנים ומספר המטבעות שיש לכל אחד (רק פעילים).
     * @return map שם->מטבעות
     */
    std::map<std::string, int> getPlayersCoinCounts() const;

    /**
     * מפת חסימות arrest עתידיות (ע"י המרגל). key: שם המוחסם, value: שם המרגל
     */
    std::map<std::string, std::string> plannedArrests;

    /**
     * מסמן חסימת arrest עתידית.
     * @param spy המרגל החוסם
     * @param target השחקן שייחסם במעצר בתור הבא
     */
    void markPlannedArrest(Player& spy, Player& target);

    /**
     * בודק האם יש לשחקן חסימת arrest עתידית.
     * @param target שחקן לבדיקה
     * @return true אם יש חסימה עתידית, אחרת false
     */
    bool isArrestBlockedNextTurn(Player& target) const;

    /**
     * מנקה חסימת arrest עתידית.
     * @param target שחקן
     */
    void clearPlannedArrest(Player& target);

    /**
     * מחזיר הפניה לכל מפת החסימות (לקריאה בלבד).
     * @return map של כל החסימות
     */
    const std::map<std::string, std::string>& getPlannedArrests() const;

signals:
    /**
     * סיגנל שמודיע ל־GUI שהמשחק נגמר ויש מנצח
     * @param winnerName שם המנצח
     */
    void gameOverSignal(const QString& winnerName);

    /**
     * סיגנל שמודיע ל־GUI ששחקן הודח מהמשחק (כולל סיבה).
     * @param playerName שם המודח
     * @param reason הסיבה
     */
    void playerEliminated(const QString& playerName, const QString& reason);

};

#endif //GAME_H
