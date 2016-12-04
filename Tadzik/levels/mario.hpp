#ifndef MARIO_HPP
#define MARIO_HPP

#define M_PI 3.14159265359

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class MARIO;

enum animationType {
    STAND, RUN, JUMP, FALL
};

struct positions {
    double left;
    double right;
    double top;
    double bottom;
};

class Effect: public AnimatedSprite {
public:
    void fromSprite(sf::Sprite& s) {
        sprite.setTexture(*s.getTexture());
        sprite.setScale(s.getScale());
    }
    void updateEffect(float delta) {
        update(delta);
        move(speedX, speedY);
        speedX+=accX;
        speedY+=accY;
    }
    Effect(AnimatedSprite &s, double sX, double sY, double aX, double aY) {
        speedX = sX;
        speedY = sY;
        accX = aX;
        accY = aY;
        setAnimation(s.currentAnimation);
        sprite.setScale(s.sprite.getScale());
        sprite.setOrigin(s.sprite.getOrigin());
        sprite.setPosition(s.sprite.getPosition());
    }
protected:
    double speedX;
    double speedY;
    double accX;
    double accY;
};

class Tile: public sf::Sprite {
public:
    Tile() {};
    Tile(MARIO* parent): p(parent) {};
    virtual void onHitAction(int i) {};
    MARIO* p;
private:

};

class BreakableBlock: public Tile {
public:
    BreakableWall() {
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
    void changeTexture(int i) {
        setTexture(*textures[i]);
    }
    bool hit() {
        totalHits++;
        if (hits<=totalHits) return 1;
        setTexture(*textures[totalHits]);
        return 0;
    }
    /*
    void onHitAction(int i) override{
        std::cout << totalHits;
        totalHits++;
        setTexture(*textures[totalHits]);
        p->breakable.erase(p->breakable.begin()+i);
    }
    */
    int left() { return getGlobalBounds().left;}
    int right() { return getGlobalBounds().left+getGlobalBounds().width;}
    int top() { return getGlobalBounds().top;}
    int bottom() { return getGlobalBounds().top+getGlobalBounds().height;}
    sf::Vector2f center() { return {getGlobalBounds().left+getGlobalBounds().width/2,
                                    getGlobalBounds().top+getGlobalBounds().height/2};}
private:
    std::vector <sf::Texture*> textures;
    int hits = 0;
    int totalHits = 0;
};

class QuestionTile: public Tile {
public:
    sf::Texture* active;
    sf::Texture* notActive;
    positions pos;
    void updatePrev() {
        pos.top = getGlobalBounds().top;
        pos.bottom = getGlobalBounds().top+getGlobalBounds().height;
        pos.left = getGlobalBounds().left;
        pos.right = getGlobalBounds().left+getGlobalBounds().width;
    }
    bool hit(int Level) {
        if (getTexture()==active) {
            setTexture(*notActive);
            return 1;
        }
        return 0;
    }
    void loadTextures(sf::Texture* Active, sf::Texture* NotActive) {
        active = Active;
        notActive = NotActive;
        setTexture(*active);
    }
    sf::Vector2f center() { return {getGlobalBounds().left+getGlobalBounds().width/2,
                                    getGlobalBounds().top+getGlobalBounds().height/2};}

};

class MovingEntity: public AnimatedSprite {
public:
    void loadAnimations(std::vector <Animation*> a) {
        animations = a;
        sprite.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height);
    }
    double speedX = 0.01, speedY = 0;
    void changeAnimation(animationType t)           {setAnimation(animations[t]); current = t;}
    void addAnimation(Animation* a)                 {animations.push_back(a);}
    void updatePrev() {
        pos.top = sprite.getGlobalBounds().top;
        pos.bottom = sprite.getGlobalBounds().top+sprite.getGlobalBounds().height;
        pos.left = sprite.getGlobalBounds().left;
        pos.right = sprite.getGlobalBounds().left+sprite.getGlobalBounds().width;
        prevX = sprite.getPosition().x;
        prevY = sprite.getPosition().y;
        prevSpeedX = speedX;
        prevSpeedY = speedY;

    }
    void flipSprite() {
        sprite.setScale(abs(sprite.getScale().x)*Utils::sgn(speedX), sprite.getScale().y);
    }
    void jump() {
        speedY=-13;
        isStanding = false;
        isJumping = true;
        changeAnimation(JUMP);
    }
    void updateEntity() {
        speedY+=0.5;
        sprite.move(0, speedY);
        //flip
        if (Utils::sgn(speedX)!=Utils::sgn(prevSpeedX) && Utils::sgn(speedX)!=0)
            flipSprite();

        if (isJumping && speedY>0) {
            isJumping = false;
            changeAnimation(FALL);
        }
        speedX*=0.9;


    }
    void updateAnimations() {
        if (abs(speedX)<0.1 && isStanding && current!=STAND) {
            changeAnimation(STAND);
        }
        if (abs(speedX)>0.1 && isStanding && current!=RUN) {
            changeAnimation(RUN);
        }
    }
    double prevX, prevY;
    double prevSpeedX, prevSpeedY;
    positions pos;
    bool isStanding = 0;
    bool isJumping = 0;
    animationType current = STAND;
private:
    std::vector <Animation*> animations;

};

class Bullet: public sf::Sprite {
public:
    Bullet(sf::Texture* tex, sf::Vector2f pos, double velocity, double angle, double acY=0.5) {
        setTexture(*tex);
        setPosition(pos);
        speedX = cos(angle)*velocity;
        speedY = sin(angle)*velocity;
        accY=acY;
        setRotation(atan2(speedY, speedX)*180.0/M_PI);
    }
    void update() {
        move(speedX, speedY);
        speedY+=accY;
        setRotation(atan2(speedY, speedX)*180.0/M_PI);
    }
private:
    double speedX;
    double speedY;
    double accX;
    double accY;
};

class MARIO: public Scene{
public:
    friend class Tile;
    MARIO(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory() {
        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")) {
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

        circle.setRadius(1000);
        circle.setOutlineThickness(1000);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOrigin(500, 500);
        circle.setOutlineColor(sf::Color::Black);

        texBreakableTile.resize(3);
        texBreakableTile[0].loadFromFile("files/textures/mario/breakable1.png");
        texBreakableTile[1].loadFromFile("files/textures/mario/breakable2.png");
        texBreakableTile[2].loadFromFile("files/textures/mario/breakable3.png");
        BreakableTile.loadTextures(texBreakableTile);
        BreakableTile.changeTexture(0);

        texCoin.loadFromFile("files/textures/mario/coin1.png"), coinRotate.addFrame(AnimationFrame(&texCoin, 500));
        spCoin.setAnimation(&coinRotate);

        texPowerupTileActive.loadFromFile("files/textures/mario/powerupTileActive.png");
        texPowerupTileInactive.loadFromFile("files/textures/mario/powerupTileInactive.png");
        PowerupTile.loadTextures(&texPowerupTileActive, &texPowerupTileInactive);

        texPowerup.loadFromFile("files/textures/mario/powerup.png"), powerupAnim.addFrame(AnimationFrame(&texPowerup, 500));
        spritePowerup.setAnimation(&powerupAnim);

        texBullet1.loadFromFile("files/textures/mario/bullet1.png");


        texEnemy1Run.resize(2);
        texEnemy1Run[0].loadFromFile("files/textures/mario/enemy1Run2.png"), enemy1Run.addFrame(AnimationFrame(&texEnemy1Run[0], 500));
        texEnemy1Run[1].loadFromFile("files/textures/mario/enemy1Run1.png"), enemy1Run.addFrame(AnimationFrame(&texEnemy1Run[1], 500));
        texEnemy1Stand.loadFromFile("files/textures/mario/enemy1Stand.png"), enemy1Stand.addFrame(AnimationFrame(&texEnemy1Stand, 500));
        spEnemy1.addAnimation(&enemy1Stand);
        spEnemy1.addAnimation(&enemy1Run);

        spEnemy1.setAnimation(&enemy1Run);
        spEnemy1.sprite.setOrigin(spEnemy1.sprite.getTextureRect().width/2, spEnemy1.sprite.getTextureRect().height);
        spEnemy1.sprite.setScale(3, 3);


        mapa.loadFromFile("files/maps/mario/map1.png");
        loadMap();

        texPlayerLVL1Run.resize(4);
        texPlayerLVL1Run[0].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikLVL1Run.addFrame(AnimationFrame(&texPlayerLVL1Run[0], 500));
        texPlayerLVL1Run[1].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikLVL1Run.addFrame(AnimationFrame(&texPlayerLVL1Run[1], 500));
        texPlayerLVL1Run[2].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikLVL1Run.addFrame(AnimationFrame(&texPlayerLVL1Run[2], 500));
        texPlayerLVL1Run[3].loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikLVL1Run.addFrame(AnimationFrame(&texPlayerLVL1Run[3], 500));
        texPlayerLVL1Jump.loadFromFile("files/textures/mario/playerSmallJump.png"),   TadzikLVL1Jump.addFrame(AnimationFrame(&texPlayerLVL1Jump, 150));
        texPlayerLVL1Stand.loadFromFile("files/textures/mario/playerSmallStand.png"), TadzikLVL1Stand.addFrame(AnimationFrame(&texPlayerLVL1Stand, 150));
        texPlayerLVL1Fall.loadFromFile("files/textures/mario/playerSmallStand.png"),  TadzikLVL1Fall.addFrame(AnimationFrame(&texPlayerLVL1Fall, 150));

        texPlayerLVL2Run.resize(4);
        texPlayerLVL2Run[0].loadFromFile("files/textures/universal/playerRun1.png"), TadzikLVL2Run.addFrame(AnimationFrame(&texPlayerLVL2Run[0], 500));
        texPlayerLVL2Run[1].loadFromFile("files/textures/universal/playerRun2.png"), TadzikLVL2Run.addFrame(AnimationFrame(&texPlayerLVL2Run[1], 500));
        texPlayerLVL2Run[2].loadFromFile("files/textures/universal/playerRun3.png"), TadzikLVL2Run.addFrame(AnimationFrame(&texPlayerLVL2Run[2], 500));
        texPlayerLVL2Run[3].loadFromFile("files/textures/universal/playerRun4.png"), TadzikLVL2Run.addFrame(AnimationFrame(&texPlayerLVL2Run[3], 500));
        texPlayerLVL2Jump.loadFromFile("files/textures/universal/playerJump.png"),   TadzikLVL2Jump.addFrame(AnimationFrame(&texPlayerLVL2Jump, 150));
        texPlayerLVL2Stand.loadFromFile("files/textures/universal/playerStand.png"), TadzikLVL2Stand.addFrame(AnimationFrame(&texPlayerLVL2Stand, 150));
        texPlayerLVL2Fall.loadFromFile("files/textures/universal/playerFall.png"),   TadzikLVL2Fall.addFrame(AnimationFrame(&texPlayerLVL2Fall, 150));

        animTadzikLVL1.push_back(&TadzikLVL1Stand);
        animTadzikLVL1.push_back(&TadzikLVL1Run);
        animTadzikLVL1.push_back(&TadzikLVL1Jump);
        animTadzikLVL1.push_back(&TadzikLVL1Fall);

        animTadzikLVL2.push_back(&TadzikLVL2Stand);
        animTadzikLVL2.push_back(&TadzikLVL2Run);
        animTadzikLVL2.push_back(&TadzikLVL2Jump);
        animTadzikLVL2.push_back(&TadzikLVL2Fall);

        spTadzik.loadAnimations(animTadzikLVL1);

        spTadzik.changeAnimation(FALL);
        spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
        spTadzik.sprite.setScale(2, 2);

    }

    virtual void onSceneActivate() {
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Space) {
                bullets.push_back(Bullet(&texBullet1, spTadzik.sprite.getPosition(), 10.0, 30.0));

            }
        }
    }

    void loadMap() {
        FloorTile.setScale(((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().width,
                           ((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().height);
        BreakableTile.setScale(((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().width,
                               ((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().height);
        PowerupTile.setScale(((double)window->getSize().y/(double)tilesPerY)/PowerupTile.getTextureRect().width,
                             ((double)window->getSize().y/(double)tilesPerY)/PowerupTile.getTextureRect().height);
        spCoin.sprite.setScale(((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getTextureRect().width,
                               ((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getTextureRect().height);
        int TileSize = FloorTile.getGlobalBounds().width;
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color(0, 0, 0)) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    floor.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(100, 100, 100)) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    hitboxlessBack.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(200, 200, 200)) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    hitboxlessFront.push_back(FloorTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(200, 100, 100)) {
                    BreakableTile.setPosition(i*TileSize, j*TileSize);
                    breakable.push_back(BreakableTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(200, 0, 100)) {
                    PowerupTile.setPosition(i*TileSize, j*TileSize);
                    powerupBlocks.push_back(PowerupTile);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 255, 0)) {
                    spCoin.setPosition(i*TileSize, j*TileSize);
                    coins.push_back(spCoin);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 0, 0)) {
                    spEnemy1.setPosition(i*TileSize, j*TileSize);
                    enemies1.push_back(spEnemy1);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 255, 255)) {
                    spritePowerup.setPosition(i*TileSize, j*TileSize);
                    powerups.push_back(spritePowerup);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*TileSize, j*TileSize);
                }
            }
        }
    }

    void gameOver() {
        isDead = false;
        sf::Text t;
        t.setString("YOU SUCK");
        t.setPosition(100, 100);
        window->draw(t);
        floor.clear();
        enemies1.clear();
        breakable.clear();
        hitboxlessBack.clear();
        hitboxlessFront.clear();
        powerupBlocks.clear();
        coins.clear();
        loadMap();
        circle.setRadius(1000);
    }

    bool isActive(sf::Sprite s, double multiplier = 1) {
        if (s.getGlobalBounds().left>window->getSize().x*multiplier ||
            s.getGlobalBounds().left+s.getGlobalBounds().width<(1.0-multiplier)*window->getSize().x) return 0;
        else return 1;
    }

    bool manageCollision(MovingEntity& entity, Tile s) {
        bool check = false;
        if (isActive(s, 2) && Collision::BoundingBoxTest(entity.sprite, s)) {
            if (s.getGlobalBounds().top+s.getGlobalBounds().height<entity.pos.top) {
                entity.speedY = 0;
                entity.sprite.setPosition(entity.sprite.getPosition().x,
                                          s.getGlobalBounds().top+s.getGlobalBounds().height+entity.sprite.getGlobalBounds().height);
                //entity.pos.top = entity.sprite.getGlobalBounds().top;
                //entity.pos.bottom = entity.sprite.getGlobalBounds().top+entity.sprite.getGlobalBounds().height;
                check = true;
            }
            if (s.getGlobalBounds().top>=entity.pos.bottom) {
                entity.isStanding = true;
                entity.sprite.setPosition(entity.sprite.getPosition().x, s.getGlobalBounds().top);
                entity.speedY = 0;
            }
            else if (s.getGlobalBounds().left>entity.pos.right-1) {
                entity.sprite.setPosition(entity.prevX, entity.sprite.getPosition().y);
                entity.speedX = 0;
            }
            else if (s.getGlobalBounds().left+s.getGlobalBounds().width<entity.pos.left+1) {
                entity.sprite.setPosition(entity.prevX, entity.sprite.getPosition().y);
                entity.speedX = 0;
            }
        }
        return check;
    }

    virtual void draw(double deltaTime) {
        //przepisywanie z poprzedniej klatki
        spTadzik.updatePrev();
        if (!isDead) spTadzik.update(abs(spTadzik.speedX)*deltaTime);

        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1)) {
                enemies1[i].updatePrev();
                enemies1[i].update(abs(enemies1[i].speedX)*deltaTime);
            }
        }

        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -speedX<maxSpeed) {
            speedX-=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && speedX<maxSpeed) {
            speedX+=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && spTadzik.isStanding && !isDead) {
            spTadzik.jump();
        }

        for (int i=effects.size()-1; i>=0; i--) {
            effects[i].updateEffect(deltaTime);
            if (!isActive(effects[i].sprite)) {
                effects.erase(effects.begin()+i);
            }
        }

        for (int i=bullets.size()-1; i>=0; i--) {
            if (isActive(bullets[i])) {
                bullets[i].update();
                /*
                for (auto a:floor)
                    if (Collision::BoundingBoxTest(bullets[i], a))
                        bullets.erase(bullets.begin()+i);
                for (auto a:breakable)
                    if (Collision::BoundingBoxTest(bullets[i], a))
                        bullets.erase(bullets.begin()+i);
                */
            }
            else bullets.erase(bullets.begin()+i);

        }



        //przewijanie i movement
        if ((spTadzik.sprite.getPosition().x>(double)window->getSize().x*(3.0/4.0) && speedX>0) ||
            (spTadzik.sprite.getPosition().x<(double)window->getSize().x*(1.0/5.0) && speedX<0 && floor[0].getPosition().x<0)) {
            for (int i=0; i<floor.size(); i++)              { floor[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessBack.size(); i++)     { hitboxlessBack[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessFront.size(); i++)    { hitboxlessFront[i].move(-speedX, 0);}
            for (int i=0; i<powerupBlocks.size(); i++)      { powerupBlocks[i].move(-speedX, 0);}
            for (int i=0; i<breakable.size(); i++)          { breakable[i].move(-speedX, 0);}
            for (int i=0; i<coins.size(); i++)              { coins[i].sprite.move(-speedX, 0);}
            for (int i=0; i<effects.size(); i++)            { effects[i].sprite.move(-speedX, 0);}
            for (int i=0; i<powerups.size(); i++)           { powerups[i].sprite.move(-speedX, 0);}
            for (int i=0; i<bullets.size(); i++)            { bullets[i].move(-speedX, 0);}
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
        spTadzik.updateEntity();
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1)) {
                enemies1[i].move(enemies1[i].speedX, 0);
                enemies1[i].updateEntity();
            }
        }

        //ogarnianie kolizji
        for (auto a:floor) manageCollision(spTadzik, a);
        for (auto a:powerupBlocks) manageCollision(spTadzik, a);

        closestBreakable = -1;
        for (int i=breakable.size()-1; i>=0; i--) {
            if (manageCollision(spTadzik, breakable[i])) {
                if (closestBreakable==-1) closestBreakable = i;
                else if (abs(breakable[closestBreakable].center().x-spTadzik.sprite.getPosition().x)>abs(breakable[i].center().x-spTadzik.sprite.getPosition().x))
                    closestBreakable = i;
            }
        }

        closestQuestionBlock = -1;
        for (int i=powerupBlocks.size()-1; i>=0; i--) {
            if (manageCollision(spTadzik, powerupBlocks[i])) {
                if (closestQuestionBlock==-1) closestQuestionBlock = i;
                else if (abs(powerupBlocks[closestQuestionBlock].center().x-spTadzik.sprite.getPosition().x)>abs(powerupBlocks[i].center().x-spTadzik.sprite.getPosition().x))
                    closestQuestionBlock = i;
            }
        if (powerLevel == 0) closestBreakable = -1;
        }

        if (closestBreakable != -1 && closestQuestionBlock != -1) {
            if (abs(breakable[closestBreakable].center().x-spTadzik.sprite.getPosition().x)>abs(powerupBlocks[closestQuestionBlock].center().x-spTadzik.sprite.getPosition().x)) {
                closestBreakable = -1;
            }
            else {
                closestQuestionBlock = -1;
            }
        }
        if (powerupBlocks.size()>0 && closestQuestionBlock!=-1) {
            if (powerupBlocks[closestQuestionBlock].hit(powerLevel) && powerLevel==0) {
                spritePowerup.setAnimation(&powerupAnim);
                spritePowerup.sprite.setPosition(powerupBlocks[closestQuestionBlock].getPosition().x,
                                             powerupBlocks[closestQuestionBlock].getPosition().y-30);
                powerups.push_back(spritePowerup);
            }
        }
        if (closestBreakable!=-1)
            if (breakable[closestBreakable].hit())
                breakable.erase(breakable.begin()+closestBreakable);


        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.2))
            {
                for (auto a:floor)
                    manageCollision(enemies1[i], a);
                for (auto a:breakable)
                    manageCollision(enemies1[i], a);
                for (auto a:powerupBlocks)
                    manageCollision(enemies1[i], a);
            }
        }

        //enemies AI
        for (int i=enemies1.size()-1; i>=0; i--) {
            if (enemies1[i].speedX==0) {
                enemies1[i].sprite.setScale(-enemies1[i].sprite.getScale().x, enemies1[i].sprite.getScale().y);
            }
            enemies1[i].speedX=1*Utils::sgn(enemies1[i].sprite.getScale().x);
            if (enemies1[i].sprite.getGlobalBounds().top > window->getSize().y+10) {
                enemies1.erase(enemies1.begin()+i);
            }
            if (Utils::randInt(0, 100)<1) bullets.push_back(Bullet(&texBullet1, enemies1[i].sprite.getPosition(), 25, Utils::randFloat(0, 2*3.1415)));
        }

        //ogarnianie monet i powerupow
        if (coins.size()>0)
            for (int i=coins.size()-1; i>=0; --i)
                if (Collision::PixelPerfectTest(coins[i].sprite, spTadzik.sprite)) {
                    score++;
                    coins.erase(coins.begin()+i);
                }
        if (powerups.size()>0)
            for (int i=powerups.size()-1; i>=0; --i)
                if (Collision::PixelPerfectTest(powerups[i].sprite, spTadzik.sprite)) {
                    powerLevel=1;
                    spTadzik.loadAnimations(animTadzikLVL2);
                    spTadzik.changeAnimation(spTadzik.current);
                    spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
                    powerups.erase(powerups.begin()+i);
                }

        //gameover
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) {
            floor.clear();
            gameOver();
        }
        for (int i=enemies1.size()-1; i>=0; i--)
            if (isActive(enemies1[i].sprite))
                if (Collision::BoundingBoxTest(spTadzik.sprite, enemies1[i].sprite)) {
                    if (enemies1[i].sprite.getGlobalBounds().top >= spTadzik.pos.bottom) {
                        spTadzik.jump();
                        effects.push_back(Effect(enemies1[i], Utils::randFloat(-5, 5), -5.0, 0.0, 0.5));
                        enemies1.erase(enemies1.begin()+i);
                    }
                    else {
                        if (Collision::PixelPerfectTest(spTadzik.sprite, enemies1[i].sprite) && invincibility.getElapsedTime().asMilliseconds()>500) {
                            if (powerLevel==0) {
                                isDead = 1;
                            }
                            else {
                                powerLevel--;
                                speedX = -3*speedX;
                                speedY = - abs(speedY);
                                spTadzik.loadAnimations(animTadzikLVL1);
                                spTadzik.changeAnimation(spTadzik.current);
                                spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
                                invincibility.restart();
                            }
                        }
                    }
                }

        //fixowanie backgroundu
        if (-Background1.getGlobalBounds().left > window->getSize().x) Background1.move(2*(int)window->getSize().x, 0);
        if (-Background2.getGlobalBounds().left > window->getSize().x) Background2.move(2*(int)window->getSize().x, 0);
        if (Background1.getGlobalBounds().left > window->getSize().x) Background1.move(-2*(int)window->getSize().x, 0);
        if (Background2.getGlobalBounds().left > window->getSize().x) Background2.move(-2*(int)window->getSize().x, 0);

        textScore.setString(Utils::stringify(score));
        if (isDead) {
            spTadzik.setPosition(spTadzik.prevX, spTadzik.prevY);
            circle.setOrigin(circle.getRadius(), circle.getRadius()+20);
            circle.setPosition(spTadzik.sprite.getPosition());
            if (circle.getRadius()>50) circle.setRadius(circle.getRadius()-10);
            else {
                gameOver();
            }
        }
        if (spTadzik.sprite.getGlobalBounds().left<0) {
            spTadzik.setPosition(spTadzik.sprite.getGlobalBounds().width/2, spTadzik.sprite.getPosition().y);
        }
        spTadzik.updateAnimations();

        //rysowanie
        window->clear();
        window->draw(Background1);
        window->draw(Background2);
        for (auto a:hitboxlessBack) {
            if (isActive(a)) window->draw(a);
        }
        for (auto a:floor) { window->draw(a);}
        for (auto a:breakable) { if (isActive(a)) window->draw(a);}
        for (auto a:coins) { window->draw(a.sprite);}
        for (auto a:bullets) { window->draw(a);}
        for (auto a:powerups) { window->draw(a.sprite);}
        for (auto a:enemies1) { window->draw(a.sprite);}
        for (auto a:effects) { window->draw(a.sprite);}
        for (auto a:powerupBlocks) { window->draw(a);}
        window->draw(spTadzik.sprite);
        for (auto a:hitboxlessFront) { if (isActive(a)) window->draw(a);}
        if (isDead) window->draw(circle);
        window->draw(textScore);

    }

protected:
    sf::Texture texBackground;
    sf::Texture texFloorTile;
    sf::Texture texPowerupTileActive;
    sf::Texture texPowerupTileInactive;
    sf::Texture texCoin;
    sf::Texture texBullet1;
    sf::Texture texPowerup;

    std::vector <sf::Texture> texEnemy1Run;
    sf::Texture texEnemy1Stand;

    std::vector <sf::Texture> texBreakableTile;


    std::vector <sf::Texture> texPlayerLVL2Run;
    sf::Texture texPlayerLVL2Stand;
    sf::Texture texPlayerLVL2Jump;
    sf::Texture texPlayerLVL2Fall;

    std::vector <sf::Texture> texPlayerLVL1Run;
    sf::Texture texPlayerLVL1Stand;
    sf::Texture texPlayerLVL1Jump;
    sf::Texture texPlayerLVL1Fall;

    sf::Image mapa;

    MovingEntity spTadzik, spEnemy1;
    AnimatedSprite spCoin;
    Animation TadzikLVL2Run, TadzikLVL2Stand, TadzikLVL2Jump, TadzikLVL2Fall;
    Animation TadzikLVL1Run, TadzikLVL1Stand, TadzikLVL1Jump, TadzikLVL1Fall;
    Animation enemy1Stand, enemy1Run;
    Animation coinRotate, powerupAnim;

    sf::Sprite Background1;
    sf::Sprite Background2;
    Tile FloorTile;
    MovingEntity spritePowerup;
    QuestionTile PowerupTile;

    std::vector <Tile> floor;
    std::vector <sf::Sprite> hitboxlessFront;
    std::vector <sf::Sprite> hitboxlessBack;
    std::vector <QuestionTile> powerupBlocks;
    std::vector <MovingEntity> powerups;
    std::vector <AnimatedSprite> coins;
    std::vector <MovingEntity> enemies1;

    std::vector <Animation*> animTadzikLVL1;
    std::vector <Animation*> animTadzikLVL2;
    std::vector <Animation*> animTadzikLVL3;

    std::vector <BreakableBlock> breakable;
    BreakableBlock BreakableTile;

    std::vector <Effect> effects;

    std::vector <Bullet> bullets;

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
    int closestQuestionBlock = -1;
    int score = 0;
    int powerLevel = 0;

    bool isDead = 0;

    sf::Text textScore;
    sf::Font font;
    sf::Clock invincibility;

    sf::CircleShape circle;
};
#endif //mario
