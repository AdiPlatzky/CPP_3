// 12adi45@gmail.com

#ifndef AUTOPLAYERSETUPWINDOW_H
#define AUTOPLAYERSETUPWINDOW_H

#include <QWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QCheckBox>
#include <vector>
#include <memory>
#include "../Player.h"
#include "../Roles/RoleFactory.h"

/**
 * @class AutoPlayerSetupWindow
 * @brief חלון הגדרת שחקנים למשחק אוטומטי - מאפשר בחירת מספר שחקנים, שמות ואפשרויות
 * @author 12adi45@gmail.com
 */
class AutoPlayerSetupWindow : public QWidget {
    Q_OBJECT

public:
    /**
     * @brief בנאי - יוצר חלון הגדרת משחק אוטומטי
     * @param parent חלון אב
     */
    explicit AutoPlayerSetupWindow(QWidget *parent = nullptr);

    signals:
        /**
         * @brief נפלט כאשר השחקנים הוגדרו ומוכנים להתחלת משחק
         * @param players רשימת השחקנים שנוצרו
         * @param showActions האם להציג הסברים מפורטים על פעולות
         */
        void playersConfigured(const std::vector<std::shared_ptr<Player>>& players, bool showActions);

    private slots:
        /**
         * @brief מעדכן שדות השמות בהתאם למספר השחקנים שנבחר
         * @param playerCount מספר השחקנים החדש
         */
        void updateNameFields(int playerCount);

        /** @brief מתחיל משחק אוטומטי עם השחקנים שהוגדרו */
        void startAutoGame();

        /** @brief חוזר לתפריט הראשי */
        void goBack();

private:
    // UI Components
    /** @brief הפריסה הראשית האנכית */
    QVBoxLayout *mainLayout;

    /** @brief פריסה לשדות שמות השחקנים */
    QVBoxLayout *nameFieldsLayout;

    /** @brief בורר מספר שחקנים (4-6) */
    QSpinBox *playerCountSpin;

    /** @brief כפתור התחלת משחק אוטומטי */
    QPushButton *startButton;

    /** @brief כפתור חזרה לתפריט הראשי */
    QPushButton *backButton;

    /** @brief תיבת סימון להצגת הסברים מפורטים על פעולות */
    QCheckBox *showActionsCheckBox;

    /** @brief תיבת סימון לשימוש בשמות רנדומליים */
    QCheckBox *useRandomNamesCheckBox;

    // Player name fields
    /** @brief רשימת שדות קלט לשמות השחקנים */
    std::vector<QLineEdit*> nameFields;

    // Default names for auto-game
    /** @brief רשימת שמות ברירת מחדל למשחק אוטומטי */
    QStringList defaultNames = {
        "Alice 🎭", "Bob 🎯", "Charlie 🎪", "Diana 🎨",
        "Eva 🎰", "Frank 🎲", "Grace 🎻", "Henry 🎪"
    };

    /** @brief בונה את ממשק המשתמש */
    void setupUI();

    /** @brief מייצר שמות רנדומליים לשחקנים מהרשימה הקבועה */
    void generateRandomNames();
};

#endif // AUTOPLAYERSETUPWINDOW_H