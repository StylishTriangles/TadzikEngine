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

class wienerSource {
public:
    sf::Sprite sprite;
    sf::Sprite background;
    sf::Text textCost;
    sf::Text textAmount;

    int amount = 0;
    double cost = 1;
    double income = 1;

    sf::Sprite getBackground() {return background;};
    sf::Sprite getSprite() {return sprite;};

    wienerSource () { };
    wienerSource (double c, double i) : cost(c), income(i) { };

    void sett(double c, double i) {
        cost = c, income = i;
    }
    void load (std::string textureLoc, std::string texBackgroundLoc) {
        texture.loadFromFile(textureLoc);
        texBackground.loadFromFile(texBackgroundLoc);
        sprite.setTexture(texture);
        background.setTexture(texBackground);
    }
    double getIncome(double deltaTime) {
        return amount*income*deltaTime;
    }
    void setPosition(int x, int y) {
        sprite.setPosition(x, y);
        background.setPosition(x, y);
    }
    void onBuy(){
        amount+=1;
        cost*=2;
    }
    void draw(sf::RenderWindow *window) {
        window->draw(background);
        window->draw(sprite);
        window->draw(textCost);
        window->draw(textAmount);
    }
protected:
    sf::Texture texture;
    sf::Texture texBackground;


};

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
        wiener.setScale(20, 20);

        Farms.load("files/textures/clicker/farms.png", "files/textures/clicker/farmsBg.png");
        Farms.setPosition(500, 10);
        Farms.sett(25, 5);

        Mines.load("files/textures/clicker/mines.png", "files/textures/clicker/minesBg.png");
        Mines.setPosition(500, 210);
        Mines.sett(125, 25);

        Factory.load("files/textures/clicker/factory.png", "files/textures/clicker/factoryBg.png");
        Factory.sett(500, 50);
        Factory.setPosition(500, 410);


        vec.push_back(Farms);
        vec.push_back(Mines);
        vec.push_back(Factory);


        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
        textScore.setFont(font);
        for (int i=0; i<vec.size(); i++) {
            vec[i].textCost.setFont(font);
            vec[i].textAmount.setFont(font);
            vec[i].textCost.setPosition(vec[i].sprite.getPosition().x+75, vec[i].sprite.getPosition().y);
            vec[i].textAmount.setPosition(vec[i].sprite.getPosition().x+75, vec[i].sprite.getPosition().y+50);
        }

    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            if (Utils::isSpriteClicked(wiener, window)) {
                wienerAmount+=wienerPerClick;
            }
            for (int i=0; i<vec.size(); i++) {
                if (Utils::isSpriteClicked(vec[i].background, window)) {
                    if (wienerAmount >= vec[i].cost) {
                        wienerAmount-=vec[i].cost;
                        vec[i].onBuy();
                    }
                }
            }

        }
    }

    void updateWieners(double deltaTime) {
//        wienerPerSecond = wienerPerCursor + wiener
        for (auto a:vec) {
            wienerAmount += a.getIncome(deltaTime);
        }
    }

    virtual void draw(double deltaTime) {
        window->clear();
        window->draw(background);
        textScore.setString(Utils::stringify(wienerAmount));
        window->draw(textScore);

        for(int i=0; i<vec.size(); i++)
        {
            vec[i].draw(window);
            vec[i].textCost.setString(Utils::stringify(vec[i].cost));
            vec[i].textAmount.setString(Utils::stringify(vec[i].amount));
        }

        if(debug) Utils::drawBoundingBox(wiener, window);
        window->draw(wiener);
        //std::cout << (int) clock.getElapsedTime().asSeconds() << "\n";

        if (clock.getElapsedTime().asSeconds()>1) {
            clock.restart();
            std::cout << wienerAmount << "\n";
        }
        updateWieners((double)clock.getElapsedTime().asSeconds());
        clock.restart();
    }

protected:
    sf::Font font;
    sf::Text textScore;

    sf::Texture texBackground;
    sf::Texture texWiener;

    std::vector <wienerSource> vec;
    wienerSource Farms;
    wienerSource Mines;
    wienerSource Factory;

    sf::Sprite wiener;
    sf::Sprite background;

    double wienerAmount = 0;
    int wienerAll = 0;
    double wienerPerSecond = 1;
    double wienerPerClick = 1;

    bool debug = true;

    sf::Clock clock;


};

#endif //CLICKER_HPP
