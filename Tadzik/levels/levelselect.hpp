#ifndef LEVELSELECT_HPP
#define LEVELSELECT_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class LEVELSELECT: public Scene{
public:
    LEVELSELECT(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}
    virtual void onSceneLoadToMemory() {
        //texBackground.loadFromFile()

        texPlayerRun1.loadFromFile("files/textures/universal/playerRun1.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun1, 500));
        texPlayerRun2.loadFromFile("files/textures/universal/playerRun2.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun2, 500));
        texPlayerRun3.loadFromFile("files/textures/universal/playerRun3.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun3, 500));
        texPlayerRun4.loadFromFile("files/textures/universal/playerRun4.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun4, 500));

        texPlayerJump.loadFromFile("files/textures/universal/playerJump.png"), TadzikJump.addFrame(AnimationFrame(&texPlayerJump, 150));

        texPlayerStand.loadFromFile("files/textures/universal/playerStand.png"), TadzikStand.addFrame(AnimationFrame(&texPlayerStand, 150));

        spTadzik.setAnimation(&TadzikRun);
        spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
        spTadzik.sprite.setScale(2, 2);
        spTadzik.sprite.setPosition(1000, 600);

        texCLICKER.loadFromFile("files/textures/levelselect/levelCLICKER.png"), spriteCLICKER.setTexture(texCLICKER);
        texJUMPER.loadFromFile("files/textures/levelselect/levelJUMPER.png"), spriteJUMPER.setTexture(texJUMPER);
        texMARIO.loadFromFile("files/textures/levelselect/levelMARIO.png"), spriteJUMPER.setTexture(texMARIO);
        //texSYNTH3D.loadFromFile("files/textures/levelselect/levelSYNTH3D.png"), spriteSYNTH3D.setTexture(texSYNTH3D);
        texTREX.loadFromFile("files/textures/levelselect/levelTREX.png"), spriteTREX.setTexture(texTREX);

        spriteCLICKER.setPosition(100, 100);
        spriteJUMPER.setPosition(100, 600);
        spriteMARIO.setPosition(600, 100);
        //spriteSYNTH3D.setPosition(300, 600);
        spriteTREX.setPosition(600, 600);

        spriteCLICKER.setScale(3, 3);
        spriteJUMPER.setScale(3, 3);
        spriteMARIO.setScale(3, 3);
        //spriteSYNTH3D.setScale(3, 3);
        spriteTREX.setScale(3, 3);
    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    virtual void draw(double deltaTime) {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) spTadzik.move(-5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) spTadzik.move(5, 0);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) spTadzik.move(0, -5);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) spTadzik.move(0, 5);

        if (Collision::BoundingBoxTest(spTadzik.sprite, spriteCLICKER)) sceneManager->setActiveScene("CLICKER");
        if (Collision::BoundingBoxTest(spTadzik.sprite, spriteJUMPER)) sceneManager->setActiveScene("JUMPER");
        if (Collision::BoundingBoxTest(spTadzik.sprite, spriteMARIO)) sceneManager->setActiveScene("MARIO");
        if (Collision::BoundingBoxTest(spTadzik.sprite, spriteSYNTH3D)) sceneManager->setActiveScene("SYNTH3D");
        if (Collision::BoundingBoxTest(spTadzik.sprite, spriteTREX)) sceneManager->setActiveScene("TREX");

        window->clear(sf::Color::White);
        window->draw(spTadzik.sprite);
        window->draw(spriteCLICKER);
        window->draw(spriteJUMPER);
        window->draw(spriteMARIO);
        window->draw(spriteSYNTH3D);
        window->draw(spriteTREX);

        if (debug) {
            Utils::drawBoundingBox(spriteCLICKER, window);
            Utils::drawBoundingBox(spriteJUMPER, window);
            Utils::drawBoundingBox(spriteMARIO, window);
            Utils::drawBoundingBox(spriteSYNTH3D, window);
            Utils::drawBoundingBox(spriteTREX, window);
        }
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlayerStand;
    sf::Texture texPlayerJump;
    sf::Texture texPlayerRun1;
    sf::Texture texPlayerRun2;
    sf::Texture texPlayerRun3;
    sf::Texture texPlayerRun4;

    sf::Texture texTREX;
    sf::Texture texJUMPER;
    sf::Texture texCLICKER;
    sf::Texture texMARIO;
    sf::Texture texSYNTH3D;

    AnimatedSprite spTadzik;
    Animation TadzikRun;
    Animation TadzikStand;
    Animation TadzikJump;

    sf::Sprite spriteTREX;
    sf::Sprite spriteJUMPER;
    sf::Sprite spriteCLICKER;
    sf::Sprite spriteMARIO;
    sf::Sprite spriteSYNTH3D;

    bool debug;
};
#endif //LEVELSELECT
