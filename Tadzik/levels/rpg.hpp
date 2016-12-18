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
                    tempMur.setPosition(i*tilesize,j*tilesize);
                    spMur.push_back(tempMur);
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
            }
        }
    }
    bool checkCollision(sf::Vector2f pos)
    {
        if (mapa.getPixel(pos.x/tilesize, pos.y/tilesize)==sf::Color(0, 0, 0))
            return 1;
        else return 0;
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

        texMur.loadFromFile("files/textures/rpg/mur.png");
        tempMur.setTexture(texMur);
        tempMur.setScale(1, 1);

        texEnemy.loadFromFile("files/textures/rpg/enemyStand.png");
        tempEnemy.setTexture(texEnemy);

        texGrass.loadFromFile("files/textures/rpg/grass.png");
        tempGrass.setTexture(texGrass);

        texDoor.loadFromFile("files/textures/rpg/door.png");
        texDoorOpen.loadFromFile("files/textures/rpg/doorOpen.png");

for(int i=0; i<spDoor.size(); i++)
            if(spTadeusz.getPosition()==spDoor[i].getPosition())
                tempDoor.setTexture(texDoorOpen);
            else tempDoor.setTexture(texDoor);


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


        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            offset += sf::Vector2f(-tilesize, 0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            offset += sf::Vector2f(0, -tilesize);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            offset += sf::Vector2f(0, tilesize);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            offset += sf::Vector2f(tilesize, 0);
        }

        if(checkCollision(spTadeusz.getPosition()+offset))
            offset=sf::Vector2f(0,0);
        spTadeusz.move(offset);

        for(int i=0; i<spDoor.size(); i++)
            if(spTadeusz.getPosition()==spDoor[i].getPosition() || spTadeusz.getPosition()+sf::Vector2f(0, -tilesize)==spDoor[i].getPosition()|| spTadeusz.getPosition()+sf::Vector2f(-tilesize, -tilesize)==spDoor[i].getPosition()|| spTadeusz.getPosition()+sf::Vector2f(-tilesize, 0)==spDoor[i].getPosition())
                 spDoor[i].setTexture(texDoorOpen);
            else spDoor[i].setTexture(texDoor);

        window->clear(sf::Color());

        for(int i = 0; i < spGrass.size(); i++)
            window->draw(spGrass[i]);



        for(int i = 0; i < spDoor.size(); i++)
            window->draw(spDoor[i]);

        window->draw(spTadeusz);

        for(int i = 0; i < spMur.size(); i++)
            window->draw(spMur[i]);

        for(int i = 0; i < spEnemy.size(); i++)
            window->draw(spEnemy[i]);


        offset=sf::Vector2f(0, 0);
    }

protected:
    sf::Font font;
    sf::Texture texTadeusz;
    sf::Sprite spTadeusz;
    sf::Image mapa;
    sf::Texture texMur;
    std::vector <sf::Sprite> spMur;
    sf::Sprite tempMur;
    int tilesize = 30;
    sf::Vector2f offset;
    sf::Texture texEnemy;
    std::vector <sf::Sprite> spEnemy;
    sf::Sprite tempEnemy;
    sf::Texture texGrass;
    std::vector <sf::Sprite> spGrass;
    sf::Sprite tempGrass;
    sf::Texture texDoor;
    sf::Texture texDoorOpen;
    std::vector <sf::Sprite> spDoor;
    sf::Sprite tempDoor;
};
#endif //RPG
