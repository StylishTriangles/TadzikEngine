#ifndef MARIO_HPP
#define MARIO_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

struct positions {
    double left;
    double right;
    double top;
    double bottom;
};
class classBreakable {
public:
    classBreakable() {
        textures.clear();
    }
    void loadTextures(std::vector <sf::Texture>& t) {
        textures.clear();
        hits = t.size();
        for (int i=0; i<t.size(); i++) {
            textures.push_back(&t[i]);
        }
    }
    void addTexture(sf::Texture* texture) {
        textures.push_back(texture);
        hits++;
    }
    void setTexture(int i) {
        sprite.setTexture(*textures[i]);
    }
    bool hit() {
        totalHits++;
        if (hits<=totalHits) return 1;
        sprite.setTexture(*textures[totalHits]);
        return 0;
    }
    int left() { return sprite.getGlobalBounds().left;}
    int right() { return sprite.getGlobalBounds().left+sprite.getGlobalBounds().width;}
    int top() { return sprite.getGlobalBounds().top;}
    int bottom() { return sprite.getGlobalBounds().top+sprite.getGlobalBounds().height;}
    sf::Vector2f center() { return {sprite.getGlobalBounds().left+sprite.getGlobalBounds().width/2,
                                    sprite.getGlobalBounds().top+sprite.getGlobalBounds().height/2};}
    sf::Sprite sprite;
    std::vector <sf::Texture*> textures;
private:
    int hits = 0;
    int totalHits = 0;
};

class classMovingEntity {
public:
    double speedX = 0.01, speedY = 0;
    updateAnimation(float delta)    {aSprite.update(delta);};
    setAnimation(Animation* a)      {aSprite.setAnimation(a);}
    setOrigin(float x, float y)     {aSprite.sprite.setOrigin(x, y);}
    setOrigin(sf::Vector2f v)       {aSprite.sprite.setOrigin(v);}
    sf::FloatRect getGlobalBounds() {return aSprite.sprite.getGlobalBounds();}
    sf::IntRect getTextureRect()    {return aSprite.sprite.getTextureRect();}
    setScale(float x, float y)      {aSprite.sprite.setScale(x, y);}
    setPosition(float x, float y)   {aSprite.sprite.setPosition(x, y);}
    sf::Vector2f getScale()         {return aSprite.sprite.getScale();}
    sf::Vector2f getPosition()      {return aSprite.sprite.getPosition();}
    move (double x, double y)       {aSprite.move(x, y);}
    updatePos() {
        pos.top = getGlobalBounds().top;
        pos.bottom = getGlobalBounds().top+getGlobalBounds().height;
        pos.left = getGlobalBounds().left;
        pos.right = getGlobalBounds().left+getGlobalBounds().width;
        prevX = getPosition().x;
        prevY = getPosition().y;
        prevSpeedX = speedX;
        prevSpeedY = speedY;

    }
    update() {

    }
    double prevX, prevY;
    double prevSpeedX, prevSpeedY;
    positions pos;
    AnimatedSprite aSprite;
    bool isStanding = 0;
    bool isJumping = 0;
private:

};

class MARIO: public Scene{
public:
    MARIO(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}
    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }
        textScore.setFont(font);
        texBackground.loadFromFile("files/textures/mario/background.png");
        Background1.setTexture(texBackground);
        Background1.setScale(double(window->getSize().x)/double(Background1.getTextureRect().width),
                             double(window->getSize().y)/double(Background1.getTextureRect().height));
        Background2=Background1;
        Background2.move(window->getSize().x, 0);

        texFloorTile.loadFromFile("files/textures/mario/floor1.png"), FloorTile.setTexture(texFloorTile);

        texBreakableTile.resize(3);
        texBreakableTile[0].loadFromFile("files/textures/mario/breakable1.png");
        texBreakableTile[1].loadFromFile("files/textures/mario/breakable2.png");
        texBreakableTile[2].loadFromFile("files/textures/mario/breakable3.png");
        BreakableTile.loadTextures(texBreakableTile);
        BreakableTile.setTexture(0);

        texCoin.loadFromFile("files/textures/mario/coin1.png"), coinRotate.addFrame(AnimationFrame(&texCoin, 500));
        spCoin.setAnimation(&coinRotate);

        texEnemy1.loadFromFile("files/textures/mario/enemy1.png"), enemy1Stand.addFrame(AnimationFrame(&texEnemy1, 500));
        spEnemy1.setAnimation(&enemy1Stand);
        spEnemy1.setOrigin(0, spEnemy1.getTextureRect().height);
        spEnemy1.setScale(3, 3);


        mapa.loadFromFile("files/maps/mario/map1.png");
        loadMap();

        texPlayerRun.resize(4);
        texPlayerRun[0].loadFromFile("files/textures/universal/playerRun1.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun[0], 500));
        texPlayerRun[1].loadFromFile("files/textures/universal/playerRun2.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun[1], 500));
        texPlayerRun[2].loadFromFile("files/textures/universal/playerRun3.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun[2], 500));
        texPlayerRun[3].loadFromFile("files/textures/universal/playerRun4.png"), TadzikRun.addFrame(AnimationFrame(&texPlayerRun[3], 500));

        texPlayerJump.loadFromFile("files/textures/universal/playerJump.png"),   TadzikJump.addFrame(AnimationFrame(&texPlayerJump, 150));
        texPlayerStand.loadFromFile("files/textures/universal/playerStand.png"), TadzikStand.addFrame(AnimationFrame(&texPlayerStand, 150));
        texPlayerFall.loadFromFile("files/textures/universal/playerFall.png"),   TadzikFall.addFrame(AnimationFrame(&texPlayerFall, 150));

        spTadzik.setAnimation(&TadzikRun);
        spTadzik.setOrigin(sf::Vector2f(spTadzik.getTextureRect().width/2, spTadzik.getTextureRect().height));
        spTadzik.setScale(2, 2);

    }

    virtual void onSceneActivate() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void loadMap() {
        FloorTile.setScale((int)((double)window->getSize().y/(double)tilesPerY)/FloorTile.getGlobalBounds().width,
                           (int)((double)window->getSize().y/(double)tilesPerY)/FloorTile.getGlobalBounds().height);
        BreakableTile.sprite.setScale((int)((double)window->getSize().y/(double)tilesPerY)/BreakableTile.sprite.getGlobalBounds().width,
                                      (int)((double)window->getSize().y/(double)tilesPerY)/BreakableTile.sprite.getGlobalBounds().height);
        spCoin.sprite.setScale((int)((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getGlobalBounds().width,
                               (int)((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getGlobalBounds().height);
        int TileSize = FloorTile.getGlobalBounds().width;
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color::Black) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    floor.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color::White) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    hitboxlessBack.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 0, 0)) {
                    BreakableTile.sprite.setPosition(i*TileSize, j*TileSize);
                    breakable.push_back(BreakableTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 255, 0)) {
                    spCoin.setPosition(i*TileSize, j*TileSize);
                    coins.push_back(spCoin);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 255, 0)) {
                    spEnemy1.setPosition(i*TileSize, j*TileSize);
                    enemies1.push_back(spEnemy1);
                }
            }
        }
    }
    void gameOver() {
        onSceneLoadToMemory();
    }

    void jump() {
        spTadzik.speedY=-13;
        spTadzik.isStanding = false;
        spTadzik.isJumping = true;
        spTadzik.setAnimation(&TadzikJump);
    }
    void flip() {
        if (spTadzik.speedX!=0)
            spTadzik.setScale(abs(spTadzik.getScale().x)*sgn(spTadzik.speedX), spTadzik.getScale().y);
    }

    bool checkForStanding(sf::Sprite s1) {
            if (Collision::BoundingBoxTest(s1, spTadzik.aSprite.sprite)) {
                standingHeight = s1.getGlobalBounds().top;
            }
        }

    bool isInWindow(sf::Sprite s) {
        if (s.getGlobalBounds().left>window->getSize().x || s.getGlobalBounds().left+s.getGlobalBounds().width<0) return 0;
        else return 1;
    }

    void manageCollision(classMovingEntity& entity) {
        for (auto s:floor) {
            if (isInWindow(s) && Collision::BoundingBoxTest(entity.aSprite.sprite, s)) {
                if (s.getGlobalBounds().top+s.getGlobalBounds().height<entity.pos.top) {
                    entity.speedY = 0;
                    entity.setPosition(entity.getPosition().x,
                                        s.getGlobalBounds().top+s.getGlobalBounds().height+entity.getGlobalBounds().height);
                }
                if (s.getGlobalBounds().top>=entity.pos.bottom) {
                    entity.isStanding = true;
                    entity.setPosition(entity.getPosition().x, s.getGlobalBounds().top);
                    entity.speedY = 0;
                    if (entity.aSprite.currentAnimation!=&TadzikRun) entity.aSprite.setAnimation(&TadzikRun);
                }
                else if (s.getGlobalBounds().left>entity.pos.right-1) {
                    entity.setPosition(entity.prevX, entity.getPosition().y);
                    entity.speedX = 0;
                }
                else if (s.getGlobalBounds().left+s.getGlobalBounds().width<entity.pos.left+1) {
                    entity.setPosition(entity.prevX, entity.getPosition().y);
                    entity.speedX = 0;
                }
            }
        }
    }




    //}
    virtual void draw(double deltaTime) {
        spTadzik.updateAnimation(abs(spTadzik.speedX)*deltaTime);

        spTadzik.updatePos();
        //przewijanie i movement
        if ((spTadzik.getPosition().x>(double)window->getSize().x*(3.0/4.0) && speedX>0) ||
            (spTadzik.getPosition().x<(double)window->getSize().x*(1.0/5.0) && speedX<0 && floor[0].getPosition().x<0)) {
            for (int i=0; i<floor.size(); i++)              { floor[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessBack.size(); i++)     { hitboxlessBack[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessFront.size(); i++)    { hitboxlessFront[i].move(-speedX, 0);}
            for (int i=0; i<breakable.size(); i++)          { breakable[i].sprite.move(-speedX, 0);}
            for (int i=0; i<coins.size(); i++) {            coins[i].sprite.move(-speedX, 0);}
            for (int i=0; i<enemies1.size(); i++)           { enemies1[i].move(-speedX, 0);}
            spTadzik.pos.left-=speedX;
            spTadzik.pos.right-=speedX;
            spTadzik.prevX-=speedX;
            Background1.move(-speedX*parallax, 0);
            Background2.move(-speedX*parallax, 0);
        }
        else {
            spTadzik.move(speedX, 0);
        }
        spTadzik.speedY+=gravity;
        spTadzik.move(0, speedY);
        for (auto a:enemies1) {
            a.speedY+=gravity;
            a.move(0, a.speedY);
        }
        //ogarnianie kolizji
        for (auto s:floor) {
            if (isInWindow(s) && Collision::BoundingBoxTest(spTadzik.aSprite.sprite, s)) {
                if (s.getGlobalBounds().top+s.getGlobalBounds().height<spTadzik.pos.top) {
                    speedY = 0;
                    spTadzik.setPosition(spTadzik.getPosition().x,
                                        s.getGlobalBounds().top+s.getGlobalBounds().height+spTadzik.getGlobalBounds().height);
                }
                if (s.getGlobalBounds().top>=spTadzik.pos.bottom) {
                    spTadzik.isStanding = true;
                    spTadzik.setPosition(spTadzik.getPosition().x, s.getGlobalBounds().top);
                    speedY = 0;
                    if (spTadzik.aSprite.currentAnimation!=&TadzikRun)spTadzik.setAnimation(&TadzikRun);
                }
                else if (s.getGlobalBounds().left>spTadzik.pos.right-1) {
                    spTadzik.setPosition(spTadzik.prevX, spTadzik.getPosition().y);
                    speedX = 0;
                }
                else if (s.getGlobalBounds().left+s.getGlobalBounds().width<spTadzik.pos.left+1) {
                    spTadzik.setPosition(spTadzik.prevX, spTadzik.getPosition().y);
                    speedX = 0;
                }
            }
        }

        //ogarnianie breakable
        closestBreakable=-1;
        for (int i=breakable.size()-1; i>=0; --i) {
            if (isInWindow(breakable[i].sprite)) {
                classBreakable& s = breakable[i];
                if (Collision::BoundingBoxTest(spTadzik.aSprite.sprite, s.sprite)) {
                    if (s.bottom()<spTadzik.pos.top) {
                        speedY = 0;
                        spTadzik.setPosition(spTadzik.getPosition().x,
                                                    s.bottom()+spTadzik.getGlobalBounds().height);
                        if (closestBreakable==-1) {
                            closestBreakable = i;
                        }
                        else if (abs(spTadzik.getPosition().x-breakable[closestBreakable].center().x) >
                                 abs(spTadzik.getPosition().x-breakable[i].center().x)) {
                                    closestBreakable = i;
                                }
                    }
                    if (s.top()>=spTadzik.pos.bottom) {
                        spTadzik.isStanding = true;
                        spTadzik.setPosition(spTadzik.getPosition().x, s.top());
                        speedY = 0;
                        if (spTadzik.aSprite.currentAnimation!=&TadzikRun)spTadzik.setAnimation(&TadzikRun);
                    }
                    else if (s.left()>spTadzik.pos.right-1) {
                        spTadzik.setPosition(spTadzik.prevX, spTadzik.getPosition().y);
                        speedX = 0;
                    }
                    else if (s.right()<spTadzik.pos.left+1) {
                        spTadzik.setPosition(spTadzik.prevX, spTadzik.getPosition().y);
                        speedX = 0;
                    }
                }
            }
        }
        if (breakable.size()>0 && closestBreakable!=-1) {
            if (breakable[closestBreakable].hit()) {
                breakable.erase(breakable.begin()+closestBreakable);
            }
        }

        //ogarnianie monet
        if (coins.size()>0) {
            for (int i=coins.size(); i>=0; --i) {
                if (Collision::PixelPerfectTest(coins[i].sprite, spTadzik.aSprite.sprite)) {
                    score++;
                    coins.erase(coins.begin()+i);
                }
            }
        }

        if (spTadzik.isJumping) {
            if (speedY>0) {
                spTadzik.isJumping = false;
                spTadzik.setAnimation(&TadzikFall);
            }
        }
        speedX*=0.9;

        if (abs(speedX)<0.01 && spTadzik.isStanding) {
            spTadzik.setAnimation(&TadzikStand);
        }
        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -speedX<maxSpeed) {
            speedX-=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && speedX<maxSpeed) {
            speedX+=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && spTadzik.isStanding) {
            jump();
        }

        //gameover
        if (spTadzik.getGlobalBounds().top>window->getSize().y) {
            floor.clear();
            gameOver();
        }

        //fixowanie backgroundu
        if (-Background1.getGlobalBounds().left > window->getSize().x) Background1.move(2*(int)window->getSize().x, 0);
        if (-Background2.getGlobalBounds().left > window->getSize().x) Background2.move(2*(int)window->getSize().x, 0);
        if (Background1.getGlobalBounds().left > window->getSize().x) Background1.move(-2*(int)window->getSize().x, 0);
        if (Background2.getGlobalBounds().left > window->getSize().x) Background2.move(-2*(int)window->getSize().x, 0);

        textScore.setString(Utils::stringify(score));
        //rysowanie
        if (Utils::sgn(spTadzik.speedX)!=Utils::sgn(spTadzik.prevSpeedX) && Utils::sgn(spTadzik.speedX)!=0) {
            flip();
        }
        window->clear();
        window->draw(Background1);
        window->draw(Background2);
        for (auto a:floor) {
            window->draw(a);
        }
        for (auto a:hitboxlessBack) {
            if (isInWindow(a)) window->draw(a);
        }
        window->draw(spTadzik.aSprite.sprite);
        for (auto a:hitboxlessFront) { if (isInWindow(a)) window->draw(a);}
        for (auto a:breakable) { if (isInWindow(a.sprite)) window->draw(a.sprite);}
        for (auto a:coins) { window->draw(a.sprite);}
        for (auto a:enemies1) { window->draw(a.aSprite.sprite);}
        window->draw(textScore);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texPlayerStand;
    sf::Texture texFloorTile;
    sf::Texture texCoin;
    sf::Texture texEnemy1;

    std::vector <sf::Texture> texBreakableTile;
    std::vector <sf::Texture> texPlayerRun;
    sf::Texture texPlayerJump;
    sf::Texture texPlayerFall;
    //sf::Texture texHealthBar;

    sf::Image mapa;

    classMovingEntity spTadzik, spEnemy1;
    AnimatedSprite spCoin;
    Animation TadzikRun, TadzikStand, TadzikJump, TadzikFall, coinRotate, enemy1Stand;

    sf::Sprite Background1;
    sf::Sprite Background2;
    sf::Sprite FloorTile;

    std::vector <sf::Sprite> floor;
    std::vector <sf::Sprite> hitboxlessFront;
    std::vector <sf::Sprite> hitboxlessBack;
    std::vector <AnimatedSprite> coins;
    std::vector <classMovingEntity> enemies1;

    std::vector <classBreakable> breakable;
    classBreakable BreakableTile;

    double& speedX = spTadzik.speedX;
    double& speedY = spTadzik.speedY;
    double maxSpeed = 10;
    double gravity = 0.5;
    double standingHeight = 0;
    double parallax = 0.66;
    double timeLeft = 30;

    //double prevLeft, prevRight, prevTop, prevBot, prevX, prevY, prevSpeedX = 0.01, prevSpeedY = 0;

    int onTile = 0;
    int tilesPerY = 20;
    int closestBreakable = -1;
    int score = 0;
    //bool isStanding = false;
    //bool isJumping = false;

    sf::Text textScore;
    sf::Font font;
};
#endif //mario
