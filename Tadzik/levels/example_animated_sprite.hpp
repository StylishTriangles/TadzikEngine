#ifndef EXAMPLE_ANIMATED_SPRITE_HPP
#define EXAMPLE_ANIMATED_SPRITE_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"

#include <string>

class AnimatedSpriteScene: public Scene{
public:
    AnimatedSpriteScene(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory(){
        std::cout << "Constructor of example_animated_sprite\n";
        t1.loadFromFile("files/textures/test_anim/frame1.jpg");
        t2.loadFromFile("files/textures/test_anim/frame2.jpg");
        t3.loadFromFile("files/textures/test_anim/frame3.jpg");
        t4.loadFromFile("files/textures/test_anim/frame4.jpg");
        anim1.addFrame(AnimationFrame(&t1, 15));
        anim1.addFrame(AnimationFrame(&t2, 15));
        anim1.addFrame(AnimationFrame(&t3, 15));
        anim1.addFrame(AnimationFrame(&t4, 15));

        sp1.setAnimation(&anim1);
    }

    virtual void onSceneActivate(){
        std::cout << "example_animated_sprite::onSceneActivate()\n";
        window->setTitle("Tadzik ~example_animated_sprite");
    }

    void example1Logic(double deltaTime){
        sp1.update(deltaTime);
    }

    virtual void draw(double deltaTime){
        example1Logic(deltaTime);

        window->clear();
        window->draw(sp1.sprite);
    }

protected:
    sf::Texture t1, t2, t3, t4;

    AnimatedSprite sp1;
    Animation anim1;
};

#endif // EXAMPLE_ANIMATED_SPRITE_HPP



