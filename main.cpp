//
// Enhanced Main.cpp - Complete COUP Game Demonstration
// Created by: Game System
//

#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <memory>
#include <random>
#include <algorithm>
#include <chrono>
#include <map>
#include <thread>

#include "Game.h"
#include "Player.h"
#include "Roles/Baron.h"
#include "Roles/Governor.h"
#include "Roles/Judge.h"
#include "Roles/General.h"
#include "Roles/Spy.h"
#include "Roles/Merchant.h"
#include "Roles/RoleFactory.h"

using namespace std;

// ANSI Color codes for beautiful console output
namespace Colors {
    const string RESET = "\033[0m";
    const string BOLD = "\033[1m";
    const string RED = "\033[31m";
    const string GREEN = "\033[32m";
    const string YELLOW = "\033[33m";
    const string BLUE = "\033[34m";
    const string MAGENTA = "\033[35m";
    const string CYAN = "\033[36m";
    const string WHITE = "\033[37m";
    const string BG_RED = "\033[41m";
    const string BG_GREEN = "\033[42m";
    const string BG_BLUE = "\033[44m";
}

class GameDemo {
private:
    Game game;
    vector<shared_ptr<Player>> players;
    mt19937 rng;

    void printHeader(const string& title) {
        cout << Colors::CYAN << Colors::BOLD;
        cout << "\n" << string(60, '=') << "\n";
        cout << "  " << title << "\n";
        cout << string(60, '=') << Colors::RESET << "\n\n";
    }

    void printSubHeader(const string& subtitle) {
        cout << Colors::YELLOW << Colors::BOLD;
        cout << "\n--- " << subtitle << " ---" << Colors::RESET << "\n";
    }

    void printPlayer(const Player& player, bool isCurrentTurn = false) {
        string roleColor;
        string roleName = player.getRole()->getName();

        if (roleName == "Governor") roleColor = Colors::BLUE;
        else if (roleName == "Baron") roleColor = Colors::MAGENTA;
        else if (roleName == "Judge") roleColor = Colors::RED;
        else if (roleName == "General") roleColor = Colors::GREEN;
        else if (roleName == "Spy") roleColor = Colors::YELLOW;
        else if (roleName == "Merchant") roleColor = Colors::CYAN;
        else roleColor = Colors::WHITE;

        string statusColor = player.isActive() ? Colors::GREEN : Colors::RED;
        string turnIndicator = isCurrentTurn ? Colors::BG_BLUE + " >>> " + Colors::RESET : "     ";

        cout << turnIndicator << roleColor << Colors::BOLD << left << setw(12) << player.getName()
             << Colors::RESET << " | " << roleColor << left << setw(10) << roleName << Colors::RESET
             << " | " << Colors::YELLOW << "💰" << right << setw(3) << player.getCoins() << Colors::RESET
             << " | " << statusColor << (player.isActive() ? "✓ Active" : "✗ Eliminated") << Colors::RESET << "\n";
    }

    void printGameState() {
        printSubHeader("Current Game State");
        cout << Colors::BOLD << left << setw(17) << "Player" << "| " << left << setw(10) << "Role"
             << "| " << right << setw(5) << "Coins" << " | Status" << Colors::RESET << "\n";
        cout << string(50, '-') << "\n";

        for (const auto& player : players) {
            bool isCurrentTurn = false;
            try {
                isCurrentTurn = (player->getName() == game.getCurrentPlayer().getName());
            } catch (...) {
                // Game might be over
            }
            printPlayer(*player, isCurrentTurn);
        }

        cout << "\n" << Colors::CYAN << "🏦 Coin Pool: " << game.getCoinPool() << Colors::RESET << "\n";
    }

    void printActionResult(const string& action, const ActionResult& result) {
        cout << Colors::BOLD << "Action: " << Colors::CYAN << action << Colors::RESET;
        if (result.success) {
            cout << " " << Colors::GREEN << "✓ SUCCESS" << Colors::RESET;
        } else {
            cout << " " << Colors::RED << "✗ FAILED" << Colors::RESET;
        }
        cout << "\nResult: " << result.message << "\n";

        if (result.requiresBlocking) {
            cout << Colors::YELLOW << "⚠️  This action can be blocked by other players!" << Colors::RESET << "\n";
        }
        cout << "\n";
    }

    void simulateBlocking(const string& /* actionName */) {
        if (uniform_int_distribution<int>(1, 3)(rng) == 1) { // 33% chance of blocking
            cout << Colors::RED << "🛡️  Action was BLOCKED by another player!" << Colors::RESET << "\n\n";
            this_thread::sleep_for(chrono::milliseconds(500));
        }
    }

    void wait(int ms = 1000) {
        this_thread::sleep_for(chrono::milliseconds(ms));
    }

    void performRandomAction(Player& player) {
        vector<string> possibleActions;

        // Add possible actions based on player state
        if (player.getCoins() < 10) {
            possibleActions.push_back("gather");
            possibleActions.push_back("tax");
        }

        if (player.getCoins() >= 4 && !player.hasUsedBribeThisTurn()) {
            possibleActions.push_back("bribe");
        }

        if (player.getCoins() >= 3) {
            possibleActions.push_back("sanction");
        }

        if (player.getCoins() >= 7) {
            possibleActions.push_back("coup");
        }

        // Find valid targets for targeted actions
        vector<shared_ptr<Player>> validTargets;
        for (const auto& p : players) {
            if (p->isActive() && p->getName() != player.getName()) {
                validTargets.push_back(p);
            }
        }

        if (!validTargets.empty() && !possibleActions.empty()) {
            possibleActions.push_back("arrest");
        }

        // Special role actions
        if (player.getRole()->getName() == "Baron" && player.getCoins() >= 3) {
            possibleActions.push_back("invest");
        }

        if (possibleActions.empty()) {
            cout << Colors::RED << player.getName() << " has no valid actions!" << Colors::RESET << "\n";
            return;
        }

        // Choose random action
        string action = possibleActions[uniform_int_distribution<size_t>(0, possibleActions.size() - 1)(rng)];

        try {
            ActionResult result;

            if (action == "gather") {
                result = game.performGather(player);
                printActionResult("💰 Gather Resources", result);
                if (result.success) {
                    simulateBlocking("gather");
                    game.applyGather(player);
                }
            }
            else if (action == "tax") {
                result = game.performTax(player);
                printActionResult("💰💰 Collect Tax", result);
                if (result.success) {
                    simulateBlocking("tax");
                    game.applyTax(player);
                }
            }
            else if (action == "bribe") {
                result = game.performBribe(player);
                printActionResult("💸 Bribe for Extra Turn", result);
                if (result.success) {
                    simulateBlocking("bribe");
                    game.applyBribe(player);
                }
            }
            else if (action == "arrest" && !validTargets.empty()) {
                auto target = validTargets[uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
                if (target->getCoins() > 0) {
                    result = game.performArrest(player, *target);
                    printActionResult("🚔 Arrest " + target->getName(), result);
                    if (result.success) {
                        simulateBlocking("arrest");
                        game.applyArrest(player, *target);
                    }
                }
            }
            else if (action == "sanction" && !validTargets.empty()) {
                auto target = validTargets[uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
                result = game.performSanction(player, *target);
                printActionResult("🚫 Sanction " + target->getName(), result);
            }
            else if (action == "coup" && !validTargets.empty()) {
                auto target = validTargets[uniform_int_distribution<size_t>(0, validTargets.size() - 1)(rng)];
                result = game.performCoup(player, *target);
                printActionResult("💥 Coup against " + target->getName(), result);
                if (result.success) {
                    simulateBlocking("coup");
                    game.applyCoup(player, *target);
                    cout << Colors::RED << Colors::BOLD << "💀 " << target->getName()
                         << " has been eliminated!" << Colors::RESET << "\n\n";
                }
            }
            else if (action == "invest") {
                result = game.performInvest(player);
                printActionResult("📈 Baron Investment", result);
            }

        } catch (const exception& e) {
            cout << Colors::RED << "Error: " << e.what() << Colors::RESET << "\n";
        }
    }

public:
    GameDemo() : rng(random_device{}()) {}

    void createPlayers() {
        printHeader("🎭 CREATING PLAYERS AND ASSIGNING ROLES");

        vector<string> names = {"Alice", "Bob", "Charlie", "Diana", "Eva", "Frank"};
        vector<string> roleNames = {"Governor", "Baron", "Judge", "General", "Spy", "Merchant"};

        for (size_t i = 0; i < min(names.size(), roleNames.size()); ++i) {
            unique_ptr<Role> role;

            if (roleNames[i] == "Governor") role = make_unique<Governor>();
            else if (roleNames[i] == "Baron") role = make_unique<Baron>();
            else if (roleNames[i] == "Judge") role = make_unique<Judge>();
            else if (roleNames[i] == "General") role = make_unique<General>();
            else if (roleNames[i] == "Spy") role = make_unique<Spy>();
            else if (roleNames[i] == "Merchant") role = make_unique<Merchant>();

            auto player = make_shared<Player>(names[i], move(role));
            players.push_back(player);
            game.addPlayer(player);

            cout << Colors::GREEN << "✓ " << Colors::BOLD << names[i]
                 << Colors::RESET << " assigned role: " << Colors::CYAN
                 << roleNames[i] << Colors::RESET << "\n";
            wait(300);
        }

        cout << "\n" << Colors::YELLOW << "🎲 Starting coins distributed to all players!" << Colors::RESET << "\n";

        // Give starting coins
        for (auto& player : players) {
            player->addCoins(uniform_int_distribution<int>(2, 5)(rng));
        }
    }

    void demonstrateRoleAbilities() {
        printHeader("🎪 DEMONSTRATING ROLE ABILITIES");

        for (auto& player : players) {
            if (!player->isActive()) continue;

            printSubHeader(player->getName() + " (" + player->getRole()->getName() + ") Special Abilities");

            string roleName = player->getRole()->getName();

            if (roleName == "Governor") {
                cout << Colors::BLUE << "🏛️  Governor Abilities:" << Colors::RESET << "\n";
                cout << "   • Gets 3 coins instead of 2 from Tax\n";
                cout << "   • Can block other players' Tax actions\n";
                cout << Colors::GREEN << "   ✓ Ready to collect enhanced taxes!" << Colors::RESET << "\n";
            }
            else if (roleName == "Baron") {
                cout << Colors::MAGENTA << "💼 Baron Abilities:" << Colors::RESET << "\n";
                cout << "   • Can invest 3 coins to get 6 coins back\n";
                cout << "   • Gets compensation when sanctioned\n";
                cout << Colors::GREEN << "   ✓ Ready to make profitable investments!" << Colors::RESET << "\n";
            }
            else if (roleName == "Judge") {
                cout << Colors::RED << "⚖️  Judge Abilities:" << Colors::RESET << "\n";
                cout << "   • Can block Bribe actions\n";
                cout << "   • Charges penalty when sanctioned\n";
                cout << Colors::GREEN << "   ✓ Ready to maintain order!" << Colors::RESET << "\n";
            }
            else if (roleName == "General") {
                cout << Colors::GREEN << "🛡️  General Abilities:" << Colors::RESET << "\n";
                cout << "   • Can defend against Coup (costs 5 coins)\n";
                cout << "   • Gets coin back when arrested\n";
                cout << Colors::GREEN << "   ✓ Defense systems activated!" << Colors::RESET << "\n";
            }
            else if (roleName == "Spy") {
                cout << Colors::YELLOW << "🕵️  Spy Abilities:" << Colors::RESET << "\n";
                cout << "   • Can see other players' coin counts\n";
                cout << "   • Can block Arrest actions\n";

                // Show intelligence report without performing game actions
                cout << "   📊 Intelligence Report:\n";
                for (const auto& p : players) {
                    if (p->getName() != player->getName()) {
                        cout << "      " << p->getName() << ": " << p->getCoins() << " coins\n";
                    }
                }
            }
            else if (roleName == "Merchant") {
                cout << Colors::CYAN << "🏪 Merchant Abilities:" << Colors::RESET << "\n";
                cout << "   • Gets bonus coin at turn start (if has 3+ coins)\n";
                cout << "   • Special defense against Arrest\n";
                cout << Colors::GREEN << "   ✓ Business ready for profit!" << Colors::RESET << "\n";
            }

            cout << "\n";
            wait(500);
        }
    }

    void runGameSimulation() {
        printHeader("🎮 STARTING COUP GAME SIMULATION");

        int turnCount = 0;
        int maxTurns = 100; // Prevent infinite loops

        while (!game.isGameOver() && turnCount < maxTurns) {
            try {
                auto& currentPlayer = game.getCurrentPlayer();

                printSubHeader("Turn " + to_string(turnCount + 1) + ": " + currentPlayer.getName() + "'s Turn");
                printGameState();

                cout << Colors::BOLD << "🎯 " << currentPlayer.getName()
                     << " (" << currentPlayer.getRole()->getName() << ") is thinking..."
                     << Colors::RESET << "\n\n";
                wait(800);

                // Check for forced coup
                if (currentPlayer.getCoins() >= 10) {
                    cout << Colors::RED << Colors::BOLD << "⚠️  " << currentPlayer.getName()
                         << " has 10+ coins and MUST perform a Coup!" << Colors::RESET << "\n\n";
                }

                // Merchant turn start bonus
                if (currentPlayer.getRole()->getName() == "Merchant" && currentPlayer.getCoins() >= 3) {
                    int beforeCoins = currentPlayer.getCoins();
                    currentPlayer.getRole()->onTurnStart(currentPlayer, game);
                    if (currentPlayer.getCoins() > beforeCoins) {
                        cout << Colors::CYAN << "💼 Merchant bonus: +1 coin!" << Colors::RESET << "\n";
                    }
                }

                performRandomAction(currentPlayer);

                game.checkGameOver();
                if (!game.isGameOver()) {
                    game.nextTurn();
                }

                turnCount++;
                wait(1000);

            } catch (const exception& e) {
                cout << Colors::RED << "Error during turn: " << e.what() << Colors::RESET << "\n";
                break;
            }
        }

        if (turnCount >= maxTurns) {
            cout << Colors::YELLOW << "⏰ Game ended due to turn limit!" << Colors::RESET << "\n";
        }
    }

    void announceWinner() {
        printHeader("🏆 GAME RESULTS");

        if (game.isGameOver()) {
            try {
                string winner = game.getWinner();
                cout << Colors::GREEN << Colors::BOLD << Colors::BG_GREEN;
                cout << "\n🎉🎉🎉 CONGRATULATIONS! 🎉🎉🎉\n\n";
                cout << "👑 WINNER: " << winner << " 👑\n\n";
                cout << Colors::RESET;

                // Find winner's role
                for (const auto& player : players) {
                    if (player->getName() == winner) {
                        cout << Colors::CYAN << "Final Role: " << player->getRole()->getName() << "\n";
                        cout << "Final Coins: " << player->getCoins() << Colors::RESET << "\n\n";
                        break;
                    }
                }

            } catch (const exception& e) {
                cout << Colors::RED << "Error determining winner: " << e.what() << Colors::RESET << "\n";
            }
        } else {
            cout << Colors::YELLOW << "Game is still in progress!" << Colors::RESET << "\n";
        }

        // Show final standings
        printSubHeader("Final Standings");
        vector<shared_ptr<Player>> standings = players;
        sort(standings.begin(), standings.end(), [](const auto& a, const auto& b) {
            if (a->isActive() != b->isActive()) return a->isActive();
            return a->getCoins() > b->getCoins();
        });

        int rank = 1;
        for (const auto& player : standings) {
            string medal;
            if (rank == 1 && player->isActive()) medal = "🥇";
            else if (rank == 2) medal = "🥈";
            else if (rank == 3) medal = "🥉";
            else medal = "  ";

            cout << medal << " " << rank << ". ";
            printPlayer(*player);
            rank++;
        }
    }

    void showGameStatistics() {
        printHeader("📊 GAME STATISTICS");

        int totalCoins = 0;
        int activePlayers = 0;
        map<string, int> roleCount;

        for (const auto& player : players) {
            totalCoins += player->getCoins();
            if (player->isActive()) activePlayers++;
            roleCount[player->getRole()->getName()]++;
        }

        cout << "🔢 Total Players: " << players.size() << "\n";
        cout << "✅ Active Players: " << activePlayers << "\n";
        cout << "💰 Total Coins in Play: " << totalCoins << "\n";
        cout << "🏦 Coins in Pool: " << game.getCoinPool() << "\n\n";

        cout << "📊 Role Distribution:\n";
        for (const auto& [role, count] : roleCount) {
            cout << "   " << role << ": " << count << "\n";
        }
    }

    void runFullDemo() {
        cout << Colors::BOLD << Colors::CYAN;
        cout << R"(
 ██████╗ ██████╗ ██╗   ██╗██████╗
██╔════╝██╔═══██╗██║   ██║██╔══██╗
██║     ██║   ██║██║   ██║██████╔╝
██║     ██║   ██║██║   ██║██╔═══╝
╚██████╗╚██████╔╝╚██████╔╝██║
 ╚═════╝ ╚═════╝  ╚═════╝ ╚═╝

         GAME DEMONSTRATION
)" << Colors::RESET << "\n";

        createPlayers();
        wait(1000);

        printGameState();
        wait(1000);

        demonstrateRoleAbilities();
        wait(1000);

        runGameSimulation();

        announceWinner();

        showGameStatistics();

        cout << "\n" << Colors::CYAN << Colors::BOLD;
        cout << "Thank you for watching the COUP game demonstration!\n";
        cout << "🎮 Game Over 🎮" << Colors::RESET << "\n\n";
    }
};

// Alternative simple demonstration functions
void quickDemo() {
    cout << Colors::YELLOW << "\n=== Quick COUP Demo ===" << Colors::RESET << "\n\n";

    Game game;
    auto alice = make_shared<Player>("Alice", make_unique<Governor>());
    auto bob = make_shared<Player>("Bob", make_unique<Baron>());
    auto charlie = make_shared<Player>("Charlie", make_unique<Spy>());

    game.addPlayer(alice);
    game.addPlayer(bob);
    game.addPlayer(charlie);

    // Give some starting coins
    alice->addCoins(3);
    bob->addCoins(4);
    charlie->addCoins(2);

    cout << "Players created with roles and starting coins:\n";
    cout << "• Alice (Governor): " << alice->getCoins() << " coins\n";
    cout << "• Bob (Baron): " << bob->getCoins() << " coins\n";
    cout << "• Charlie (Spy): " << charlie->getCoins() << " coins\n\n";

    // Demonstrate a few actions
    cout << "Turn 1 - Alice (Governor) performs Tax:\n";
    auto result = game.performTax(*alice);
    if (result.success) {
        game.applyTax(*alice);
        cout << "✓ Alice now has " << alice->getCoins() << " coins (Governor bonus!)\n\n";
    }

    game.nextTurn();
    cout << "Turn 2 - Bob (Baron) invests:\n";
    if (bob->getCoins() >= 3) {
        result = game.performInvest(*bob);
        cout << "✓ Bob invested and now has " << bob->getCoins() << " coins!\n\n";
    }

    game.nextTurn();
    cout << "Turn 3 - Charlie (Spy) gathers resources:\n";
    result = game.performGather(*charlie);
    if (result.success) {
        game.applyGather(*charlie);
        cout << "✓ Charlie now has " << charlie->getCoins() << " coins\n\n";
    }

    cout << Colors::GREEN << "Quick demo completed!" << Colors::RESET << "\n";
}

void interactiveDemo() {
    cout << Colors::BLUE << "\n=== Interactive COUP Demo ===" << Colors::RESET << "\n";
    cout << "This demo allows you to control the game manually.\n\n";

    Game game;
    vector<shared_ptr<Player>> players;

    // Create players with random roles
    vector<string> names = {"Alex", "Blake", "Casey", "Dana"};
    for (const auto& name : names) {
        auto role = RoleFactory::createRandomRole();
        auto player = make_shared<Player>(name, move(role));
        player->addCoins(3); // Starting coins
        players.push_back(player);
        game.addPlayer(player);

        cout << "• " << name << " (" << player->getRole()->getName() << ")\n";
    }

    cout << "\nGame ready! Players can now take turns...\n";
    cout << Colors::YELLOW << "Note: In a real game, this would be interactive.\n";
    cout << "For now, showing automatic turn progression:" << Colors::RESET << "\n\n";

    for (int turn = 0; turn < 5 && !game.isGameOver(); ++turn) {
        auto& current = game.getCurrentPlayer();
        cout << "Turn " << (turn + 1) << ": " << current.getName()
             << " (" << current.getRole()->getName() << ") - "
             << current.getCoins() << " coins\n";

        // Simple action: gather if low on coins, otherwise tax
        if (current.getCoins() < 3) {
            auto result = game.performGather(current);
            if (result.success) {
                game.applyGather(current);
                cout << "   → Gathered resources (+1 coin)\n";
            }
        } else {
            auto result = game.performTax(current);
            if (result.success) {
                game.applyTax(current);
                cout << "   → Collected tax (+2+ coins)\n";
            }
        }

        if (!game.isGameOver()) {
            game.nextTurn();
        }
    }

    cout << Colors::GREEN << "\nInteractive demo completed!" << Colors::RESET << "\n";
}

int main() {
    cout << Colors::BOLD << "Welcome to the COUP Game System!" << Colors::RESET << "\n\n";

    cout << "Choose demonstration mode:\n";
    cout << "1. Full Demo (Complete game with all features)\n";
    cout << "2. Quick Demo (Basic functionality)\n";
    cout << "3. Interactive Demo (Manual control)\n";
    cout << "4. Run all demos\n\n";

    cout << "Enter choice (1-4) or any other key for default full demo: ";

    char choice;
    cin >> choice;

    try {
        switch (choice) {
            case '1': {
                GameDemo demo;
                demo.runFullDemo();
                break;
            }
            case '2':
                quickDemo();
                break;
            case '3':
                interactiveDemo();
                break;
            case '4': {
                cout << Colors::CYAN << "\n🎯 Running All Demonstrations!\n" << Colors::RESET;

                GameDemo fullDemo;
                fullDemo.runFullDemo();

                quickDemo();
                interactiveDemo();

                cout << Colors::GREEN << Colors::BOLD << "\n🎊 All demonstrations completed!" << Colors::RESET << "\n";
                break;
            }
            default: {
                cout << Colors::YELLOW << "\nDefaulting to full demo...\n" << Colors::RESET;
                GameDemo demo;
                demo.runFullDemo();
                break;
            }
        }
    } catch (const exception& e) {
        cout << Colors::RED << "\nError during demonstration: " << e.what() << Colors::RESET << "\n";
        return 1;
    }

    cout << "\n" << Colors::BOLD << "Press Enter to exit...";
    cin.ignore();
    cin.get();

    return 0;
}