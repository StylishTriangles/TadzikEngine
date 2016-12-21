#include "../include/Utils.hpp"

namespace Utils{

sf::FloatRect getBiggerBoundingBox(sf::Sprite& sp){
    sf::FloatRect retBox = sp.getGlobalBounds();
    retBox.left -= retBox.width/2;
    retBox.width *= 2;
    retBox.top -= retBox.height;
    retBox.height *= 2;
    return retBox;
}

void drawBoundingBox(sf::Sprite& sp, sf::RenderWindow* window){
    sf::RectangleShape shp(sf::Vector2f(sp.getGlobalBounds().width, sp.getGlobalBounds().height));
    shp.setPosition(sf::Vector2f(sp.getGlobalBounds().left, sp.getGlobalBounds().top));
    shp.setOutlineColor(sf::Color::Green);
    shp.setFillColor(sf::Color(0,0,0,0));
    shp.setOutlineThickness(1);
    window->draw(shp);
}

float randFloat(float from, float to) {
    return ((float)rand()/RAND_MAX*(to-from)+from);
}

int randInt(int from, int to) {
    return (rand()%(to-from))+from;
}

bool isMouseOnSprite(sf::Sprite& sp, sf::RenderWindow* window) {
    if (sf::Mouse::getPosition(*window).x >= sp.getPosition().x && sf::Mouse::getPosition(*window).x <= sp.getPosition().x+sp.getGlobalBounds().width &&
        sf::Mouse::getPosition(*window).y >= sp.getPosition().y && sf::Mouse::getPosition(*window).y <= sp.getPosition().y+sp.getGlobalBounds().height)
        return true;
    return false;
}

std::string stringify(int x){
    std::ostringstream o;
    o << x;
    return o.str();
}

float det3f(sf::Vector3f top, sf::Vector3f mid, sf::Vector3f bot)
{
    // top.x mid.x bot.x
    // top.y mid.y bot.y
    // top.z mid.z bot.z
    return top.x*(mid.y*bot.z - bot.y*mid.z) - mid.x*(top.y*bot.z - bot.y*top.z) + bot.x*(top.y*mid.z - mid.y*top.z);
}

int sgn(double x)
{
    if (x==0) return 0;
    if (x>0) return 1;
    if (x<0) return -1;
}

}
