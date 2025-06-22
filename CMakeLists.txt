cmake_minimum_required(VERSION 3.16)
project(CoupGame)

set(CMAKE_CXX_STANDARD 20)

find_package(Qt5 REQUIRED COMPONENTS Widgets)

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
        GUI/MainWindow.cpp
        GUI/MainWindow.h
)

# קבצי בדיקות
set(TEST_SOURCES
        #Tests/doctest.h
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
target_link_libraries(CoupGame Qt5::Widgets)

# בניית הבדיקות (אם רוצים)
add_executable(TestGame ${TEST_SOURCES} ${SOURCE_COMMON})
target_link_libraries(TestGame Qt5::Widgets)
