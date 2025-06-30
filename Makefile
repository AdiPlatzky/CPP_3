# 12adi45@gmail.com
CXX = g++

CXXFLAGS = -Wall -Wextra -std=c++14 -g

SRC = Player.cpp Game.cpp Roles/Baron.cpp Roles/General.cpp Roles/Governor.cpp Roles/Judge.cpp Roles/Merchant.cpp Roles/Spy.cpp Roles/RoleFactory.cpp GUI/PlayerRegistrationScreen.cpp GUI/InstructionsWindow.cpp GUI/GameBoardWindow.cpp GUI/MainWindow.cpp GUI/BlockingDialog.cpp


TEST = Tests/Player_test.cpp Tests/Game_test.cpp Tests/Baron_test.cpp Tests/General_test.cpp Tests/Governor_test.cpp Tests/Judge_test.cpp Tests/Merchant_test.cpp Tests/Spy_test.cpp


IMCLUDES = -I. -I./doctest

TARGET_MAIN = run_main
TARGET_TEST = run_test

.PHONY: all clean run-main run-test test main

all: $(TARGET_MAIN) $(TARGET_TEST)

$(TARGET_MAIN): $(SRC) main.cpp
	$(CXX) $(CXXFLAGS) $(SRC) main.cpp -o $(TARGET_MAIN) $(INCLUDES)

$(TARGET_TEST): $(SRC) $(TEST)
	$(CXX) $(CXXFLAGS) $(SRC) $(TEST) -o $(TARGET_TEST) $(INCLUDES)

run-main: $(TARGET_MAIN)
	./$(TARGET_MAIN)

run-test: $(TARGET_TEST)
	./$(TARGET_TEST)

main: run-main
test: run-test

valgrind: $(TARGET_TEST)
	valgrind --leak-check=full ./$(TARGET_TEST)

clean:
	rm -f $(TARGET_MAIN) $(TARGET_TEST)