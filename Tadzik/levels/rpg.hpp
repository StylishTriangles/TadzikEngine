
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
#include <fstream>

class RPG : public Scene
{
public:
    RPG(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        : Scene(_name, mgr, w) {}

    enum Direction { None = 0, Down = 1, Up = -1, Right = 1, Left = -1 };

    class Entity
    {
    public:
        Entity(sf::Vector2f V, ARO::Anim* A)
        {
            aSprite.setAnimation(A);
            aSprite.setPosition(V);
        }
        Entity()
        {
        }
        ARO::AnimSprite aSprite;
        int health = 100;
        int damage = 10;
        float basespeed = 10;
        float speedX = 0;
        float speedY = 0;
        char dir = 'N';
        int side = Right;
        bool isAttacking;
        std::vector<sf::Vector2i> equipment;

        void draw(sf::RenderWindow* w)
        {
            w->draw(aSprite);
        }
        void move(sf::Vector2f a)
        {
            aSprite.move(a);
        }
        void move(float x, float y)
        {
            if (x == 0 || y == 0)
                aSprite.move(sf::Vector2f(x, y));
            else
                aSprite.move(
                    sf::Vector2f(x * (double)0.70710678, y * (double)0.70710678));
            //std::cout << dir << "\t" << speedY << "\n";
        }
        void changeAnimation(ARO::Anim* a)
        {
            aSprite.move(sf::Vector2f(aSprite.getAnim()->width-a->width, aSprite.getAnim()->height-a->height)*aSprite.getScale().x);
            std::cout << "\n" <<  aSprite.getAnim()->width-a->width << "\t" << aSprite.getAnim()->width-a->width;
            aSprite.setAnimation(a);
        }
        void AImove(const sf::Sprite s, RPG* erpeg)
        {
            float dist = erpeg->distance(aSprite, s);
            int x=0, y=0;
            aSprite.move(basespeed,0);
            if( erpeg->distance(aSprite,s)<dist ) x=1;
            aSprite.move(-2*basespeed,0);
            if(erpeg->distance(aSprite,s)<dist) x=-1;
            aSprite.move(basespeed,basespeed);
            if(erpeg->distance(aSprite,s)<dist) y=1;
            aSprite.move(0,-2*basespeed);
            if(erpeg->distance(aSprite,s)<dist) y=-1;
            aSprite.move(0,basespeed);
            speedX=x*basespeed;
            speedY=y*basespeed;
            erpeg->staticCollision(*this);
            aSprite.move(speedX,speedY);

        }
    };

    class Slice : public ARO::AnimSprite
    {
    public:
        Slice(sf::Vector2f V, ARO::Anim* A, int dmg, bool b=1)
        {
            setAnimation(A);
            setPosition(V);
            damage = dmg;
            isPlayer = b;
        }
        int damage = 50;
        bool isPlayer = 0;
        void collide(std::vector<Entity>& E)
        {
            for (int i = E.size()-1; i >= 0; i--)
                if (Collision::PixelPerfectTest(*this, E[i].aSprite))
                {
                    E[i].health-=damage;
                    E[i].aSprite.setColor(sf::Color(255,0,0));
                }
        }
    };

    class Chest: public sf::Sprite
    {
    public:
        Chest(sf::Vector2f V, const sf::Texture& T, int n = 5)
        {
            setPosition(V);
            setTexture(T);
            setScale(n,n);
        }
        int number = 3;
    };

    class Chestwo: public sf::Sprite
    {
    public:
        Chestwo(int x, int y, std::vector<sf::Vector2i> V, sf::Texture& t, int scale = 5)
        {
            items = V;
            setTexture(t);
            setScale(scale,scale);
            setPosition(sf::Vector2f(x*30,y*30));
        }
        std::vector <sf::Vector2i> items;

        void giveItem(sf::Vector2i V)
        {
            for(int i=items.size()-1; i>=0; i--)
                if(items[i].x==V.x)
                {
                    items[i].y+=V.y;
                    break;
                }
                else if(i==0)
                    items.push_back(V);
            std::sort(items.begin(), items.end(), [](sf::Vector2i a, sf::Vector2i b)
            {
                return b.x < a.x;
            });
        }
        void takeItem(sf::Vector2i V)
        {
            for(int i=items.size()-1; i>=0; i--)
                if(items[i].x==V.x)
                {
                    items[i].y-=V.y;
                    if(items[i].y<=0)
                        items.erase(items.begin()+i);
                    break;
                }
        }
    };
    class Item: public sf::Sprite
    {
    public:
        Item(int i, std::string n, const sf::Texture &tex, const sf::IntRect I, int v,int d=0, int scale=30)
        {
            setTexture(tex);
            setTextureRect(I);
            name=n;
            id=i;
            value=v;
            damage=d;
            setScale(scale,scale);
        }
        std::string name;
        int id;
        int value;
        int damage = 0;
    };

    void loadMap(sf::Image mapa, std::string s)
    {
        for (int i = 0; i < mapa.getSize().x; i++)
        {
            for (int j = 0; j < mapa.getSize().y; j++)
            {
                if (mapa.getPixel(i, j) == sf::Color(0, 0, 0))
                {
                    tempWall.setPosition(i * tilesize, j * tilesize);
                    spWall.push_back(tempWall);
                }
                else
                {
                    tempGrass.setPosition(i * tilesize, j * tilesize);
                    spGrass.push_back(tempGrass);

                }

                if (mapa.getPixel(i, j) == sf::Color(255,128,0))
                {
                    //   vecChest.push_back(Chest(sf::Vector2f(i*tilesize,j*tilesize),texChest));
                }
                if (mapa.getPixel(i, j) == sf::Color(0, 0, 255))
                {
                    Player.aSprite.setPosition(i * tilesize, j * tilesize);
                }

                if (mapa.getPixel(i, j) == sf::Color(255, 0, 0))
                {
                    vecSkeleton.push_back(Entity(sf::Vector2f(i*tilesize, j*tilesize), &enemySkeleton));
                    vecSkeleton[vecSkeleton.size()-1].aSprite.setScale(5,5);
                    vecSkeleton[vecSkeleton.size()-1].basespeed=2;
                }
            }
        }

        std::ifstream chests;
        chests.open(s);

        int x, y;
        while(chests >> x >> y)
        {
            std::vector <sf::Vector2i> V;
            sf::Vector2i Vi;
            while(1)
            {
                chests >> Vi.x >> Vi.y;
                if(Vi.x==-1)
                    break;
                V.push_back(Vi);
            }
            vecChest.push_back(Chestwo(x,y,V, texChest));
        }
    }

    float distance(sf::Sprite spA, sf::Sprite spB)
    {
        return sqrt((spA.getPosition().x - spB.getPosition().x) *
                    (spA.getPosition().x - spB.getPosition().x) +
                    (spA.getPosition().y - spB.getPosition().y) *
                    (spA.getPosition().y - spB.getPosition().y));
    }

    void staticCollision(Entity& E)
    {
        sf::FloatRect R = E.aSprite.getGlobalBounds();
        bool b = 0;
        int w;
        for (int i = 0; i < spWall.size(); i++)

            if (spWall[i].getGlobalBounds().intersects(R))
            {
                b = 1;
                w = i;
                break;
            }
        if (b)
        {
            E.speedX = 0;
            E.speedY = 0;
            E.move((spWall[w].getPosition().x - E.aSprite.getPosition().x) * 2,
                   (spWall[w].getPosition().y - E.aSprite.getPosition().y) * 2);
        }
        else
        {
            /// x
            R.left += E.speedX;
            for (int i = 0; i < spWall.size(); i++)
                if (spWall[i].getGlobalBounds().intersects(R))
                {
                    E.speedX = 0;
                    break;
                }
            R = E.aSprite.getGlobalBounds();
            /// y
            R.top += E.speedY;
            for (int i = 0; i < spWall.size(); i++)
                if (spWall[i].getGlobalBounds().intersects(R))
                {
                    E.speedY = 0;
                    break;
                }
        }
    }

    virtual void onSceneLoadToMemory()
    {

        window->setView(view);
        ///itemy
        texItemSheet.loadFromFile("files/textures/rpg/items.png");
        std::ifstream baseItem("files/maps/rpg/items.txt");
        int itemsInRow = 5;
        for(int j=0, sizeItem =texItemSheet.getSize().x/itemsInRow ;    j<texItemSheet.getSize().y/sizeItem;    j++)
        {
            int id, value, damage;
            std::string name;
            for(int i=0;    i<itemsInRow;   i++)
            {
                baseItem >> id >> name >> value >> damage;
              //  std::cout << id << name << value <<damage << std::endl;
                vecItem.push_back(Item(id,name,texItemSheet, sf::IntRect(i*sizeItem, j*sizeItem,sizeItem,sizeItem),value,damage));
            }
        }
        texInventoryBackground.loadFromFile("files/textures/rpg/inventoryBackground.png");
        inventoryBackground.setTexture(texInventoryBackground);
        inventoryBackground.setOrigin(texInventoryBackground.getSize().x*0.5,0.5*texInventoryBackground.getSize().y);
        inventoryBackground.setPosition(window->getSize().x/2,window->getSize().y/2);
        inventoryBackground.setScale(10,10);

        texUpRun.loadFromFile("files/textures/rpg/up/run.png");
        upRun.setSpriteSheet(&texUpRun, 17, sf::milliseconds(60));

        texDownRun.loadFromFile("files/textures/rpg/down/run.png");
        downRun.setSpriteSheet(&texDownRun, 17, sf::milliseconds(60));

        texRightRun.loadFromFile("files/textures/rpg/right/run.png");
        rightRun.setSpriteSheet(&texRightRun, 23, sf::milliseconds(60));

        texLeftRun.loadFromFile("files/textures/rpg/left/run.png");
        leftRun.setSpriteSheet(&texLeftRun, 23, sf::milliseconds(60));

        texDownAttack.loadFromFile("files/textures/rpg/down/attack.png");
        downAttack.setSpriteSheet(&texDownAttack, 30, sf::milliseconds(100));

        texUpAttack.loadFromFile("files/textures/rpg/up/attack.png");
        upAttack.setSpriteSheet(&texUpAttack, 24, sf::milliseconds(100));

        texRightAttack.loadFromFile("files/textures/rpg/right/attack.png");
        rightAttack.setSpriteSheet(&texRightAttack, 30, sf::milliseconds(100));

        texLeftAttack.loadFromFile("files/textures/rpg/left/attack.png");
        leftAttack.setSpriteSheet(&texLeftAttack, 30, sf::milliseconds(100));

        texSliceDown.loadFromFile("files/textures/rpg/down/projectile.png");
        sliceDown.setSpriteSheet(&texSliceDown, 50, sf::milliseconds(12));

        texSliceRight.loadFromFile("files/textures/rpg/right/projectile.png");
        sliceRight.setSpriteSheet(&texSliceRight, 66, sf::milliseconds(12));


        texSliceLeft.loadFromFile("files/textures/rpg/left/projectile.png");
        sliceLeft.setSpriteSheet(&texSliceLeft, 66, sf::milliseconds(12));

        texIdle.loadFromFile("files/textures/rpg/down/idle.png");
        Idle.setSpriteSheet(&texIdle, 15, sf::milliseconds(300));

        Player.aSprite.setAnimation(&Idle);
        Player.aSprite.setScale(5, 5);
        // Player.aSprite.setOrigin(sf::Vector2f(Player.aSprite.getTextureRect().width*0.5,Player.aSprite.getTextureRect().height*0.5));

        texWall.loadFromFile("files/textures/rpg/Wall.png");
        tempWall.setTexture(texWall);
        tempWall.setScale(1, 1);

        texSkeleton.loadFromFile("files/textures/rpg/enemyStand.png");
        enemySkeleton.setSpriteSheet(&texSkeleton, texSkeleton.getSize().x, sf::milliseconds(100));

        texChest.loadFromFile("files/textures/rpg/chest.png");

        texGrass.loadFromFile("files/textures/rpg/grass.png");
        tempGrass.setTexture(texGrass);

        texDoor.loadFromFile("files/textures/rpg/door.png");
        texDoorOpen.loadFromFile("files/textures/rpg/doorOpen.png");

        window->setMouseCursorVisible(0);
        texCrosshair.loadFromFile("files/textures/rpg/crosshair.png");
        spCrosshair.setTexture(texCrosshair);
        spCrosshair.setOrigin(spCrosshair.getTextureRect().height * 0.5,
                              spCrosshair.getTextureRect().width * 0.5);

        mapa.loadFromFile("files/maps/rpg/mapa1.png");
        chestPath="files/maps/rpg/chest1.txt";
        loadMap(mapa, chestPath);
    }

    virtual void onSceneActivate() {}

    void deliverEvent(sf::Event& event) {}

    virtual void draw(sf::Time deltaTime)
    {
        float dT = deltaTime.asMilliseconds();

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Tab))
        {   if(tab>1)
            tab=1;
            tab*=-1;
        }
        if(tab>0)
        {
            window->draw(inventoryBackground);
        if(tab>1)
            window->draw(vecItem[vecChest[tab-2].items[0].x]);
        }
        else
        {
            // Player.speedX=0, Player.speedY=0;
            /// Movement
            Player.speedX = Player.speedX * 0.5;
            Player.speedY = Player.speedY * 0.5;
            /// Kierunki
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
                    Player.isAttacking == false)
            {
                Player.isAttacking = true;
                if (Player.dir == 'U')
                {
                    Player.changeAnimation(&upAttack);
                    sliceVec.push_back(
                        Slice(Player.aSprite.getPosition(), &sliceRight, 10, true));
                }
                else if (Player.dir == 'R')
                {
                    Player.changeAnimation(&rightAttack);
                    sliceVec.push_back(
                        Slice(Player.aSprite.getPosition(), &sliceRight, 10, true));
                }
                else if (Player.dir == 'L')
                {
                    Player.changeAnimation(&leftAttack);
                    sliceVec.push_back(
                        Slice(Player.aSprite.getPosition(), &sliceLeft, 10, true));
                }
                else
                {
                    Player.changeAnimation(&downAttack);
                    sliceVec.push_back(
                        Slice(Player.aSprite.getPosition(), &sliceDown, 10, true));

                }

                Player.aSprite.reset();
                Player.aSprite.setLooped(false);

                sliceVec[sliceVec.size() - 1].setLooped(false);
                sliceVec[sliceVec.size() - 1].setScale(5, 5);
            }
            if (Player.aSprite.shouldDestroy())
            {
                Player.isAttacking = false;
                Player.aSprite.setLooped(true);
            }
            if (Player.isAttacking == false)
            {
                if (Player.dir != 'N')
                {
                    if ((Player.dir == 'U' || Player.dir == 'D') &&
                            std::abs(Player.speedY) < std::abs(Player.speedX))
                        Player.dir = 'N', Player.changeAnimation(&Idle);
                    if ((Player.dir == 'R' || Player.dir == 'L') &&
                            std::abs(Player.speedY) > std::abs(Player.speedX))
                        Player.dir = 'N', Player.changeAnimation(&Idle);
                    else if (std::abs(Player.speedX) < 0.5 && std::abs(Player.speedY) < 0.5)
                        Player.dir = 'N', Player.changeAnimation(&Idle);
                }
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
                {
                    Player.speedY -= 0.6 * Player.basespeed;
                    if (Player.dir == 'N' || Player.dir == 'D')
                    {
                        Player.dir = 'U';
                        Player.changeAnimation(&upRun);
                    }
                }
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
                {
                    Player.speedY += 0.6 * Player.basespeed;
                    if (Player.dir == 'N' || Player.dir == 'U')
                    {
                        Player.dir = 'D';
                        Player.changeAnimation(&downRun);
                    }
                }
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                {
                    Player.speedX -= 0.6 * Player.basespeed;
                    if (Player.dir == 'N' || Player.dir == 'R')
                    {
                        Player.dir = 'L';
                        Player.changeAnimation(&leftRun);
                    }
                }
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
                {
                    Player.speedX += 0.6 * Player.basespeed;
                    if (Player.dir == 'N' || Player.dir == 'L')
                    {
                        Player.dir = 'R';
                        Player.changeAnimation(&rightRun);
                    }
                }

                if (Player.speedX < -Player.basespeed) Player.speedX = -Player.basespeed;
                if (Player.speedY < -Player.basespeed) Player.speedY = -Player.basespeed;
                if (Player.speedX > Player.basespeed) Player.speedX = Player.basespeed;
                if (Player.speedY > Player.basespeed) Player.speedY = Player.basespeed;

                staticCollision(Player);
                if (Player.aSprite.getAnim() == &Idle)
                    Player.aSprite.update(dT);
                else if (((std::abs(Player.speedX) + std::abs(Player.speedY)) /
                          Player.basespeed) > 1)
                    Player.aSprite.update(dT);
                else
                    Player.aSprite.update(
                        dT * (std::abs(Player.speedX) + std::abs(Player.speedY)) /
                        (2 * Player.basespeed));
                Player.move(Player.speedX, Player.speedY);
                view.setCenter(Player.aSprite.getPosition());
                //window-> setView(view);
            }
            else
            {
                staticCollision(Player);
                if (Player.aSprite.shouldDestroy() == 0) Player.aSprite.update(dT);
                if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
                        sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
                    Player.dir = 'R';
                else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
                    Player.dir = 'L';
                else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
                    Player.dir = 'D';
                else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                          sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
                    Player.dir = 'U';
            }
            /// Kolizja gdzies tu

            if (Player.speedX < -Player.basespeed) Player.speedX = -Player.basespeed;
            if (Player.speedY < -Player.basespeed) Player.speedY = -Player.basespeed;
            if (Player.speedX > Player.basespeed) Player.speedX = Player.basespeed;
            if (Player.speedY > Player.basespeed) Player.speedY = Player.basespeed;

            spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

            // OBRACANIE



            // DRAW STARTS
            window-> clear(sf::Color());
            for (int i = 0; i < spGrass.size(); i++) window-> draw(spGrass[i]);
            for (int i = 0; i < spWall.size(); i++)  window-> draw(spWall[i]);


            window->draw(spCrosshair);

            for (int i = 0; i < vecChest.size(); i++)
            {
                window->draw(vecChest[i]);
            }
for(int i=0; i<vecChest.size();i++)
   std::cout << "arek", window->draw(vecChest[i]);
            Player.draw(window);

            // SKELETON MOVEMENT
            for (int i = vecSkeleton.size()-1; i>=0; i--)
            {
                if(vecSkeleton[i].health<=0)
                {


                    vecSkeleton.erase(vecSkeleton.begin()+i);
                    break;
                }
                vecSkeleton[i].AImove(Player.aSprite,this);
                vecSkeleton[i].draw(window);
                if(vecSkeleton[i].aSprite.getColor()==sf::Color(255,0,0))
                    vecSkeleton[i].aSprite.setColor(sf::Color(255,255,255));

            }

            for (int i = sliceVec.size() - 1; i >= 0; i--)
                if (sliceVec[i].isPlayer == true)
                {
                    if (Player.isAttacking == 0) sliceVec.erase(sliceVec.begin() + i);
                }
                else if (sliceVec[i].shouldDestroy())
                    sliceVec.erase(sliceVec.begin() + i);


            for (int i = 0; i < sliceVec.size(); i++)
            {
                sliceVec[i].update(dT);
                window->draw(sliceVec[i]);
                sliceVec[i].collide(vecSkeleton);
            }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                for(int i=0; i<vecChest.size();i++)
            if(Collision::BoundingBoxTest(Player.aSprite,vecChest[i]))
                tab=i+2;
            }
        }
    }

protected:
    int tab=1;

    sf::View view = sf::View(sf::FloatRect(0, 0, 1920, 1080));

    sf::Image mapa;
    std::string chestPath;

    sf::Texture texWall;
    std::vector<sf::Sprite> spWall;
    sf::Sprite tempWall;

    int tilesize = 30;

    sf::Texture texSkeleton;
    ARO::Anim enemySkeleton;
    std::vector<Entity> vecSkeleton;

    sf::Texture texChest;
    std::vector<Chestwo> vecChest;

    sf::Texture texGrass;
    std::vector<sf::Sprite> spGrass;
    sf::Sprite tempGrass;

    sf::Texture texDoor;
    sf::Texture texDoorOpen;
    std::vector<sf::Sprite> spDoor;
    sf::Sprite tempDoor;

    sf::Texture texCrosshair;
    sf::Sprite spCrosshair;

    std::vector<Slice> sliceVec;
    sf::Texture texSliceDown;
    ARO::Anim sliceDown;
    sf::Texture texSliceRight;
    ARO::Anim sliceRight;
    sf::Texture texSliceLeft;
    ARO::Anim sliceLeft;

    Entity Player;
    sf::Texture texIdle;
    ARO::Anim Idle;
    sf::Texture texDownRun;
    ARO::Anim downRun;
    sf::Texture texUpRun;
    ARO::Anim upRun;
    sf::Texture texRightRun;
    ARO::Anim rightRun;
    sf::Texture texLeftRun;
    ARO::Anim leftRun;
    sf::Texture texDownAttack;
    ARO::Anim downAttack;
    sf::Texture texUpAttack;
    ARO::Anim upAttack;
    sf::Texture texRightAttack;
    ARO::Anim rightAttack;
    sf::Texture texLeftAttack;
    ARO::Anim leftAttack;

    sf::Texture texItemSheet;
    std::vector<Item> vecItem;

    sf::Sprite inventoryBackground;
    sf::Texture texInventoryBackground;

};
#endif  // RPG
