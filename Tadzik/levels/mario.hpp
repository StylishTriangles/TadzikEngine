#ifndef MARIO_HPP
#define MARIO_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class MARIO: public Scene{
public:
    MARIO(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}
    virtual void onSceneLoadToMemory() {
        texBackground.loadFromFile("files/textures/mario/background.png");
        Background1.setTexture(texBackground);
        Background1.setScale(double(window->getSize().x)/double(Background1.getTextureRect().width),
                            double(window->getSize().y)/double(Background1.getTextureRect().height));
        Background2=Background1;
        Background2.move(window->getSize().x, 0);

        texFloorTile.loadFromFile("files/textures/mario/floor1.png");
        mapa.loadFromFile("files/maps/mario/map1.png");
        loadMap();

        texPlayerRun1.loadFromFile("files/textures/universal/playerRun1.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun1, 500));
        texPlayerRun2.loadFromFile("files/textures/universal/playerRun2.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun2, 500));
        texPlayerRun3.loadFromFile("files/textures/universal/playerRun3.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun3, 500));
        texPlayerRun4.loadFromFile("files/textures/universal/playerRun4.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun4, 500));

        texPlayerJump.loadFromFile("files/textures/universal/playerJump.png"), TadzikJump.addFrame(AnimationFrame(&texPlayerJump, 150));

        texPlayerStand.loadFromFile("files/textures/universal/playerStand.png"), TadzikStand.addFrame(AnimationFrame(&texPlayerStand, 150));

        spTadzik.setAnimation(&TadzikRun);
        spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
        spTadzik.sprite.setScale(2, 2);

    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void loadMap() {
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color::Black) {
                    sf::Sprite tmp;
                    tmp.setTexture(texFloorTile);
                    tmp.setScale((int)((double)window->getSize().y/(double)tilesPerY)/tmp.getGlobalBounds().height,
                                 (int)((double)window->getSize().y/(double)tilesPerY)/tmp.getGlobalBounds().height);
                    tmp.setPosition(i*tmp.getGlobalBounds().width, j*tmp.getGlobalBounds().height);
                    floor.push_back(tmp);
                }
                else if(mapa.getPixel(i, j)==sf::Color::White) {
                    sf::Sprite tmp;
                    tmp.setTexture(texFloorTile);
                    tmp.setScale((int)((double)window->getSize().y/(double)tilesPerY)/tmp.getGlobalBounds().height,
                                 (int)((double)window->getSize().y/(double)tilesPerY)/tmp.getGlobalBounds().height);
                    tmp.setPosition(i*tmp.getGlobalBounds().width, j*tmp.getGlobalBounds().height);
                    hitboxlessBack.push_back(tmp);
                }
            }
        }
    }
    void gameOver() {
        onSceneLoadToMemory();
    }

    void jump() {
        speedY=13;
        isStanding = false;
        isJumping = true;
        spTadzik.setAnimation(&TadzikJump);
    }
    void flip() {
        spTadzik.sprite.setScale(-spTadzik.sprite.getScale().x, spTadzik.sprite.getScale().y);
    }

    bool checkForStanding(sf::Sprite s1) {
            if (Collision::BoundingBoxTest(s1, spTadzik.sprite)) {
                standingHeight = s1.getGlobalBounds().top;
            }
        }


    virtual void draw(double deltaTime) {
        spTadzik.update(abs(speedX)*deltaTime);
        prevTop = spTadzik.sprite.getGlobalBounds().top;
        prevBot = spTadzik.sprite.getGlobalBounds().top+spTadzik.sprite.getGlobalBounds().height;
        prevLeft = spTadzik.sprite.getGlobalBounds().left;
        prevRight = spTadzik.sprite.getGlobalBounds().left+spTadzik.sprite.getGlobalBounds().width;
        prevX = spTadzik.sprite.getPosition().x;
        prevY = spTadzik.sprite.getPosition().y;
        prevSpeedX = speedX;
        prevSpeedY = speedY;

        if ((spTadzik.sprite.getPosition().x>(double)window->getSize().x*(3.0/4.0) && speedX>0) ||
            (spTadzik.sprite.getPosition().x<(double)window->getSize().x*(1.0/5.0) && speedX<0 && floor[0].getPosition().x<0)) {
            for (int i=0; i<floor.size(); i++) {
                floor[i].move(-speedX, 0);
            }
            for (int i=0; i<hitboxlessBack.size(); i++) {
                hitboxlessBack[i].move(-speedX, 0);
            }
            for (int i=0; i<hitboxlessFront.size(); i++) {
                hitboxlessFront[i].move(-speedX, 0);
            }
            prevLeft-=speedX;
            prevRight-=speedX;
            prevX-=speedX;
            Background1.move(-speedX, 0);
            Background2.move(-speedX, 0);
        }
        else {
            spTadzik.move(speedX, 0);
        }
        speedY-=gravity;
        spTadzik.sprite.move(0, -speedY);


        for (auto s:floor) {
            if (Collision::BoundingBoxTest(spTadzik.sprite, s)) {
                if (s.getGlobalBounds().top+s.getGlobalBounds().height<prevTop) {
                    speedY = 0;
                    spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                                                s.getGlobalBounds().top+s.getGlobalBounds().height+spTadzik.sprite.getGlobalBounds().height);
                }
                if (s.getGlobalBounds().top>=prevBot) {
                    isStanding = true;
                    touched = true;
                    //prevY = s.getGlobalBounds().top;
                    spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x, s.getGlobalBounds().top);
                    speedY = 0;
                    if (spTadzik.currentAnimation!=&TadzikRun)spTadzik.setAnimation(&TadzikRun);
                }
                else if (s.getGlobalBounds().left>prevRight) {
                    spTadzik.sprite.setPosition(prevX, spTadzik.sprite.getPosition().y);
                    speedX = 0;
                }
                else if (s.getGlobalBounds().left+s.getGlobalBounds().width<prevLeft+1) {
                    spTadzik.sprite.setPosition(prevX, spTadzik.sprite.getPosition().y);
                    speedX = 0;
                }
            }
        }
        touched = false;
        if (isJumping) {
            if (speedY>0) isJumping = false;
        }
        speedX*=0.9;

        if (abs(speedX)<0.01 && isStanding) {
            spTadzik.setAnimation(&TadzikStand);
        }
        //input z klawiatury
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A) && -speedX<maxSpeed) {
            speedX-=1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D) && speedX<maxSpeed) {
            speedX+=1;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && isStanding) {
            jump();
        }

        //gameover
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) {
            floor.clear();
            gameOver();
        }

        //fixowanie backgroundu
        if (-Background1.getGlobalBounds().left > window->getSize().x) Background1.move(2*(int)window->getSize().x, 0);
        if (-Background2.getGlobalBounds().left > window->getSize().x) Background2.move(2*(int)window->getSize().x, 0);
        if (Background1.getGlobalBounds().left > window->getSize().x) Background1.move(-2*(int)window->getSize().x, 0);
        if (Background2.getGlobalBounds().left > window->getSize().x) Background2.move(-2*(int)window->getSize().x, 0);
        //rysowanie
        if (Utils::sgn(speedX)!=Utils::sgn(prevSpeedX) && Utils::sgn(speedX)!=0) {
            flip();
        }
        window->clear();
        window->draw(Background1);
        window->draw(Background2);
        for (auto a:floor) {
            window->draw(a);
        }
        for (auto a:hitboxlessBack) {
            window->draw(a);
        }
        window->draw(spTadzik.sprite);
        for (auto a:hitboxlessFront) {
            window->draw(a);
        }
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlayerStand;
    sf::Texture texFloorTile;

    sf::Texture texPlayerRun1;
    sf::Texture texPlayerRun2;
    sf::Texture texPlayerRun3;
    sf::Texture texPlayerRun4;
    sf::Texture texPlayerJump;

    sf::Image mapa;

    AnimatedSprite spTadzik;
    Animation TadzikRun, TadzikStand, TadzikJump;

    sf::Sprite Background1;
    sf::Sprite Background2;
    sf::Sprite FloorTile;

    std::vector <sf::Sprite> floor;
    std::vector <sf::Sprite> hitboxlessFront;
    std::vector <sf::Sprite> hitboxlessBack;

    double speedX = 0.01, speedY=0;
    double maxSpeed = 10;
    double gravity = 0.5;
    double standingHeight = 0;

    double prevLeft, prevRight, prevTop, prevBot, prevX, prevY, prevSpeedX = 0.01, prevSpeedY = 0;

    int onTile = 0;
    int tilesPerY = 20;

    bool isStanding = false;
    bool isJumping = false;
    bool touched = false;
};
#endif //mario
