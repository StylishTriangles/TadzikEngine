#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <sstream>

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp);
void drawBoundingBox(sf::Sprite& sp, sf::RenderWindow* window);
float randF(float from, float to);
bool isSpriteClicked(sf::Sprite& sp, sf::RenderWindow* window);
std::string stringify(int x);

}

#endif //UTILS_HPP
