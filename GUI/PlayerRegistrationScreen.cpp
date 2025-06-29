#include "PlayerRegistrationScreen.h"
#include <QLabel>
#include <QHBoxLayout>

PlayerRegistrationScreen::PlayerRegistrationScreen(QWidget *parent)
    : QWidget(parent) {

    setWindowTitle("Player Setup");
    resize(400, 500);

    mainLayout = new QVBoxLayout(this);

    QLabel *label = new QLabel("בחר מספר שחקנים (4-6):", this);
    mainLayout->addWidget(label);

    playerCountSpin = new QSpinBox(this);
    playerCountSpin->setRange(4, 6);
    playerCountSpin->setValue(4);
    mainLayout->addWidget(playerCountSpin);

    formLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);

    connect(playerCountSpin, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &PlayerRegistrationScreen::updateNameFields);

    startButton = new QPushButton("התחל משחק", this);
    mainLayout->addWidget(startButton);
    connect(startButton, &QPushButton::clicked, this, &PlayerRegistrationScreen::startGame);

    updateNameFields(playerCountSpin->value());
}

void PlayerRegistrationScreen::updateNameFields(int count) {
    QLayoutItem *item;
    while ((item = formLayout->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    nameFields.clear();
    for (int i = 0; i < count; ++i) {
        QLineEdit *line = new QLineEdit(this);
        line->setPlaceholderText(QString("שם שחקן %1").arg(i + 1));
        formLayout->addWidget(line);
        nameFields.push_back(line);
    }
}

void PlayerRegistrationScreen::startGame() {
    std::vector<std::shared_ptr<Player>> players;
    for (QLineEdit *line : nameFields) {
        QString name = line->text().trimmed();
        if (name.isEmpty()) name = "Player";
        auto role = roleFactory.createRandomRole();
        players.push_back(std::make_shared<Player>(name.toStdString(), std::move(role)));
    }
    emit playerRegistered(players);
    close();  // סגור את חלון ההרשמה
}
