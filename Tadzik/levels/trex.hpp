#ifndef TREX_HPP
#define TREX_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

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

        animRunDuckTexture1.loadFromFile("files/textures/trex/trexDuck0.png");
        animRunDuckTexture2.loadFromFile("files/textures/trex/trexDuck1.png");

        sf::Sprite tmp1;
        texBackground.loadFromFile("files/textures/trex/background.png");
        tmp1.setTexture(texBackground);
        tmp1.setScale(sf::Vector2f((float)window->getSize().x/(float)texBackground.getSize().x*1.1,
                                   (float)window->getSize().y/(float)texBackground.getSize().y));
        background.push_back(tmp1);
        tmp1.setPosition(window->getSize().x, 0);
        background.push_back(tmp1);

        sf::Texture tmp;
        tmp.loadFromFile("files/textures/trex/cactus0.png"), texCactus.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cactus1.png"), texCactus.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cactus2.png"), texCactus.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cactus3.png"), texCactus.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cactus4.png"), texCactus.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cactus5.png"), texCactus.push_back(tmp);

        tmp.loadFromFile("files/textures/trex/cloud0.png"), texCloud.push_back(tmp);
        tmp.loadFromFile("files/textures/trex/cloud1.png"), texCloud.push_back(tmp);

        textScore.setFont(Common::Font::Carnevalee_Freakshow);

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
        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::S || event.key.code == sf::Keyboard::Down){
                if (isDucking) {
                    isDucking = false;
                    if (!gameOver) spTadzik.setAnimation(&animTadzikRun);
                }
                quickFall = false;
            }
        }
    }

    virtual void draw(sf::Time deltaTime){
        if(gameOver){
            std::cout << result << std::endl;
            textScore.setString("Your score: "+Utils::stringify((int)result)+"\nPress space to play again");
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space)){
                spTadzik.setAnimation(&animTadzikRun);
                spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
                gameOver=false;
                vecCactus.clear();
                vecVulture.clear();
                result=0;
                isJumping=false;
                isDucking=false;
                obstacleChance=1;
                speedX = 5.0;
                lastObstacle = 0;
            }
        }
        else{
            spTadzik.sprite.move(0, deltaTime.asMilliseconds() * -speedY * scaleFactor);
            spTadzik.update(deltaTime.asMilliseconds());
            if ((int)result%100==0) speedX+=0.1;
            double critHeight = window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY;
            if(spTadzik.sprite.getGlobalBounds().top < critHeight) {
                if (quickFall) speedY-=3*g;
                else speedY-=g;
            }
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
                if (!isDucking)
                {
                    if (isJumping) quickFall = true;
                    else {
                        isDucking = true;
                        spTadzik.setAnimation(&animTadzikDuck);
                        spTadzik.sprite.setPosition(offsetX, window->getSize().y-spTadzik.sprite.getGlobalBounds().height-offsetY);
                    }
                }
            }

            manageObjects();

            textScore.setString("score: " + Utils::stringify((int)result));

            result += speedX/10.0;
        }
        window->clear(sf::Color::Black); //kolor okna
        window->draw(background[0]);
        window->draw(background[1]);
        for(sf::Sprite& sp : vecCactus){
            if(debug)
                Utils::drawBoundingBox(sp, window);
            window->draw(sp);
        }
        for(AnimatedSprite& sp : vecVulture){
            sp.update(deltaTime.asMilliseconds());
            if(debug)
                Utils::drawBoundingBox(sp.sprite, window);
            window->draw(sp.sprite);
        }
        for(sf::Sprite& sp : vecCloud) {
            if(debug)
                Utils::drawBoundingBox(sp, window);
            window->draw(sp);
        }
        if (debug) // boundingbox tadzika
        {
            Utils::drawBoundingBox(spTadzik.sprite, window);
        }
        window->draw(spTadzik.sprite);
        window->draw(textScore);
    }

    void jump(){
        isJumping=true;
        speedY=0.5;
        spTadzik.sprite.move(0, -10);
    }

    void manageObjects(){
        if (result - lastObstacle > minObstacleInterval){
            obstacleChance+=0.01;
            if (obstacleChance > Utils::randFloat(0, 1))
            {
                lastObstacle = result;
                obstacleChance-=Utils::randFloat(0.2, 1);
                if (Utils::randFloat(0, 1)<0.5){
                    for (int i = 0; i<rand()%3+1; i++) {
                        sf::Sprite spTmp(texCactus[rand()%6]);
                        spTmp.setScale(scaleFactor*(Utils::randFloat(0.6, 0.8)), scaleFactor*(Utils::randFloat(0.6, 0.8)));
                        spTmp.setPosition(window->getSize().x+50+i*10*scaleFactor, window->getSize().y-spTmp.getGlobalBounds().height-offsetY);
                        vecCactus.push_back(spTmp);
                    }
                }
                else {
                    AnimatedSprite spTmp;
                    spTmp.setAnimation(&animVultureFly);
                    spTmp.sprite.setScale(scaleFactor*Utils::randFloat(0.4, 0.5), scaleFactor*Utils::randFloat(0.4, 0.5));
                    spTmp.sprite.setPosition(window->getSize().x+50,
                                             window->getSize().y-spTmp.sprite.getGlobalBounds().height-offsetY-Utils::randFloat(0, 200));
                    vecVulture.push_back(spTmp);
                }
            }
        }

        if (result>nextCloud) {
            lastCloud = result;
            nextCloud += Utils::randFloat(75, 150);
            sf::Sprite tmp(texCloud[rand()%texCloud.size()]);
            tmp.setScale(scaleFactor*Utils::randFloat(1, 2), scaleFactor*Utils::randFloat(1, 2));
            tmp.setPosition(window->getSize().x+50+Utils::randFloat(0, 1)*10*scaleFactor, 75);
            vecCloud.push_back(tmp);
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

        for(int i=vecCloud.size()-1; i>=0; i--) {
            if(vecCloud[i].getGlobalBounds().left <-400) {
                vecCloud.erase(vecCloud.begin()+i);
            }
        }
        for(int i=vecCloud.size()-1; i>=0; i--) {
            vecCloud[i].move(-speedX*scaleFactor*parallax1, 0);
        }

        background[0].move(-speedX*scaleFactor*parallax2, 0);
        background[1].move(-speedX*scaleFactor*parallax2, 0);
        if(background[1].getGlobalBounds().left<0) {
            std::swap(background[0], background[1]);
            background[1].move(window->getSize().x, 0);
        }
    }

protected:
    Animation animTadzikRun, animVultureFly, animTadzikDuck;
    sf::Texture animRunTexture1, animRunTexture2;
    sf::Texture animVultureFlyTexture1, animVultureFlyTexture2;
    sf::Texture animRunDuckTexture1, animRunDuckTexture2;
    sf::Texture texBackground;
    std::vector <sf::Texture> texCactus;
    std::vector <sf::Texture> texCloud;

    std::vector<sf::Sprite> vecCactus;
    std::vector<AnimatedSprite> vecVulture;
    std::vector<sf::Sprite> vecCloud;
    std::vector<sf::Sprite> background;

    AnimatedSprite spTadzik;

    double speedX=5, speedY=0.0;
    double offsetX = 32, offsetY=4;
    double scaleFactor = 3.0;
    double parallax1 = 0.2, parallax2 = 0.1;
    double obstacleChance = 1;
    double g = 0.03;
    double result=0;

    bool isJumping=false;
    bool isDucking=false;
    bool gameOver=false;
    bool quickFall=false;

    sf::Text textScore;

    int minObstacleInterval = 20, maxObstacleInterval = 100, lastObstacle = 0;
    int lastCloud = 0, nextCloud = 100;
    bool debug=false;

};

#endif // TREX_HPP




