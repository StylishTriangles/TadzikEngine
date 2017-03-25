#ifndef MARIO_HPP
#define MARIO_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>

class MARIO;


class MARIO: public Scene{
public:
    MARIO(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

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
        BreakableBlock() {
            textures.clear();
        }
        void loadTextures(std::vector <sf::Texture>& t) {
            textures.clear();
            hits = t.size();
            for (unsigned int i=0; i<t.size(); i++) {
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
        void updatePrev() {
            pos.top = sprite.getGlobalBounds().top;
            pos.bottom = sprite.getGlobalBounds().top+sprite.getGlobalBounds().height;
            pos.left = sprite.getGlobalBounds().left;
            pos.right = sprite.getGlobalBounds().left+sprite.getGlobalBounds().width;
            prevPosition = sprite.getPosition();
            prevVelocity = velocity;

        }
        void flipSprite() {
            sprite.setScale(std::abs(sprite.getScale().x)*Utils::sgn(velocity.x), sprite.getScale().y);
        }
        virtual void updateEntity() {
            velocity.y+=0.5;
            //flip

        }
        virtual void updateAnimations() {

        }
        virtual void onHit() {

        }
        virtual void onCollision() {

        }
        positions pos;
        animationType current = STAND;
        sf::Vector2f velocity = sf::Vector2f(0.01, 0);
        sf::Vector2f prevVelocity;
        sf::Vector2f prevPosition;
        bool isStanding = 0;
        bool isJumping = 0;
        bool shouldDestroy = false;
    };

    class Enemy: public MovingEntity {
    public:

    };

    class Snek: public Enemy {
    public:
        Snek() {};
        Snek(Animation) {

        }
        void onHit() {
            shouldDestroy = true;
        }
    };

    class Armadillo: public Enemy {
    public:
        void onHit() {
            level--;
            if (level == 0) {
                setAnimation(animHide);
            }
            if (level < 0) {
                shouldDestroy = true;
            }
        }
        int level = 1;
        Animation* animWalk;
        Animation* animHide;
    };

    class Player: public MovingEntity {
    public:
        Player() {

        }
        void jump() {
            velocity.y=-13;
            isStanding = false;
            isJumping = true;
            changeAnimation(JUMP);
        }
        void updateAnimations() {
            if (std::abs(velocity.x)<0.1 && isStanding && current!=STAND) {
                changeAnimation(STAND);
            }
            if (std::abs(velocity.x)>0.1 && isStanding && current!=RUN) {
                changeAnimation(RUN);
            }
        }
        void updateEntity() {
            velocity.y+=0.5;
            //flip
            if (Utils::sgn(velocity.x)!=Utils::sgn(prevVelocity.x) && Utils::sgn(velocity.x)!=0)
                flipSprite();

            if (isJumping && velocity.y>0) {
                isJumping = false;
                changeAnimation(FALL);
            }
            velocity.x*=0.9;
            if (sprite.getGlobalBounds().left<0) {
                setPosition(sprite.getGlobalBounds().width/2, sprite.getPosition().y);
            }
        }
        void loadAnimations(std::vector <Animation*> a) {
            animations = a;
            sprite.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height);
        }
        void changeAnimation(animationType t)           {setAnimation(animations[t]); current = t;}
        void addAnimation(Animation* a)                 {animations.push_back(a);}
        int powerLevel = 0;
        bool isDead = false;
        sf::Clock invincibility;
    private:
        std::vector <Animation*> animations;
    };

    class Bullet: public sf::Sprite {
    public:
        Bullet(sf::Texture* tex, sf::Vector2f pos, double v, double angle, double acY=0.5) {
            setTexture(*tex);
            setPosition(pos);
            velocity.x = cos(angle)*v;
            velocity.y = sin(angle)*v;
            acceleration.y=acY;
            setRotation(atan2(velocity.y, velocity.x)*180.0/M_PI);
        }
        void update() {
            move(velocity);
            velocity.y+=acceleration.y;
            setRotation(atan2(velocity.y, velocity.x)*180.0/M_PI);
        }
    private:
        sf::Vector2f velocity;
        sf::Vector2f acceleration;
    };

    virtual void onSceneLoadToMemory() {
        texBackground.loadFromFile("files/textures/mario/background.png");
        texBackground.setRepeated(true);

        texFloorTile.loadFromFile("files/textures/mario/floor1.png"), FloorTile.setTexture(texFloorTile);

        texBreakableTile.resize(3);
        texBreakableTile[0].loadFromFile("files/textures/mario/breakable1.png");
        texBreakableTile[1].loadFromFile("files/textures/mario/breakable2.png");
        texBreakableTile[2].loadFromFile("files/textures/mario/breakable3.png");
        BreakableTile.loadTextures(texBreakableTile);
        BreakableTile.changeTexture(0);

        texCoin.loadFromFile("files/textures/mario/coin1.png"), coinRotate.addFrame(AnimationFrame(&texCoin, 500));
        tmpCoin.setAnimation(&coinRotate);
        tmpCoin.sprite.setOrigin(tmpCoin.sprite.getTextureRect().width/2, tmpCoin.sprite.getTextureRect().height/2);

        texPowerupTileActive.loadFromFile("files/textures/mario/powerupTileActive.png");
        texPowerupTileInactive.loadFromFile("files/textures/mario/powerupTileInactive.png");
        PowerupTile.loadTextures(&texPowerupTileActive, &texPowerupTileInactive);

        texPowerup.loadFromFile("files/textures/mario/powerup.png"), powerupAnim.addFrame(AnimationFrame(&texPowerup, 500));
        spritePowerup.setAnimation(&powerupAnim);
        spritePowerup.sprite.setOrigin(spritePowerup.sprite.getTextureRect().width/2, spritePowerup.sprite.getTextureRect().height/2);

        texBullet1.loadFromFile("files/textures/mario/bullet1.png");

        texSnekWalk.resize(2);
        texSnekWalk[0].loadFromFile("files/textures/mario/enemy1Run2.png"), snekWalk.addFrame(AnimationFrame(&texSnekWalk[0], 500));
        texSnekWalk[1].loadFromFile("files/textures/mario/enemy1Run1.png"), snekWalk.addFrame(AnimationFrame(&texSnekWalk[1], 500));
        tmpSnek.setAnimation(&snekWalk);

        tmpSnek.sprite.setOrigin(tmpSnek.sprite.getTextureRect().width/2, tmpSnek.sprite.getTextureRect().height);
        tmpSnek.sprite.setScale(3, 3);

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

        texWaterfall.loadFromFile("files/textures/mario/waterfall.png");
        texWaterfall.setRepeated(true);
        tempWaterfall.setTexture(texWaterfall);
        tempWaterfall.setColor(sf::Color(255,255,255,Utils::randInt(100,200)));

        texColumn.loadFromFile("files/textures/mario/column.png");
        texColumn.setRepeated(true);
        tempColumn.setTexture(texColumn);

        mapa.loadFromFile("files/maps/mario/map2.png");
    }

    virtual void onSceneActivate() {
        textScore.setFont(Common::Font::Comic_Sans);
        spBackground.setTexture(texBackground);
        spBackground.setTextureRect(sf::IntRect(0, 0, 100000000, window->getSize().y));
        spBackground.setScale((double)window->getSize().y/(double)texBackground.getSize().y,
                             (double)window->getSize().y/(double)texBackground.getSize().y);
        circle.setRadius(1000);
        circle.setOutlineThickness(1000);
        circle.setFillColor(sf::Color::Transparent);
        circle.setOrigin(500, 500);
        circle.setOutlineColor(sf::Color::Black);
        loadMap();
        gameView.reset(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
        windowSize = sf::Vector2i(window->getSize());
        mapSize = {mapa.getSize().x*TileSize, mapa.getSize().y*TileSize};

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
        tilesPerY = mapa.getSize().y;
        FloorTile.setScale(((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().width,
                           ((double)window->getSize().y/(double)tilesPerY)/FloorTile.getTextureRect().height);
        BreakableTile.setScale(((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().width,
                               ((double)window->getSize().y/(double)tilesPerY)/BreakableTile.getTextureRect().height);
        PowerupTile.setScale(((double)window->getSize().y/(double)tilesPerY)/PowerupTile.getTextureRect().width,
                             ((double)window->getSize().y/(double)tilesPerY)/PowerupTile.getTextureRect().height);
        tmpCoin.sprite.setScale(((double)window->getSize().y/(double)tilesPerY)/tmpCoin.sprite.getTextureRect().width,
                                ((double)window->getSize().y/(double)tilesPerY)/tmpCoin.sprite.getTextureRect().height);
        TileSize = FloorTile.getGlobalBounds().width;
        for (unsigned int i=0; i<mapa.getSize().x; i++) {
            for (unsigned int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color(0, 0, 0)) {
                    FloorTile.setPosition(i*TileSize, j*TileSize);
                    vecFloor.push_back(FloorTile);
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
                    tmpCoin.setPosition((i+0.5)*TileSize, (j+0.5)*TileSize);
                    vecCoins.push_back(tmpCoin);
                }
                else if(mapa.getPixel(i, j)==sf::Color(255, 0, 0)) {
                    tmpSnek.setPosition(i*TileSize, j*TileSize);
                    vecEnemies.push_back(new Snek);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 255, 255)) {
                    spritePowerup.setPosition((i+0.5)*TileSize, (j+0.5)*TileSize);
                    vecPowerups.push_back(spritePowerup);
                }
                else if(mapa.getPixel(i, j)==sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*TileSize, j*TileSize);
                }
                else if (mapa.getPixel(i, j)==sf::Color(123, 123, 123)) {
                    tempColumn.setPosition(i*TileSize, j*TileSize);
                    tempColumn.setTextureRect(sf::IntRect(0, 0, texColumn.getSize().x, 1000 ));
                    hitboxlessFront.push_back(tempColumn);
                    tempColumn.move(sf::Vector2f(0,tempColumn.getGlobalBounds().width));
                    tempColumn.setRotation(-90);
                    tempColumn.setTextureRect(sf::IntRect(0, 0, texColumn.getSize().x, 120 ));
                    hitboxlessFront.push_back(tempColumn);
                    tempColumn.setRotation(0);
                    tempColumn.setPosition(i*TileSize, j*TileSize);

                }
                 else if (mapa.getPixel(i, j)==sf::Color(231, 231, 231)) {
                    tempColumn.setPosition(i*TileSize, j*TileSize);
                    tempColumn.setTextureRect(sf::IntRect(0, 0, texColumn.getSize().x, 1000 ));
                    hitboxlessFront.push_back(tempColumn);
                }
                 else if (mapa.getPixel(i, j)==sf::Color(123, 231, 231)) {
                    tempWaterfall.setPosition(i*TileSize, j*TileSize);
                    vecWaterfall.push_back(tempWaterfall);
                }
            }
        }
    }

    void gameOver() {
        spTadzik.isDead = false;
        sf::Text t;
        t.setString("YOU SUCK");
        t.setPosition(100, 100);
        window->draw(t);
        vecFloor.clear();
        for (int i=0; i<vecEnemies.size(); i++) {
            delete vecEnemies[i];
        }
        vecEnemies.clear();
        breakable.clear();
        hitboxlessBack.clear();
        hitboxlessFront.clear();
        powerupBlocks.clear();
        vecCoins.clear();
        loadMap();
        circle.setRadius(1000);
    }

    bool isActive(sf::Sprite s, double multiplier = 1) {
        if (s.getGlobalBounds().left-viewOffset.x>window->getSize().x*multiplier ||
            s.getGlobalBounds().left+s.getGlobalBounds().width-viewOffset.x<(1.0-multiplier)*window->getSize().x) return 0;
        else return 1;
    }

    virtual void draw(double deltaTime) {
        ///OGARNIANIE VIEW
        sf::Vector2i pos = window->mapCoordsToPixel(spTadzik.sprite.getPosition(), gameView);
        int scrollArea = 200;
        if (pos.x > windowSize.x-scrollArea && viewOffset.x+windowSize.x<mapSize.x) {
            gameView.move(pos.x-windowSize.x+scrollArea, 0);
            viewOffset.x+=pos.x-windowSize.x+scrollArea;
            if (viewOffset.x+windowSize.x>mapSize.x) {
                gameView.move(-viewOffset.x-windowSize.x+mapSize.x, 0);
                viewOffset.x=mapSize.x-windowSize.x;
            }
        }
        else if (pos.x < scrollArea && viewOffset.x>0) {
            gameView.move(pos.x-scrollArea, 0);
            viewOffset.x+=pos.x-scrollArea;
            if (viewOffset.x<0) {
                gameView.move(-viewOffset.y, 0);
                viewOffset.x=0;
            }
        }
        window->setView(gameView);

        //przepisywanie z poprzedniej klatki
        spTadzik.updatePrev();
        if (!spTadzik.isDead) spTadzik.update(std::abs(spTadzik.velocity.x)*deltaTime);
        std::cout << "LELELELELEL";

        for (unsigned int i=0; i<vecEnemies.size(); i++) {
            if (isActive(vecEnemies[i]->sprite, 1.1)) {
                vecEnemies[i]->updatePrev();
                vecEnemies[i]->update(std::abs(vecEnemies[i]->velocity.x)*deltaTime);
            }
        }

        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -speedX<maxSpeed) {
            speedX-=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && speedX<maxSpeed) {
            speedX+=1;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && spTadzik.isStanding && !spTadzik.isDead) {
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
        //spTadzik.move(speedX, 0);

        spTadzik.move(0, speedY);
        sf::FloatRect intersection;
        for (auto a:vecFloor) {
            if (spTadzik.sprite.getGlobalBounds().intersects(a.getGlobalBounds(), intersection)) {
                spTadzik.move(0, -Utils::sgn(speedY)*intersection.height), std::cout << intersection.height;
                if (speedY>0) {
                    spTadzik.isStanding = true;
                    spTadzik.isJumping = false;
                }
                speedY=0;
            }
        }
        spTadzik.move(speedX, 0);
        intersection = sf::FloatRect(0, 0, 0, 0);
        for (auto a:vecFloor) {
            if (spTadzik.sprite.getGlobalBounds().intersects(a.getGlobalBounds(), intersection)) {
                spTadzik.move(-Utils::sgn(speedX)*intersection.width, 0);
                speedX=0;
            }
        }
        speedY+=gravity;
        speedX*=0.9;
        //spTadzik.velocity.y+=gravity;

        //spTadzik.updateEntity();
        for (unsigned int i=0; i<vecEnemies.size(); i++) {
            if (isActive(vecEnemies[i]->sprite, 1.1)) {
                vecEnemies[i]->move(vecEnemies[i]->velocity.x, 0);
                vecEnemies[i]->updateEntity();
            }
        }

        //ogarnianie kolizji
        //for (auto a:floor) manageCollision(spTadzik, a);
        //for (auto a:powerupBlocks) manageCollision(spTadzik, a);

        /*
        closestBreakable = -1;
        for (int i=breakable.size()-1; i>=0; i--) {
            if (manageCollision(spTadzik, breakable[i])) {
                if (closestBreakable==-1) closestBreakable = i;
                else if (std::abs(breakable[closestBreakable].center().x-spTadzik.sprite.getPosition().x)>std::abs(breakable[i].center().x-spTadzik.sprite.getPosition().x))
                    closestBreakable = i;
            }
        }
        */

        /*
        closestQuestionBlock = -1;
        for (int i=powerupBlocks.size()-1; i>=0; i--) {
            if (manageCollision(spTadzik, powerupBlocks[i])) {
                if (closestQuestionBlock==-1) closestQuestionBlock = i;
                else if (std::abs(powerupBlocks[closestQuestionBlock].center().x-spTadzik.sprite.getPosition().x)>std::abs(powerupBlocks[i].center().x-spTadzik.sprite.getPosition().x))
                    closestQuestionBlock = i;
            }
        if (spTadzik.powerLevel == 0) closestBreakable = -1;
        }

        if (closestBreakable != -1 && closestQuestionBlock != -1) {
            if (std::abs(breakable[closestBreakable].center().x-spTadzik.sprite.getPosition().x)>std::abs(powerupBlocks[closestQuestionBlock].center().x-spTadzik.sprite.getPosition().x)) {
                closestBreakable = -1;
            }
            else {
                closestQuestionBlock = -1;
            }
        }
        if (powerupBlocks.size()>0 && closestQuestionBlock!=-1) {
            if (powerupBlocks[closestQuestionBlock].hit(spTadzik.powerLevel) && spTadzik.powerLevel==0) {
                spritePowerup.setAnimation(&powerupAnim);
                spritePowerup.sprite.setPosition(powerupBlocks[closestQuestionBlock].getPosition().x+0.5*TileSize,
                                                 powerupBlocks[closestQuestionBlock].getPosition().y-0.5*TileSize);
                vecPowerups.push_back(spritePowerup);
            }
        }
        if (closestBreakable!=-1)
            if (breakable[closestBreakable].hit())
                breakable.erase(breakable.begin()+closestBreakable);



        for (unsigned int i=0; i<vecSnekes.size(); i++) {
            if (isActive(vecSnekes[i].sprite, 1.2))
            {
                for (auto a:vecFloor)
                    manageCollision(vecSnekes[i], a);
                for (auto a:breakable)
                    manageCollision(vecSnekes[i], a);
                for (auto a:powerupBlocks)
                    manageCollision(vecSnekes[i], a);
            }
        }
        */

        //enemies AI
        for (int i=vecEnemies.size()-1; i>=0; i--) {
            if (vecEnemies[i]->velocity.x==0) {
                vecEnemies[i]->sprite.setScale(-vecEnemies[i]->sprite.getScale().x, vecEnemies[i]->sprite.getScale().y);
            }
            vecEnemies[i]->velocity.x=1*Utils::sgn(vecEnemies[i]->sprite.getScale().x);
            if (vecEnemies[i]->sprite.getGlobalBounds().top > window->getSize().y+10) {
                delete vecEnemies[i];
                vecEnemies.erase(vecEnemies.begin()+i);
            }
            if (Utils::randInt(0, 100)<1) bullets.push_back(Bullet(&texBullet1, vecEnemies[i]->sprite.getPosition(), 25, Utils::randFloat(0, 2*3.1415)));
        }

        //ogarnianie monet i powerupow
        if (vecCoins.size()>0)
            for (int i=vecCoins.size()-1; i>=0; --i)
                if (Collision::PixelPerfectTest(vecCoins[i].sprite, spTadzik.sprite)) {
                    score++;
                    vecCoins.erase(vecCoins.begin()+i);
                }
        if (vecPowerups.size()>0)
            for (int i=vecPowerups.size()-1; i>=0; --i)
                if (Collision::PixelPerfectTest(vecPowerups[i].sprite, spTadzik.sprite)) {
                    spTadzik.powerLevel=1;
                    spTadzik.loadAnimations(animTadzikLVL2);
                    spTadzik.changeAnimation(spTadzik.current);
                    spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
                    vecPowerups.erase(vecPowerups.begin()+i);
                }

        //gameover
        if (spTadzik.sprite.getGlobalBounds().top>window->getSize().y) {
            vecFloor.clear();
            gameOver();
        }
        for (int i=vecEnemies.size()-1; i>=0; i--)
            if (isActive(vecEnemies[i]->sprite))
                if (Collision::BoundingBoxTest(spTadzik.sprite, vecEnemies[i]->sprite)) {
                    if (vecEnemies[i]->sprite.getGlobalBounds().top >= spTadzik.pos.bottom) {
                        spTadzik.jump();
                        effects.push_back(Effect(*vecEnemies[i], Utils::randFloat(-5, 5), -5.0, 0.0, 0.5));
                        vecEnemies[i]->onHit();
                        if (vecEnemies[i]->shouldDestroy) {
                            delete vecEnemies[i];
                            vecEnemies.erase(vecEnemies.begin()+i);
                        }
                    }
                    else {
                        if (Collision::PixelPerfectTest(spTadzik.sprite, vecEnemies[i]->sprite) && spTadzik.invincibility.getElapsedTime().asMilliseconds()>500) {
                            if (spTadzik.powerLevel==0) {
                                spTadzik.isDead = 1;
                            }
                            else {
                                spTadzik.powerLevel--;
                                speedX = -3*speedX;
                                speedY = - std::abs(speedY);
                                spTadzik.loadAnimations(animTadzikLVL1);
                                spTadzik.changeAnimation(spTadzik.current);
                                spTadzik.sprite.setOrigin(sf::Vector2f(spTadzik.sprite.getTextureRect().width/2, spTadzik.sprite.getTextureRect().height));
                                spTadzik.invincibility.restart();
                            }
                        }
                    }
                }

        textScore.setString(Utils::stringify(score));
        if (spTadzik.isDead) {
            spTadzik.setPosition(spTadzik.prevPosition);
            circle.setOrigin(circle.getRadius(), circle.getRadius()+20);
            circle.setPosition(spTadzik.sprite.getPosition());
            if (circle.getRadius()>50) circle.setRadius(circle.getRadius()-10);
            else {
                gameOver();
            }
        }
        spTadzik.updateAnimations();

        ///rysowanie
        window->clear();
        window->draw(spBackground);

        for (auto a:hitboxlessBack) {
            if (isActive(a)) window->draw(a);
        }
        for (auto a:vecFloor)              { window->draw(a);}
        for (auto a:breakable)          { if (isActive(a)) window->draw(a);}
        for (auto a:vecCoins)           { window->draw(a.sprite);}
        for (auto a:bullets)            { window->draw(a);}
        for (auto a:vecPowerups)        { window->draw(a.sprite);}
        for (auto a:vecEnemies)          { window->draw(a->sprite);}
        for (auto a:effects)            { window->draw(a.sprite);}
        for (auto a:powerupBlocks)      { window->draw(a);}
        window->draw(spTadzik.sprite);
        for (auto a:hitboxlessFront)    { if (isActive(a)) window->draw(a);}
        for (unsigned int i=0; i<vecWaterfall.size(); i++) {
            vecWaterfall[i].setTextureRect(sf::IntRect(0, -clock.getElapsedTime().asMilliseconds()/10, texWaterfall.getSize().x, 1000 ));
            window->draw(vecWaterfall[i]);
        }
        if (spTadzik.isDead) window->draw(circle);
        window->draw(textScore);
    }

protected:
    sf::View gameView;
    sf::Texture texBackground;
    sf::Texture texFloorTile;
    sf::Texture texPowerupTileActive;
    sf::Texture texPowerupTileInactive;
    sf::Texture texCoin;
    sf::Texture texBullet1;
    sf::Texture texPowerup;


    std::vector <sf::Texture> texSnekWalk;

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

    Player spTadzik;
    Snek tmpSnek;
    Armadillo tmpArmadillo;
    AnimatedSprite tmpCoin;
    Animation TadzikLVL2Run, TadzikLVL2Stand, TadzikLVL2Jump, TadzikLVL2Fall;
    Animation TadzikLVL1Run, TadzikLVL1Stand, TadzikLVL1Jump, TadzikLVL1Fall;
    Animation snekWalk;
    Animation coinRotate, powerupAnim;
    Animation armadilloWalk, armadilloHide;

    sf::Sprite spBackground;
    Tile FloorTile;
    MovingEntity spritePowerup;
    QuestionTile PowerupTile;

    std::vector <Tile> vecFloor;
    std::vector <sf::Sprite> hitboxlessFront;
    std::vector <sf::Sprite> hitboxlessBack;
    std::vector <QuestionTile> powerupBlocks;
    std::vector <MovingEntity> vecPowerups;
    std::vector <AnimatedSprite> vecCoins;
    std::vector <Enemy*> vecEnemies;

    std::vector <Animation*> animTadzikLVL1;
    std::vector <Animation*> animTadzikLVL2;
    std::vector <Animation*> animTadzikLVL3;

    std::vector <BreakableBlock> breakable;
    BreakableBlock BreakableTile;

    std::vector <Effect> effects;

    std::vector <Bullet> bullets;

    float& speedX = spTadzik.velocity.x;
    float& speedY = spTadzik.velocity.y;
    double maxSpeed = 10;
    double gravity = 0.5;
    double standingHeight = 0;
    double parallax = 0.66;
    double timeLeft = 30;

    int onTile = 0;
    int tilesPerY;
    int TileSize;
    int closestBreakable = -1;
    int closestQuestionBlock = -1;
    int score = 0;


    sf::Text textScore;
    sf::Clock clock;

    sf::CircleShape circle;

    //tu bylem domingo
    sf::Texture texColumn;
    sf::Texture texWaterfall;
    sf::Sprite tempColumn;
    sf::Sprite tempWaterfall;
    std::vector <sf::Sprite> vecWaterfall;

    sf::Vector2f viewOffset = {0, 0};
    sf::Vector2i windowSize;
    sf::Vector2f mapSize;
};
#endif //mario
