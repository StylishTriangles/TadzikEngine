#ifndef COMMON_HPP
#define COMMON_HPP

#include <SFML/Graphics.hpp>

#ifndef M_PI
#define M_PI 3.1415926535897
#endif

namespace Common {
    namespace Font {
        sf::Font Carnevalee_Freakshow;
        sf::Font Days_Later;
        sf::Font Comic_Sans;
        sf::Font Digital_7;
    }
    void loadFonts() {
        Font::Carnevalee_Freakshow.loadFromFile("files/fonts/Carnevalee_Freakshow.ttf");
        Font::Days_Later.loadFromFile("files/fonts/28_Days_Later.ttf");
        Font::Comic_Sans.loadFromFile("files/fonts/Comic_Sans.ttf");
        Font::Digital_7.loadFromFile("files/fonts/Digital_7.ttf");
    }
}

#endif //COMMON_HPP
