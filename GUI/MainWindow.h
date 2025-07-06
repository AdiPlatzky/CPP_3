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
    //~MainWindow();

    private slots:
        //void openNewGame();
        void openInstructions();
    void openPlayerRegistration(); // כדי לפתוח את המסך לבחירת שחקנים
    void startGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players); //  כשסיימנו לבחור שחקנים, נתחיל משחק איתם
    void openAutoPlayerSetup(); // פתיחת מסך הגדרת שחקנים למשחק אוטומטי
    void startAutoGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players, bool showActions); // התחלת משחק אוטומטי

private:
    QPushButton *newGameButton;
    QPushButton *autoDemoButton; // כפתור חדש
    QPushButton *instructionsButton;
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H