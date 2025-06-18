cmake_minimum_required(VERSION 3.16)
project(CoupGame)

set(CMAKE_CXX_STANDARD 20)

find_package(SFML 2.5 COMPONENTS graphics window system REQUIRED)

# קבצים משותפים (למשחק ולבדיקות)
set(SOURCE_COMMON
        Game.cpp
        Player.cpp
        Roles/Governor.cpp
        Roles/Baron.cpp
        Roles/Spy.cpp
        Roles/General.cpp
        Roles/Judge.cpp
        #CreateRole.cpp
        Roles/Merchant.cpp
        #MainMenu.cpp
        #PlayerRegisterScreen.cpp
        #ScreenType.cpp
        #PlayerTurnScreen.cpp
        #SelectPlayerScreen.cpp
        #ScreenManager.cpp
        #ActionTransitionScreen.cpp
        #ScreenSwitchingFailed.cpp
        #BlockPromptScreen.cpp
        #AndGame.cpp
)

# קבצים לבדיקות (לבדיקה בלבד)
set(TEST_SOURCES
        #test_game.cpp
        #doctest.h
        #test_rol.cpp
        #test_player.cpp
        #test_spy.cpp
        #test_merchant.cpp
        #test_judge.cpp
        #test_governor.cpp
        #test_general.cpp
        #test_creatRole.cpp
)

# בניית ה-target של המשחק הראשי
add_executable(CoupGame main.cpp ${SOURCE_COMMON})
target_link_libraries(CoupGame sfml-graphics sfml-window sfml-system)

# בניית ה-target של הבדיקות
add_executable(TestGame test_game.cpp ${SOURCE_COMMON} ${TEST_SOURCES})
target_link_libraries(TestGame sfml-graphics sfml-window sfml-system)
