//
// Created by 12adi on 30/06/2025.
//

#ifndef BLOCKINGDIALOG_H
#define BLOCKINGDIALOG_H
#include <QDialog>
#include <QString>

class QLabel;
class QPushButton;

class BlockingDialog : public QDialog {
  Q_OBJECT

  public:
  explicit BlockingDialog(const QString &attacker,
                             const QString &actionName,
                             const QStringList &blockers,
                             const QString & target,
                             QWidget *parent = nullptr);

  signals:
     void blockConfirmed();   // השחקן לחץ "חסום"
  void skipDeclined();    // השחקן לחץ "דלג"

  private slots:
    void onBlockButtonClicked();
  void onSkipButtonClicked();

private:
  QLabel *messageLabel;
  QPushButton *blockButton;
  QPushButton *skipButton;
};

#endif //BLOCKINGDIALOG_H
