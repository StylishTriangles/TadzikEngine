#ifndef EXAMPLES1
#define EXAMPLES1

#include "Scene.hpp"
#include "AnimatedSprite.hpp"

#include <string>

class ExampleScene1: public Scene{
public:
    ExampleScene1(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {
        std::cout << "Constructor of ExampleScene1\n";
        x = window->getSize().x / 2.0f;
        y = window->getSize().y / 2.0f;
    }

    virtual ~ExampleScene1(){
        std::cout << "Destructor of ExampleScene1\n";
    }

    virtual void onSceneActivate(){
        std::cout << "ExampleScene1::onSceneActivate()\n";
        window->setTitle("Tadzik ~Scene1");
    }

    void example1Logic(double deltaTime){
        deltaTime *= 1000;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){
            x-=deltaTime*100.0;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){
            x+=deltaTime*100.0;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){
            y-=deltaTime*100.0;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){
            y+=deltaTime*100.0;
        }
    }

    virtual void draw(double deltaTime){
        example1Logic(deltaTime);

        window->clear();
        sf::CircleShape shape(std::sin(stoper.getElapsedTime().asSeconds()) * window->getSize().y / 8 + window->getSize().y / 4);
        shape.setOrigin(sf::Vector2f( shape.getRadius(), shape.getRadius()));
        shape.setPosition(x, y);
        shape.setFillColor(sf::Color::Yellow);
        window->draw(shape);
    }
    virtual void deliverEvent(sf::Event& e){
        if(e.type == sf::Event::KeyPressed && e.key.code == sf::Keyboard::N) {
            sceneManager->setActiveScene("Example2");
        }
    }
    virtual void onSceneDeactivate(){
        std::cout << "ExampleScene1::onSceneDeactivate()\n";
    }
protected:
    sf::Clock stoper;
    float x, y;
};

#endif // EXAMPLES1


