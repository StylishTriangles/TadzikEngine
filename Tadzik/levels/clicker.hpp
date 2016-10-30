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
    sf::Sprite background;
    sf::Text textCost;
    sf::Text textAmount;

    std::vector <sf::Sprite> container;

    int amount = 0;
    double cost = 1;
    double income = 1;

    wienerSource () { };
    wienerSource (double c, double i) : cost(c), income(i) { };

    void setValues(double Cost, double Income) {
        cost = Cost, income = Income;
    }
    void load (std::string textureLoc, std::string texBackgroundLoc) {
        texture.loadFromFile(textureLoc);
        texBackground.loadFromFile(texBackgroundLoc);
        background.setTexture(texBackground);
    }
    double getIncome(double deltaTime) {
        return amount*income*deltaTime;
    }
    void setPosition(int x, int y) {
        background.setPosition(x, y);
    }
    void onBuy(){
        amount+=1;
        cost*=1.1;
        sf::Sprite tmp;
        tmp.setTexture(texture);
        tmp.setPosition(background.getPosition().x+Utils::randF(0, 0.5)*(background.getTextureRect().width*background.getScale().x-tmp.getTextureRect().width),
                        background.getPosition().y+Utils::randF(0, 0.5)*(background.getTextureRect().height*background.getScale().y-tmp.getTextureRect().height));
        container.push_back(tmp);
    }
    void draw(sf::RenderWindow *window) {
        window->draw(background);
        window->draw(textCost);
        window->draw(textAmount);
        for (auto a:container) {
            window->draw(a);
        }
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
        wiener.setPosition(0, window->getSize().y/2-wiener.getTextureRect().height*wiener.getScale().y/2);

        Farms.load("files/textures/clicker/farms.png", "files/textures/clicker/farmsBg.png");
        Farms.setPosition(window->getSize().x/3,
                          window->getSize().y/10*2-Farms.background.getTextureRect().height/2);
        Farms.background.setScale(((double)window->getSize().x*2/3)/(double)(Farms.background.getTextureRect().width) *0.99, 1);
        Farms.setValues(25, 5);

        Mines.load("files/textures/clicker/mines.png", "files/textures/clicker/minesBg.png");
        Mines.setPosition(window->getSize().x-Mines.background.getTextureRect().width-10, 210);
        Mines.setValues(125, 25);

        Factory.load("files/textures/clicker/factory.png", "files/textures/clicker/factoryBg.png");
        Factory.setValues(500, 50);
        Factory.setPosition(window->getSize().x-Factory.background.getTextureRect().width-10, 410);


        vec.push_back(Farms);
        vec.push_back(Mines);
        vec.push_back(Factory);


        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }

        mainTitle.setFont(font);
        textScore.setFont(font);
        perSecond.setFont(font);
        temp.setFont(font);

        mainTitle.setString("Tadzik's sausage stand");
        mainTitle.setPosition(0, 0);
        textScore.setPosition(0, 30);
        perSecond.setPosition(0, 60);
        for (int i=0; i<vec.size(); i++) {
            vec[i].textCost.setFont(font);
            vec[i].textAmount.setFont(font);
            vec[i].textCost.setPosition(vec[i].background.getPosition().x+75, vec[i].background.getPosition().y);
            vec[i].textAmount.setPosition(vec[i].background.getPosition().x+75, vec[i].background.getPosition().y+50);
        }

    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            wienerPerSecond = 0;
            for(auto a:vec) {
                wienerPerSecond += a.getIncome(1);
            }
            if (Utils::isMouseOnSprite(wiener, window)) {


            }
            for (int i=0; i<vec.size(); i++) {
                if (Utils::isMouseOnSprite(vec[i].background, window)) {
                    if (wienerAmount >= vec[i].cost) {
                        wienerAmount-=vec[i].cost;
                        vec[i].onBuy();
                    }
                }
            }

        }
        if (event.type == sf::Event::MouseButtonReleased) {
            if (Utils::isMouseOnSprite(wiener, window)) {
                wienerAmount+=wienerPerClick;
                temp.setString("+"+Utils::stringify(wienerPerClick));
                temp.setColor(sf::Color(255, 255, 255, 255));
                temp.setPosition((sf::Vector2f)sf::Mouse::getPosition(*window));
                clickAnimation.push_back(temp);
            }
        }
    }

    void updateWieners(double deltaTime) {
        for (auto a:vec) {
            wienerAmount += a.getIncome(deltaTime);
        }
    }

    virtual void draw(double deltaTime) {
        for (int i=0; i<clickAnimation.size(); i++) {
                clickAnimation[i].move(0, -1);
            clickAnimation[i].setColor(sf::Color(
                    clickAnimation[i].getColor().r, clickAnimation[i].getColor().b, clickAnimation[i].getColor().r, clickAnimation[i].getColor().a-5));
            if(clickAnimation[i].getColor().a==0) clickAnimation.erase(clickAnimation.begin()+i);
        }

        if (Utils::isMouseOnSprite(wiener, window)) {
            isPressed = true;
            if(wiener.getScale().x>19) wiener.setScale(wiener.getScale().x*0.99, wiener.getScale().y*0.99);
        }

        if (isPressed) {
            if (!Utils::isMouseOnSprite(wiener, window)) {
                isPressed = false;
                if (wiener.getScale().x<20)wiener.setScale (wiener.getScale().x*1.01, wiener.getScale().y*1.01);
                if (wiener.getScale().x>20)wiener.setScale (20, 20);
            }
        }

        window->clear();
        window->draw(background);
        textScore.setString(Utils::stringify(wienerAmount)+" wieners");
        perSecond.setString("per second: "+Utils::stringify(wienerPerSecond));
        window->draw(textScore);
        window->draw(perSecond);
        window->draw(mainTitle);
        if(debug) Utils::drawBoundingBox(wiener, window);
        window->draw(wiener);
        for(auto a:clickAnimation) {
            window->draw(a);
        }

        for(int i=0; i<vec.size(); i++)
        {
            vec[i].draw(window);
            vec[i].textCost.setString(Utils::stringify(vec[i].cost));
            vec[i].textAmount.setString(Utils::stringify(vec[i].amount));
        }


        //std::cout << (int) clock.getElapsedTime().asSeconds() << "\n";

        if (playTime.getElapsedTime().asSeconds()>1) {

        }
        updateWieners((double)clock.getElapsedTime().asSeconds());
        clock.restart();
    }

protected:
    sf::Font font;
    sf::Text textScore;
    sf::Text perSecond;
    sf::Text mainTitle;

    sf::Texture texBackground;
    sf::Texture texWiener;

    sf::Text temp;

    std::vector <sf::Text> clickAnimation;

    std::vector <wienerSource> vec;
    wienerSource Farms;
    wienerSource Mines;
    wienerSource Factory;

    sf::Sprite wiener;
    sf::Sprite background;

    double wienerAmount = 1000;
    int wienerAll = 0;
    double wienerPerSecond = 1;
    double wienerPerClick = 10;

    bool clickingWiener = false;
    bool isPressed = false;
    bool debug = true;

    sf::Clock clock;
    sf::Clock playTime;


};

#endif //CLICKER_HPP
