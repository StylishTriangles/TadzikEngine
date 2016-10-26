#ifndef EXAMPLES2_HPP
#define EXAMPLES2_HPP

#include "Scene.hpp"

#include <string>

class ExampleScene2: public Scene{
public:
    ExampleScene2(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {
        std::cout << "Constructor of ExampleScene2\n";
    }

    virtual ~ExampleScene2(){
        std::cout << "Destructor of ExampleScene2\n";
    }

    virtual void onSceneActivate(){
        std::cout << "ExampleScene2::onSceneActivate()\n";
        window->setTitle("Tadzik ~Scene2");
    }

    virtual void draw(double deltaTime){
        window->clear();
        sf::CircleShape shape(std::sin( stoper.getElapsedTime().asSeconds()) * window->getSize().y / 8 + window->getSize().y / 4);
        shape.setOrigin(sf::Vector2f( shape.getRadius(), shape.getRadius()));
        shape.setPosition(window->getSize().x / 2.0f, window->getSize().y / 2.0f);
        shape.setFillColor(sf::Color::Red );
        window->draw(shape);
    }
    virtual void deliverEvent(sf::Event& e){
        if(e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::N) {
            sceneManager->setActiveScene("Example1");
        }
    }
    virtual void onSceneDeactivate(){
        std::cout << "ExampleScene2::onSceneDeactivate()\n";
    }
protected:
    sf::Clock stoper;
};

#endif // EXAMPLES2_HPP



