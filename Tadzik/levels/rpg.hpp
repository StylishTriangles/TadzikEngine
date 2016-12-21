#ifndef RPG_HPP
#define RPG_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class RPG: public Scene
{
public:
    RPG(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

    void loadMap()
    {
        for (int i=0; i<mapa.getSize().y; i++)
        {
            for (int j=0; j<mapa.getSize().x; j++)
            {
                if (mapa.getPixel(i, j)==sf::Color(0, 0, 0))
                {
                    tempWall.setPosition(i*tilesize,j*tilesize);
                    spWall.push_back(tempWall);
                }
                else if(mapa.getPixel(i,j)==sf::Color(128,32,0))
                {
                    tempDoor.setPosition(i*tilesize,j*tilesize);
                    spDoor.push_back(tempDoor);
                }
                else
                {
                    tempGrass.setPosition(i*tilesize,j*tilesize);
                    spGrass.push_back(tempGrass);
                }

                if (mapa.getPixel(i, j)==sf::Color(0, 0, 255))
                {
                    spTadeusz.setPosition(i*tilesize,j*tilesize);
                }

                if (mapa.getPixel(i, j)==sf::Color(255, 0, 0))
                {
                    tempEnemy.setPosition(i*tilesize,j*tilesize);
                    spEnemy.push_back(tempEnemy);
                }
                if(mapa.getPixel(i, j)==sf::Color(0, 0, 255))
                {
                    spWaterfall.setPosition(i*tilesize, j*tilesize);
                }
            }
        }
    }

    float distance(sf::Sprite spA, sf::Sprite spB)
    {
        return sqrt((spA.getPosition().x-spB.getPosition().x)*(spA.getPosition().x-spB.getPosition().x)+(spA.getPosition().y-spB.getPosition().y)*(spA.getPosition().y-spB.getPosition().y));
    }

    virtual void onSceneLoadToMemory()
    {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf"))
        {
            std::cout << "cannot load font\n";
        }
        texTadeusz.loadFromFile("files/textures/rpg/playerStand.png");

        spTadeusz.setTexture(texTadeusz);
        spTadeusz.setScale(1, 1);
        spTadeusz.setOrigin(spTadeusz.getTextureRect().width*0.5,spTadeusz.getTextureRect().height*0.5);

        texWall.loadFromFile("files/textures/rpg/Wall.png");
        tempWall.setTexture(texWall);
        tempWall.setScale(1, 1);

        texEnemy.loadFromFile("files/textures/rpg/enemyStand.png");
        tempEnemy.setTexture(texEnemy);
        tempEnemy.setOrigin(tempEnemy.getTextureRect().width*0.5, tempEnemy.getTextureRect().height*0.5);

        texGrass.loadFromFile("files/textures/rpg/grass.png");
        tempGrass.setTexture(texGrass);

        texDoor.loadFromFile("files/textures/rpg/door.png");
        texDoorOpen.loadFromFile("files/textures/rpg/doorOpen.png");

        texWaterfall.loadFromFile("files/textures/rpg/Waterfall.png");
        spWaterfall.setTexture(texWaterfall);
        spWaterfall.setScale(4,4);


        window->setMouseCursorVisible(0);
        texCrosshair.loadFromFile("files/textures/rpg/crosshair.png");
        spCrosshair.setTexture(texCrosshair);
        spCrosshair.setOrigin(spCrosshair.getTextureRect().height*0.5, spCrosshair.getTextureRect().width*0.5);

//attackcircle
        Attack.setFillColor(sf::Color(0, 0, 0, 0));
        Attack.setOutlineThickness(2);
        Attack.setOutlineColor(sf::Color(250, 150, 100));
        Attack.setRadius(50);
        Attack.setOrigin(Attack.getRadius(), Attack.getRadius());

        mapa.loadFromFile("files/maps/rpg/mapa1.png");
        loadMap();

    }

    virtual void onSceneActivate()
    {

    }

    void deliverEvent(sf::Event& event)
    {


    }

    virtual void draw(double deltaTime)
    {

        spWaterfall.setTextureRect(sf::IntRect( 0,  spWaterfall.getTextureRect().height-tilesize-w,    spWaterfall.getTextureRect().width, tilesize ));
        w++;
        if((texWaterfall.getSize().y-tilesize<w))
            w=0;


//MOVEMENT
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            offset += sf::Vector2f(-tilesize*0.1, 0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            offset += sf::Vector2f(0, -tilesize*0.1);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            offset += sf::Vector2f(0, tilesize*0.1);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            offset += sf::Vector2f(tilesize*0.1, 0);
        }
        spTadeusz.move(offset);

//COLLISION
        for(int i = 0; i<spEnemy.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz,spEnemy[i]))
                spTadeusz.move(-offset);
        for(int i = 0; i<spWall.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz,spWall[i]))
                spTadeusz.move(-offset);;
        offset=sf::Vector2f(0, 0);

        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

//OBRACANIE        spTadeusz.setRotation(90+180/M_PI*atan2(sf::Mouse::getPosition(*window).y-spTadeusz.getPosition().y, sf::Mouse::getPosition(*window).x-spTadeusz.getPosition().x));

//SKELETON MOVEMENT

        for(int i = 0; i < spEnemy.size(); i++)
            spEnemy[i].move(sf::Vector2f(Utils::randInt(-50,50)*tilesize*0.001,Utils::randInt(-50,50)*tilesize*0.001));


//DOORS
        for(int i=0; i<spDoor.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz,spDoor[i]))
                spDoor[i].setTexture(texDoorOpen);
            else spDoor[i].setTexture(texDoor);

//DRAW STARTS
        window->clear(sf::Color());
        for(int i = 0; i < spGrass.size(); i++)
            window->draw(spGrass[i]);
        for(int i = 0; i < spWall.size(); i++)
            window->draw(spWall[i]);
        for(int i = 0; i < spDoor.size(); i++)
            window->draw(spDoor[i]);
        for(int i = 0; i < spEnemy.size(); i++)
            window->draw(spEnemy[i]);

        window->draw(spWaterfall);

        window->draw(spCrosshair);
        window->draw(spTadeusz);

//ATTACK
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
        {
            Attack.setPosition(spTadeusz.getPosition());
            window->draw(Attack);
            for(int i = spEnemy.size()-1; i>=0; i--)
                if(distance(spEnemy[i],spTadeusz)<=50)

                    spEnemy.erase(spEnemy.begin()+i);



        }

    }

protected:
    sf::Font font;

    sf::Texture texTadeusz;
    sf::Sprite spTadeusz;

    sf::Image mapa;

    sf::Texture texWall;
    std::vector <sf::Sprite> spWall;
    sf::Sprite tempWall;

    int tilesize = 30;

    sf::Vector2f offset;

    sf::Texture texEnemy;
    std::vector <sf::Sprite> spEnemy;
    sf::Sprite tempEnemy;

    sf::Texture texGrass;
    std::vector <sf::Sprite> spGrass;
    sf::Sprite tempGrass;
    int w = 0;
    sf::Texture texWaterfall;
    sf::Sprite spWaterfall;

    sf::Texture texDoor;
    sf::Texture texDoorOpen;
    std::vector <sf::Sprite> spDoor;
    sf::Sprite tempDoor;

    sf::Texture texCrosshair;
    sf::Sprite spCrosshair;

    sf::CircleShape Attack;
};
#endif //RPG
