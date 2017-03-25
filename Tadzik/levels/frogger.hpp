#ifndef FROGGER_HPP
#define FROGGER_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class FROGGER: public Scene
{
public:
    FROGGER(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}
    virtual void onSceneLoadToMemory()
    {
        texPlayer.loadFromFile("files/textures/frogger/player.png");
        spPlayer.setTexture(texPlayer), spPlayer.setScale(10,10);
    }
    ;
    virtual void onSceneActivate()
    {

    }

    void deliverEvent(sf::Event& event)
    {

    }

    virtual void draw(double deltaTime)
    {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            offset+=sf::Vector2f(0,-10);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            offset+=sf::Vector2f(0,10);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            offset+=sf::Vector2f(10,0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            offset+=sf::Vector2f(-10,0);
        }

        spPlayer.move(offset);
        offset = sf::Vector2f(0,0);
        window->clear();
        window->draw(spPlayer);

    }


protected:
    sf::Texture texPlayer;
    sf::Sprite  spPlayer;
    sf::Vector2f offset = sf::Vector2f(0,0);
};
#endif //FROGGER
