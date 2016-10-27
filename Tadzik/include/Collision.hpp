#ifndef COLLISION_HPP
#define COLLISION_HPP

#include "Utils.hpp"

namespace Collision {
    bool PixelPerfectTest(sf::Sprite& Object1 ,sf::Sprite& Object2, sf::Uint8 AlphaLimit = 0);
    bool CreateTextureAndBitmask(sf::Texture &LoadInto, std::string& Filename);
    bool CircleTest(sf::Sprite& Object1,sf::Sprite& Object2);
    bool BoundingBoxTest(sf::Sprite& Object1, sf::Sprite& Object2);
}

#endif  /* COLLISION_HPP */
