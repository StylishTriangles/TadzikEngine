#include "Utils.hpp"

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

    return ((float)rand()/RAND_MAX*(to-from)+from);
}

}
