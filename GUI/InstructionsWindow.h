//
// Created by 12adi on 23/06/2025.
//

#ifndef INSTRUCTIONSWINDOW_H
#define INSTRUCTIONSWINDOW_H

#include <QWidget>

class QTextEdit;

class InstructionsWindow : public QWidget {
  Q_OBJECT

  public:
    explicit InstructionsWindow(QWidget *parent = nullptr);
};



#endif //INSTRUCTIONSWINDOW_H
