# מאת: 12adi45@gmail.com
CXX = g++

# הגדרות קומפילציה עם Qt
CXXFLAGS = -Wall -Wextra -std=c++17 -g $(shell pkg-config --cflags Qt6Widgets)
LDFLAGS = $(shell pkg-config --libs Qt6Widgets)

# קבצי קוד
SRC = \
	Player.cpp Game.cpp \
	Roles/Baron.cpp Roles/General.cpp Roles/Governor.cpp Roles/Judge.cpp \
	Roles/Merchant.cpp Roles/Spy.cpp Roles/RoleFactory.cpp \
	GUI/PlayerRegistrationScreen.cpp GUI/InstructionsWindow.cpp \
	GUI/GameBoardWindow.cpp GUI/MainWindow.cpp GUI/BlockingDialog.cpp

# קבצי טסטים
TEST_SRC = \
	Tests/Baron_test.cpp Tests/Game_test.cpp Tests/General_test.cpp \
	Tests/Governor_test.cpp Tests/Judge_test.cpp \
	Tests/Merchant_test.cpp Tests/Player_test.cpp Tests/Spy_test.cpp

# כיווני include
INCLUDES = -I. -I./GUI -I./Roles -I./Tests

# קובץ main של המשחק
MAIN = GUI/main.cpp

# שמות קבצי הבינארי
TARGET_MAIN = run_main
TARGET_TEST = run_test

.PHONY: all main test run-main run-test clean valgrind

all: $(TARGET_MAIN) $(TARGET_TEST)

# בניית המשחק
$(TARGET_MAIN): $(SRC) $(MAIN)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# בניית הבדיקות
$(TARGET_TEST): $(SRC) $(TEST_SRC)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)

# הרצת המשחק
run-main: $(TARGET_MAIN)
	./$(TARGET_MAIN)

# הרצת הבדיקות
run-test: $(TARGET_TEST)
	./$(TARGET_TEST)

# קיצורים
main: run-main
test: run-test

# בדיקת זליגות זיכרון
valgrind: $(TARGET_TEST)
	valgrind --leak-check=full ./$(TARGET_TEST)

# ניקיון
clean:
	rm -f $(TARGET_MAIN) $(TARGET_TEST)
	rm -rf build/ cmake-build-*/ *.o
