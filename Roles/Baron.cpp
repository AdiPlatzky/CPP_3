//
// Created by 12adi on 17/06/2025.
//
// מימוש מחלקת הברון

#include <iostream>
#include "Baron.h"
#include "../Player.h"
#include "../Game.h"

/**
 * @brief בנאי ברירת מחדל של Baron.
 */
Baron::Baron(){}

/**
 * @brief דסטרקטור של Baron.
 */
Baron::~Baron(){}

/**
 * @brief מחזיר את שם התפקיד ("Baron").
 * @return מחרוזת "Baron"
 */
std::string Baron::getName() const {
    return "Baron";
}

/**
 * @brief פעולה שמופעלת כאשר הברון סופג סנקציה — הברון מקבל מטבע אחד כפיצוי.
 * @param player הברון עצמו (הנפגע)
 * @param attacker התוקף (לא בשימוש)
 * @param game רפרנס למשחק (לא בשימוש בפונקציה זו)
 */
void Baron::onSanction(Player& player, Player&, Game&) {
    player.addCoins(1);
}

/**
 * @brief פעולה ייחודית לברון — השקעה: משלם 3 מטבעות ומקבל 6 מטבעות אם יש מספיק בקופה.
 * מבצע בדיקות: האם הברון פעיל, האם יש לו מספיק מטבעות, האם אינו חייב לבצע coup, האם יש מספיק מטבעות בקופה המרכזית.
 * במידה ויש בעיה, תופס חריגה או כותב הודעת שגיאה.
 * @param self רפרנס לברון
 * @param game רפרנס למשחק
 */
void Baron::onInvest(Player& self, Game& game){
    if(!self.isActive()){
        std::cout << "Dear baron: Your activity has been blocked \n Your game over! Bye - Bye";
        return;
    }

    if(self.getCoins() < 3){
        std::cout << "Dear baron you do not enough coins to invest.";
        return;
    }

    if (self.getCoins() >= 10) {
        std::cout << "Dear baron you have 10 or more coins.\n Must perform a coup before... See you later";
        return;
    }

    if(game.getCoinPool() < 3){
        throw std::runtime_error("Dear baron the government in crisis and it not enough coins in pool coins to return profit - we sorry about that.");
    }

    self.removeCoins(3);
    self.addCoins(6);
    game.addToCoinPool(-3);
}
