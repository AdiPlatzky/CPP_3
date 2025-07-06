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

class AutoPlayerSetupWindow : public QWidget {
    Q_OBJECT

public:
    explicit AutoPlayerSetupWindow(QWidget *parent = nullptr);

    signals:
        void playersConfigured(const std::vector<std::shared_ptr<Player>>& players, bool showActions);

    private slots:
        void updateNameFields(int playerCount);
    void startAutoGame();
    void goBack();

private:
    // UI Components
    QVBoxLayout *mainLayout;
    QVBoxLayout *nameFieldsLayout;
    QSpinBox *playerCountSpin;
    QPushButton *startButton;
    QPushButton *backButton;
    QCheckBox *showActionsCheckBox;
    QCheckBox *useRandomNamesCheckBox;

    // Player name fields
    std::vector<QLineEdit*> nameFields;

    // Default names for auto-game
    QStringList defaultNames = {
        "Alice 🎭", "Bob 🎯", "Charlie 🎪", "Diana 🎨",
        "Eva 🎰", "Frank 🎲", "Grace 🎻", "Henry 🎪"
    };

    void setupUI();
    void generateRandomNames();
};

#endif // AUTOPLAYERSETUPWINDOW_H