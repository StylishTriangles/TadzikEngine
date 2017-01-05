#ifndef COMMON_HPP
#define COMMON_HPP

#include <SFML/Graphics.hpp>

//const double M_PI = 3.1415926535897;

namespace Common {
    namespace Font {
        sf::Font Carnevalee_Freakshow;
        sf::Font Days_Later;
        sf::Font Comic_Sans;
    }
    void loadFonts() {
        Font::Carnevalee_Freakshow.loadFromFile("files/Carnevalee_Freakshow.ttf");
        Font::Days_Later.loadFromFile("files/28_Days_Later.ttf");
        Font::Comic_Sans.loadFromFile("files/Comic_Sans.ttf");
    }
}

#endif //COMMON_HPP
