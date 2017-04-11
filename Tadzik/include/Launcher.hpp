#ifndef LAUNCHER_HPP
#define LAUNCHER_HPP

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"
#include "../include/Common.hpp"

#include "../levels/trex.hpp"
//#include "../levels/clicker.hpp"
#include "../levels/jumper.hpp"
#include "../levels/synth3d.hpp"
#include "../levels/marioRewritten.hpp"
#include "../levels/shooter2D.hpp"
#include "../levels/rpg.hpp"
#include "../levels/isayparty.hpp"

struct LevelData{
    LevelData();
    LevelData(std::string _name, std::string _nginName, sf::Texture* avatar)
    :name(_name), nginName(_nginName)
    {
        sprite = sf::Sprite(*avatar);
        sprite.setScale(6.0, 6.0);
    }
    std::string name;
    std::string nginName;
    std::string path;
    sf::Sprite sprite;
};

class LAUNCHER: public Scene{
public:
    LAUNCHER(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory() {
        texTREX.loadFromFile("files/textures/icons/trex.png");
        texJumper.loadFromFile("files/textures/icons/jumper.png");
        texSynth3d.loadFromFile("files/textures/icons/synth3de.png");
        texMario.loadFromFile("files/textures/icons/player.png");
        texRpg.loadFromFile("files/textures/icons/RPG.png");
        texShooter2d.loadFromFile("files/textures/icons/shooter2d.png");
        texDisco.loadFromFile("files/textures/icons/iSayParty.png");

        texBackground.loadFromFile("files/textures/icons/background.png");

        vecLvlData.push_back(LevelData("Trex", "TREX", &texTREX));
        vecLvlData.push_back(LevelData("Jumper", "JUMPER", &texJumper));
        vecLvlData.push_back(LevelData("Synth3d", "SYNTH3D", &texSynth3d));
        vecLvlData.push_back(LevelData("Mario", "MARIO2", &texMario));
        vecLvlData.push_back(LevelData("Rpg", "RPG", &texRpg));
        vecLvlData.push_back(LevelData("Shooter 2d", "SHOOTER2D", &texShooter2d));
        vecLvlData.push_back(LevelData("Disco Tadeuszek", "ISAYPARTY", &texDisco));

        int sWidth = vecLvlData[0].sprite.getGlobalBounds().width, sHeight = vecLvlData[0].sprite.getGlobalBounds().height;

        vecLvlData[0].sprite.setPosition(margin, margin*1.3);
        vecLvlData[1].sprite.setPosition(margin, margin+sHeight*0.5 + (window->getSize().y - margin-sHeight)/2.0);
        vecLvlData[2].sprite.setPosition(window->getSize().x*(0.25)-sWidth/2.0, window->getSize().y-sHeight-margin);
        vecLvlData[3].sprite.setPosition(window->getSize().x*(0.5)-sWidth/2.0, window->getSize().y-sHeight-margin);
        vecLvlData[4].sprite.setPosition(window->getSize().x*(0.75)-sWidth/2.0, window->getSize().y-sHeight-margin);
        vecLvlData[5].sprite.setPosition(window->getSize().x-sWidth-margin, margin+sHeight*0.5 + (window->getSize().y - margin-sHeight)/2.0);
        vecLvlData[6].sprite.setPosition(window->getSize().x-sWidth-margin, margin*1.3);

        player = sf::Sprite(texMario);
        player.setScale(5, 5);
        player.setPosition(window->getSize().x/2.0 - sWidth/2.0, window->getSize().y/2.0 - sHeight/2.0);

        backgroundTexture.loadFromFile("files/textures/icons/background.png");
        logoTexture.loadFromFile("files/textures/icons/logo.png");

        backgroundSprite = sf::Sprite(backgroundTexture);
        logoSprite = sf::Sprite(logoTexture);
        logoSprite.setScale(3.0, 3.0);
    }

    void onSceneActivate() {
        player.setPosition(window->getSize().x/2.0, window->getSize().y/2.0);
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
        if (event.type == sf::Event::MouseButtonReleased) {

        }
    }

    virtual void draw(sf::Time deltaTime) {
        float tedeuszWajhePrzeusz=200;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            player.move(-tedeuszWajhePrzeusz*deltaTime.asSeconds(), 0);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            player.move(tedeuszWajhePrzeusz*deltaTime.asSeconds(), 0);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            player.move(0, -tedeuszWajhePrzeusz*deltaTime.asSeconds());
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            player.move(0, tedeuszWajhePrzeusz*deltaTime.asSeconds());

        for(int i = 0; i < vecLvlData.size(); i++){
            if(Collision::PixelPerfectTest(vecLvlData[i].sprite, player)){
                sceneManager->setActiveScene(vecLvlData[i].nginName);
            }
        }

        window->clear();
        backgroundSprite.setScale(
            window->getSize().x / backgroundSprite.getLocalBounds().width,
            window->getSize().y / backgroundSprite.getLocalBounds().height);
        window->draw(backgroundSprite);
        logoSprite.setPosition(window->getSize().x/2-logoSprite.getGlobalBounds().width/2, margin);
        window->draw(logoSprite);

        for(int i = 0; i < vecLvlData.size(); i++){
            window->draw(vecLvlData[i].sprite);
            sf::Text description = sf::Text(vecLvlData[i].name, Common::Font::Comic_Sans);
            description.setScale(0.7, 0.7);
            description.setPosition(vecLvlData[i].sprite.getGlobalBounds().left + vecLvlData[i].sprite.getGlobalBounds().width/2.0 - description.getGlobalBounds().width/2.0,
                                    vecLvlData[i].sprite.getGlobalBounds().top+vecLvlData[i].sprite.getGlobalBounds().height);
            if(!sceneManager->scenes[vecLvlData[i].nginName]->isLocked()){
                description.setColor(sf::Color(0.0, 255.0, 0.0, 255.0));
            }
            else{
                description.setColor(sf::Color(52.0, 25.0, 255.0, 255.0));
            }
            window->draw(description);
            if (Utils::isMouseOnSprite(vecLvlData[i].sprite, window) && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                sceneManager->setActiveScene(vecLvlData[i].nginName);
            }
        }
        window->draw(player);
    }

protected:
    sf::Texture texTREX;
    sf::Texture texJumper;
    sf::Texture texSynth3d;
    sf::Texture texMario;
    sf::Texture texRpg;
    sf::Texture texShooter2d;
    sf::Texture texDisco;
    sf::Texture texBackground;
    std::vector<LevelData> vecLvlData;
    sf::Sprite player; //test

    sf::Texture backgroundTexture;
    sf::Sprite backgroundSprite;
    sf::Texture logoTexture;
    sf::Sprite logoSprite;

    int margin = 75;
};

#endif // LAUNCHER_HPP
