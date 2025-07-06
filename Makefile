CXX      = g++
CXXFLAGS = -Wall -Wextra -std=c++17 -g `pkg-config --cflags Qt6Widgets`
LDFLAGS  = `pkg-config --libs Qt6Widgets`
MOC      = moc-qt6

SRC_COMMON = \
    Game.cpp Player.cpp \
    Roles/Governor.cpp Roles/Baron.cpp Roles/Spy.cpp Roles/General.cpp \
    Roles/Judge.cpp Roles/Merchant.cpp Roles/RoleFactory.cpp \
    GUI/MainWindow.cpp GUI/GameBoardWindow.cpp GUI/InstructionsWindow.cpp \
    GUI/PlayerRegistrationScreen.cpp GUI/BlockingDialog.cpp GUI/AutoDemoWindow.cpp


# הקובץ הראשי למצב GUI
GUI_MAIN_SRC = GUI/main.cpp

# הקובץ הראשי למצב קונסולה
CONSOLE_MAIN_SRC = main.cpp

# קבצי טסטים
TEST_SRCS = \
    Tests/Baron_test.cpp Tests/Game_test.cpp Tests/General_test.cpp \
    Tests/Governor_test.cpp Tests/Judge_test.cpp Tests/Merchant_test.cpp \
    Tests/Player_test.cpp Tests/Spy_test.cpp Tests/Action_test.cpp Tests/EdgeCase_test.cpp Tests/Integration_test.cpp

# קבצי header עם Q_OBJECT (איתור אוטומטי)
QOBJECT_HDRS := $(shell grep -rl 'Q_OBJECT' . | grep '\.h$$' | sed 's|^\./||')
MOC_SRCS     := $(QOBJECT_HDRS:.h=.moc.cpp)

# כלל ליצירת קבצי moc
%.moc.cpp: %.h
	$(MOC) $< -o $@

MAIN_EXEC  = CoupGame
CONSOLE_EXEC = console
TEST_EXEC  = TestGame

all: $(MAIN_EXEC)

# הפעלת מצב GUI
$(MAIN_EXEC): $(GUI_MAIN_SRC) $(SRC_COMMON) $(MOC_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# הפעלת מצב קונסולה
$(CONSOLE_EXEC): $(CONSOLE_MAIN_SRC) $(SRC_COMMON) $(MOC_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)


# טסטים
$(TEST_EXEC): $(SRC_COMMON) $(TEST_SRCS) $(MOC_SRCS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

gui: $(MAIN_EXEC)
	./$(MAIN_EXEC)

main: $(CONSOLE_EXEC)
	./$(CONSOLE_EXEC)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

valgrind: $(MAIN_EXEC)
	valgrind --leak-check=full ./$(MAIN_EXEC)

clean:
	rm -f $(MAIN_EXEC) $(CONSOLE_EXEC) $(TEST_EXEC) *.moc.cpp GUI/*.moc.cpp Roles/*.moc.cpp

.PHONY: all gui main test valgrind clean

