#ifndef CLICKER_HPP
#define CLICKER_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"

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

    class menuWindow {
    public:
        sf::Sprite background;
        sf::Sprite buyButton;

        int amount = 1;

        void switchBuyButton() {
            if (selected == 2) {
                selected = 0, amount = 1;
            } else selected++, amount*=10;
            buyButton.setTexture(howMany[selected]);
        }
        void setPosition (int x, int y) {
            background.setPosition(x, y);
            buyButton.setPosition(x+0.99*background.getTextureRect().width*background.getScale().x-
                                  buyButton.getTextureRect().width*buyButton.getScale().x, y+10);
        }
        void draw(sf::RenderWindow *window) {
            window->draw(background);
            window->draw(buyButton);
        }
        void load (std::string bg, std::string B1, std::string B2, std::string B3) {
            texBackground.loadFromFile(bg);
            buyOne.loadFromFile(B1);
            buyTen.loadFromFile(B2);
            buyHundred.loadFromFile(B3);
            howMany.push_back(buyOne);
            howMany.push_back(buyTen);
            howMany.push_back(buyHundred);
            buyButton.setTexture(buyOne);
            background.setTexture(texBackground);
        }
    protected:
        sf::Texture texBackground;
        sf::Texture buyOne;
        sf::Texture buyTen;
        sf::Texture buyHundred;
        int selected = 0;
        std::vector <sf::Texture> howMany;
        std::vector <sf::Sprite> upgrades;
    };

    class wienerSource {
    public:
        sf::Sprite background;
        sf::Sprite buyButton;
        sf::Text textCost;
        sf::Text textAmount;
        sf::Text incomePerSecond;
        sf::Text producing;
        sf::Text perSecond;

        std::vector <sf::Sprite> container;

        int amount = 0;
        double cost = 1;
        double costShown = 1;
        double income = 1;

        wienerSource () { };
        wienerSource (double c, double i) : cost(c), income(i) { };

        void buyAmountChanged (int amount) {
            costShown=0;
            double tmpCost = cost;
            for (int i=0; i<amount; i++) {
                costShown += tmpCost;
                tmpCost*=multiplier;
            }
            textCost.setString(Utils::stringify(costShown));
        }
        void setValues(double Cost, double Income) {
            cost = Cost, income = Income, costShown=cost;
        }
        void load (std::string textureLoc, std::string texBackgroundLoc, std::string texButtonLoc) {
            texture.loadFromFile(textureLoc);
            texBackground.loadFromFile(texBackgroundLoc);
            background.setTexture(texBackground);
            texBuyButton.loadFromFile(texButtonLoc);
            buyButton.setTexture(texBuyButton);
            producing.setString("Producing");
            incomePerSecond.setString("0 Wieners");
            perSecond.setString("per second");
        }
        double getIncome(double deltaTime) {
            return amount*income*deltaTime;
        }
        void setPosition(int x, int y) {
            background.setPosition(x, y);
            buyButton.setPosition(x+0.99*background.getTextureRect().width*background.getScale().x-
                                  buyButton.getTextureRect().width*buyButton.getScale().x, y+10);
            textCost.setPosition(x+0.75*(buyButton.getPosition().x-background.getPosition().x), y);
            textAmount.setPosition(x+0.75*(buyButton.getPosition().x-background.getPosition().x), y+40);
            producing.setPosition(x, y);
            incomePerSecond.setPosition(x, y+20);
            perSecond.setPosition(x, y+40);
        }
        void onBuy(int howMany){
            amount+=howMany;
            sf::Sprite tmp;
            tmp.setTexture(texture);
            for (int i=0; i<howMany; i++) {
                cost*=multiplier;
                tmp.setPosition(background.getPosition().x+Utils::randFloat(0.2, 0.65)*(background.getGlobalBounds().width-tmp.getTextureRect().width),
                background.getPosition().y+Utils::randFloat(0, 0.5)*(background.getGlobalBounds().height-tmp.getTextureRect().height));
                container.push_back(tmp);
            }
            buyAmountChanged(howMany);
        }
        void draw(sf::RenderWindow *window) {
            window->draw(background);
            window->draw(textCost);
            window->draw(textAmount);
            window->draw(incomePerSecond);
            window->draw(buyButton);
            window->draw(producing);
            window->draw(perSecond);
            for (auto a:container) {
                window->draw(a);
            }
        }
        void setFont(sf::Font font) {

        }
    protected:
        sf::Texture texture;
        sf::Texture texBackground;
        sf::Texture texBuyButton;
        double multiplier = 1.1;
    };

    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
        texBackground.loadFromFile("files/textures/clicker/background.png");
        background.setTexture(texBackground);
        background.setScale((float)window->getSize().x/(float)background.getTextureRect().width,
                            (float)window->getSize().y/(float)background.getTextureRect().height);

        texWiener.loadFromFile("files/textures/clicker/wiener.png");
        wiener.setTexture(texWiener);
        wiener.setScale(20, 20);
        wiener.setPosition(window->getSize().x/6-wiener.getTextureRect().width*wiener.getScale().x/2,
                           window->getSize().y/2-wiener.getTextureRect().height*wiener.getScale().y/2);

        mWindow.load("files/textures/clicker/menuBackground.png", "files/textures/clicker/buyOne.png",
                     "files/textures/clicker/buyTen.png", "files/textures/clicker/buyHundred.png");
        mWindow.background.setScale(((double)window->getSize().x*2/3)/(double)(mWindow.background.getTextureRect().width) *0.99, 1);
        mWindow.setPosition(window->getSize().x/3,
                            window->getSize().y/8*1-mWindow.background.getTextureRect().height/2);

        Farms.load("files/textures/clicker/farms.png", "files/textures/clicker/farmsBg.png", "files/textures/clicker/buyButton.png");
        Mines.load("files/textures/clicker/mines.png", "files/textures/clicker/minesBg.png", "files/textures/clicker/buyButton.png");
        Factory.load("files/textures/clicker/factory.png", "files/textures/clicker/factoryBg.png", "files/textures/clicker/buyButton.png");

        Farms.setValues(25, 5);
        Mines.setValues(125, 25);
        Factory.setValues(500, 50);

        vec.push_back(Farms);
        vec.push_back(Mines);
        vec.push_back(Factory);

        for (unsigned int i=0; i<vec.size(); i++) {
            vec[i].background.setScale(((double)window->getSize().x*2/3)/(double)(vec[i].background.getTextureRect().width) *0.99, 1);
            vec[i].setPosition(window->getSize().x/3,
                               window->getSize().y/8*(i+2)-vec[i].background.getTextureRect().height/2);
            vec[i].textCost.setFont(font);
            vec[i].textAmount.setFont(font);
            vec[i].producing.setFont(font);
            vec[i].incomePerSecond.setFont(font);
            vec[i].perSecond.setFont(font);

        }
        mainTitle.setFont(font);
        textScore.setFont(font);
        perSecond.setFont(font);
        temp.setFont(font);

        mainTitle.setString("Tadzik's sausage stand");
        mainTitle.setPosition(0, 0);
        textScore.setPosition(0, 30);
        perSecond.setPosition(0, 60);
    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            wienerPerSecond = 0;

            if (Utils::isMouseOnSprite(mWindow.buyButton, window)) {
                mWindow.switchBuyButton();
                for (auto a:vec) {
                    a.buyAmountChanged(mWindow.amount);
                }
            }
            if (Utils::isMouseOnSprite(wiener, window)) {
                isPressed = true;
            }
            for (unsigned int i=0; i<vec.size(); i++) {
                if (Utils::isMouseOnSprite(vec[i].buyButton, window)) {
                    if (wienerAmount >= vec[i].costShown) {
                        wienerAmount -= vec[i].costShown;
                        vec[i].onBuy(mWindow.amount);
                    }
                }
            }
        }
        if (event.type == sf::Event::MouseButtonReleased) {
            for(unsigned int i=0; i<vec.size(); i++) {
                wienerPerSecond += vec[i].getIncome(1);
                vec[i].incomePerSecond.setString(Utils::stringify(vec[i].getIncome(1))+" Wieners");
            }
            isReleased = true;
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
        for (unsigned int i=0; i<clickAnimation.size(); i++) {
            clickAnimation[i].move(0, -1);
            clickAnimation[i].setColor(sf::Color( 255, 255, 255, clickAnimation[i].getColor().a-5));
            if(clickAnimation[i].getColor().a==0) clickAnimation.erase(clickAnimation.begin()+i);
        }

        if (isPressed) {
            if (wiener.getScale().x>19) {
                wiener.setScale(wiener.getScale().x*0.99, wiener.getScale().y*0.99);
                wiener.setPosition(window->getSize().x/6-wiener.getTextureRect().width*wiener.getScale().x/2,
                                   window->getSize().y/2-wiener.getTextureRect().height*wiener.getScale().y/2);
            }
            else if (!Utils::isMouseOnSprite(wiener, window)){
                isPressed = false;
                isReleased = true;
            }
        }

        if (isReleased or !Utils::isMouseOnSprite(wiener, window)) {
            isPressed = false;
            wiener.setScale (wiener.getScale().x*1.01, wiener.getScale().y*1.01);
            if (wiener.getScale().x>20) {
                wiener.setScale (20, 20);
                isReleased = false;
            }
            wiener.setPosition(window->getSize().x/6-wiener.getTextureRect().width*wiener.getScale().x/2,
                               window->getSize().y/2-wiener.getTextureRect().height*wiener.getScale().y/2);
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
        mWindow.draw(window);
        for(auto a:clickAnimation) {
            window->draw(a);
        }

        for(unsigned int i=0; i<vec.size(); i++) {
            vec[i].draw(window);
            vec[i].buyAmountChanged(mWindow.amount); //bez tego sie nie zmienia i chuj wie dlaczego
            if (vec[i].costShown< wienerAmount) vec[i].buyButton.setColor(sf::Color::White);
            if (vec[i].costShown>=wienerAmount) vec[i].buyButton.setColor(sf::Color::Black);
            vec[i].textCost.setString("Cost:  " + Utils::stringify(vec[i].costShown));
            vec[i].textAmount.setString("Owned: " + Utils::stringify(vec[i].amount));
        }

        if (playTime.getElapsedTime().asSeconds()>1) {
            //std::cout << vec[0].costShown << std::endl;
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

    menuWindow mWindow;

    sf::Sprite wiener;
    sf::Sprite background;

    double wienerAmount = 1000;
    int wienerAll = 0;
    double wienerPerSecond = 0;
    double wienerPerClick = 10;

    bool isPressed = false;
    bool isReleased = false;
    bool debug = true;

    sf::Clock clock;
    sf::Clock playTime;
};

#endif //CLICKER_HPP
