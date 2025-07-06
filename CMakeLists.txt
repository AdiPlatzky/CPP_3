cmake_minimum_required(VERSION 3.16)
project(CoupGame)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# מוצא את ספריית Qt
find_package(Qt6 REQUIRED COMPONENTS Widgets)

# תיקיות include
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/GUI
        ${CMAKE_CURRENT_SOURCE_DIR}/Roles
        ${CMAKE_CURRENT_SOURCE_DIR}/Tests
)

# קבצי מקור
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
        GUI/AutoDemoWindow.cpp
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
        Tests/Action_test.cpp
        Tests/EdgeCase_test.cpp
        Tests/Integration_test.cpp
)

# בניית המשחק הראשי
add_executable(CoupGame GUI/main.cpp ${SOURCE_COMMON})
target_link_libraries(CoupGame Qt6::Widgets)

# בניית הבדיקות
add_executable(TestGame ${SOURCE_COMMON} ${TEST_SOURCES})
target_link_libraries(TestGame Qt6::Widgets)

# תצורת מערכת הבדיקות (ctest)
enable_testing()
add_test(NAME RunAllTests COMMAND TestGame)
