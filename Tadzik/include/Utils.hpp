#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp);
void drawBoundingBox(sf::Sprite& sp, sf::RenderWindow* window);

}

#endif //UTILS_HPP
