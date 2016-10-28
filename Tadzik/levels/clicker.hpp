#ifndef CLICKER_HPP
#define CLICKER_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <sstream>

class CLICKER: public Scene{
public:
    CLICKER(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory() {
        texBackground.loadFromFile("files/textures/clicker/background.png");
        background.setTexture(texBackground);
        background.setScale((float)window->getSize().x/(float)background.getTextureRect().width,
                            (float)window->getSize().y/(float)background.getTextureRect().height);
        texWiener.loadFromFile("files/textures/clicker/wiener.png");
        wiener.setTexture(texWiener);

        texFactoryBg.loadFromFile("files/textures/clicker/factoryBg.png");
        factoryBg.setTexture(texFactoryBg);

        texFactory.loadFromFile("files/textures/clicker/factory.png");
        factory.setTexture(texFactory);

        texMinesBg.loadFromFile("files/textures/clicker/minesBg.png");
        minesBg.setTexture(texMinesBg);

        texMines.loadFromFile("files/textures/clicker/mines.png");
        mines.setTexture(texMines);


    };
    virtual void onSceneActivate() {

    }
    virtual void draw(double deltaTime) {
        window->clear();
        window->draw(background);
        window->draw(wiener);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texWiener;

    sf::Texture texCursor;
    sf::Texture texFactoryBg;
    sf::Texture texFactory;
    sf::Texture texMinesBg;
    sf::Texture texMines;

    sf::Sprite cursor;
    sf::Sprite background;
    sf::Sprite wiener;
    sf::Sprite factoryBg;
    sf::Sprite factory;
    sf::Sprite minesBg;
    sf::Sprite mines;

    sf::Font font;

    int wienerAmount = 0;
    int wienerAll = 0;

    bool debug = false;

};

#endif //CLICKER_HPP
