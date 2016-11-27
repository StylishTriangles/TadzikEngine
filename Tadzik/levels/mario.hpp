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

enum animationType {
    STAND, RUN, JUMP, FALL
};

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
    void loadAnimations(std::vector <Animation*> a) { animations = a;}
    double speedX = 0.01, speedY = 0;
    void updateAnimation(float delta)    {aSprite.update(delta);};
    void setAnimation(Animation* a)      {aSprite.setAnimation(a);}
    void changeAnimation(animationType a)          {aSprite.setAnimation(animations[a]), currentAnimation = a;}
    void addAnimation(Animation* a)      {animations.push_back(a);}
    void setOrigin(float x, float y)     {aSprite.sprite.setOrigin(x, y);}
    void setOrigin(sf::Vector2f v)       {aSprite.sprite.setOrigin(v);}
    sf::FloatRect getGlobalBounds() {return aSprite.sprite.getGlobalBounds();}
    sf::IntRect getTextureRect()    {return aSprite.sprite.getTextureRect();}
    void setScale(float x, float y)      {aSprite.sprite.setScale(x, y);}
    void setPosition(float x, float y)   {aSprite.sprite.setPosition(x, y);}
    sf::Vector2f getScale()         {return aSprite.sprite.getScale();}
    sf::Vector2f getPosition()      {return aSprite.sprite.getPosition();}
    void move (double x, double y)       {aSprite.move(x, y);}
    void updatePrev() {
        pos.top = getGlobalBounds().top;
        pos.bottom = getGlobalBounds().top+getGlobalBounds().height;
        pos.left = getGlobalBounds().left;
        pos.right = getGlobalBounds().left+getGlobalBounds().width;
        prevX = getPosition().x;
        prevY = getPosition().y;
        prevSpeedX = speedX;
        prevSpeedY = speedY;

    }
    void flipSprite() {
        setScale(abs(getScale().x)*Utils::sgn(speedX), getScale().y);
    }
    void jump() {
        speedY=-13;
        isStanding = false;
        isJumping = true;
        changeAnimation(JUMP);
    }
    void update() {
        speedY+=0.5;
        move(0, speedY);

        //flip
        if (Utils::sgn(speedX)!=Utils::sgn(prevSpeedX) && Utils::sgn(speedX)!=0)
            flipSprite();

        if (isJumping && speedY>0) {
            isJumping = false;
            changeAnimation(FALL);
        }
        speedX*=0.9;

        if (abs(speedX)<0.1 && isStanding && currentAnimation!=STAND) {
            changeAnimation(STAND);
        }
        if (abs(speedX)>0.1 && isStanding && currentAnimation!=RUN) {
            changeAnimation(RUN);
        }

    }
    double prevX, prevY;
    double prevSpeedX, prevSpeedY;
    positions pos;
    AnimatedSprite aSprite;
    std::vector <Animation*> animations;
    bool isStanding = 0;
    bool isJumping = 0;
    animationType currentAnimation = STAND;
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

        texPowerup.loadFromFile("files/textures/mario/powerup.png"), powerupAnim.addFrame(AnimationFrame(&texPowerup, 500));
        spPowerup.setAnimation(&powerupAnim);

        texEnemy1Stand.loadFromFile("files/textures/mario/enemy1Stand.png"), enemy1Stand.addFrame(AnimationFrame(&texEnemy1Stand, 500));
        texEnemy1Run.loadFromFile("files/textures/mario/enemy1Run.png"), enemy1Run.addFrame(AnimationFrame(&texEnemy1Run, 500));
        spEnemy1.addAnimation(&enemy1Stand);
        spEnemy1.addAnimation(&enemy1Run);

        spEnemy1.setAnimation(&enemy1Stand);
        spEnemy1.setOrigin(spEnemy1.getTextureRect().width/2, spEnemy1.getTextureRect().height);
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

        texPlayerMiniRun.resize(4);
        texPlayerMiniRun[0].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikMiniRun.addFrame(AnimationFrame(&texPlayerMiniRun[0], 500));
        texPlayerMiniRun[1].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikMiniRun.addFrame(AnimationFrame(&texPlayerMiniRun[1], 500));
        texPlayerMiniRun[2].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikMiniRun.addFrame(AnimationFrame(&texPlayerMiniRun[2], 500));
        texPlayerMiniRun[3].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikMiniRun.addFrame(AnimationFrame(&texPlayerMiniRun[3], 500));

        texPlayerMiniJump.loadFromFile("files/textures/mario/playerSmallJump.png"),   TadzikMiniJump.addFrame(AnimationFrame(&texPlayerMiniJump, 150));
        texPlayerMiniStand.loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikMiniStand.addFrame(AnimationFrame(&texPlayerMiniStand, 150));
        texPlayerMiniFall.loadFromFile("files/textures/mario/playerSmallStand.png"),   TadzikMiniFall.addFrame(AnimationFrame(&texPlayerMiniFall, 150));

        animTadzikMedium.push_back(&TadzikStand);
        animTadzikMedium.push_back(&TadzikRun);
        animTadzikMedium.push_back(&TadzikJump);
        animTadzikMedium.push_back(&TadzikFall);


        animTadzikSmall.push_back(&TadzikMiniStand);
        animTadzikSmall.push_back(&TadzikMiniRun);
        animTadzikSmall.push_back(&TadzikMiniJump);
        animTadzikSmall.push_back(&TadzikMiniFall);

        spTadzik.loadAnimations(animTadzikSmall);

        spTadzik.changeAnimation(FALL);
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
        FloorTile.setScale(((double)window->getSize().y/(double)tilesPerY)/FloorTile.getGlobalBounds().width,
                           ((double)window->getSize().y/(double)tilesPerY)/FloorTile.getGlobalBounds().height);
        BreakableTile.sprite.setScale(((double)window->getSize().y/(double)tilesPerY)/BreakableTile.sprite.getGlobalBounds().width,
                                      ((double)window->getSize().y/(double)tilesPerY)/BreakableTile.sprite.getGlobalBounds().height);
        spCoin.sprite.setScale(((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getGlobalBounds().width,
                               ((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getGlobalBounds().height);
        int TileSize = FloorTile.getGlobalBounds().width;
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color(0, 0, 0)) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    floor.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 255, 255)) {
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
                else if(mapa.getPixel(i, j)==sf::Color(0, 255, 255)) {
                    spPowerup.setPosition(i*TileSize, j*TileSize);
                    powerups.push_back(spPowerup);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*TileSize, j*TileSize);
                }
            }
        }
    }

    void gameOver() {
        sf::Text t;
        t.setString("YOU SUCK");
        t.setPosition(100, 100);
        window->draw(t);
        floor.clear();
        enemies1.clear();
        breakable.clear();
        hitboxlessBack.clear();
        hitboxlessFront.clear();
        coins.clear();
        loadMap();
        //spTadzik.setPosition(100, 10);
    }

    bool isActive(sf::Sprite s, double multiplier = 1) {
        if (s.getGlobalBounds().left>window->getSize().x*multiplier ||
            s.getGlobalBounds().left+s.getGlobalBounds().width<(1.0-multiplier)*window->getSize().x) return 0;
        else return 1;
    }

    void manageCollision(classMovingEntity& entity, sf::Sprite s) {
        if (isActive(s, 1.2) && Collision::BoundingBoxTest(entity.aSprite.sprite, s)) {
            if (s.getGlobalBounds().top+s.getGlobalBounds().height<entity.pos.top) {
                entity.speedY = 0;
                entity.setPosition(entity.getPosition().x,
                                    s.getGlobalBounds().top+s.getGlobalBounds().height+entity.getGlobalBounds().height);
            }
            if (s.getGlobalBounds().top>=entity.pos.bottom) {
                entity.isStanding = true;
                entity.setPosition(entity.getPosition().x, s.getGlobalBounds().top);
                entity.speedY = 0;
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

    virtual void draw(double deltaTime) {
        spTadzik.updatePrev();
        spTadzik.updateAnimation(abs(spTadzik.speedX)*deltaTime);

        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].aSprite.sprite, 1.1)) {
                enemies1[i].updatePrev();
                enemies1[i].updateAnimation(enemies1[i].speedX*deltaTime);
            }
        }

        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -speedX<maxSpeed) {
            speedX-=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && speedX<maxSpeed) {
            speedX+=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && spTadzik.isStanding) {
            spTadzik.jump();
        }

        //przewijanie i movement
        if ((spTadzik.getPosition().x>(double)window->getSize().x*(3.0/4.0) && speedX>0) ||
            (spTadzik.getPosition().x<(double)window->getSize().x*(1.0/5.0) && speedX<0 && floor[0].getPosition().x<0)) {
            for (int i=0; i<floor.size(); i++)              { floor[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessBack.size(); i++)     { hitboxlessBack[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessFront.size(); i++)    { hitboxlessFront[i].move(-speedX, 0);}
            for (int i=0; i<breakable.size(); i++)          { breakable[i].sprite.move(-speedX, 0);}
            for (int i=0; i<coins.size(); i++)              { coins[i].sprite.move(-speedX, 0);}
            for (int i=0; i<powerups.size(); i++)              { powerups[i].sprite.move(-speedX, 0);}
            for (int i=0; i<enemies1.size(); i++)           { enemies1[i].move(-speedX, 0), enemies1[i].updatePrev();}
            spTadzik.pos.left-=speedX;
            spTadzik.pos.right-=speedX;
            spTadzik.prevX-=speedX;
            Background1.move(-speedX*parallax, 0);
            Background2.move(-speedX*parallax, 0);
        }
        else {
            spTadzik.move(speedX, 0);
        }
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].aSprite.sprite, 1.1))
                enemies1[i].move(enemies1[i].speedX, 0);
        }
        spTadzik.update();
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].aSprite.sprite, 1.1)) {
                enemies1[i].update();
            }
        }

        //ogarnianie kolizji
        for (auto a:floor) manageCollision(spTadzik, a);
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].aSprite.sprite, 1.1))
            {
                for (auto a:floor)
                    manageCollision(enemies1[i], a);
                for (auto a:breakable)
                    manageCollision(enemies1[i], a.sprite);
            }
        }

        //enemies AI
        for (int i=0; i<enemies1.size(); i++) {
            if (enemies1[i].speedX==0) {
                enemies1[i].setScale(-enemies1[i].getScale().x, enemies1[i].getScale().y);
            }
            enemies1[i].speedX=1*Utils::sgn(enemies1[i].getScale().x);
        }

        //ogarnianie breakable
        closestBreakable=-1;
        for (int i=breakable.size()-1; i>=0; --i) {
            if (isActive(breakable[i].sprite, 1)) {
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

        //ogarnianie monet i powerupow
        if (coins.size()>0) {
            for (int i=coins.size()-1; i>=0; --i) {
                if (Collision::PixelPerfectTest(coins[i].sprite, spTadzik.aSprite.sprite)) {
                    score++;
                    coins.erase(coins.begin()+i);
                }
            }
        }
        if (powerups.size()>0) {
            for (int i=powerups.size()-1; i>=0; --i) {
                if (Collision::PixelPerfectTest(powerups[i].sprite, spTadzik.aSprite.sprite)) {
                    powerLevel++;
                    spTadzik.loadAnimations(animTadzikMedium);
                    spTadzik.changeAnimation(spTadzik.currentAnimation);
                    spTadzik.setOrigin(sf::Vector2f(spTadzik.getTextureRect().width/2, spTadzik.getTextureRect().height));
                    powerups.erase(powerups.begin()+i);
                }
            }
        }

        //gameover
        if (spTadzik.getGlobalBounds().top>window->getSize().y) {
            floor.clear();
            gameOver();
        }
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].aSprite.sprite)) {
                if (Collision::PixelPerfectTest(spTadzik.aSprite.sprite, enemies1[i].aSprite.sprite)) {
                    gameOver();
                }
            }
        }

        //fixowanie backgroundu
        if (-Background1.getGlobalBounds().left > window->getSize().x) Background1.move(2*(int)window->getSize().x, 0);
        if (-Background2.getGlobalBounds().left > window->getSize().x) Background2.move(2*(int)window->getSize().x, 0);
        if (Background1.getGlobalBounds().left > window->getSize().x) Background1.move(-2*(int)window->getSize().x, 0);
        if (Background2.getGlobalBounds().left > window->getSize().x) Background2.move(-2*(int)window->getSize().x, 0);

        textScore.setString(Utils::stringify(score));

        //rysowanie
        window->clear();
        window->draw(Background1);
        window->draw(Background2);
        for (auto a:floor) {
            window->draw(a);
        }
        for (auto a:hitboxlessBack) {
            if (isActive(a)) window->draw(a);
        }
        window->draw(spTadzik.aSprite.sprite);
        for (auto a:hitboxlessFront) { if (isActive(a)) window->draw(a);}
        for (auto a:breakable) { if (isActive(a.sprite)) window->draw(a.sprite);}
        for (auto a:coins) { window->draw(a.sprite);}
        for (auto a:powerups) { window->draw(a.sprite);}
        for (auto a:enemies1) { window->draw(a.aSprite.sprite);}
        window->draw(textScore);
    }

protected:
    sf::Texture texBackground;
    sf::Texture texFloorTile;
    sf::Texture texCoin;

    sf::Texture texEnemy1Stand;
    sf::Texture texEnemy1Run;

    std::vector <sf::Texture> texBreakableTile;

    std::vector <sf::Texture> texPlayerRun;
    sf::Texture texPlayerStand;
    sf::Texture texPlayerJump;
    sf::Texture texPlayerFall;
    sf::Texture texPowerup;

    std::vector <sf::Texture> texPlayerMiniRun;
    sf::Texture texPlayerMiniStand;
    sf::Texture texPlayerMiniJump;
    sf::Texture texPlayerMiniFall;

    sf::Image mapa;

    classMovingEntity spTadzik, spEnemy1;
    AnimatedSprite spCoin, spPowerup;
    Animation TadzikRun, TadzikStand, TadzikJump, TadzikFall;
    Animation TadzikMiniRun, TadzikMiniStand, TadzikMiniJump, TadzikMiniFall;
    Animation enemy1Stand, enemy1Run;
    Animation coinRotate, powerupAnim;

    sf::Sprite Background1;
    sf::Sprite Background2;
    sf::Sprite FloorTile;

    std::vector <sf::Sprite> floor;
    std::vector <sf::Sprite> hitboxlessFront;
    std::vector <sf::Sprite> hitboxlessBack;
    std::vector <AnimatedSprite> powerups;
    std::vector <AnimatedSprite> coins;
    std::vector <classMovingEntity> enemies1;

    std::vector <Animation*> animTadzikSmall;
    std::vector <Animation*> animTadzikMedium;
    std::vector <Animation*> animTadzikBig;

    std::vector <classBreakable> breakable;
    classBreakable BreakableTile;

    double& speedX = spTadzik.speedX;
    double& speedY = spTadzik.speedY;
    double maxSpeed = 10;
    double gravity = 0.5;
    double standingHeight = 0;
    double parallax = 0.66;
    double timeLeft = 30;

    int onTile = 0;
    int tilesPerY = 20;
    int closestBreakable = -1;
    int score = 0;
    int powerLevel = 0;

    sf::Text textScore;
    sf::Font font;
};
#endif //mario
