#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <sstream>

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp);
void drawBoundingBox(sf::Sprite& sp, sf::RenderWindow* window);
float randFloat(float from, float to);
int randInt(int from, int to);
bool isMouseOnSprite(sf::Sprite& sp, sf::RenderWindow* window);
std::string stringify(int x);
float det3f(sf::Vector3f top, sf::Vector3f mid, sf::Vector3f bot);
int sgn(double x);

}

#endif //UTILS_HPP
