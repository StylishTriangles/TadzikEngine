#ifndef JUMPER_HPP
#define JUMPER_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>



class platform {
public:
    sf::Sprite sprite;
    platform(sf::Sprite nSprite) {sprite = nSprite;};
    bool isAbove(sf::Sprite sp2) {
        if ((sp2.getPosition().y < sprite.getPosition().y)!=playerAbove) {
            playerAbove=!playerAbove;
            return true;
        }
        return false;
    }
    bool testForStanding(sf::Sprite sp2) {
        if ((sp2.getGlobalBounds().left+sp2.getGlobalBounds().width > sprite.getPosition().x) &&
            (sp2.getGlobalBounds().left < sprite.getPosition().x+sprite.getTextureRect().width*sprite.getScale().x)) {
            return true;
        }
        return false;
    }
protected:
    bool playerAbove = false;
    bool jumpedOn = false;
    double timeTillBreak = 3;
};

class JUMPER: public Scene{
public:
    JUMPER(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory() {
        textScore.setFont(Common::Font::Comic_Sans);

        texBackground.loadFromFile("files/textures/jumper/background.png");
        background1.setTexture(texBackground);
        background2.setTexture(texBackground);
        background1.setScale((float)window->getSize().x/(float)background1.getTextureRect().width,
                             (float)window->getSize().y/(float)background1.getTextureRect().height);
        background2.setScale((float)window->getSize().x/(float)background2.getTextureRect().width,
                             (float)window->getSize().y/(float)background2.getTextureRect().height);
        background2.setPosition(0, -0.99*window->getSize().y);

        texPlayerRun1.loadFromFile("files/textures/universal/playerRun1.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun1, 500));
        texPlayerRun2.loadFromFile("files/textures/universal/playerRun2.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun2, 500));
        texPlayerRun3.loadFromFile("files/textures/universal/playerRun3.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun3, 500));
        texPlayerRun4.loadFromFile("files/textures/universal/playerRun4.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun4, 500));

        texPlayerJump.loadFromFile("files/textures/universal/playerJump.png"), TadzikJump.addFrame(AnimationFrame(&texPlayerJump, 150));
        texPlayerJumpSuper.loadFromFile("files/textures/jumper/playerJumpSuper.png"), TadzikJumpSuper.addFrame(AnimationFrame(&texPlayerJumpSuper, 150));

        texPlayerRoll.loadFromFile("files/textures/jumper/playerRoll.png"), TadzikRoll.addFrame(AnimationFrame(&texPlayerRoll, 150));

        texPlayerStand.loadFromFile("files/textures/universal/playerStand.png"), TadzikStand.addFrame(AnimationFrame(&texPlayerStand, 150));

        texPlayerFall.loadFromFile("files/textures/universal/playerFall.png"), TadzikFall.addFrame(AnimationFrame(&texPlayerFall, 150));

        spTadzik.setAnimation(&TadzikRun);
        spTadzik.sprite.setPosition(window->getSize().x/2-spTadzik.sprite.getTextureRect().width/2,
                                    window->getSize().y-spTadzik.sprite.getTextureRect().height*6);
        spTadzik.sprite.setScale(3, 3);
        spTadzik.sprite.setOrigin(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height);

        texPlatform.loadFromFile("files/textures/jumper/platform.png");
        tmpPlatform.setTexture(texPlatform);
        tmpPlatform.setPosition(0, window->getSize().y-30);
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width, 1);
        platforms.push_back(platform(tmpPlatform));
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width/3, 1);
        tmpPlatform.move(10, -(1.0/2.0)*window->getSize().y);
        platforms.push_back(platform(tmpPlatform));
        tmpPlatform.move(10, -(1.0/2.0)*window->getSize().y);
        platforms.push_back(platform(tmpPlatform));

        texPowerup1.loadFromFile("files/textures/jumper/powerup1.png");
        powerup1.setTexture(texPowerup1);
    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void jump(double speedX) {
        //std::cout << "jump\n";
        isStanding = false;
        isJumping = true;
        speedY=-speedY;
        speedY-=abs(speedX);
        if (abs(speedY)>27) {
            if (!derpMode) spTadzik.sprite.setOrigin(spTadzik.sprite.getTextureRect().width/2,
                                                     spTadzik.sprite.getTextureRect().height/2);
            spTadzik.setAnimation(&TadzikRoll);
            isRolling = true;
        }
        else if (!isSuperman) spTadzik.setAnimation(&TadzikJump);
        else spTadzik.setAnimation(&TadzikJumpSuper);
        if (abs(speedY)<20) speedY = -20;
    }
    void flip() {
        if (speedX!=0) spTadzik.sprite.setScale(-spTadzik.sprite.getScale().x, spTadzik.sprite.getScale().y);
        //std::cout << "flip\n";
    }
    void gameOver() {
        //std::cout << "gameOver\n";
        clock.restart();
        isJumping=false;
        isStanding=false;
        isSuperman=false;
        background1.setPosition(0, 0);
        background2.setPosition(0, -0.99*window->getSize().y);
        spTadzik.sprite.setPosition(window->getSize().x/2-spTadzik.sprite.getTextureRect().width/2,
                                    window->getSize().y-spTadzik.sprite.getTextureRect().height*6);
        spTadzik.sprite.setScale(abs(spTadzik.sprite.getScale().x), spTadzik.sprite.getScale().y);

        platforms.clear();
        effects.clear();
        powerups.clear();
        tmpPlatform.setPosition(0, window->getSize().y-30);
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width, 1);
        platforms.push_back(platform(tmpPlatform));
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width/3, 1);
        tmpPlatform.move(10, -(1.0/2.0)*window->getSize().y);
        platforms.push_back(platform(tmpPlatform));
        tmpPlatform.move(10, -(1.0/2.0)*window->getSize().y);
        platforms.push_back(platform(tmpPlatform));
        //onSceneLoadToMemory();
        highScore = 0;
        score = 0;
        speedX=0.01, speedY=0, prevSpeedX = 0.01;
        standingPlatformNumber=0;
        lastPlatformGenerated=0;
        scrolling = 0;
    }
    virtual void draw(sf::Time deltaTime) {
        //std::cout << powerups.size() << "\n";

        //statyczne scrollowanie
        if (clock.getElapsedTime().asSeconds()>5) {
            scrolling+=0.1;
            clock.restart();
        }
        background1.move(0, scrolling);
        background2.move(0, scrolling);
        for (unsigned int i=0; i<platforms.size(); ++i) {
            platforms[i].sprite.move(0, scrolling);
        }
        for (unsigned int i=0; i<powerups.size(); i++) {
            powerups[i].move(0, scrolling);
        }
        for (unsigned int i=0; i<effects.size(); i++) {
            effects[i].move(0, scrolling);
        }

        //sprawdzanie czy tadzik jest w planszy
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) gameOver();
        if (spTadzik.sprite.getGlobalBounds().left<0 && Utils::sgn(speedX)==-1) {
            speedX = -1.1*speedX;
            spTadzik.sprite.setPosition(spTadzik.sprite.getGlobalBounds().width/2, spTadzik.sprite.getPosition().y);
        }
        else if (spTadzik.sprite.getGlobalBounds().left+spTadzik.sprite.getGlobalBounds().width>window->getSize().x && Utils::sgn(speedX)==1) {
            speedX = -1.1*speedX;
            spTadzik.sprite.setPosition(window->getSize().x-spTadzik.sprite.getGlobalBounds().width/2, spTadzik.sprite.getPosition().y);
        }

        //input z klawiatury
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            speedX-=addSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            speedX+=addSpeed;
        }

        //przypadki
        if (isStanding) {
            if (abs(speedX)<0.001 && activeAnim!=0) {
                spTadzik.setAnimation(&TadzikStand);
                activeAnim = 0;
            }
            else if (abs(speedX)>0.001 && activeAnim!=1) {
                spTadzik.setAnimation(&TadzikRun);
                activeAnim = 1;
            }
            spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                                        platforms[standingPlatformNumber].sprite.getPosition().y);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                jump(speedX+Utils::sgn(speedX)*10);
            }
            if (!platforms[standingPlatformNumber].testForStanding(spTadzik.sprite)) isStanding = false;
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
                isStanding = false;
            }

        }
        if (isSuperman && isJumping) {
            if (goUp<0) {
                //spTadzik.sprite.setTexture(texPlayerJumpSuper);
                isSuperman = false;
            }
            else {
                speedY-=1;
                goUp--;
                spTadzik.setAnimation(&TadzikJumpSuper);
            }
        }
        if (isRolling) {
            spTadzik.sprite.setRotation(Utils::sgn(speedX)*speedY*20);
        }
        if (!isStanding) {
            speedY += gravity;
            if (speedY>0) {
                if (isJumping) spTadzik.setAnimation(&TadzikFall);
                isJumping = false;
                isRolling = false;
                spTadzik.sprite.setOrigin(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height);
                spTadzik.sprite.setRotation(0);
            }
            if (isJumping) {
                if (spTadzik.sprite.getPosition().y>window->getSize().y*(1.0/3.0)) {
                    spTadzik.sprite.move(0, speedY);
                }
                else {
                    background1.move(0, -speedY);
                    background2.move(0, -speedY);
                    for (unsigned int i=0; i<platforms.size(); ++i) {
                        platforms[i].sprite.move(0, -speedY);
                    }
                    for (unsigned int i=0; i<powerups.size(); i++) {
                        powerups[i].move(0, -speedY);
                    }
                }
            }
            else {
                spTadzik.sprite.move(0, speedY);
            }
            score-=speedY;
        }

        //sprawdzanie czy stoi na jakiejs platformie
        for (unsigned int i=0; i<platforms.size(); ++i) {
            if (platforms[i].isAbove(spTadzik.sprite)) {
                if (platforms[i].testForStanding(spTadzik.sprite) && !isJumping) {
                    //std::cout << "standing on " << standingPlatformNumber << "\n";
                    isStanding = true;
                    standingPlatformNumber = i;
                    spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                                       platforms[standingPlatformNumber].sprite.getPosition().y);
                                       spTadzik.setAnimation(&TadzikRun);
                    speedY=0;
                }
            }
        }

        //obracanie
        if (Utils::sgn(prevSpeedX)!=Utils::sgn(speedX)) {
            flip();
            prevSpeedX=speedX;
        }

        //naprawianie tla
        if (background1.getPosition().y>window->getSize().y) {
            background1.move(0, -1.98*window->getSize().y);
        }
        if (background2.getPosition().y>window->getSize().y) {
            background2.move(0, -1.98*window->getSize().y);
        }

        //dodawanie platform i powerupow
        if (score - lastPlatformGenerated > window->getSize().y/2) {
            if (Utils::randFloat(0, 50)<10) {
                powerup1.setPosition(Utils::randFloat(10, window->getSize().x-10), Utils::randFloat(-100, -50));
                powerup1.setScale(3, 3);
                powerups.push_back(powerup1);
            }
            int tmp = Utils::randFloat(-100, -50);
            lastPlatformGenerated = score + tmp;
            tmpPlatform.setPosition(Utils::randFloat(0, window->getSize().x-tmpPlatform.getTextureRect().width), tmp);
            tmpPlatform.setScale(Utils::randFloat(1, 3), 1);
            platforms.push_back(platform(tmpPlatform));
        }

        //glupoty
        spTadzik.sprite.move(speedX, 0);
        speedX*=(1-airResistance);
        spTadzik.update(abs(speedX)*deltaTime.asMilliseconds());
        if (score>highScore) highScore=score;
        textScore.setString(Utils::stringify(highScore/10));

        //rysowanie
        window->clear();
        window->draw(background1);
        window->draw(background2);
        window->draw(textScore);


        //ogarnianie objektow
        for (int i=platforms.size()-1; i>=0; --i) {
            if (platforms[i].sprite.getPosition().y>window->getSize().y+500) {
                platforms.erase(platforms.begin()+i);
            }
            window->draw(platforms[i].sprite);
        }
        for (int i=powerups.size()-1; i>=0; --i) {
            if (Collision::PixelPerfectTest(powerups[i], spTadzik.sprite)) {
                isSuperman = true;
                goUp = 40;
                powerups.erase(powerups.begin()+i);
            }
            if (powerups[i].getPosition().y>window->getSize().y+500) {
                powerups.erase(powerups.begin()+i);
            }
            if(powerups.size() > 0)
                window->draw(powerups[i]);
        }
        window->draw(spTadzik.sprite);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlatform;
    sf::Texture texPowerup1;

    sf::Texture texPlayerRun1;
    sf::Texture texPlayerRun2;
    sf::Texture texPlayerRun3;
    sf::Texture texPlayerRun4;
    sf::Texture texPlayerRoll;
    sf::Texture texPlayerJump;
    sf::Texture texPlayerJumpSuper;
    sf::Texture texPlayerStand;
    sf::Texture texPlayerFall;

    sf::Text textScore;

    sf::Sprite background1;
    sf::Sprite background2;
    sf::Sprite tmpPlatform;
    sf::Sprite powerup1;

    AnimatedSprite spTadzik;
    Animation TadzikRun, TadzikStand, TadzikRoll, TadzikJump, TadzikJumpSuper, TadzikFall;

    double speedX = 0.01;
    double prevSpeedX = 0.01;
    double scrolling = 0;
    double speedY = 0;
    double gravity = 0.5;
    double addSpeed = 1;
    double airResistance = 0.05;
    double lastPlatformGenerated = 0;
    double goUp = 0;

    int standingPlatformNumber = 0;
    int activeAnim = 0;  //stand = 0, run = 1, jump = 2

    std::vector <platform> platforms;
    std::vector <sf::Sprite> powerups;
    std::vector <sf::Sprite> effects;
    bool isJumping = false;
    bool isStanding = false;
    bool isRolling = false;
    bool isSuperman = false;
    bool derpMode = false;

    double score = 0;
    double highScore = 0;

    sf::Clock clock;

};
#endif //JUMPER_HPP
