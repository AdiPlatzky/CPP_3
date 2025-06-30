//
// Created by 12adi on 30/06/2025.
//

#include "BlockingDialog.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>


BlockingDialog::BlockingDialog(const QString &attacker,
                               const QString &actionName,
                               const QStringList &blockers,
                               const QString & target,
                               QWidget *parent)
    : QDialog(parent)
{
  setWindowTitle("Blocking options");

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
  messageLabel = new QLabel(message, this);
  blockButton = new QPushButton("BLOCK", this);
  skipButton = new QPushButton("SKIP", this);

  connect(blockButton, &QPushButton::clicked, this, &BlockingDialog::onBlockButtonClicked);
  connect(skipButton, &QPushButton::clicked, this, &BlockingDialog::onSkipButtonClicked);

  QHBoxLayout *buttonLayout = new QHBoxLayout();
  buttonLayout->addWidget(blockButton);
  buttonLayout->addWidget(skipButton);

  QVBoxLayout *mainLayout = new QVBoxLayout(this);
  mainLayout->addWidget(messageLabel);
  mainLayout->addLayout(buttonLayout);

  setLayout(mainLayout);

}

void BlockingDialog::onBlockButtonClicked(){
  emit blockConfirmed();
  accept();
}

void BlockingDialog::onSkipButtonClicked(){
  emit skipDeclined();
  reject();
}
