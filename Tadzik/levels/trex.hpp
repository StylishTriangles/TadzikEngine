#ifndef TREX_HPP
#define TREX_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"

#include <string>

class TREX: public Scene{
public:
    TREX(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory(){
        animRunTexture1.loadFromFile("files/textures/trex/trexRun0.png");
        animRunTexture2.loadFromFile("files/textures/trex/trexRun1.png");
        animTadzikRun.addFrame(AnimationFrame(&animRunTexture1, 300));
        animTadzikRun.addFrame(AnimationFrame(&animRunTexture2, 300));
        spTadzik.setAnimation(&animTadzikRun);
    }

    virtual void onSceneActivate(){
        std::cout << "example_animated_sprite::onSceneActivate()\n";
        window->setTitle("Tadzik ~TREX");
    }

    virtual void draw(double deltaTime){
        spTadzik.update(deltaTime);

        window->clear();
        window->draw(spTadzik.sprite);
    }

protected:
    Animation animTadzikRun;
    sf::Texture animRunTexture1, animRunTexture2;

    AnimatedSprite spTadzik;
};

#endif // TREX_HPP




