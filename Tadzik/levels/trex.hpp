#ifndef TREX_HPP
#define TREX_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

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

        animRunDuckTexture1.loadFromFile("files/textures/trex/trexDuck0.png");
        animRunDuckTexture2.loadFromFile("files/textures/trex/trexDuck1.png");

        texCactus.loadFromFile("files/textures/trex/cactus.png");

        if (!font.loadFromFile("files/textures/trex/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }

        textScore.setFont(font);

        animTadzikRun.addFrame(AnimationFrame(&animRunTexture1, 150));
        animTadzikRun.addFrame(AnimationFrame(&animRunTexture2, 150));

        animVultureFly.addFrame(AnimationFrame(&animVultureFlyTexture1, 150));
        animVultureFly.addFrame(AnimationFrame(&animVultureFlyTexture2, 150));

        animTadzikDuck.addFrame(AnimationFrame(&animRunDuckTexture1, 150));
        animTadzikDuck.addFrame(AnimationFrame(&animRunDuckTexture2, 150));
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~TREX");
        spTadzik.setAnimation(&animTadzikRun);
        spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
        spTadzik.sprite.setScale(scaleFactor, scaleFactor);
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::KeyPressed && !isDucking && !gameOver)
        {
            if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down){
                isDucking=true;
                spTadzik.setAnimation(&animTadzikDuck);
                spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
            }
        }
        if (event.type == sf::Event::KeyReleased && isDucking && !gameOver)
        {
            if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down){
                isDucking=false;
                spTadzik.setAnimation(&animTadzikRun);
                spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
            }
        }

    }

    virtual void draw(double deltaTime){
        if(gameOver){
            textScore.setString("press space to play again");
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                spTadzik.setAnimation(&animTadzikRun);
                spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
                gameOver=false;
                vecCactus.clear();
                vecVulture.clear();
                result=0;
                isJumping=false;
                isDucking=false;
            }
        }
        else{
            spTadzik.sprite.move(0, deltaTime * -speedY * scaleFactor);
            spTadzik.update(deltaTime);
            double critHeight = window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY;
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

            textScore.setString("score: " + stringify((int)result));

            result += speedX/10.0;
        }
        window->clear();
        for(sf::Sprite& sp : vecCactus){
            window->draw(sp);
        }
        for(AnimatedSprite& sp : vecVulture){
            sp.update(deltaTime);
            window->draw(sp.sprite);
        }
        if (true) // borderbox tadzika
        {
            sf::RectangleShape shp(sf::Vector2f(spTadzik.sprite.getGlobalBounds().width, spTadzik.sprite.getGlobalBounds().height));
            shp.setPosition(sf::Vector2f(spTadzik.sprite.getGlobalBounds().left, spTadzik.sprite.getGlobalBounds().top));
            shp.setOutlineColor(sf::Color::Green);
            shp.setFillColor(sf::Color(0,0,0,0));
            shp.setOutlineThickness(1);
            window->draw(shp);
            window->draw(spTadzik.sprite);
            window->draw(textScore);
        }
    }

    std::string stringify(int x){
        std::ostringstream o;
        o << x;
        return o.str();
    }

    void jump(){
        isJumping=true;
        speedY=0.5;
        spTadzik.sprite.move(0, -10);
    }

    void manageObstacles(){
        if (result - lastObstacle > minObstacleInterval){
            obstacleChance+=0.01;
            if (obstacleChance > (double)rand()/RAND_MAX)
            {
                lastObstacle = result;
                obstacleChance-=(double)rand()/RAND_MAX*0.8+0.2;
                if ((double)rand()/RAND_MAX<0.66){
                    sf::Sprite spTmp(texCactus);
                    spTmp.setPosition(0, 0);
                    spTmp.setScale(scaleFactor*(0.4+((double)rand()/RAND_MAX)*0.1), scaleFactor*(0.4+((double)rand()/RAND_MAX)*0.1));
                    spTmp.setPosition(window->getSize().x+50, window->getSize().y-spTmp.getGlobalBounds().height-offsetY);
                    vecCactus.push_back(spTmp);
                }
                else {
                    AnimatedSprite spTmp;
                    spTmp.setAnimation(&animVultureFly);
                    spTmp.sprite.setPosition(0, 0);
                    spTmp.sprite.setScale(scaleFactor*(0.4+((double)rand()/RAND_MAX)*0.1), scaleFactor*(0.4+((double)rand()/RAND_MAX)*0.1));
                    spTmp.sprite.setPosition(window->getSize().x+50, window->getSize().y-spTmp.sprite.getGlobalBounds().height-offsetY-(double)rand()/RAND_MAX*100);
                    vecVulture.push_back(spTmp);
                }
            }
        }
        if (result - lastObstacle >= maxObstacleInterval) {
            obstacleChance = 1;
        }
        for(int i = vecCactus.size()-1; i >= 0; i--){
            if(vecCactus[i].getGlobalBounds().left < -100){
                vecCactus.erase(vecCactus.begin()+i);
            }
        }

        for(int i = vecCactus.size()-1; i >= 0; i--){
            vecCactus[i].move(-speedX*scaleFactor, 0);
            if(Collision::PixelPerfectTest(spTadzik.sprite, vecCactus[i])) {
                gameOver=true;
            }
        }

        for(int i = vecVulture.size()-1; i >= 0; i--){
            if(vecVulture[i].sprite.getGlobalBounds().left < -100){
                vecVulture.erase(vecVulture.begin()+i);
            }
        }

        for(int i = vecVulture.size()-1; i >= 0; i--){
            vecVulture[i].sprite.move(-speedX*scaleFactor, 0);
            if(Collision::PixelPerfectTest(spTadzik.sprite, vecVulture[i].sprite)) {
                gameOver=true;
            }
        }
    }

protected:
    Animation animTadzikRun, animVultureFly, animTadzikDuck;
    sf::Texture animRunTexture1, animRunTexture2;
    sf::Texture animVultureFlyTexture1, animVultureFlyTexture2;
    sf::Texture animRunDuckTexture1, animRunDuckTexture2;
    sf::Texture texCactus;

    AnimatedSprite spTadzik;

    double speedX=5, speedY=0.0;

    double offsetX = 32, offsetY=4;
    bool isJumping=false;
    double g = 0.03;

    double result=0;

    std::vector<sf::Sprite> vecCactus;
    std::vector<AnimatedSprite> vecVulture;
    sf::Font font;
    sf::Text textScore;
    bool gameOver=false;
    bool isDucking=false;
    double scaleFactor=2.0;

    double obstacleChance = 1;
    int minObstacleInterval = 20, maxObstacleInterval = 100, lastObstacle = 0;

};

#endif // TREX_HPP




