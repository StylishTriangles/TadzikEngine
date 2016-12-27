#ifndef UTILS_HPP
#define UTILS_HPP

#include <SFML/Graphics.hpp>
#include <sstream>
#include <ctime>

const double M_PI = 3.1415926535897;

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp);
void drawBoundingBox(sf::Sprite& sp, sf::RenderWindow* window);
float randFloat(float from, float to);
int randInt(int from, int to);
sf::Vector2i randVector2i (sf::IntRect i);
sf::Vector2f randVector2f (sf::FloatRect f);
sf::Color randColor (int from = 0, int to = 255);
bool isMouseOnSprite(sf::Sprite& sp, sf::RenderWindow* window);
std::string stringify(int x);
float det3f(sf::Vector3f top, sf::Vector3f mid, sf::Vector3f bot);
int sgn(double x);
std::string getDate();

}

#endif //UTILS_HPP
