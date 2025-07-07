# 🎭 COUP Game

משחק קלפי אסטרטגיה עם ממשק גרפי וקונסול.

## 📋 דרישות מערכת

### Ubuntu/Debian:
```bash
sudo apt update
sudo apt install build-essential cmake qt6-base-dev qt6-tools-dev valgrind
```

### Fedora/RHEL:
```bash
sudo dnf install gcc-c++ cmake qt6-qtbase-devel qt6-qttools-devel valgrind
```

### Arch Linux:
```bash
sudo pacman -S base-devel cmake qt6-base qt6-tools valgrind
```

## 🚀 הפעלה מהירה

### שיטה 1: עם Makefile (מומלץ)
```bash
# בנייה והפעלת משחק קונסול
make main

# בנייה והפעלת ממשק גרפי
make gui

# הפעלת בדיקות
make test

# בדיקת זליגות זיכרון
make valgrind

# ניקוי קבצים
make clean
```

### שיטה 2: עם סקריפט הפעלה
```bash
# תן הרשאות הפעלה לסקריפט
chmod +x run.sh

# הפעל משחק קונסול
./run.sh main

# הפעל ממשק גרפי
./run.sh gui

# הפעל בדיקות
./run.sh test

# בדוק זליגות זיכרון
./run.sh valgrind

# נקה קבצים
./run.sh clean
```

### שיטה 3: עם CMake ישירות
```bash
mkdir build
cd build
cmake ..
make

# הפעלת המשחק
./main        # משחק קונסול
./gui         # ממשק גרפי
./tests       # בדיקות
```

## 🎮 איך לשחק

### משחק קונסול
המשחק כולל דמו אוטומטי עם כל התפקידים:
- **Governor (נגיד)** - מקבל בונוס מס וחוסם מיסים
- **Baron (ברון)** - יכול להשקיע ומרוויח מסנקציות
- **Judge (שופט)** - חוסם שוחד וגובה קנסות
- **General (גנרל)** - מתגונן מהפיכות ומעצרים
- **Spy (מרגל)** - רואה מטבעות וחוסם מעצרים
- **Merchant (סוחר)** - מקבל בונוס בתחילת התור

### ממשק גרפי
- **משחק רגיל** - משחק אינטראקטיבי עם שחקנים אמיתיים
- **משחק אוטומטי** - צפייה במשחק בינה מלאכותית

## 🔧 פתרון בעיות

### שגיאת Qt
```bash
# התקן Qt6
sudo apt install qt6-base-dev qt6-tools-dev

# או עבור Qt5
sudo apt install qtbase5-dev qttools5-dev
```

### שגיאת CMake
```bash
# התקן CMake חדש יותר
sudo apt install cmake

# או מקמפל מקוד מקור אם הגרסה ישנה
```

### שגיאת Make
```bash
# התקן build tools
sudo apt install build-essential
```

### בעיות הרשאות
```bash
# תן הרשאות לקבצים
chmod +x run.sh
chmod +x build/main
chmod +x build/gui
```

## 📁 מבנה הפרויקט

```
CoupGame/
├── main.cpp              # משחק קונסול ראשי
├── Game.cpp/h            # לוגיקת המשחק העיקרית
├── Player.cpp/h          # מחלקת שחקן
├── Role.h                # ממשק תפקידים
├── Roles/                # תפקידי השחקנים
│   ├── Governor.cpp/h
│   ├── Baron.cpp/h  
│   ├── Judge.cpp/h
│   ├── General.cpp/h
│   ├── Spy.cpp/h
│   ├── Merchant.cpp/h
│   └── RoleFactory.cpp/h
├── GUI/                  # ממשק גרפי
│   ├── main.cpp
│   ├── MainWindow.cpp/h
│   ├── GameBoardWindow.cpp/h
│   └── ...
├── Tests/                # בדיקות יחידה
├── CMakeLists.txt        # קובץ בנייה
├── Makefile              # Makefile פשוט
└── run.sh               # סקריפט הפעלה
```

## 🧪 בדיקות

המערכת כוללת בדיקות מקיפות:
```bash
# הפעלת כל הבדיקות
make test

# בדיקות ספציפיות
./build/tests
```

## 💾 בדיקת זליגות זיכרון

```bash
# עם Makefile
make valgrind

# ישירות
valgrind --leak-check=full ./build/main
```

## 🎯 תכונות המשחק

- ✅ 6 תפקידים ייחודיים עם יכולות מיוחדות
- ✅ מערכת חסימות ותגובות
- ✅ ממשק גרפי אינטואיטיבי
- ✅ משחק אוטומטי עם בינה מלאכותית
- ✅ בדיקות יחידה מקיפות
- ✅ בדיקת זליגות זיכרון
- ✅ תמיכה במספר פלטפורמות

## 🆘 עזרה

אם יש בעיות:
1. וודא שכל התלויות מותקנות
2. נסה `make clean` ואז `make main`
3. בדוק הרשאות קבצים
4. הפעל `./run.sh help` לעזרה נוספת

---
**גרסה:** 1.0.0  
**מפתח:** COUP Game Team  
**רישיון:** MIT