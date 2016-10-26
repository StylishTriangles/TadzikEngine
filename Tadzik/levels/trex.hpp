#ifndef TREX_HPP
#define TREX_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

class TREX: public Scene{
public:
    TREX(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory(){
        animRunTexture1.loadFromFile("files/textures/trex/trexRun0.png");
        animRunTexture2.loadFromFile("files/textures/trex/trexRun1.png");

        animVultureFlyTexture1.loadFromFile("files/textures/trex/vultureFly0.png");
        animVultureFlyTexture2.loadFromFile("files/textures/trex/vultureFly1.png");

        texCactus.loadFromFile("files/textures/trex/cactus.png");

        if (!font.loadFromFile("files/textures/trex/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }

        animTadzikRun.addFrame(AnimationFrame(&animRunTexture1, 150));
        animTadzikRun.addFrame(AnimationFrame(&animRunTexture2, 150));

        animVultureFly.addFrame(AnimationFrame(&animVultureFlyTexture1, 150));
        animVultureFly.addFrame(AnimationFrame(&animVultureFlyTexture2, 150));
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~TREX");
        spTadzik.setAnimation(&animTadzikRun);
        spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
    }

    virtual void draw(double deltaTime){
        spTadzik.sprite.move(0, deltaTime * -speedY);
        spTadzik.update(deltaTime);
        double critHeight = window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY;
        //std::cout << spTadzik.sprite.getGlobalBounds().top << " " << critHeight << "\n";
        if(spTadzik.sprite.getGlobalBounds().top < critHeight)speedY-=g;
        if(spTadzik.sprite.getGlobalBounds().top > critHeight){
            speedY=0;
            spTadzik.sprite.setPosition(offsetX, critHeight);
            isJumping=false;
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)
           || sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
            if(!isJumping){
                jump();
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
            //schylanie
        }

        manageObstacles();

        window->clear();
        for(sf::Sprite& sp : vecCactus){
            window->draw(sp);
        }
        window->draw(spTadzik.sprite);
    }


    void jump(){
        isJumping=true;
        speedY=0.5;
        spTadzik.sprite.move(0, -10);
    }

    void manageObstacles(){
        double addCactusChance = (double)rand()/RAND_MAX;
        if(addCactusChance <= 0.01){
            sf::Sprite spTmp(texCactus);
            spTmp.setPosition(0, 0);
            spTmp.setScale(0.4+((double)rand()/RAND_MAX)*0.1, 0.4+((double)rand()/RAND_MAX)*0.1);
            spTmp.setPosition(window->getSize().x+50, window->getSize().y-spTmp.getGlobalBounds().height-offsetY);
            vecCactus.push_back(spTmp);
        }

        for(int i = vecCactus.size()-1; i >= 0; i--){
            if(vecCactus[i].getGlobalBounds().left < -100){
                vecCactus.erase(vecCactus.begin()+i);
            }
        }

        for(int i = vecCactus.size()-1; i >= 0; i--){
            vecCactus[i].move(-speedX, 0);
        }
    }

protected:
    Animation animTadzikRun, animVultureFly;
    sf::Texture animRunTexture1, animRunTexture2;
    sf::Texture animVultureFlyTexture1, animVultureFlyTexture2;
    sf::Texture texCactus;

    AnimatedSprite spTadzik;

    double speedX=3, speedY=0.0;

    double offsetX = 32, offsetY=4;
    bool isJumping=false;
    double g = 0.03;

    std::vector<sf::Sprite> vecCactus;
    sf::Font font;
};

#endif // TREX_HPP




