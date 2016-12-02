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

class Effect: public AnimatedSprite{
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

class classBreakable: public sf::Sprite{
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
    void changeTexture(int i) {
        setTexture(*textures[i]);
    }
    bool hit() {
        totalHits++;
        if (hits<=totalHits) return 1;
        setTexture(*textures[totalHits]);
        return 0;
    }
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

class classMovingEntity: public AnimatedSprite{
public:
    void loadAnimations(std::vector <Animation*> a) { animations = a;}
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
    void update1() {
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

class MARIO: public Scene{
public:
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

        texPowerup.loadFromFile("files/textures/mario/powerup.png"), powerupAnim.addFrame(AnimationFrame(&texPowerup, 500));
        spPowerup.setAnimation(&powerupAnim);

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
        spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
        spTadzik.sprite.setScale(2, 2);

    }

    virtual void onSceneActivate() {
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void loadMap() {
        FloorTile.setScale(((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().width,
                           ((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().height);
        BreakableTile.setScale(((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().width,
                               ((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().height);
        spCoin.sprite.setScale(((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getTextureRect().width,
                               ((double)window->getSize().y/(double)tilesPerY)/spCoin.sprite.getTextureRect().height);
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
                    BreakableTile.setPosition(i*TileSize, j*TileSize);
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
        coins.clear();
        loadMap();
        circle.setRadius(1000);
    }

    bool isActive(sf::Sprite s, double multiplier = 1) {
        if (s.getGlobalBounds().left>window->getSize().x*multiplier ||
            s.getGlobalBounds().left+s.getGlobalBounds().width<(1.0-multiplier)*window->getSize().x) return 0;
        else return 1;
    }

    void manageCollision(classMovingEntity& entity, sf::Sprite s) {
        if (isActive(s, 1.2) && Collision::BoundingBoxTest(entity.sprite, s)) {
            if (s.getGlobalBounds().top+s.getGlobalBounds().height<entity.pos.top) {
                entity.speedY = 0;
                entity.sprite.setPosition(entity.sprite.getPosition().x,
                                          s.getGlobalBounds().top+s.getGlobalBounds().height+entity.sprite.getGlobalBounds().height);
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
    }

    virtual void draw(double deltaTime) {
        spTadzik.updatePrev();
        spTadzik.update(abs(spTadzik.speedX)*deltaTime);

        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1)) {
                enemies1[i].updatePrev();
                enemies1[i].update(abs(enemies1[i].speedX)*deltaTime);
            }
        }
        for (int i=effects.size()-1; i>=0; i--) {
            effects[i].updateEffect(deltaTime);
            if (!isActive(effects[i].sprite)) {
                effects.erase(effects.begin()+i);
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
        if ((spTadzik.sprite.getPosition().x>(double)window->getSize().x*(3.0/4.0) && speedX>0) ||
            (spTadzik.sprite.getPosition().x<(double)window->getSize().x*(1.0/5.0) && speedX<0 && floor[0].getPosition().x<0)) {
            for (int i=0; i<floor.size(); i++)              { floor[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessBack.size(); i++)     { hitboxlessBack[i].move(-speedX, 0);}
            for (int i=0; i<hitboxlessFront.size(); i++)    { hitboxlessFront[i].move(-speedX, 0);}
            for (int i=0; i<breakable.size(); i++)          { breakable[i].move(-speedX, 0);}
            for (int i=0; i<coins.size(); i++)              { coins[i].sprite.move(-speedX, 0);}
            for (int i=0; i<effects.size(); i++)            { effects[i].sprite.move(-speedX, 0);}
            for (int i=0; i<powerups.size(); i++)           { powerups[i].sprite.move(-speedX, 0);}
            for (int i=0; i<enemies1.size(); i++)           { enemies1[i].move(-speedX, 0), enemies1[i].updatePrev();}
            spTadzik.pos.left-=speedX;
            spTadzik.pos.right-=speedX;
            spTadzik.prevX-=speedX;
            Background1.move(-speedX*parallax, 0);
            Background2.move(-speedX*parallax, 0);
        }
        else {
            if (!isDead) spTadzik.move(speedX, 0);
        }
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1))
                enemies1[i].move(enemies1[i].speedX, 0);
        }
        spTadzik.update1();
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1)) {
                enemies1[i].update1();
            }
        }

        //ogarnianie kolizji

        for (auto a:floor) manageCollision(spTadzik, a);
        for (int i=0; i<enemies1.size(); i++) {
            if (isActive(enemies1[i].sprite, 1.1))
            {
                for (auto a:floor)
                    manageCollision(enemies1[i], a);
                for (auto a:breakable)
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
        }

        //ogarnianie breakable
        closestBreakable=-1;
        for (int i=breakable.size()-1; i>=0; --i) {
            if (isActive(breakable[i], 1)) {
                classBreakable& s = breakable[i];
                if (Collision::BoundingBoxTest(spTadzik.sprite, s)) {
                    if (s.bottom()<spTadzik.pos.top) {
                        speedY = 0;
                        spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x,
                                                    s.bottom()+spTadzik.sprite.getGlobalBounds().height);
                        if (closestBreakable==-1) {
                            closestBreakable = i;
                        }
                        else if (abs(spTadzik.sprite.getPosition().x-breakable[closestBreakable].center().x) >
                                 abs(spTadzik.sprite.getPosition().x-breakable[i].center().x)) {
                                    closestBreakable = i;
                                }
                    }
                    if (s.top()>=spTadzik.pos.bottom) {
                        spTadzik.isStanding = true;
                        spTadzik.sprite.setPosition(spTadzik.sprite.getPosition().x, s.top());
                        speedY = 0;
                    }
                    else if (s.left()>spTadzik.pos.right-1) {
                        spTadzik.sprite.setPosition(spTadzik.prevX, spTadzik.sprite.getPosition().y);
                        speedX = 0;
                    }
                    else if (s.right()<spTadzik.pos.left+1) {
                        spTadzik.sprite.setPosition(spTadzik.prevX, spTadzik.sprite.getPosition().y);
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
                if (Collision::PixelPerfectTest(coins[i].sprite, spTadzik.sprite)) {
                    score++;
                    coins.erase(coins.begin()+i);
                }
            }
        }
        if (powerups.size()>0) {
            for (int i=powerups.size()-1; i>=0; --i) {
                if (Collision::PixelPerfectTest(powerups[i].sprite, spTadzik.sprite)) {
                    powerLevel++;
                    spTadzik.loadAnimations(animTadzikMedium);
                    spTadzik.changeAnimation(spTadzik.current);
                    spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
                    powerups.erase(powerups.begin()+i);
                }
            }
        }

        //gameover
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) {
            floor.clear();
            gameOver();
        }
        for (int i=enemies1.size()-1; i>=0; i--) {
            if (isActive(enemies1[i].sprite)) {
                if (Collision::BoundingBoxTest(spTadzik.sprite, enemies1[i].sprite)) {
                    if (enemies1[i].sprite.getGlobalBounds().top >= spTadzik.pos.bottom) {
                        spTadzik.jump();
                        effects.push_back(Effect(enemies1[i], Utils::randFloat(-5, 5), -5.0, 0.0, 0.5));
                        enemies1.erase(enemies1.begin()+i);
                    }
                    else {
                        isDead = 1;
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
        window->draw(spTadzik.sprite);

        for (auto a:hitboxlessFront) { if (isActive(a)) window->draw(a);}
        for (auto a:breakable) { if (isActive(a)) window->draw(a);}
        for (auto a:coins) { window->draw(a.sprite);}
        for (auto a:powerups) { window->draw(a.sprite);}
        for (auto a:enemies1) { window->draw(a.sprite);}
        for (auto a:effects) { window->draw(a.sprite);}
        window->draw(textScore);
        if (isDead) {
            circle.setOrigin(circle.getRadius(), circle.getRadius()+20);
            circle.setPosition(spTadzik.sprite.getPosition());
            window->draw(circle);
            if (circle.getRadius()>50) circle.setRadius(circle.getRadius()-10);
            else {
                gameOver();
            }
        }
    }

protected:
    sf::Texture texBackground;
    sf::Texture texFloorTile;
    sf::Texture texCoin;

    std::vector <sf::Texture> texEnemy1Run;
    sf::Texture texEnemy1Stand;

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

    std::vector <Effect> effects;

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

    bool isDead = 0;

    sf::Text textScore;
    sf::Font font;

    sf::CircleShape circle;
};
#endif //mario
