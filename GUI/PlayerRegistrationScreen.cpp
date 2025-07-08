//
// 12adi45@gmail.com
//
#include "PlayerRegistrationScreen.h"
#include "MainWindow.h"
#include <QLabel>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QCheckBox>
#include <QPushButton>
#include <QComboBox>
#include <QSpacerItem>
#include <QFont>

/**
 * @brief בנאי - יוצר מסך רישום עם שדות שמות, אפשרויות וכפתורים
 */
PlayerRegistrationScreen::PlayerRegistrationScreen(QWidget *parent)
    : QWidget(parent)
{
    // יצירת הפריסה הראשית
    mainLayout = new QVBoxLayout(this);

    // יצירת כותרת המסך
    QLabel *titleLabel = new QLabel("🎲 משחק אמיתי - הגדרת שחקנים", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "color: white; "
        "background-color: #3498db; "
        "padding: 15px; "
        "border-radius: 10px; "
        "margin: 10px;"
    );
    mainLayout->addWidget(titleLabel);

    // יצירת טקסט הסבר
    QLabel *descLabel = new QLabel("במסך זה תוכל להגדיר את שמות השחקנים והגדרות נוספות למשחק. \n "
                                   "לאחר ההגדרה, המשחק יתחיל ותוכל לצפות בלוח ובפעולות השחקנים בזמן אמת.", this);
    descLabel->setWordWrap(true);
    descLabel->setAlignment(Qt::AlignCenter);
    descLabel->setStyleSheet(
        "color: #34495e; "
        "background-color: #ecf0f1; "
        "padding: 10px; "
        "border-radius: 5px; "
        "margin: 5px;"
    );
    mainLayout->addWidget(descLabel);

    // קבוצת בחירת מספר שחקנים
    QGroupBox *countGroup = new QGroupBox("🎯 מספר שחקנים", this);
    QHBoxLayout *countLayout = new QHBoxLayout(countGroup);

    QLabel *countLabel = new QLabel("בחר מספר שחקנים (4-6):", this);
    playerCountSpin = new QSpinBox(this);
    playerCountSpin->setRange(4, 6);
    playerCountSpin->setValue(4);
    playerCountSpin->setStyleSheet("font-size: 14px; padding: 5px;");

    countLayout->addWidget(countLabel);
    countLayout->addWidget(playerCountSpin);
    countLayout->addStretch();

    mainLayout->addWidget(countGroup);

    // קבוצת אפשרויות מתקדמות
    QGroupBox *optionsGroup = new QGroupBox("⚙️ אפשרויות", this);
    QVBoxLayout *optionsLayout = new QVBoxLayout(optionsGroup);

    useRandomNamesCheckBox = new QCheckBox("השתמש בשמות רנדומליים", this);
    useRandomNamesCheckBox->setChecked(true);
    useRandomNamesCheckBox->setStyleSheet("font-size: 14px;");

    showActionsCheckBox = new QCheckBox("הצג הסברים מפורטים על פעולות", this);
    showActionsCheckBox->setChecked(true);
    showActionsCheckBox->setStyleSheet("font-size: 14px;");

    optionsLayout->addWidget(useRandomNamesCheckBox);
    optionsLayout->addWidget(showActionsCheckBox);

    mainLayout->addWidget(optionsGroup);

    // אזור שמות השחקנים
    QGroupBox *namesGroup = new QGroupBox("👥 שמות שחקנים", this);
    formLayout = new QVBoxLayout(namesGroup);
    mainLayout->addWidget(namesGroup);

    // כפתורים תחתונים
    QHBoxLayout *btnLayout = new QHBoxLayout;

    backButton = new QPushButton("🏠 חזרה", this);
    backButton->setStyleSheet(
        "QPushButton { "
        "background-color: #95a5a6; "
        "color: white; "
        "font-size: 16px; "
        "font-weight: bold; "
        "padding: 12px 20px; "
        "border: none; "
        "border-radius: 8px; "
        "}"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );

    startButton = new QPushButton("התחל משחק אמיתי 🎲", this);
    startButton->setStyleSheet(
        "QPushButton { "
        "background-color: #27ae60; "
        "color: white; "
        "font-size: 16px; "
        "font-weight: bold; "
        "padding: 12px 20px; "
        "border: none; "
        "border-radius: 8px; "
        "}"
        "QPushButton:hover { background-color: #2ecc71; }"
    );

    btnLayout->addWidget(backButton);
    btnLayout->addStretch();
    btnLayout->addWidget(startButton);

    mainLayout->addLayout(btnLayout);

    // חיבור סיגנלים לפונקציות
    connect(playerCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PlayerRegistrationScreen::updateNameFields);
    connect(startButton, &QPushButton::clicked, this, &PlayerRegistrationScreen::startGame);
    connect(backButton, &QPushButton::clicked, this, &PlayerRegistrationScreen::goBack);

    // אתחול ראשוני של השדות
    updateNameFields(playerCountSpin->value());
}

/**
 * @brief מעדכן את מספר שדות השמות בהתאם למספר השחקנים שנבחר
 */
void PlayerRegistrationScreen::updateNameFields(int count)
{
    // מחיקת שדות קיימים
    QLayoutItem *item;
    while ((item = formLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    nameFields.clear();

    // יצירת שדות חדשים
    for (int i = 0; i < count; ++i) {
        QHBoxLayout *fieldLayout = new QHBoxLayout();

        /** @brief יוצר תווית ושדה הכנסה לכל שחקן */
        QLabel *label = new QLabel(QString("שחקן %1:").arg(i + 1), this);
        label->setMinimumWidth(80);
        label->setStyleSheet("font-weight: bold;");

        QLineEdit *nameField = new QLineEdit(this);
        nameField->setPlaceholderText(QString("שם שחקן %1").arg(i + 1));
        nameField->setStyleSheet(
            "padding: 8px; "
            "border: 2px solid #bdc3c7; "
            "border-radius: 5px; "
            "font-size: 14px;"
        );

        fieldLayout->addWidget(label);
        fieldLayout->addWidget(nameField);
        formLayout->addLayout(fieldLayout);
        nameFields.push_back(nameField);
    }
}

/**
 * @brief חוזר לתפריט הראשי
 */
void PlayerRegistrationScreen::goBack() {
    auto *mainMenu = new MainWindow();
    mainMenu->show();
    close();
}

/**
 * @brief אוסף את נתוני השחקנים, יוצר אותם ומתחיל את המשחק
 */
void PlayerRegistrationScreen::startGame()
{
    std::vector<std::shared_ptr<Player>> players;

    // מעבר על כל שדות השמות ויצירת השחקנים
    for (QLineEdit *line : nameFields) {
        QString name = line->text().trimmed();

        /** @brief שם ברירת מחדל לשדות ריקים */
        if (name.isEmpty()) {
            name = "Player";
        }

        // יצירת תפקיד רנדומלי ושחקן חדש
        auto role = roleFactory.createRandomRole();
        players.push_back(std::make_shared<Player>(name.toStdString(), std::move(role)));
    }

    /** @brief פליטת סיגנל עם השחקנים שנוצרו */
    emit playerRegistered(players);
    close();
}