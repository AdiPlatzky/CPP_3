//
// 12adi45@gmail.com
//

#ifndef BLOCKINGDIALOG_H
#define BLOCKINGDIALOG_H
#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;

/**
 * @class BlockingDialog
 * @brief דיאלוג המאפשר לשחקן לבחור האם לחסום פעולה של שחקן אחר
 * @author 12adi45@gmail.com
 */
class BlockingDialog : public QDialog {
  Q_OBJECT

  public:
  /**
   * @brief בנאי - יוצר דיאלוג חסימה עם פרטי הפעולה
   * @param attacker שם השחקן המבצע את הפעולה
   * @param actionName שם הפעולה (Gather, Tax, וכו')
   * @param blockers רשימת שחקנים שיכולים לחסום
   * @param target שם המטרה (ריק אם אין מטרה ספציפית)
   * @param parent חלון אב
   */
  explicit BlockingDialog(const QString &attacker,
                             const QString &actionName,
                             const QStringList &blockers,
                             const QString & target,
                             QWidget *parent = nullptr);

  signals:
     /** @brief נפלט כאשר השחקן בוחר לחסום את הפעולה */
     void blockConfirmed();

  /** @brief נפלט כאשר השחקן בוחר לדלג על החסימה */
  void skipDeclined();

  private slots:
    /** @brief מטפל בלחיצה על כפתור BLOCK */
    void onBlockButtonClicked();

  /** @brief מטפל בלחיצה על כפתור SKIP */
  void onSkipButtonClicked();

private:
  /** @brief תווית המציגה את הודעת החסימה למשתמש */
  QLabel *messageLabel;

  /** @brief כפתור לחסימת הפעולה */
  QPushButton *blockButton;

  /** @brief כפתור לדילוג על החסימה */
  QPushButton *skipButton;
};

#endif //BLOCKINGDIALOG_H