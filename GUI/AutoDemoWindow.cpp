//12adi45@gmail.com


#include "AutoDemoWindow.h"
#include "MainWindow.h"
#include "../Roles/Baron.h"
#include <QMessageBox>
#include <QScrollBar>
#include <QDateTime>
#include <QFont>
#include <random>

/*!
  * @brief בנאי — בונה את חלון המשחק האוטומטי, יוצר את כל רכיבי הממשק ומאתחל את המשחק.
  * @param players רשימת שחקנים (shared_ptr) שמועברים למשחק.
  * @param showDetailedActions האם להציג את הפעולות בפירוט (מגדיר את אופן הדיווח ביומן).
  * @param parent מצביע ל־QWidget האב (ברירת מחדל nullptr).
  */
AutoDemoWindow::AutoDemoWindow(const std::vector<std::shared_ptr<Player>>& inputPlayers,
                               bool showDetailedActions, QWidget *parent)
    : QWidget(parent), players(inputPlayers), showDetailedActions(showDetailedActions),
      rng(std::random_device{}()), currentState(STOPPED), currentStep(0), maxTurns(INFINITY)
{
    mainLayout = new QVBoxLayout(this); // רק כאן!

    setWindowTitle("🤖 COUP - משחק אוטומטי");
    resize(1200, 800);

    setupUI();
    setupGame();

    demoTimer = new QTimer(this);
    connect(demoTimer, &QTimer::timeout, this, &AutoDemoWindow::onDemoStep);
    demoTimer->setInterval(3000); // 3 seconds default
}

/*!
 * בונה את כל הממשק הגרפי של החלון:
 * כפתורי חזרה, כותרת, תצוגת שחקנים, בקרות משחק, יומן, סטטוס, ועוד.
 */
void AutoDemoWindow::setupUI() {
    // הגדרת כותרת וכפתור חזרה לתפריט הראשי
    if (!mainLayout)
        mainLayout = new QVBoxLayout(this);

    QHBoxLayout *headerLayout = new QHBoxLayout();

    // כפתור חזרה
    homeButton = new QPushButton("🏠 חזרה לתפריט", this);
    homeButton->setStyleSheet(
        "QPushButton { "
        "background-color: #95a5a6; "
        "color: white; "
        "font-weight: bold; "
        "padding: 8px 15px; "
        "border: none; "
        "border-radius: 5px; "
        "}"
        "QPushButton:hover { background-color: #7f8c8d; }"
    );
    connect(homeButton, &QPushButton::clicked, [this]() {
        auto *mainMenu = new MainWindow(); // יוצר מסך תפריט חדש
        mainMenu->show();
        this->close(); // סוגר את המסך הנוכחי
    });

    // כותרת
    QLabel *titleLabel = new QLabel("🤖 משחק אוטומטי - COUP", this);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(16);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    titleLabel->setAlignment(Qt::AlignCenter);

    headerLayout->addWidget(homeButton);
    headerLayout->addStretch();
    headerLayout->addWidget(titleLabel);
    headerLayout->addStretch();

    mainLayout->addLayout(headerLayout);

    // קריאה לפונקציות שמוסיפות תצוגת שחקנים, בקרות, יומן ובית קברות
    setupPlayerDisplay();
    setupControls();
    setupActionLog();
    setupGraveyard();

    // סטטוס ותור נוכחי
    QHBoxLayout* statusLayout = new QHBoxLayout();

    statusLabel = new QLabel("📊 סטטוס: עצור", this);
    QFont statusFont = statusLabel->font();
    statusFont.setPointSize(12);
    statusFont.setBold(true);
    statusLabel->setFont(statusFont);

    turnLabel = new QLabel("🎯 תור: טרם החל", this);
    QFont turnFont = turnLabel->font();
    turnFont.setPointSize(12);
    turnFont.setBold(true);
    turnLabel->setFont(turnFont);

    statusLayout->addWidget(statusLabel);
    statusLayout->addSpacing(20);
    statusLayout->addWidget(turnLabel);

    mainLayout->addLayout(statusLayout);
}

/*!
 * בונה את תצוגת קלפי השחקנים (כרטיסים עם שם, תפקיד, מטבעות, סטטוס).
 * התצוגה מוצגת בראשית המסך וממופה לשמות השחקנים.
 */
void AutoDemoWindow::setupPlayerDisplay() {
    QLabel *playersLabel = new QLabel("👥 שחקנים:", this);
    playersLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 10px 0px 5px 0px;");
    mainLayout->addWidget(playersLabel);

    playerLayout = new QHBoxLayout();

    for (const auto& player : players) {
        QString name = QString::fromStdString(player->getName());
        QString role = QString::fromStdString(player->getRole()->getName());
        QString label = name + "\n(" + role + ")";

        QLabel *playerLabel = new QLabel(label, this);
        playerLabel->setStyleSheet(
            "border: 2px solid #3498db; "
            "padding: 15px; "
            "background: #ecf0f1; "
            "font-size: 12px; "
            "font-weight: bold; "
            "border-radius: 8px; "
            "min-height: 80px; "
            "max-width: 150px;"
        );
        playerLabel->setAlignment(Qt::AlignCenter);
        playerLabel->setWordWrap(true);

        playerLabelMap[name] = playerLabel; // מיפוי לפי שם
        playerLayout->addWidget(playerLabel);
    }

    mainLayout->addLayout(playerLayout);

    // תצוגת תור נוכחי (כחולה)
    turnLabel = new QLabel("🎯 תור: טרם החל", this);
    turnLabel->setAlignment(Qt::AlignCenter);
    QFont turnFont = turnLabel->font();
    turnFont.setPointSize(14);
    turnFont.setBold(true);
    turnLabel->setFont(turnFont);
    turnLabel->setStyleSheet(
        "background-color: #3498db; "
        "color: white; "
        "padding: 10px; "
        "border-radius: 8px; "
        "margin: 10px;"
    );
    mainLayout->addWidget(turnLabel);

    // תצוגת תוצאה (הודעות משתמש)
    actionResultLabel = new QLabel("בחר פעולה או התחל את המשחק", this);
    actionResultLabel->setAlignment(Qt::AlignCenter);
    actionResultLabel->setStyleSheet(
        "background-color: #f8f9fa; "
        "padding: 8px; "
        "border: 1px solid #dee2e6; "
        "border-radius: 5px; "
        "margin: 5px;"
    );
    mainLayout->addWidget(actionResultLabel);
}


/*!
 * בונה את כל בקרות המשחק (כפתורי שליטה, סליידר מהירות וכו'), מחבר את כל הסיגנלים, ומציב אותם בממשק.
 */
void AutoDemoWindow::setupControls() {
    // קופסת בקרה עם כותרת
    QGroupBox *controlGroup = new QGroupBox("🎮 בקרות משחק", this);
    controlLayout = new QGridLayout(controlGroup);

    // כפתור התחל
    startButton = new QPushButton("▶️ התחל", this);
    startButton->setStyleSheet("QPushButton { background-color: #27ae60; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");

    // כפתור השהה (לא פעיל בתחילה)
    pauseButton = new QPushButton("⏸️ השהה", this);
    pauseButton->setStyleSheet("QPushButton { background-color: #f39c12; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    pauseButton->setEnabled(false);

    // כפתור עצור (לא פעיל בתחילה)
    stopButton = new QPushButton("⏹️ עצור", this);
    stopButton->setStyleSheet("QPushButton { background-color: #e74c3c; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");
    stopButton->setEnabled(false);

    // כפתור "צעד אחד" (פעיל תמיד)
    stepButton = new QPushButton("⏭️ צעד אחד", this);
    stepButton->setStyleSheet("QPushButton { background-color: #3498db; color: white; font-weight: bold; padding: 10px; border-radius: 5px; }");

    // בקרה למהירות – טקסט + סליידר + תווית טקסט
    QLabel *speedLabelText = new QLabel("מהירות:", this);
    speedSlider = new QSlider(Qt::Horizontal, this); // סליידר אופקי
    speedSlider->setRange(1, 10); // טווח ערכים
    speedSlider->setValue(5); // ברירת מחדל = רגילה
    speedLabel = new QLabel("רגילה", this);

    // מיקום בלייאאוט
    controlLayout->addWidget(startButton, 0, 0);
    controlLayout->addWidget(pauseButton, 0, 1);
    controlLayout->addWidget(stopButton, 0, 2);
    controlLayout->addWidget(stepButton, 0, 3);
    controlLayout->addWidget(speedLabelText, 1, 0);
    controlLayout->addWidget(speedSlider, 1, 1, 1, 2);
    controlLayout->addWidget(speedLabel, 1, 3);

    mainLayout->addWidget(controlGroup);

    // חיבור כל כפתור לסלוט שלו
    connect(startButton, &QPushButton::clicked, this, &AutoDemoWindow::startDemo);
    connect(pauseButton, &QPushButton::clicked, this, &AutoDemoWindow::pauseDemo);
    connect(stopButton, &QPushButton::clicked, this, &AutoDemoWindow::stopDemo);
    connect(stepButton, &QPushButton::clicked, this, &AutoDemoWindow::stepDemo);
    connect(speedSlider, &QSlider::valueChanged, this, &AutoDemoWindow::adjustSpeed);
}

/*!
 * בונה את יומן הפעולות (Action Log) של המשחק – תיבה שמציגה לכל אורך המשחק את כל מה שקורה.
 */
void AutoDemoWindow::setupActionLog() {
    // כותרת ליומן
    QLabel *logLabel = new QLabel("📋 יומן פעולות:", this);
    logLabel->setStyleSheet("font-weight: bold; font-size: 14px; margin: 10px 0px 5px 0px;");
    mainLayout->addWidget(logLabel);

    // תיבת טקסט לקריאה בלבד (קונסולה קטנה)
    actionLog = new QTextEdit(this);
    actionLog->setReadOnly(true);
    actionLog->setMaximumHeight(200); // גובה מקסימלי
    actionLog->setFont(QFont("Consolas", 10)); // גופן קבוע רוחב (קונסולה)
    actionLog->setStyleSheet(
        "background-color: #2c3e50; "
        "color: #ecf0f1; "
        "border: 2px solid #34495e; "
        "border-radius: 5px;"
    );

    mainLayout->addWidget(actionLog);
}

/*!
 * בונה את "בית הקברות" – רשימת שחקנים שהודחו מהמשחק (QDockWidget).
 * בפועל, כרגע רק נבנה ומוסתר (לא משולב בדוק אמיתי).
 */
void AutoDemoWindow::setupGraveyard() {
    graveyardDock = new QDockWidget("🪦 שחקנים שהודחו", this);
    graveyardDock->setAllowedAreas(Qt::RightDockWidgetArea);
    graveyardDock->setFeatures(QDockWidget::DockWidgetMovable);

    graveyardList = new QListWidget(graveyardDock);
    graveyardList->setStyleSheet("background-color: #f4f4f4; font-size: 12px;");
    graveyardDock->setWidget(graveyardList);

    graveyardDock->setMinimumWidth(150);
    graveyardDock->setMaximumWidth(200);
    graveyardDock->hide(); // מוסתר כברירת מחדל

    // הערה: בד"כ מחברים את זה לחלון ראשי עם DOCKS אמיתיים (פה זה יופיע ביומן)
}


/*!
 * אתחול אובייקט המשחק (Game), הוספת שחקנים, חיבור סיגנלים ועדכון ראשוני של התצוגות.
 */
void AutoDemoWindow::setupGame() {
    game = std::make_unique<Game>(this); // יצירת מופע משחק (QObject - יש צורך ב-parent)

    // מוסיפים את כל השחקנים למשחק (game->addPlayer)
    for (const auto& player : players) {
        game->addPlayer(player);
    }

    // חיבור סיגנלים חשובים (סיום, הדחה)
    connect(game.get(), &Game::gameOverSignal, this, &AutoDemoWindow::onGameEnd);
    connect(game.get(), &Game::playerEliminated, this, &AutoDemoWindow::onPlayerEliminated);

    updatePlayerDisplay(); // עדכון תצוגה ראשונית
    updateGameStatus();    // עדכון סטטוס
}

/*!
 * מעדכנת את התצוגה הגרפית של כל שחקן: שם, תפקיד, מטבעות, סטטוס, צבע.
 * מפעילה גם הדגשה לשחקן הנוכחי.
 */
void AutoDemoWindow::updatePlayerDisplay() {
    for (const auto& player : players) {
        QString name = QString::fromStdString(player->getName());
        QString role = QString::fromStdString(player->getRole()->getName());
        QString coins = QString::number(player->getCoins());

        if (playerLabelMap.contains(name)) {
            QLabel* label = playerLabelMap[name];

            // עדכון הטקסט
            QString displayText = name + "\n(" + role + ")\n💰 " + coins;
            if (!player->isActive()) {
                displayText += "\n❌ הודח";
            }
            label->setText(displayText);

            // צבעים לפי תפקיד/סטטוס
            QString roleColor = getRoleColor(role);
            QString bgColor = player->isActive() ? "#ecf0f1" : "#bdc3c7";
            QString borderColor = player->isActive() ? roleColor : "#7f8c8d";

            label->setStyleSheet(
                QString("border: 3px solid %1; "
                        "padding: 15px; "
                        "background: %2; "
                        "font-size: 12px; "
                        "font-weight: bold; "
                        "border-radius: 8px; "
                        "min-height: 80px; "
                        "max-width: 150px;")
                .arg(borderColor)
                .arg(bgColor)
            );
        }
    }
    highlightCurrentPlayer(); // הדגשת מי שבתורו
}


/*!
 * מדגישה גרפית את השחקן שבתורו (רק אחד בכל רגע), ע"י עיצוב מסגרת/רקע ייחודיים.
 * אם המשחק נגמר/לא התחיל – הפונקציה לא עושה כלום.
 */
void AutoDemoWindow::highlightCurrentPlayer() {
    // Reset לכל הקלפים (הסרת הדגשה) – כבר מטופל ב־updatePlayerDisplay

    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString currentName = QString::fromStdString(currentPlayer.getName());

        if (playerLabelMap.contains(currentName)) {
            QLabel* label = playerLabelMap[currentName];
            QString role = QString::fromStdString(currentPlayer.getRole()->getName());
            QString roleColor = getRoleColor(role);

            label->setStyleSheet(
                QString("border: 4px solid %1; "
                        "padding: 15px; "
                        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 #ecf0f1); "
                        "font-size: 12px; "
                        "font-weight: bold; "
                        "border-radius: 8px; "
                        "min-height: 80px; "
                        "max-width: 150px; "
                        "animation: pulse 1s infinite;")
                .arg(roleColor)
            );
        }
    } catch (...) {
        // ייתכן שטרם התחיל משחק או שהסתיים – לא לעשות כלום
    }
}


/*!
 * מעדכנת את סטטוס המשחק (רץ/מושהה/עצור/הסתיים) ואת התור הנוכחי (כולל שם שחקן, תור).
 * מחליפה טקסט בלייבלים ומאפשרת/משביתה כפתורים בהתאם למצב.
 */
void AutoDemoWindow::updateGameStatus() {
    QString status = "📊 סטטוס: ";
    QString turnText = "🎯 תור: ";

    switch (currentState) {
        case STOPPED:
            status += "עצור";
            turnText += "טרם החל";
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(true);
            break;

        case RUNNING:
            status += "רץ";
            try {
                QString playerName = QString::fromStdString(game->getCurrentPlayer().getName());
                QString roleName = QString::fromStdString(game->getCurrentPlayer().getRole()->getName());
                turnText += playerName + " (" + roleName + ") - תור " + QString::number(currentStep);
            } catch (...) {
                turnText += "לא ידוע";
            }
            startButton->setEnabled(false);
            pauseButton->setEnabled(true);
            stopButton->setEnabled(true);
            stepButton->setEnabled(false);
            break;

        case PAUSED:
            status += "מושהה";
            try {
                QString playerName = QString::fromStdString(game->getCurrentPlayer().getName());
                turnText += playerName + " - תור " + QString::number(currentStep);
            } catch (...) {
                turnText += "לא ידוע";
            }
            startButton->setText("▶️ המשך");
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(true);
            stepButton->setEnabled(true);
            break;

        case FINISHED:
            status += "הסתיים";
            turnText += "המשחק הסתיים";
            startButton->setText("🔄 משחק חדש");
            startButton->setEnabled(true);
            pauseButton->setEnabled(false);
            stopButton->setEnabled(false);
            stepButton->setEnabled(false);
            break;
    }

    if (statusLabel) statusLabel->setText(status);
    if (turnLabel) turnLabel->setText(turnText);
}


/*!
 * רושמת הודעה ליומן הפעולות (actionLog) – כולל חותמת זמן, עיצוב צבעוני, וגלילה אוטומטית לתחתית.
 * @param message – ההודעה להצגה ביומן.
 * @param color – צבע ההודעה (ברירת מחדל: אפור בהיר).
 */
void AutoDemoWindow::logAction(const QString &message, const QString &color) {
    // הפקת חותמת זמן לשורה (שעה:דקה:שנייה)
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss");
    // בניית ההודעה כ-HTML עם צבע
    QString coloredMessage = QString("<span style='color: %1'>[%2] %3</span>")
                            .arg(color)
                            .arg(timestamp)
                            .arg(message);

    actionLog->append(coloredMessage); // מוסיף שורה חדשה ליומן

    // גלילה אוטומטית לתחתית היומן
    QScrollBar *scrollBar = actionLog->verticalScrollBar();
    scrollBar->setValue(scrollBar->maximum());
}


/*!
 * מתחיל משחק אוטומטי חדש או ממשיך משחק מושהה/מסתיים.
 * מאפס את הצעדים והשחקנים, רושם את מצב הפתיחה ביומן, ומפעיל את הדמו.
 */
void AutoDemoWindow::startDemo() {
    // אם המשחק לא התחיל/הסתיים – יש לאפס הכל
    if (currentState == STOPPED || currentState == FINISHED) {
        currentStep = 0;

        // איפוס מצב כל השחקנים (כמות מטבעות התחלתית)
        for (auto& player : players) {
            player->addCoins(2); // קובע לכל שחקן 2 מטבעות (יכול לדרוש איפוס סטטוס נוסף)
        }

        logAction("🎮 התחלת משחק אוטומטי חדש!", "#27ae60");
        logAction("🎭 " + QString::number(players.size()) + " שחקנים נכנסו למשחק", "#3498db");

        // רישום מצב פתיחה של כל שחקן
        for (const auto& player : players) {
            QString name = QString::fromStdString(player->getName());
            QString role = QString::fromStdString(player->getRole()->getName());
            QString coins = QString::number(player->getCoins());
            logAction("👤 " + name + " (" + role + ") מתחיל עם " + coins + " מטבעות",
                     getRoleColor(role));
        }
    }

    // עובר למצב ריצה
    currentState = RUNNING;
    demoTimer->start();           // מפעיל את הלולאה האוטומטית
    updateGameStatus();           // מעדכן תצוגה
    updatePlayerDisplay();
}


/*!
 * משהה את הדמו (אם הוא רץ), עוצר את הטיימר, ורושם הודעה ביומן.
 */
void AutoDemoWindow::pauseDemo() {
    if (currentState == RUNNING) {
        currentState = PAUSED;
        demoTimer->stop(); // עצירת לולאת הדמו
        logAction("⏸️ המשחק הושהה", "#f39c12");
        updateGameStatus();
    }
}


/*!
 * עוצר לחלוטין את הדמו (לא מאפס צעדים/שחקנים) ועובר למצב עצור.
 */
void AutoDemoWindow::stopDemo() {
    currentState = STOPPED;
    demoTimer->stop();
    logAction("⏹️ המשחק נעצר", "#e74c3c");
    updateGameStatus();
}


/*!
 * מבצע צעד בודד בדמו (פועל רק אם המשחק לא הסתיים), ע"י קריאה ל־onDemoStep.
 */
void AutoDemoWindow::stepDemo() {
    if (currentState != FINISHED) {
        onDemoStep();
    }
}


/*!
 * מבצע שלב אחד בלולאת הדמו:
 * 1. בודק אם המשחק נגמר/עבר את מגבלת התורות – ואם כן מסיים.
 * 2. מבצע מהלך של השחקן הנוכחי (AI).
 * 3. מעדכן סטטוס ותצוגה, מעביר תור, בודק סיום משחק.
 */
void AutoDemoWindow::onDemoStep() {
    // סיום לולאה אם הגעת למגבלת תורות או שהמשחק נגמר
    if (currentState == FINISHED || game->isGameOver() || currentStep >= maxTurns) {
        if (!game->isGameOver() && currentStep >= maxTurns) {
            logAction("⏰ המשחק הסתיים עקב מגבלת תורות!", "#f39c12");
        }
        currentState = FINISHED;
        demoTimer->stop();
        updateGameStatus();
        return;
    }

    try {
        // שליפת שחקן נוכחי
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());
        QString roleName = QString::fromStdString(currentPlayer.getRole()->getName());

        currentStep++; // ספירת תורות

        logAction("═══ תור " + QString::number(currentStep) + ": " + playerName + " (" + roleName + ") ═══",
                 getRoleColor(roleName));

        // עדכון מטבעות בלוג
        logAction("💰 " + playerName + " יש לו " + QString::number(currentPlayer.getCoins()) + " מטבעות",
                 "#f39c12");

        // ביצוע מהלך AI עבור השחקן
        performAIAction();

        // עדכון התצוגות
        updatePlayerDisplay();
        updateGameStatus();

        // בדיקת סיום
        game->checkGameOver();
        if (!game->isGameOver()) {
            game->nextTurn();
        }

    } catch (const std::exception& e) {
        logAction("❌ שגיאה בתור: " + QString(e.what()), "#e74c3c");
        currentState = FINISHED;
        demoTimer->stop();
        updateGameStatus();
    }
}

/*!
 * מבצע מהלך "AI" של השחקן הנוכחי: בוחר פעולה מתאימה ומדמה את ביצועה,
 * כולל טיפול במצבי קצה (למשל, חובה לבצע coup).
 */
void AutoDemoWindow::performAIAction() {
    try {
        Player& currentPlayer = game->getCurrentPlayer();
        QString playerName = QString::fromStdString(currentPlayer.getName());

        // בדיקה אם חייב לבצע הפיכה (יותר מ־10 מטבעות)
        if (currentPlayer.getCoins() >= 10) {
            logAction("⚠️ " + playerName + " חייב לבצע הפיכה (10+ מטבעות)!", "#e74c3c");
            simulatePlayerDecision("coup", currentPlayer);
            return;
        }

        // בחירת פעולה אקראית (AI)
        QString action = getRandomAction(currentPlayer);
        logAction("🤔 " + playerName + " בחר לבצע: " + action, "#9b59b6");

        simulatePlayerDecision(action, currentPlayer);

    } catch (const std::exception& e) {
        logAction("❌ שגיאה בביצוע פעולה: " + QString(e.what()), "#e74c3c");
    }
}


/*!
 * בוחר פעולה רנדומלית אפשרית (בהתאם לכללים) עבור שחקן מסוים.
 * @param player – שחקן (const).
 * @return שם פעולה (מחרוזת).
 */
QString AutoDemoWindow::getRandomAction(const Player& player) {
    std::vector<QString> possibleActions;

    // פעולות בסיסיות (אם אין יותר מ־10 מטבעות)
    if (player.getCoins() < 10) {
        possibleActions.push_back("gather");
        possibleActions.push_back("tax");
    }

    // ברייב – אם יש מספיק מטבעות ולא בוצע בסיבוב הזה
    if (player.getCoins() >= 4 && !player.hasUsedBribeThisTurn()) {
        possibleActions.push_back("bribe");
    }

    // סנקציה – דורש 3 מטבעות
    if (player.getCoins() >= 3) {
        possibleActions.push_back("sanction");
    }

    // הפיכה – דורש 7 מטבעות
    if (player.getCoins() >= 7) {
        possibleActions.push_back("coup");
    }

    // מעצר – תמיד זמין אם יש שחקנים
    if (!players.empty()) {
        possibleActions.push_back("arrest");
    }

    // פעולה ייחודית לברון
    if (player.getRole()->getName() == "Baron" && player.getCoins() >= 3) {
        possibleActions.push_back("invest");
    }

    // ברירת מחדל – "gather"
    if (possibleActions.empty()) {
        return "gather";
    }

    // בוחר פעולה רנדומלית מתוך הרשימה
    std::uniform_int_distribution<size_t> dis(0, possibleActions.size() - 1);
    return possibleActions[dis(rng)];
}


/*!
 * בוחר שחקן יעד רנדומלי (שונה מהשחקן התוקף) מתוך השחקנים הפעילים.
 * @param attacker – שחקן שמבצע את הפעולה.
 * @return מצביע חכם לשחקן יעד.
 */
std::shared_ptr<Player> AutoDemoWindow::getRandomTarget(const Player& attacker) {
    std::vector<std::shared_ptr<Player>> validTargets;

    for (const auto& player : players) {
        if (player->isActive() && player->getName() != attacker.getName()) {
            validTargets.push_back(player);
        }
    }

    if (validTargets.empty()) {
        return nullptr;
    }

    std::uniform_int_distribution<size_t> dis(0, validTargets.size() - 1);
    return validTargets[dis(rng)];
}

/*!
 * @brief מדמה ביצוע פעולה עבור שחקן מסוים (כולל טיפול בבלוקים, עדכון סטטוס והוספה ליומן).
 *        בודק את שם הפעולה, מבצע אותה על ה־Game, מטפל בתוצאה, ומוסיף רשומות ליומן.
 * @param action שם הפעולה לביצוע (gather, tax, bribe וכו')
 * @param player רפרנס לשחקן המבצע
 */
void AutoDemoWindow::simulatePlayerDecision(const QString& action, Player& player) {
    // הפקת שם השחקן לצורך הודעות
    QString playerName = QString::fromStdString(player.getName());

    try {
        ActionResult result; // תוצאה גנרית מהמשחק (הצלחה/כישלון + הודעה)

        // =====================
        // איסוף (gather)
        // =====================
        if (action == "gather") {
            // מבצע את הפעולה על האובייקט Game
            result = game->performGather(player);
            // עדכון תצוגה: הודעה מעל הלוח
            actionResultLabel->setText("💰 " + playerName + " אוסף משאבים");
            if (result.success) {
                // מדמה אפשרות לחסימה ע"י שחקן אחר (30% סיכוי)
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 3) {
                    logAction("🛡️ הפעולה נחסמה על ידי שחקן אחר!", "#e74c3c");
                } else {
                    // אם לא נחסם – מוסיף מטבע לשחקן
                    game->applyGather(player);
                    logAction("✅ " + playerName + " אסף 1 מטבע", "#27ae60");
                }
            } else {
                // טיפול בכישלון
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        // =====================
        // גביית מס (tax)
        // =====================
        else if (action == "tax") {
            result = game->performTax(player);
            actionResultLabel->setText("💰💰 " + playerName + " גובה מס");
            if (result.success) {
                // 20% סיכוי לחסימה ע"י נגיד
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                    logAction("🛡️ נגיד חסם את גביית המס!", "#e74c3c");
                } else {
                    game->applyTax(player);
                    // תוספת בונוס לנגיד (Governor)
                    int bonus = (player.getRole()->getName() == "Governor") ? 1 : 0;
                    logAction("✅ " + playerName + " גבה " + QString::number(2 + bonus) + " מטבעות", "#27ae60");
                }
            } else {
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        // =====================
        // שוחד (bribe)
        // =====================
        else if (action == "bribe") {
            result = game->performBribe(player);
            actionResultLabel->setText("💸 " + playerName + " נותן שוחד");
            if (result.success) {
                // 20% סיכוי שהשופט (Judge) יחסום
                if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                    logAction("⚖️ השופט חסם את השוחד!", "#e74c3c");
                } else {
                    game->applyBribe(player);
                    logAction("✅ " + playerName + " קיבל תור נוסף בשוחד!", "#27ae60");
                }
            } else {
                logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
            }
        }
        // =====================
        // מעצר (arrest)
        // =====================
        else if (action == "arrest") {
            auto target = getRandomTarget(player); // בוחר יעד אקראי
            if (target && target->getCoins() > 0) { // רק אם היעד פעיל ויש לו מטבעות
                QString targetName = QString::fromStdString(target->getName());
                result = game->performArrest(player, *target);
                actionResultLabel->setText("🚔 " + playerName + " עוצר את " + targetName);
                if (result.success) {
                    // 20% סיכוי שהמרגל (Spy) יחסום
                    if (std::uniform_int_distribution<int>(1, 10)(rng) <= 2) {
                        logAction("🕵️ המרגל חסם את המעצר!", "#e74c3c");
                    } else {
                        game->applyArrest(player, *target);
                        logAction("✅ " + playerName + " עצר את " + targetName + " (גנב 1 מטבע)", "#27ae60");
                    }
                } else {
                    logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
                }
            }
        }
        // =====================
        // חרם (sanction)
        // =====================
        else if (action == "sanction") {
            auto target = getRandomTarget(player);
            if (target) {
                QString targetName = QString::fromStdString(target->getName());
                result = game->performSanction(player, *target);
                actionResultLabel->setText("🚫 " + playerName + " מטיל חרם על " + targetName);
                logAction("✅ " + playerName + " הטיל חרם על " + targetName, "#8e44ad");
            }
        }
        // =====================
        // הפיכה (coup)
        // =====================
        else if (action == "coup") {
            auto target = getRandomTarget(player);
            if (target) {
                QString targetName = QString::fromStdString(target->getName());
                result = game->performCoup(player, *target);
                actionResultLabel->setText("💥 " + playerName + " מבצע הפיכה נגד " + targetName);
                if (result.success) {
                    // 70% סיכוי שהגנרל (General) יגן על עצמו אם יש לו מספיק מטבעות
                    if (target->getRole()->getName() == "General" && target->getCoins() >= 5 &&
                        std::uniform_int_distribution<int>(1, 10)(rng) <= 7) {
                        logAction("🛡️ " + targetName + " (גנרל) הגן מפני ההפיכה!", "#27ae60");
                    } else {
                        game->applyCoup(player, *target);
                        logAction("💀 " + targetName + " הודח מהמשחק בהפיכה!", "#e74c3c");
                    }
                } else {
                    logAction("❌ " + QString::fromStdString(result.message), "#e74c3c");
                }
            }
        }
        // =====================
        // השקעה (invest, ייחודי לברון)
        // =====================
        else if (action == "invest") {
            result = game->performInvest(player);
            actionResultLabel->setText("📈 " + playerName + " (ברון) משקיע");
            logAction("✅ " + playerName + " השקיע 3 מטבעות וקיבל 6 בחזרה!", "#9b59b6");
        }

    } catch (const std::exception& e) {
        // טיפול בשגיאות כלליות
        logAction("❌ שגיאה: " + QString(e.what()), "#e74c3c");
    }
}


QString AutoDemoWindow::getRoleColor(const QString &roleName) {
    if (roleName == "Governor") return "#3498db";
    else if (roleName == "Baron") return "#9b59b6";
    else if (roleName == "Judge") return "#e74c3c";
    else if (roleName == "General") return "#27ae60";
    else if (roleName == "Spy") return "#f1c40f";
    else if (roleName == "Merchant") return "#1abc9c";
    return "#34495e";
}

QString AutoDemoWindow::getPlayerInfo(const Player &player) {
    QString name = QString::fromStdString(player.getName());
    QString role = QString::fromStdString(player.getRole()->getName());
    QString coins = QString::number(player.getCoins());
    QString status = player.isActive() ? "פעיל" : "הודח";

    return name + " (" + role + ") - " + coins + " מטבעות - " + status;
}

void AutoDemoWindow::addPlayerToGraveyard(const QString &name, const QString &reason) {
    // For now, just log it since we don't have a proper dock widget setup
    logAction("⚰️ " + name + " נוסף לבית הקברות: " + reason, "#7f8c8d");
}

void AutoDemoWindow::adjustSpeed(int value) {
    // Convert slider value to timer interval
    // Value 1 = 5000ms (slow), Value 10 = 500ms (fast)
    int interval = 5500 - (value * 500);
    demoTimer->setInterval(interval);

    QString speedText;
    if (value <= 3) speedText = "איטי";
    else if (value <= 7) speedText = "רגיל";
    else speedText = "מהיר";

    speedLabel->setText(speedText);
}

void AutoDemoWindow::onGameEnd(const QString& winnerName) {
    logAction("🏆🏆🏆 המשחק הסתיים! 🏆🏆🏆", "#f1c40f");
    logAction("👑 המנצח: " + winnerName + " 👑", "#e74c3c");

    currentState = FINISHED;
    demoTimer->stop();
    updateGameStatus();

    showGameOverDialog(winnerName);
}

void AutoDemoWindow::onPlayerEliminated(const QString& playerName, const QString& reason) {
    logAction("💀 " + playerName + " הודח מהמשחק!", "#e74c3c");
    logAction("📝 סיבה: " + reason, "#95a5a6");
    addPlayerToGraveyard(playerName, reason);
    updatePlayerDisplay();
}

void AutoDemoWindow::showGameOverDialog(const QString& winner) {
    // Find winner details
    QString winnerRole = "";
    int winnerCoins = 0;

    for (const auto& player : players) {
        if (QString::fromStdString(player->getName()) == winner) {
            winnerRole = QString::fromStdString(player->getRole()->getName());
            winnerCoins = player->getCoins();
            break;
        }
    }

    QString message = QString("🎉 מזל טוב! 🎉\n\n"
                             "👑 המנצח: %1\n"
                             "🎭 תפקיד: %2\n"
                             "💰 מטבעות: %3\n"
                             "🎯 תורות: %4\n\n"
                             "האם תרצה להתחיל משחק חדש?")
                     .arg(winner)
                     .arg(winnerRole)
                     .arg(winnerCoins)
                     .arg(currentStep);

    int reply = QMessageBox::question(this, "🏆 סיום המשחק", message,
                                     QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        // Reset for new game
        currentState = STOPPED;
        currentStep = 0;

        // Reset players
        for (auto& player : players) {
            // Reset coins
            player->addCoins(2 - player->getCoins()); // Reset to 2 coins
            // Reactivate if needed (you might need to add this method to Player class)
        }

        updatePlayerDisplay();
        updateGameStatus();

        logAction("🔄 התחלת משחק חדש!", "#3498db");
    }
}