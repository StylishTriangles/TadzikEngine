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
  class Weapon: public sf::Sprite
    { public:
        Weapon(sf::RenderWindow* w)
        {
            window = w;
        }




        void update(double delta)
        {
         // setPosition(spTadeusz.getPosition());
            timeElapsed += delta;
            setRotation(-angleRotate+0.5*timeElapsed+90+angleStart);

        }
          void startRotating()
        {
            shouldRotate = true;
            timeElapsed = 0;
            angleStart = 180/M_PI*atan2(sf::Mouse::getPosition(*window).y-getPosition().y, sf::Mouse::getPosition(*window).x-getPosition().x);
        }

        bool shouldDraw = false;
        bool shouldRotate = false;
        double timeElapsed = 0;
        double angleRotate = 60;
        double angleStart = 0;
        sf::RenderWindow* window;

    };


class Player: public AnimatedSprite
    {public:
        AnimatedSprite Body;
        AnimatedSprite Legs;

        void draw(sf::RenderWindow* w)
        {
            w->draw(Body.sprite);
            w->draw(Legs.sprite);
            }

    };
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

        texRun.resize(2);
        texRun[0].loadFromFile("files/textures/rpg/Run0.png"), Run.addFrame(AnimationFrame(&texRun[0], 250));
        texRun[1].loadFromFile("files/textures/rpg/Run1.png"), Run.addFrame(AnimationFrame(&texRun[1], 250));




////////////////////////////////

        spTadeusz.sprite.setOrigin(spTadeusz.sprite.getTextureRect().width*0.5,spTadeusz.sprite.getTextureRect().height*0.5);

        texSword.loadFromFile("files/textures/rpg/sword.png");
        spSword.setTexture(texSword);
        spSword.setOrigin(sf::Vector2f(3, spSword.getTextureRect().height));
        spSword.setScale(5,5);

        Blade.setTexture(texSword);
        Blade.setScale(5,5);
        Blade.setOrigin(sf::Vector2f(3, Blade.getTextureRect().height));

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
        spWaterfall.setColor(sf::Color(255,255,255,Utils::randInt(200,255)));

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

        idleTime = -sf::seconds(1);


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



//MOVEMENT
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            offset += sf::Vector2f(-tilesize*0.1, 0);
            if(spTadeusz.getAnimation()!=&Run)
                spTadeusz.setAnimation(&Run);
            idleTime = clock.getElapsedTime();
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            offset += sf::Vector2f(0, -tilesize*0.1);
            if(spTadeusz.getAnimation()!=&Run)
                spTadeusz.setAnimation(&Run);
            idleTime = clock.getElapsedTime();
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            offset += sf::Vector2f(0, tilesize*0.1);
            if(spTadeusz.getAnimation()!=&Run)
                spTadeusz.setAnimation(&Run);
            idleTime = clock.getElapsedTime();
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            offset += sf::Vector2f(tilesize*0.1, 0);
            if(spTadeusz.getAnimation()!=&Run)
                spTadeusz.setAnimation(&Run);
            idleTime = clock.getElapsedTime();
        }

        if((clock.getElapsedTime()-idleTime).asSeconds()>0.2)
            if(spTadeusz.getAnimation()!=&Idle)
                spTadeusz.setAnimation(&Idle);


//COLLISION
        for(int i = 0; i<spEnemy.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz.sprite,spEnemy[i]))
                spTadeusz.sprite.move(-offset);
        for(int i = 0; i<spWall.size(); i++)
            if(Collision::BoundingBoxTest(spTadeusz.sprite,spWall[i]))
                spTadeusz.sprite.move(-offset);


        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

//OBRACANIE        spTadeusz.sprite.setRotation(90+180/M_PI*atan2(sf::Mouse::getPosition(*window).y-spTadeusz.sprite.getPosition().y, sf::Mouse::getPosition(*window).x-spTadeusz.sprite.getPosition().x));

//WATERFALL RAK
        spWaterfall.setTextureRect(sf::IntRect(0, -w, texWaterfall.getSize().x, tilesize*2 ));
        w++;


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




        spTadeusz.sprite.move(offset);
        offset=sf::Vector2f(0, 0);
        window->draw(spTadeusz.sprite);


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
        {
            Blade.setPosition(spTadeusz.sprite.getPosition());
            Blade.startRotating();
            Blade.shouldDraw=1;
            Blade.shouldRotate=1;
            Blade.angleRotate=30;
            attackTime=clock.getElapsedTime();
        }
        if((attackTime-clock.getElapsedTime()).asSeconds()>1)
            Blade.shouldRotate=0, Blade.shouldDraw = 0;

            if(Blade.shouldDraw)
            window->draw(Blade);
        window->draw(spWaterfall);

        if(Blade.shouldRotate)
            Blade.update(deltaTime);


    }


protected:
    sf::Font font;

    sf::Clock clock;
    sf::Time idleTime;
    sf::Time attackTime;

    Weapon Blade=Weapon(window);

    sf::Texture tempTex;

    sf::Texture texTadeusz;
    AnimatedSprite spTadeusz;

    Animation Idle;
    std::vector <sf::Texture> texIdle;
    Animation Run;
    std::vector <sf::Texture> texRun;



    sf::Sprite spSword;
    sf::Texture texSword;

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
