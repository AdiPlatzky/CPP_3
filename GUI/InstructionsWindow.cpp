//
// Created by 12adi on 23/06/2025.
//

#include "InstructionsWindow.h"
#include "MainWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>

InstructionsWindow::InstructionsWindow(QWidget *parent) : QWidget(parent) {
  setWindowTitle("הוראות משחק");
  resize(400, 300);

  //auto *label = new QVBoxLayout(this);
  auto *instruction = new QTextEdit(this);
  instruction->setReadOnly(true);
  instruction->setText("Welcome to COUP! \n כאן יופיעו הוראות המשחק");

  auto *homeButton = new QPushButton("🏠 חזרה לתפריט", this);
  connect(homeButton, &QPushButton::clicked, [this]() {
      auto *mainMenu = new MainWindow();
      mainMenu->show();
      this->close();
  });
  layout()->addWidget(instruction);
  layout()->addWidget(homeButton);
}
