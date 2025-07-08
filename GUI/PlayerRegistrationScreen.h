//
// 12adi45@gmail.com
//

#ifndef PLAYERREGISTRATIONSCREEN_H
#define PLAYERREGISTRATIONSCREEN_H
#include <QWidget>
#include <vector>
#include <memory>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QCheckBox>
#include "../Player.h"
#include "../Roles/RoleFactory.h"

/**
 * @class PlayerRegistrationScreen
 * @brief מסך רישום שחקנים למשחק רגיל עם בחירת שמות ואפשרויות
 */
class PlayerRegistrationScreen : public QWidget {
  Q_OBJECT

public:
  /**
   * @brief בנאי - יוצר מסך רישום עם שדות שמות, אפשרויות וכפתורים
   * @param parent מצביע לחלון אב
   */
  explicit PlayerRegistrationScreen(QWidget *parent = nullptr);

  signals:
    /**
     * @brief סיגנל שנפלט כאשר רישום השחקנים הושלם בהצלחה
     * @param players וקטור של shared_ptr לשחקנים שנוצרו
     */
    void playerRegistered(const std::vector<std::shared_ptr<Player>>& players);

  private slots:
    /** @brief מעדכן את מספר שדות השמות בהתאם למספר השחקנים שנבחר */
    void updateNameFields(int playerCount);

  /** @brief אוסף את נתוני השחקנים, יוצר אותם ומתחיל את המשחק */
  void startGame();

  /** @brief חוזר לתפריט הראשי */
  void goBack();

private:
  /** @brief ספין-בוקס לבחירת מספר שחקנים (4-6) */
  QSpinBox *playerCountSpin;

  /** @brief כפתור התחלת המשחק (ירוק) */
  QPushButton *startButton;

  /** @brief כפתור חזרה לתפריט (אפור) */
  QPushButton *backButton;

  /** @brief הפריסה הראשית האנכית */
  QVBoxLayout *mainLayout;

  /** @brief פריסה ייעודית לשדות השמות */
  QVBoxLayout *formLayout;

  /** @brief וקטור של שדות הכנסת השמות */
  std::vector<QLineEdit*> nameFields;

  /** @brief יצרן התפקידים לבחירה רנדומלית */
  RoleFactory roleFactory;

  /** @brief תיבת סימון לשמות רנדומליים */
  QCheckBox *useRandomNamesCheckBox;

  /** @brief תיבת סימון להצגת פעולות מפורטות */
  QCheckBox *showActionsCheckBox;
};

#endif //PLAYERREGISTRATIONSCREEN_H