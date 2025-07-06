//
// 12adi45@gmail.com
//

#ifndef ROLE_H
#define ROLE_H
#include <string>

/**
 * מחלקת בסיס אבסטרקטית (מופשטת) לכל תפקידי השחקנים במשחק.
 * כל תפקיד במשחק (לדוג' General, Judge, Baron) יורש ממחלקה זו, ומממש/משנה פונקציות בהתאם לתכונותיו הייחודיות.
 * המחלקה מגדירה ממשק אחיד לכל התפקידים.
 */
class Player;
class Game;

class Role {
  public:
    /**
     * דסטרקטור וירטואלי דרוש למחלקות ירושה.
     * מאפשר למחוק מצביעים ל־Role בצורה בטוחה (חשוב כשמשתמשים ב־unique_ptr).
     */
    virtual ~Role() = default;

    /**
     * מחזיר את שם התפקיד. פונקציה מופשטת — חייבת להיות ממומשת ע"י כל מחלקה יורשת.
     * @return מחרוזת עם שם התפקיד ("Baron", "Spy" וכו').
     */
    virtual std::string getName() const = 0;

    /**
     * פעולה שמתרחשת בתחילת תור של שחקן עם תפקיד זה.
     * ברירת מחדל — לא עושה כלום. מחלקות מיוחדות (כמו Merchant) מממשות פונקציה זו כדי להעניק בונוס.
     * @param player רפרנס לשחקן
     * @param game רפרנס למשחק
     */
    virtual void onTurnStart(Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר השחקן מבצע או מושפע מפעולת tax (לדוג', Governor מגיב).
     * ברירת מחדל — לא עושה כלום.
     * @param actor השחקן שמבצע את הפעולה
     * @param target השחקן עליו מופעלת הפעולה
     * @param game רפרנס למשחק
     */
    virtual void onTax(Player&, Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר השחקן מבצע או מושפע מפעולת bribe (שוחד).
     * לדוג' Judge יכול לחסום או לשנות את השפעת הפעולה.
     * ברירת מחדל — לא עושה כלום.
     * @param actor המבצע
     * @param target הנפגע/המקבל
     * @param game רפרנס למשחק
     */
    virtual void onBribe(Player&, Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר השחקן מבצע או מושפע מפעולת arrest (מעצר).
     * לדוג', General יכול להגן על שחקן אחר, Merchant עשוי להחזיר מטבע.
     * ברירת מחדל — לא עושה כלום.
     * @param actor התוקף
     * @param target הנעצור
     * @param game רפרנס למשחק
     */
    virtual void onArrest(Player&, Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר השחקן מבצע או מושפע מפעולת sanction (סנקציה).
     * לדוג', Baron עשוי להרוויח מסנקציה, Judge יכול להעניש.
     * ברירת מחדל — לא עושה כלום.
     * @param actor התוקף
     * @param target הנפגע
     * @param game רפרנס למשחק
     */
    virtual void onSanction(Player&, Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר השחקן מבצע או מושפע מפעולת coup (הפיכה).
     * ברירת מחדל — לא עושה כלום.
     * @param actor התוקף
     * @param target המודח
     * @param game רפרנס למשחק
     */
    virtual void onCoup(Player&, Player&, Game&) {}

    /**
     * פעולה שמופעלת כאשר הברון מבצע פעולה של invest (השקעה).
     * ברירת מחדל — לא עושה כלום.
     * @param player השחקן (הברון)
     * @param game רפרנס למשחק
     */
    virtual void onInvest(Player&, Game&) {}

    /**
     * בודקת האם התפקיד יכול לחסום פעולה מסוימת (למשל Judge יכול לחסום bribe).
     * ברירת מחדל — מחזירה false, כלומר התפקיד אינו יכול לחסום אף פעולה.
     * כל מחלקה יורשת יכולה לממש ולחזיר true לפעולה מסוימת.
     * @param actionName שם הפעולה (לדוג', "tax", "bribe")
     * @return true אם ניתן לחסום, אחרת false
     */
    virtual bool canBlock(const std::string & /*actionName*/) const {
        return false;
    }
};

#endif //ROLE_H
