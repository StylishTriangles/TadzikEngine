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
protected:

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
        player.setPosition(window->getSize().x/2-player.getTextureRect().width/2, 100);
        player.setScale(3, 3);

        texPlatform.loadFromFile("files/textures/jumper/platform.png");
        tmpPlatform.setTexture(texPlatform);
        tmpPlatform.setPosition(0, window->getSize().y-30);
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width, 0.5);
        platforms.push_back(platform(tmpPlatform));
        tmpPlatform.setScale((double)window->getSize().x/(double)tmpPlatform.getTextureRect().width/3, 0.5);
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
        isJumping = true;
        speedY=-1.5*speedY;
        speedY-=abs(speedX);
    }

    void gameOver() {
        window->close();
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
            speedX-=0.5;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            speedX+=0.5;
        }
        speedY += gravity;
        if (speedY<0) {
            isJumping = true;
        }
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
        player.move(speedX, 0);
        for (int i=0; i<platforms.size(); ++i) {
            if (!isJumping) {
                //if (Collision::PixelPerfectTest(player, platforms[i].sprite) &&
                //    player.getPosition().y+player.getTextureRect().height*player.getScale().y <= platforms[i].sprite.getPosition().y+10) {
                if (Collision::PixelPerfectTest(player, platforms[i].sprite)) {
                    jump(speedX);
                }
            }
        }

        speedX -= znak(speedX)*airResistance;
        if (background1.getPosition().y>window->getSize().y) {
            background1.move(0, -1.98*window->getSize().y);
        }
        if (background2.getPosition().y>window->getSize().y) {
            background2.move(0, -1.98*window->getSize().y);
        }

        score-=speedY;
        std::cout << score/100 << std::endl;

        if (score - lastPlatformGenerated > window->getSize().y/2) {
            int tmp = Utils::randF(-100, -50);
            lastPlatformGenerated = score + tmp;
            tmpPlatform.setPosition(Utils::randF(0, window->getSize().x-tmpPlatform.getTextureRect().width), tmp);
            platforms.push_back(platform(tmpPlatform));
        }
        window->clear();
        window->draw(background1);
        window->draw(background2);
        window->draw(player);
        for (int i=0; i<platforms.size(); ++i) {
            if (platforms[i].sprite.getPosition().y>window->getSize().y+100) {
                platforms.erase(platforms.begin()+i);
            }
            window->draw(platforms[i].sprite);
        }
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
    double gravity = 0.3;
    double airResistance = 0.003;
    double lastPlatformGenerated = 0;

    std::vector <platform> platforms;
    bool isJumping = false;
    bool isFalling = false;

    double score = 0;

};
#endif //JUMPER_HPP
