#ifndef SYNTH3D_HPP
#define SYNTH3D_HPP

#pragma once

#include "Scene.hpp"
#include "Utils.hpp"

class camera
{

};
class SYNTH3D: public Scene
{
public:
    SYNTH3D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name,mgr,w)
    {}

    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
    }
    virtual void onSceneActivate(){}
    virtual void draw(double dt){
        window->draw(pkty, 4, sf::Points);
    }
protected:
    sf::Font font;
    sf::Vertex pkty[4] =
    {
        sf::Vertex(sf::Vector2f(100, 100)),
        sf::Vertex(sf::Vector2f(200, 100)),
        sf::Vertex(sf::Vector2f(100, 200)),
        sf::Vertex(sf::Vector2f(200, 200)),
    };
};
#endif //SYNTH3D_HPP
