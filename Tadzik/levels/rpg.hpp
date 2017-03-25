#ifndef RPG_HPP
#define RPG_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

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

    enum RunDirection
    {
        None, Right, Left, Up, Down
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

    class TwoSpriteEntity
    {
    public:
        ARO::AnimSprite Body;
        ARO::AnimSprite Legs;
        std::vector <sf::Vector2f> pointBody;
        std::vector <sf::Vector2f> pointLegs;
        sf::Vector2f offset;
        float speed = 10;
        RunDirection direction;
        bool isAttacking;

        void changeDirection(RunDirection d)
        {
            //player.direction
        }

        void setAnimDomin(AnimDomin* a, bool b)
        {
            if(b)   Body.setAnimation(a), pointBody = a->pointOrigin;
            else    Legs.setAnimation(a), pointLegs = a->pointOrigin;
        }
        void changeAnimDomin(AnimDomin *a, bool b)
        {
            if(b)
            {
                if(Body.getAnim()!=a)
                {
                    Body.setAnimation(a);
                    pointBody = a->pointOrigin;
                }
            }
            else   if(Legs.getAnim()!=a)
                Legs.setAnimation(a), pointLegs = a->pointOrigin;
        }


        void draw(sf::RenderWindow* w)
        {
            w->draw(Legs);
            w->draw(Body);
        }
        void move(sf::Vector2f a)
        {
            if(!(a.x == 0 || a.y == 0)) a*=(float)0.70710678118;
            Legs.move(a*speed);
            offset = sf::Vector2f(0,0);
        }
        void move(double y, double x)
        {
            Legs.move(sf::Vector2f(y,x));
        }
    };

    void loadMap(sf::Image mapa)
    {
        for (int i=0; i<mapa.getSize().x; i++)
        {
            for (int j=0; j<mapa.getSize().y; j++)
            {
                if (mapa.getPixel(i, j)==sf::Color(0, 0, 0))
                {
                    tempWall.setPosition(i*tilesize,j*tilesize);
                    spWall.push_back(tempWall);
                }
                else
                {
                    tempGrass.setPosition(i*tilesize,j*tilesize);
                    spGrass.push_back(tempGrass);
                }

                if (mapa.getPixel(i, j)==sf::Color(0, 0, 255))
                {
                    Player.Legs.setPosition(i*tilesize,j*tilesize);
                }

                if (mapa.getPixel(i, j)==sf::Color(255, 0, 0))
                {
                    tempEnemy.setPosition(i*tilesize,j*tilesize);
                    spEnemy.push_back(tempEnemy);
                }
                if(0)
                {
                    spWaterfall.setPosition(i*tilesize, j*tilesize);
                }
            }
        }
        std::cout << "success";
    }

    float distance(sf::Sprite spA, sf::Sprite spB)
    {
        return sqrt((spA.getPosition().x-spB.getPosition().x)*(spA.getPosition().x-spB.getPosition().x)+(spA.getPosition().y-spB.getPosition().y)*(spA.getPosition().y-spB.getPosition().y));
    }



    void handleCollision(TwoSpriteEntity* s1, std::vector <sf::Sprite>& walls)
    {
        for (unsigned int i=0; i<walls.size(); ++i)
        {
            sf::FloatRect intersection;
            if (Collision::PixelPerfectTest(walls[i], s1->Legs) && s1->Legs.getGlobalBounds().intersects(walls[i].getGlobalBounds(), intersection))
            {
                sf::Vector2f direction = walls[i].getPosition() - s1->Legs.getPosition();
                sf::Vector2f offset;
                // X collision
                if (std::abs(direction.x) > std::abs(direction.y))
                    offset.x = ((direction.x<0)?-1:1)*intersection.width;
                // Y collision
                if (std::abs(direction.x) < std::abs(direction.y))
                    offset.y = ((direction.y<0)?-1:1)*intersection.height;
                s1->move(offset);
            }
        }
    }

    virtual void onSceneLoadToMemory()
    {
        window->setView(view);

        ///ANIMDOMIN
        texBody.loadFromFile("files/textures/rpg/test/body.png");
        texLegs.loadFromFile("files/textures/rpg/test/idle.png");
        testBody.setSpriteSheet(&texBody, 15, sf::milliseconds(600));
        testLegs.setSpriteSheet(&texLegs, 15, sf::milliseconds(300));
        testBody.setPoints();
        testLegs.setPoints();
        Player.setAnimDomin(&testBody,1);
        Player.setAnimDomin(&testLegs,0);
        Player.Body.setScale(5,5);
        Player.Legs.setScale(5,5);

        texIdleSword.loadFromFile("files/textures/rpg/idleSword.png");
        IdleSword.setSpriteSheet(&texIdleSword,28,sf::milliseconds(600));
        IdleSword.setPoints();
        Player.setAnimDomin(&IdleSword,1);

        texRun[Right].loadFromFile( "files/textures/rpg/run/right.png");
        texRun[Left].loadFromFile(  "files/textures/rpg/run/left.png");
        texRun[Down].loadFromFile(  "files/textures/rpg/run/down.png");
        texRun[Up].loadFromFile(    "files/textures/rpg/run/up.png");
        Run[Right].setSpriteSheet(  &texRun[Right], 25, sf::milliseconds(200)),   Run[Right].setPoints();
        Run[Left].setSpriteSheet(   &texRun[Left],  25, sf::milliseconds(200));
        Run[Left].setPoints();
        Run[Down].setSpriteSheet(   &texRun[Down],  15, sf::milliseconds(200));
        Run[Down].setPoints();
        Run[Up].setSpriteSheet(     &texRun[Up],    15, sf::milliseconds(200));
        Run[Up].setPoints();

        texAttack.loadFromFile("files/textures/rpg/attackDown.png");
        Attack.setSpriteSheet(&texAttack,35,sf::milliseconds(70));
        Attack.setPoints();

        texWall.loadFromFile("files/textures/rpg/Wall.png");
        tempWall.setTexture(texWall);
        tempWall.setScale(1, 1);

        texEnemy.loadFromFile("files/textures/rpg/enemyStand.png");
        tempEnemy.setTexture(texEnemy);
        tempEnemy.setOrigin(tempEnemy.getTextureRect().width*0.5, tempEnemy.getTextureRect().height*0.5);
        tempEnemy.setScale(5,5);

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
        loadMap(mapa);
    }

    virtual void onSceneActivate()
    {

    }

    void deliverEvent(sf::Event& event)
    {

    }

    virtual void draw(double deltaTime)
    {
        Player.Body.update(deltaTime);
        Player.Legs.update(deltaTime);
Player.offset = sf::Vector2f(0,0);

        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        {
            //if(direction==None)
                Player.changeDirection(Up);

            Player.offset+=sf::Vector2f(0,-1);
            Player.changeAnimDomin(&Run[Up],0);

        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        {
            Player.offset+=sf::Vector2f(0,1);
            Player.changeAnimDomin(&Run[Down],0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        {
            Player.offset+=sf::Vector2f(1,0);
            Player.changeAnimDomin(&Run[Right],0);
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        {
            Player.offset+=sf::Vector2f(-1,0);
            Player.changeAnimDomin(&Run[Left],0);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K) )
        {
            Player.Body.setScale(Player.Body.getScale()+sf::Vector2f(  0.1,  0.1));
            Player.Legs.setScale(Player.Legs.getScale()+sf::Vector2f(  0.1,  0.1));
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::L) )
        {
            Player.Body.setScale(Player.Body.getScale()-sf::Vector2f(  0.1,  0.1));
            Player.Legs.setScale(Player.Legs.getScale()-sf::Vector2f(  0.1,  0.1));
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space))
            Player.changeAnimDomin(&Attack,1);

        if(Player.Body.getAnim()==&Attack)
            if(Player.Body.getLoops()>0) Player.setAnimDomin(&IdleSword,1), Player.Body.reset();


///Kolizja !!!
        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

//OBRACANIE        spPlayer.sprite.setRotation(90+180/M_PI*atan2(sf::Mouse::getPosition(*window).y-spPlayer.sprite.getPosition().y, sf::Mouse::getPosition(*window).x-spPlayer.sprite.getPosition().x));

//WATERFALL RAK
        spWaterfall.setTextureRect(sf::IntRect(0, -w, texWaterfall.getSize().x, tilesize*2 ));
        w++;

//SKELETON MOVEMENT
        for(int i = 0; i < spEnemy.size(); i++)
            spEnemy[i].move(sf::Vector2f(Utils::randInt(-50,50)*tilesize*0.001,Utils::randInt(-50,50)*tilesize*0.001));

///DOORS
        /*      for(int i=0; i<spDoor.size(); i++)
                  if(Collision::BoundingBoxTest(spPlayer.sprite,spDoor[i]))
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

        Player.move(Player.offset);
        Player.Body.setPosition(
            Player.Legs.getPosition()
            +Player.pointLegs[Player.Legs.currentFrame]*Player.Legs.getScale().x
            -Player.pointBody[Player.Body.currentFrame]*Player.Body.getScale().x
            +sf::Vector2f(0, 2*Player.Body.getScale().x));
        Player.draw(window);

        view.setCenter(Player.Legs.getPosition());
        window->setView(view);
    }


protected:
    sf::View view = sf::View( sf::FloatRect(0, 0, 1280, 720) );

    sf::Clock clock;
    sf::Time idleTime;
    sf::Time attackTime;

    sf::Texture tempTex;

    TwoSpriteEntity Player;

    AnimDomin testLegs;
    AnimDomin testBody;
    sf::Texture texLegs;
    sf::Texture texBody;

    sf::Texture texRun[4];
    AnimDomin Run[4];

    AnimDomin Attack;
    sf::Texture texAttack;

    AnimDomin IdleSword;
    sf::Texture texIdleSword;

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
