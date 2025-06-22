#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void startNewGame();
    void showInstructions();

private:
    QPushButton *newGameButton;
    QPushButton *instructionsButton;
};

#endif // MAINWINDOW_H
