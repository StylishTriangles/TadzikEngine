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

int znak(double x) {
    if (x==0) return 0;
    if (x>0) return 1;
    if (x<0) return -1;
}

class platform {
public:
    sf::Sprite sprite;
    platform(sf::Sprite nSprite) {sprite = nSprite;};
    bool isAbove(sf::Sprite sp2) {
        if ((sp2.getPosition().y+sp2.getTextureRect().height*sp2.getScale().y < sprite.getPosition().y)!=playerAbove) {
            playerAbove=!playerAbove;
            return true;
        }
        return false;
    }
    bool testForStanding(sf::Sprite sp2) {
        if ((sp2.getPosition().x+sp2.getTextureRect().width*sp2.getScale().x > sprite.getPosition().x) &&
            (sp2.getPosition().x < sprite.getPosition().x+sprite.getTextureRect().width*sprite.getScale().x)) {
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
        texBackground.loadFromFile("files/textures/jumper/background.png");
        background1.setTexture(texBackground);
        background2.setTexture(texBackground);
        background1.setScale((float)window->getSize().x/(float)background1.getTextureRect().width,
                             (float)window->getSize().y/(float)background1.getTextureRect().height);
        background2.setScale((float)window->getSize().x/(float)background2.getTextureRect().width,
                             (float)window->getSize().y/(float)background2.getTextureRect().height);
        background2.setPosition(0, -0.99*window->getSize().y);

        texPlayer.loadFromFile("files/textures/jumper/player.png");
        player.setTexture(texPlayer);
        player.setPosition(window->getSize().x/2-player.getTextureRect().width/2, window->getSize().y-player.getTextureRect().height*6);
        player.setScale(3, 3);

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
        isStanding = false;
        isJumping = true;
        speedY=-1.5*speedY;
        speedY-=abs(speedX);
    }

    void gameOver() {
        window->close();
        std::cin.ignore();
    }
    virtual void draw(double deltaTime) {
        if (player.getPosition().y>window->getSize().y) gameOver();
        if (player.getPosition().x<0 && znak(speedX)==-1) {
            speedX = -0.66*speedX;
            player.setPosition(0, player.getPosition().y);
        }
        if (player.getPosition().x>window->getSize().x-player.getTextureRect().width*player.getScale().x && znak(speedX)==1) {
            speedX = -0.66*speedX;
            player.setPosition(window->getSize().x-player.getTextureRect().width, player.getPosition().y);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            speedX-=addSpeed;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            speedX+=addSpeed;
        }
        if (isStanding) {
            player.setPosition(player.getPosition().x,
                               platforms[standingPlatformNumber].sprite.getPosition().y-player.getTextureRect().height*player.getScale().y);
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
                jump(speedX+znak(speedX)*10);
            }
            if (!platforms[standingPlatformNumber].testForStanding(player)) isStanding = false;

        }
        if (!isStanding) {
            speedY += gravity;
            if (speedY>0) {
                isJumping = false;
            }
            if (isJumping) {
                if (player.getPosition().y>window->getSize().y*(1.0/4.0)) {
                    player.move(0, speedY);
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
                player.move(0, speedY);
            }
            score-=speedY;
        }
        for (int i=0; i<platforms.size(); ++i) {
            if (platforms[i].isAbove(player)) {
                if (platforms[i].testForStanding(player) && !isJumping) {
                    isStanding = true;
                    standingPlatformNumber = i;
                    player.setPosition(player.getPosition().x,
                                       platforms[standingPlatformNumber].sprite.getPosition().y-player.getTextureRect().height*player.getScale().y);
                }
            }

        }

        player.move(speedX, 0);
        speedX*=(1-airResistance);
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
            platforms.push_back(platform(tmpPlatform));
        }
        window->clear();
        window->draw(background1);
        window->draw(background2);
        for (int i=0; i<platforms.size(); ++i) {
            if (platforms[i].sprite.getPosition().y>window->getSize().y+100) {
                platforms.erase(platforms.begin()+i);
            }
            window->draw(platforms[i].sprite);
        }
        window->draw(player);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlatform;
    sf::Texture texPlayer;

    sf::Font font;

    sf::Sprite background1;
    sf::Sprite background2;
    sf::Sprite player;
    sf::Sprite tmpPlatform;

    double speedX = 0;
    double speedY = 0;
    double gravity = 0.5;
    double addSpeed = 1;
    double airResistance = 0.05;
    double lastPlatformGenerated = 0;

    double standingHeight = 0;
    int standingPlatformNumber = 0;

    std::vector <platform> platforms;
    bool isJumping = false;
    bool isStanding = false;

    double score = 0;

};
#endif //JUMPER_HPP
