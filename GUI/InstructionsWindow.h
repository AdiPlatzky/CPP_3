//
// 12adi45@gmail.com
//

#ifndef INSTRUCTIONSWINDOW_H
#define INSTRUCTIONSWINDOW_H

#include <QWidget>

// הכרזה מוקדמת כדי למנוע הכללה מיותרת של קובץ ה-header
class QTextEdit;

/**
 * @class InstructionsWindow
 * @brief חלון הצגת הוראות המשחק עם תיבת טקסט וכפתור חזרה
 */
class InstructionsWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר חלון הוראות עם תוכן ועיצוב מלא
     * @param parent מצביע לחלון אב
     */
    explicit InstructionsWindow(QWidget *parent = nullptr);

private:
    /** @note המחלקה לא מכילה משתנים פרטיים - כל הרכיבים נוצרים בבנאי */
};

#endif //INSTRUCTIONSWINDOW_H