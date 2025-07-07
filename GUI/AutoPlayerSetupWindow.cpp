#include "AutoPlayerSetupWindow.h"
#include "MainWindow.h"
#include <QFont>
#include <QMessageBox>
#include <QGridLayout>
#include <QFrame>
#include <random>
#include <algorithm>

AutoPlayerSetupWindow::AutoPlayerSetupWindow(QWidget *parent)
    : QWidget(parent)
{
    setWindowTitle("🤖 הגדרת משחק אוטומטי - COUP");
    resize(500, 600);
    setupUI();
}

void AutoPlayerSetupWindow::setupUI() {
    mainLayout = new QVBoxLayout(this);

    // Header
    QLabel *titleLabel = new QLabel("🤖 משחק אוטומטי - הגדרת שחקנים", this);
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

    // Description
    QLabel *descLabel = new QLabel(
        "במשחק האוטומטי, הבינה המלאכותית תשחק במקום השחקנים.\n"
        "אתה יכול לצפות ולבקר במהלך המשחק.", this);
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

    // Player count section
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

    // Options section
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

    // Player names section
    QGroupBox *namesGroup = new QGroupBox("👥 שמות שחקנים", this);
    nameFieldsLayout = new QVBoxLayout(namesGroup);

    mainLayout->addWidget(namesGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout;

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

    startButton = new QPushButton("🚀 התחל משחק אוטומטי", this);
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

    buttonLayout->addWidget(backButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(startButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(playerCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &AutoPlayerSetupWindow::updateNameFields);
    connect(startButton, &QPushButton::clicked, this, &AutoPlayerSetupWindow::startAutoGame);
    connect(backButton, &QPushButton::clicked, this, &AutoPlayerSetupWindow::goBack);
    connect(useRandomNamesCheckBox, &QCheckBox::toggled, this, &AutoPlayerSetupWindow::generateRandomNames);

    // Initialize with default player count
    updateNameFields(playerCountSpin->value());
}

void AutoPlayerSetupWindow::updateNameFields(int count) {
    // Clear existing fields
    QLayoutItem *item;
    while ((item = nameFieldsLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
    nameFields.clear();

    // Create new fields
    for (int i = 0; i < count; ++i) {
        QHBoxLayout *fieldLayout = new QHBoxLayout();

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

        nameFieldsLayout->addLayout(fieldLayout);
        nameFields.push_back(nameField);
    }

    // Generate random names if checkbox is checked
    if (useRandomNamesCheckBox && useRandomNamesCheckBox->isChecked()) {
        generateRandomNames();
    }
}

void AutoPlayerSetupWindow::generateRandomNames() {
    if (!useRandomNamesCheckBox->isChecked()) return;

    // Shuffle default names
    auto shuffledNames = defaultNames;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(shuffledNames.begin(), shuffledNames.end(), g);

    // Assign to fields
    for (int i = 0; i < nameFields.size() && i < shuffledNames.size(); ++i) {
        nameFields[i]->setText(shuffledNames[i]);
    }
}

void AutoPlayerSetupWindow::startAutoGame() {
    std::vector<std::shared_ptr<Player>> players;

    // Create players
    for (int i = 0; i < nameFields.size(); ++i) {
        QString name = nameFields[i]->text().trimmed();
        if (name.isEmpty()) {
            name = QString("Player %1").arg(i + 1);
        }

        // Create random role
        auto role = RoleFactory::createRandomRole();
        auto player = std::make_shared<Player>(name.toStdString(), std::move(role));

        // Add starting coins (2-5)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(2, 5);
        player->addCoins(dis(gen));

        players.push_back(player);
    }

    // Emit signal with players and show actions option
    emit playersConfigured(players, showActionsCheckBox->isChecked());

    // Close this window
    close();
}

void AutoPlayerSetupWindow::goBack() {
    auto *mainMenu = new MainWindow();
    mainMenu->show();
    close();
}