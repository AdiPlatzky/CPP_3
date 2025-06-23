#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "GameBoardWindow.h"

class QPushButton;
class QVBoxLayout;

class MainWindow : public QWidget {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openNewGame();
    void openInstructions();

private:
    QPushButton *newGameButton;
    QPushButton *instructionsButton;
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H
