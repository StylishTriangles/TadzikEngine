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
    {
    public:
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

    class AnimDomin: public ARO::Anim
    {
    public:
        std::vector <sf::Vector2f> pointOrigin;
        void setPoints()
        {
            sf::Image img = spriteSheet->copyToImage();

            for(int i = 0; i<frames ; i++)
                for(int x = 0; x<width; x++)
                    for(int y=0; y<height; y++)
                        if (img.getPixel(x+i*width, y)==sf::Color(1, 1, 1, 1))
                            pointOrigin.push_back(sf::Vector2f(x,y));
        }
    };

    class Player
    {
    public:
        ARO::AnimSprite Body;
        ARO::AnimSprite Legs;
        std::vector <sf::Vector2f> pointBody;
        std::vector <sf::Vector2f> pointLegs;

        void setAnimDomin(AnimDomin* a, bool b)
        {
            if(b)
                Body.setAnimation(a), pointBody = a->pointOrigin;
            else
                Legs.setAnimation(a), pointLegs = a->pointOrigin;
        }

        void move(sf::Vector2f offset)
        {
            Body.move(offset);
            Legs.move(offset);
        }

        void move(double y, double x)
        {
            Body.move(sf::Vector2f(y,x));
            Legs.move(sf::Vector2f(y,x));

        }

        void draw(sf::RenderWindow* w)
        {
            w->draw(Legs);
            w->draw(Body);

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
                    Tadeusz.Legs.setPosition(i*tilesize,j*tilesize);
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



    void handleCollision(Player* s1, std::vector <sf::Sprite>& walls) {
        for (unsigned int i=0; i<walls.size(); ++i) {
            sf::FloatRect intersection;
            if (Collision::PixelPerfectTest(walls[i], s1->Legs) && s1->Legs.getGlobalBounds().intersects(walls[i].getGlobalBounds(), intersection)) {
                sf::Vector2f direction = walls[i].getPosition() - s1->Legs.getPosition();
                sf::Vector2f offset;
                // X collision
                if (abs(direction.x) > abs(direction.y))
                    offset.x = ((direction.x<0)?-1:1)*intersection.width;
                // Y collision
                if (abs(direction.x) < abs(direction.y))
                    offset.y = ((direction.y<0)?-1:1)*intersection.height;
                s1->move(offset);
            }
        }
    }

    virtual void onSceneLoadToMemory()
    {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf"))
        {
            std::cout << "cannot load font\n";

        }
        ///ANIMDOMIN
        texBody.loadFromFile("files/textures/rpg/test/body.png");
        texLegs.loadFromFile("files/textures/rpg/test/idle.png");
        testBody.setSpriteSheet(&texBody, 15, 500);
        testLegs.setSpriteSheet(&texLegs, 15, 150);
        testBody.setPoints();
        testLegs.setPoints();
        Tadeusz.setAnimDomin(&testBody,1);
        Tadeusz.setAnimDomin(&testLegs,0);
        Tadeusz.Body.setScale(5,5);
        Tadeusz.Legs.setScale(5,5);


        texAttack.loadFromFile("files/textures/rpg/test/attack.png");
        Attack.setSpriteSheet(&texAttack,30,250);
        Attack.setPoints();
        ///

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
        Tadeusz.Body.update(deltaTime);
        Tadeusz.Legs.update(deltaTime);


//MOVEMENT
        /*   if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
           {
               offset += sf::Vector2f(-tilesize*0.1, 0);
               if(spTadeusz.getAnimation()!=&Run)
                   spTadeusz.setAnimation(&Run);
               idleTime = clock.getElapsedTime();
           }
           */

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            Tadeusz.Legs.move(0,-10);

        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            Tadeusz.Legs.move(0,10);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            Tadeusz.Legs.move(-10,0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            Tadeusz.Legs.move(10,0);
        }
handleCollision(&Tadeusz, spWall);
handleCollision(&Tadeusz, spEnemy);
        /*     if((clock.getElapsedTime()-idleTime).asSeconds()>0.2)
                 if(spTadeusz.getAnimation()!=&Idle)
                     spTadeusz.setAnimation(&Idle); */
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::H) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            Tadeusz.setAnimDomin(&Attack,1);
        }

///Kolizja !!!


        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

//OBRACANIE        spTadeusz.sprite.setRotation(90+180/M_PI*atan2(sf::Mouse::getPosition(*window).y-spTadeusz.sprite.getPosition().y, sf::Mouse::getPosition(*window).x-spTadeusz.sprite.getPosition().x));

//WATERFALL RAK
        spWaterfall.setTextureRect(sf::IntRect(0, -w, texWaterfall.getSize().x, tilesize*2 ));
        w++;


//SKELETON MOVEMENT

        for(int i = 0; i < spEnemy.size(); i++)
            spEnemy[i].move(sf::Vector2f(Utils::randInt(-50,50)*tilesize*0.001,Utils::randInt(-50,50)*tilesize*0.001));


///DOORS
        /*      for(int i=0; i<spDoor.size(); i++)
                  if(Collision::BoundingBoxTest(spTadeusz.sprite,spDoor[i]))
                      spDoor[i].setTexture(texDoorOpen);
                  else spDoor[i].setTexture(texDoor);
        */
//DRAW STARTS
        window->clear(sf::Color());
        for(int i = 0; i < spGrass.size(); i++)
            window->draw(spGrass[i]);
        for(int i = 0; i < spWall.size(); i++)
            window->draw(spWall[i]);
        //  for(int i = 0; i < spDoor.size(); i++)
        //      window->draw(spDoor[i]);
        for(int i = 0; i < spEnemy.size(); i++)
            window->draw(spEnemy[i]);

        window->draw(spWaterfall);

        window->draw(spCrosshair);


        Tadeusz.Body.setPosition(
            Tadeusz.Legs.getPosition()+Tadeusz.pointLegs[Tadeusz.Legs.currentFrame]*Tadeusz.Legs.getScale().x-Tadeusz.Body.getScale().x*Tadeusz.pointBody[Tadeusz.Body.currentFrame]+sf::Vector2f(0,Tadeusz.Body.getScale().x));
        Tadeusz.draw(window);





    }


protected:
    sf::Font font;

    sf::Clock clock;
    sf::Time idleTime;
    sf::Time attackTime;

    sf::Texture tempTex;

    Player Tadeusz;

    AnimDomin testLegs;
    AnimDomin testBody;
    sf::Texture texLegs;
    sf::Texture texBody;

    AnimDomin Attack;
    sf::Texture texAttack;

    sf::Image mapa;

    sf::Texture texWall;
    std::vector <sf::Sprite> spWall;
    sf::Sprite tempWall;

    int tilesize = 30;

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
};
#endif //RPG
