#ifndef JUMPER_HPP
#define JUMPER_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

int sgn(double x) {
    if (x==0) return 0;
    if (x>0) return 1;
    if (x<0) return -1;
}
//int abs(double x) {
//    return (x>=0)*x;
//}

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
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
        textScore.setFont(font);

        texBackground.loadFromFile("files/textures/jumper/background.png");
        background1.setTexture(texBackground);
        background2.setTexture(texBackground);
        background1.setScale((float)window->getSize().x/(float)background1.getTextureRect().width,
                             (float)window->getSize().y/(float)background1.getTextureRect().height);
        background2.setScale((float)window->getSize().x/(float)background2.getTextureRect().width,
                             (float)window->getSize().y/(float)background2.getTextureRect().height);
        background2.setPosition(0, -0.99*window->getSize().y);

        texPlayerRun1.loadFromFile("files/textures/jumper/playerRun1.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun1, 500));
        texPlayerRun2.loadFromFile("files/textures/jumper/playerRun2.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun2, 500));
        texPlayerRun3.loadFromFile("files/textures/jumper/playerRun3.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun3, 500));
        texPlayerRun4.loadFromFile("files/textures/jumper/playerRun4.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun4, 500));

        texPlayerJump.loadFromFile("files/textures/jumper/playerJump.png"), TadzikJump.addFrame(AnimationFrame(&texPlayerJump, 150));

        texPlayerRoll.loadFromFile("files/textures/jumper/playerRoll.png");

        texPlayerStand.loadFromFile("files/textures/jumper/playerStand.png"), TadzikStand.addFrame(AnimationFrame(&texPlayerStand, 150));

        spTadzik.setAnimation(&TadzikRun);
        //spTadzik.sprite.setTexture(texPlayerRun);
        spTadzik.sprite.setPosition(window->getSize().x/2-spTadzik.sprite.getTextureRect().width/2,
                                    window->getSize().y-spTadzik.sprite.getTextureRect().height*6);
        spTadzik.sprite.setScale(3, 3);
        spTadzik.sprite.setOrigin(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height);
        /*
        player.setTexture(texPlayerRun);
        player.setPosition(window->getSize().x/2-player.getTextureRect().width/2, window->getSize().y-player.getTextureRect().height*6);
        player.setScale(3, 3);
        player.setOrigin(player.getTextureRect().width/2, player.getTextureRect().height);
        */
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
    }
    void flip() {
        if (speedX!=0) spTadzik.sprite.setScale(-spTadzik.sprite.getScale().x, spTadzik.sprite.getScale().y);
        //std::cout << "flip\n";
    }
    void gameOver() {
        //std::cout << "gameOver\n";
        clock.restart();
        scrolling = 0;
        isJumping=false;
        isStanding=false;
        background1.setPosition(0, 0);
        background2.setPosition(0, -0.99*window->getSize().y);
        spTadzik.sprite.setPosition(window->getSize().x/2-spTadzik.sprite.getTextureRect().width/2, window->getSize().y-spTadzik.sprite.getTextureRect().height*6);
        spTadzik.sprite.setScale(abs(spTadzik.sprite.getScale().x), spTadzik.sprite.getScale().y);

        platforms.clear();
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
        speedX=sgn(speedX)*0.01, speedY=0;
        standingPlatformNumber=0;
        lastPlatformGenerated=0;
    }
    virtual void draw(double deltaTime) {
        std::cout << deltaTime << "\n";
        spTadzik.update(abs(speedX)*deltaTime);
        {
            if (clock.getElapsedTime().asSeconds()>5) {
                scrolling+=0.1;
                clock.restart();
            }
            background1.move(0, scrolling);
            background2.move(0, scrolling);
            for (int i=0; i<platforms.size(); ++i) {
                platforms[i].sprite.move(0, scrolling);
            }
        }
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) gameOver();
        if (spTadzik.sprite.getGlobalBounds().left<0 && sgn(speedX)==-1) {
            speedX = -0.66*speedX;
            spTadzik.sprite.setPosition(spTadzik.sprite.getGlobalBounds().width/2, spTadzik.sprite.getPosition().y);
        }
        else if (spTadzik.sprite.getGlobalBounds().left+spTadzik.sprite.getGlobalBounds().width>window->getSize().x && sgn(speedX)==1) {
            speedX = -0.66*speedX;
            spTadzik.sprite.setPosition(window->getSize().x-spTadzik.sprite.getGlobalBounds().width/2, spTadzik.sprite.getPosition().y);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            speedX-=addSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            speedX+=addSpeed;
        }
        if (isStanding) {
            spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                               platforms[standingPlatformNumber].sprite.getPosition().y);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
                jump(speedX+sgn(speedX)*10);
                spTadzik.setAnimation(&TadzikJump);
            }
            if (!platforms[standingPlatformNumber].testForStanding(spTadzik.sprite)) isStanding = false;

        }
        if (!isStanding) {
            speedY += gravity;
            if (speedY>0) {
                if (isJumping) spTadzik.setAnimation(&TadzikRun);
                isJumping = false;
            }
            if (isJumping) {
                if (spTadzik.sprite.getPosition().y>window->getSize().y*(1.0/3.0)) {
                    spTadzik.sprite.move(0, speedY);
                }
                else {
                    background1.move(0, -speedY);
                    background2.move(0, -speedY);
                    for (int i=0; i<platforms.size(); ++i) {
                        platforms[i].sprite.move(0, -speedY);
                    }
                }
            }
            else {
                spTadzik.sprite.move(0, speedY);
            }
            score-=speedY;
        }
        for (int i=0; i<platforms.size(); ++i) {
            if (platforms[i].isAbove(spTadzik.sprite)) {
                if (platforms[i].testForStanding(spTadzik.sprite) && !isJumping) {
                    //std::cout << "standing on " << standingPlatformNumber << "\n";
                    isStanding = true;
                    standingPlatformNumber = i;
                    spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                                       platforms[standingPlatformNumber].sprite.getPosition().y);
                    speedY=0;
                }
            }

        }

        spTadzik.sprite.move(speedX, 0);
        speedX*=(1-airResistance);
        if (sgn(prevSpeedX)!=sgn(speedX)) {
            flip();
            prevSpeedX=speedX;
        }
        if (background1.getPosition().y>window->getSize().y) {
            background1.move(0, -1.98*window->getSize().y);
        }
        if (background2.getPosition().y>window->getSize().y) {
            background2.move(0, -1.98*window->getSize().y);
        }

        //std::cout << score/100 << std::endl;

        if (score - lastPlatformGenerated > window->getSize().y/2) {
            int tmp = Utils::randF(-100, -50);
            lastPlatformGenerated = score + tmp;
            tmpPlatform.setPosition(Utils::randF(0, window->getSize().x-tmpPlatform.getTextureRect().width), tmp);
            tmpPlatform.setScale(Utils::randF(1, 3), 1);
            platforms.push_back(platform(tmpPlatform));
        }
        if (score>highScore) highScore=score;
        textScore.setString(Utils::stringify(highScore/10));
        window->clear();
        window->draw(background1);
        window->draw(background2);
        window->draw(textScore);
        for (int i=0; i<platforms.size(); ++i) {
            if (platforms[i].sprite.getPosition().y>window->getSize().y+500) {
                platforms.erase(platforms.begin()+i);
            }
            window->draw(platforms[i].sprite);
        }
        if (isStanding) {
            if (abs(speedX)<0.001 && activeAnim!=0) {
                spTadzik.setAnimation(&TadzikStand);
                activeAnim = 0;
            }
            else if (abs(speedX)>0.001 && activeAnim!=1) {
                spTadzik.setAnimation(&TadzikRun);
                activeAnim = 1;
            }
        }
        window->draw(spTadzik.sprite);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlatform;
    sf::Texture texPlayerRun1;
    sf::Texture texPlayerRun2;
    sf::Texture texPlayerRun3;
    sf::Texture texPlayerRun4;
    sf::Texture texPlayerRoll;
    sf::Texture texPlayerJump;
    sf::Texture texPlayerStand;

    sf::Text textScore;

    sf::Font font;

    sf::Sprite background1;
    sf::Sprite background2;
    //sf::Sprite player;
    sf::Sprite tmpPlatform;

    AnimatedSprite spTadzik;
    Animation TadzikRun, TadzikStand, TadzikRoll, TadzikJump;

    double speedX = 0.01;
    double prevSpeedX = 0.01;
    double scrolling = 0;
    double speedY = 0;
    double gravity = 0.5;
    double addSpeed = 1;
    double airResistance = 0.05;
    double lastPlatformGenerated = 0;

    //double standingHeight = 0;
    int standingPlatformNumber = 0;
    int activeAnim = 0;  //stand = 0, run = 1, jump = 2

    std::vector <platform> platforms;
    bool isJumping = false;
    bool isStanding = false;

    double score = 0;
    double highScore = 0;

    sf::Clock clock;

};
#endif //JUMPER_HPP
