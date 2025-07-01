cmake_minimum_required(VERSION 3.16)
project(CoupGame)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# מציאת Qt5
find_package(Qt5 REQUIRED COMPONENTS Core Gui Widgets)

# הוספת תיקיות header אם יש קבצי include בין תיקיות
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI
        ${CMAKE_CURRENT_SOURCE_DIR}/Roles
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)

# קבצי המקור
set(SOURCE_COMMON
        Game.cpp
        Player.cpp
        Roles/Governor.cpp
        Roles/Baron.cpp
        Roles/Spy.cpp
        Roles/General.cpp
        Roles/Judge.cpp
        Roles/Merchant.cpp
        Roles/RoleFactory.cpp
        GUI/MainWindow.cpp
        GUI/GameBoardWindow.cpp
        GUI/InstructionsWindow.cpp
        GUI/PlayerRegistrationScreen.cpp
        GUI/BlockingDialog.cpp
)

# קבצי בדיקות
set(TEST_SOURCES
        Tests/Baron_test.cpp
        Tests/Game_test.cpp
        Tests/General_test.cpp
        Tests/Governor_test.cpp
        Tests/Judge_test.cpp
        Tests/Merchant_test.cpp
        Tests/Player_test.cpp
        Tests/Spy_test.cpp
)

# בניית המשחק הראשי
add_executable(CoupGame GUI/main.cpp ${SOURCE_COMMON})
target_link_libraries(CoupGame Qt5::Core Qt5::Gui Qt5::Widgets)

# בניית הבדיקות (אם רוצים)
add_executable(TestGame ${TEST_SOURCES} ${SOURCE_COMMON})
target_link_libraries(TestGame Qt5::Core Qt5::Gui Qt5::Widgets)
