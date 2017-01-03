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
                    spTadeusz.sprite.setPosition(i*tilesize,j*tilesize);
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
//PLAYER ANIMATION
        spTadeusz.sprite.setScale(5,5);

        texIdle.resize(2);
        texIdle[0].loadFromFile("files/textures/rpg/idle0.png"), Idle.addFrame(AnimationFrame(&texIdle[0],500));
        texIdle[1].loadFromFile("files/textures/rpg/idle1.png"), Idle.addFrame(AnimationFrame(&texIdle[1],500));
        spTadeusz.setAnimation(&Idle);

        texRunLeft.resize(2);
        texRunLeft[0].loadFromFile("files/textures/rpg/runLeft0.png"), RunLeft.addFrame(AnimationFrame(&texRunLeft[0], 250));
        texRunLeft[1].loadFromFile("files/textures/rpg/runLeft1.png"), RunLeft.addFrame(AnimationFrame(&texRunLeft[1], 250));

        texJump.resize(3);
        texJump[0].loadFromFile("files/textures/rpg/jump0.png"), Jump.addFrame(AnimationFrame(&texJump[0],200));
        texJump[1].loadFromFile("files/textures/rpg/jump1.png"), Jump.addFrame(AnimationFrame(&texJump[1],200));
        texJump[2].loadFromFile("files/textures/rpg/jump2.png"), Jump.addFrame(AnimationFrame(&texJump[2],200));



////////////////////////////////

        spTadeusz.sprite.setOrigin(spTadeusz.sprite.getTextureRect().width*0.5,spTadeusz.sprite.getTextureRect().height*0.5);


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
        texWaterfall.setRepeated(true);
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

        attackTime = -sf::seconds(1);


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
        spTadeusz.update(deltaTime);

        spWaterfall.setTextureRect(sf::IntRect(0, -w, texWaterfall.getSize().x, tilesize*2 ));
        w++;
        spWaterfall.setColor(sf::Color(255,255,255,Utils::randInt(200,255)));

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
            if(spTadeusz.getAnimation()!=&RunLeft)
                spTadeusz.setAnimation(&RunLeft);
        }


//COLLISION
        for(int i = 0; i<spEnemy.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz.sprite,spEnemy[i]))
                spTadeusz.sprite.move(-offset);
        for(int i = 0; i<spWall.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz.sprite,spWall[i]))
                spTadeusz.sprite.move(-offset);


        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

//OBRACANIE        spTadeusz.sprite.setRotation(90+180/M_PI*atan2(sf::Mouse::getPosition(*window).y-spTadeusz.sprite.getPosition().y, sf::Mouse::getPosition(*window).x-spTadeusz.sprite.getPosition().x));

//SKELETON MOVEMENT

        for(int i = 0; i < spEnemy.size(); i++)
            spEnemy[i].move(sf::Vector2f(Utils::randInt(-50,50)*tilesize*0.001,Utils::randInt(-50,50)*tilesize*0.001));


//DOORS
        for(int i=0; i<spDoor.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz.sprite,spDoor[i]))
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


//ATTACK
        if((clock.getElapsedTime()-attackTime).asSeconds()>1)
        {

            if(sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            {
                spTadeusz.setAnimation(&Jump);

                for(int i = spEnemy.size()-1; i>=0; i--)
                    if(distance(spEnemy[i],spTadeusz.sprite)<=1000)

                        spEnemy.erase(spEnemy.begin()+i);

                attackTime = clock.getElapsedTime();
                std::cout << attackTime.asSeconds() <<std::endl;
            }
            else

                spTadeusz.sprite.move(offset),  spTadeusz.setAnimation(&Idle);




        }
        offset=sf::Vector2f(0, 0);
        window->draw(spTadeusz.sprite);
        window->draw(spWaterfall);
    }


protected:
    sf::Font font;
    sf::Clock clock;
    sf::Time attackTime;

    sf::Texture tempTex;

    sf::Texture texTadeusz;
    AnimatedSprite spTadeusz;

    Animation Idle;
    std::vector <sf::Texture> texIdle;
    Animation RunLeft;
    std::vector <sf::Texture> texRunLeft;
    Animation Jump;
    std::vector <sf::Texture> texJump;



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
