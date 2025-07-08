//
// 12adi45@gmail.com
//

#include "InstructionsWindow.h"
#include "MainWindow.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>

/**
 * @brief בנאי - יוצר חלון הוראות עם תיבת טקסט וכפתור חזרה
 * @param parent מצביע לחלון אב
 */
InstructionsWindow::InstructionsWindow(QWidget *parent) : QWidget(parent) {

    // הגדרת מאפייני החלון הבסיסיים
    setWindowTitle("הוראות משחק");
    resize(400, 300);

    // יצירת פריסה אנכית ראשית
    auto *mainLayout = new QVBoxLayout(this);

    // יצירת תיבת טקסט להוראות המשחק
    auto *instruction = new QTextEdit(this);
    instruction->setReadOnly(true);
    instruction->setText("Welcome to COUP! \n כאן יופיעו הוראות המשחק");

    // יצירת כפתור חזרה לתפריט ראשי
    auto *homeButton = new QPushButton("🏠 חזרה לתפריט", this);

    /** @brief מחבר כפתור חזרה ליצירת MainWindow חדש וסגירת החלון הנוכחי */
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow();
        mainMenu->show();
        this->close();
    });

    // הוספת הרכיבים לפריסה
    mainLayout->addWidget(instruction);
    mainLayout->addWidget(homeButton);
}