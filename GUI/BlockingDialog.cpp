//
// 12adi45@gmail.com
//

#include "BlockingDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>

/**
 * @brief בנאי - יוצר דיאלוג חסימה עם הודעה מותאמת ושני כפתורי בחירה
 */
BlockingDialog::BlockingDialog(const QString &attacker,
                               const QString &actionName,
                               const QStringList &blockers,
                               const QString & target,
                               QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle("Blocking options");

  // בניית הודעת החסימה בהתאם לסוג הפעולה
  QString message;
  if(target.isEmpty()){
    message = QString("%1 Try to make the action %2, Do you want to block?").arg(attacker).arg(actionName);
  }
  else{
    message = QString("%1 Try to make the action %2 on %3. Do you want to blocked him?")
             .arg(attacker)
             .arg(actionName)
             .arg(target);
  }

  // יצירת רכיבי הממשק
  messageLabel = new QLabel(message, this);
  blockButton = new QPushButton("BLOCK", this);
  skipButton = new QPushButton("SKIP", this);

  // חיבור סיגנלים
  connect(blockButton, &QPushButton::clicked, this, &BlockingDialog::onBlockButtonClicked);
  connect(skipButton, &QPushButton::clicked, this, &BlockingDialog::onSkipButtonClicked);

  // פריסת כפתורים אופקית
  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(blockButton);
  buttonLayout->addWidget(skipButton);

  // פריסה ראשית אנכית
  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(messageLabel);
  mainLayout->addLayout(buttonLayout);

  setLayout(mainLayout);
}

/**
 * @brief מטפל בלחיצה על כפתור BLOCK - פולט סיגנל וסוגר את הדיאלוג
 */
void BlockingDialog::onBlockButtonClicked(){
  emit blockConfirmed();
  accept();
}

/**
 * @brief מטפל בלחיצה על כפתור SKIP - פולט סיגנל וסוגר את הדיאלוג
 */
void BlockingDialog::onSkipButtonClicked(){
  emit skipDeclined();
  reject();
}