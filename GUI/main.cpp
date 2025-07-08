// 12adi45@gmail.com

/**
 * @file main.cpp
 * @brief נקודת הכניסה הראשית לתוכנית המשחק COUP
 */

#include <QApplication>
#include "MainWindow.h"

/**
 * @brief נקודת הכניסה הראשית - מאתחל Qt, יוצר MainWindow ומפעיל לולאת אירועים
 * @param argc מספר ארגומנטים מהשורת פקודה
 * @param argv מערך ארגומנטים מהשורת פקודה
 * @return קוד יציאה (0 = הצלחה)
 */
int main(int argc, char *argv[]) {
    /** @brief יוצר אובייקט QApplication לניהול האפליקציה */
    QApplication app(argc, argv);

    /** @brief יוצר את החלון הראשי של המשחק */
    MainWindow window;

    /** @brief מציג את החלון למשתמש */
    window.show();

    /** @brief מפעיל לולאת אירועים ומחזיר קוד יציאה */
    return app.exec();
}