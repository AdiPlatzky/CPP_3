// 12adi45@gmail.com
#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QWidget>
#include <QMainWindow>
#include <QPushButton>
#include <QVBoxLayout>
#include "GameBoardWindow.h"

class QPushButton;
class QVBoxLayout;

/**
 * @class MainWindow
 * @brief התפריט הראשי של המשחק עם 3 אפשרויות: משחק רגיל, אוטומטי, והוראות
 */
class MainWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר תפריט ראשי עם כותרת, כפתורים ועיצוב מלא
     * @param parent מצביע לחלון אב
     */
    explicit MainWindow(QWidget *parent = nullptr);

    private slots:
        /** @brief פותח את חלון הוראות המשחק */
        void openInstructions();

    /** @brief פותח את מסך רישום השחקנים למשחק רגיל */
    void openPlayerRegistration();

    /** @brief מתחיל משחק רגיל עם השחקנים שנרשמו */
    void startGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players);

    /** @brief פותח את מסך הגדרות המשחק האוטומטי */
    void openAutoPlayerSetup();

    /** @brief מתחיל משחק אוטומטי עם השחקנים והאפשרויות שנבחרו */
    void startAutoGameWithPlayers(const std::vector<std::shared_ptr<Player>>& players, bool showActions);

private:
    /** @brief כפתור התחלת משחק רגיל (ירוק) */
    QPushButton *newGameButton;

    /** @brief כפתור התחלת משחק אוטומטי (כחול) */
    QPushButton *autoDemoButton;

    /** @brief כפתור הצגת הוראות (כתום) */
    QPushButton *instructionsButton;

    /** @brief הפריסה הראשית האנכית של החלון */
    QVBoxLayout *layout;
};

#endif // MAINWINDOW_H