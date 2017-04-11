#ifndef MARIO2_HPP
#define MARIO2_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

#define GRAVITY 0.5;

class MARIO2: public Scene{
public:
    MARIO2(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    class SpriteSheet {
    public:
        void loadFromFile(std::string dir, int tSize) {
            sps.loadFromFile(dir);
            tileSize = tSize;
        }
        void setSpriteTexture(sf::Sprite* sprite, sf::IntRect rect, sf::Vector2i origin = sf::Vector2i(0,0)) {
            sprite->setTexture(sps);
            sprite->setTextureRect(sf::IntRect(rect.left*tileSize, rect.top*tileSize, rect.width*tileSize, rect.height*tileSize));
            //sprite->setOrigin(sf::Vector2f(origin.x*tileSize+tileSize/2, origin.y*tileSize+tileSize/2));
        }
        sf::Texture sps;
        int tileSize;
    };

    class BlockInSpritesheet {
    public:
        BlockInSpritesheet() {}
        BlockInSpritesheet(SpriteSheet* s, sf::IntRect pos, int ID = 0) {
            sps = s;
            position = pos;
            item_ID = ID;
        }

        int item_ID;
        sf::IntRect position;
        SpriteSheet* sps;
    };

    class Effect: public sf::Drawable, sf::Transformable {
    public:
        Effect(sf::Sprite& s, MARIO2* g, sf::Vector2f vel) {
            sprite.setTexture(*s.getTexture());
            sprite.setTextureRect(s.getTextureRect());
            sprite.setOrigin(s.getOrigin());
            sprite.setScale(s.getScale());
            sprite.setPosition(s.getPosition());
            velocity = vel;
            game = g;
        }
        void update() {
            sprite.move(velocity);
            velocity.y+=GRAVITY;
            if (sprite.getGlobalBounds().top+sprite.getGlobalBounds().height<0 || sprite.getGlobalBounds().top>game->rGame.getSize().y) {
                shouldDestroy = true;
            }
        }
        sf::Vector2f velocity;
        MARIO2* game;
        sf::Sprite sprite;
        bool shouldDestroy = false;
    private:
        virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const {
            target.draw(sprite, states);
        }
    };

    class MovingEntity: public ARO::AnimSprite {
    public:
        enum direction {
            LEFT, RIGHT
        };
        MovingEntity(MARIO2* g) {
            game = g;
        }
        MovingEntity(MARIO2* g, sf::Vector2f pos, sf::Vector2f v) {
            game = g;
            velocity = v;
            prevVelocity = v;
            setPosition(pos);
        }
        virtual void updateEntity() {
            velocity.y+=GRAVITY;
            prevPosition = getPosition();
            prevGlobalBounds = getGlobalBounds();
            move(velocity);
        }
        void flipSprite() {
            setScale(std::abs(getScale().x)*Utils::sgn(velocity.x), getScale().y);
            setPlaySpeed(Utils::sgn(getScale().x));
            if (Utils::sgn(getScale().x)>0) {
                currentDirection = RIGHT;
            }
            else
                currentDirection = LEFT;
        }
        sf::Vector2f velocity;
        sf::Vector2f prevVelocity = {0.1, 0};
        sf::Vector2f prevPosition;
        sf::FloatRect prevGlobalBounds;
        MARIO2* game;
        direction currentDirection = RIGHT;
        bool canJump = false;
    };

    class Tile: public ARO::AnimSprite {
    public:
        Tile(){};
        Tile(ARO::Anim* a, sf::Vector2f pos, MARIO2* g) {
            setPosition(pos);
            setAnimation(a);
            game = g;
        }
        virtual void onHitBelow(int level = 0) {

        }
        virtual void onHitAbove(int level = 0) {

        }
        virtual void onCollision(MovingEntity* m, float area) {

        }
        virtual void updateTile() {

        }
        bool collidable = true;
        MARIO2* game;
    };

    class StaticTile: public Tile {
    public:
        StaticTile(SpriteSheet& s, sf::IntRect coords, sf::Vector2f position, sf::Color c = sf::Color::White) : Tile() {
            s.setSpriteTexture(this, coords);
            setPosition(position);
            setColor(c);
        }
        StaticTile(BlockInSpritesheet& b, sf::Vector2f position, sf::Color c = sf::Color::White) {
            b.sps->setSpriteTexture(this, b.position);
            setPosition(position);
            setColor(c);
        }
    };

    class Tile_Breakable: public Tile {
    public:
        Tile_Breakable(ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Tile(a, pos, g) {
            setLooped(false);
        }
        void onHitBelow(int level = 0) {
            if (level == 1)
                nextFrame();
        }
    };

    class Tile_PowerUp: public Tile {
    public:
        Tile_PowerUp(ARO::Anim* a, sf::Vector2f pos, ARO::Anim* d, MARIO2* g) : Tile(a, pos, g) {
            setLooped(false);
            deactivated = d;
        }
        void onHitBelow(int level = 0) {
            if (active) {
                setAnimation(deactivated);
                active = false;
                game->vecPowerups.push_back(new Powerup(game, getPosition()-sf::Vector2f(0, game->tileSize), sf::Vector2f(3, -2 )));
            }
        }
        ARO::Anim* deactivated;
        bool active = true;
    };

    class Tile_Water: public Tile {
    public:
        Tile_Water(ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Tile(a, pos, g) {
            setLooped(true);
            collidable = false;
        }
        void onCollision(MovingEntity* m, float area) {
            m->velocity.y*=0.99;
            m->velocity.y-=0.35*(area/(32*32));
            if (m->velocity.y<0 && area/(32*32)<0.5)
                m->canJump = true;
        }
    };

    class Tile_Timed: public Tile {
    public:
        Tile_Timed(ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Tile(a, pos, g) {
            setPlaySpeed(0.0000001);
        }
        ~Tile_Timed() {
            game->vecEffects.push_back(Effect(*this, game, sf::Vector2f(Utils::randFloat(-5, 5), -5)));
        }
        void onHitAbove(int level) {
            if (!primed) {
                primed = true;
                timeElapsed.restart();
            }
        }
        void onHitBelow(int level) {
            if (!primed) {
                primed = true;
                timeElapsed.restart();
            }
        }
        void updateTile() {
            if (primed) {
                if (timeElapsed.getElapsedTime()>decayTime)
                    m_destroy = true;
                else
                    setFrame(animation->frames*(timeElapsed.getElapsedTime()/decayTime));
            }
        }
        bool primed = false;
        sf::Clock timeElapsed;
        sf::Time decayTime = sf::seconds(1);
    };

    class Powerup: public MovingEntity {
    public:
        Powerup(MARIO2* g, sf::Vector2f pos, sf::Vector2f v) : MovingEntity(g, pos, v) {
            setAnimation(&g->aPowerup);
        }
        virtual void updatePowerup(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(0, -Utils::sgn(velocity.y)*intersection.height);
                        velocity.y*=-1;
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            move(velocity.x, 0);
            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(-Utils::sgn(velocity.x)*intersection.width, 0);
                        velocity.x*=-1;
                        flipSprite();
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            velocity.y+=0.5;
            //velocity.x*=0.9;
        }
        virtual void onPickup() {
            game->TADZIK.levelUp();
        }
    };

    class Coin: public Powerup {
    public:
        void updatePowerup() {};
        void onPickup() {
            game->score+=500;
        }

    };

    class Player: public MovingEntity {
    public:
        enum animationType {
            IDLE, RUN, JUMP, FALL
        };
        Player(MARIO2* g) : MovingEntity(g) {
            game = g;
            spriteSheets.resize(3);
            for (unsigned int i=0; i<3; i++) {
                spriteSheets[i].resize(4);
            }
        }
        void setTadzikSize(int l, ARO::Anim* idle_, ARO::Anim* run_, ARO::Anim* jump_, ARO::Anim* fall_) {
            spriteSheets[l][0]=idle_;
            spriteSheets[l][1]=run_;
            spriteSheets[l][2]=jump_;
            spriteSheets[l][3]=fall_;
        }
        void updatePlayer(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);

            sf::FloatRect intersection, intersectionTMP;
            Tile* tileCollided;
            float minDistance=999999;

            move(0, velocity.y);

            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersectionTMP)) {
                    if (a->collidable) {
                        float t = Utils::getMagnitude(getPosition(), a->getPosition());
                        if (t<minDistance) {
                            minDistance = t;
                            tileCollided = a;
                            intersection = intersectionTMP;
                        }
                    }
                    else {
                        a->onCollision(this, Utils::getArea(intersection));
                    }

                }
            }
            if (minDistance!=999999) {
                tileCollided->onCollision(this, 32*32);
                move(0, -Utils::sgn(velocity.y)*intersection.height);
                if (velocity.y>0) {
                    tileCollided->onHitAbove(level);
                    //spTadzik.isStanding = true;
                    if (currentAnimationType!=RUN) {
                        setAnimation(spriteSheets[level][RUN]);
                        currentAnimationType=RUN;
                    }
                    canJump = true;
                    if (std::abs(velocity.x)<0.1 && currentAnimationType!=IDLE) {
                        setAnimation(spriteSheets[level][IDLE]);
                        currentAnimationType=IDLE;
                    }
                }
                else
                    tileCollided->onHitBelow(level);
                velocity.y=0;
            }
            move(velocity.x, 0);

            ///KOLIZJA Z PRZECIWNIKAMI
            for (int i=game->vecEnemies.size()-1; i>=0; i--) {
                if (getGlobalBounds().intersects(game->vecEnemies[i]->getGlobalBounds(), intersection) && lastHit.getElapsedTime()>invincibilityTime) {
                    if (velocity.y>0 && prevGlobalBounds.top+prevGlobalBounds.height<=game->vecEnemies[i]->getGlobalBounds().top) {
                        velocity.y=-10;
                        game->vecEnemies[i]->onHit();
                        move(0, game->vecEnemies[i]->getGlobalBounds().top-getGlobalBounds().top-getGlobalBounds().height);
                        break;
                    }
                    else if (Collision::PixelPerfectTest(*this, *(game->vecEnemies[i]))) {
                        levelDown();
                    }
                }
            }

            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(-Utils::sgn(velocity.x)*intersection.width, 0);
                        velocity.x=0;
                    }
                    else {
                        a->onCollision(this, Utils::getArea(intersection));
                    }
                }
            }
            velocity.y+=GRAVITY;
            velocity.x*=0.9;
            if ((velocity.x>0 && currentDirection!=RIGHT) || (velocity.x<0 && currentDirection!=LEFT))
                flipSprite();
            if (Utils::sgn(velocity.y)!=Utils::sgn(prevVelocity.y) && velocity.y>0 && currentAnimationType!=FALL) {
                setAnimation(spriteSheets[level][FALL]);
                currentAnimationType=FALL;
            }
            prevVelocity=velocity;
            prevGlobalBounds = getGlobalBounds();
        }

        void jump() {
            if (canJump) {
                velocity.y = -jumpHeight;
                canJump = false;
                setAnimation(spriteSheets[level][JUMP]);
                currentAnimationType=JUMP;
            }
        }

        void levelUp() {
            if (level == 0) {
                level++;
                setScale(Utils::sgn(getScale().x)*2,2);
            }
        }

        void levelDown() {
            if (level == 0) {
                isDead = true;
                deadTime.restart();
            }
            else {
                setScale(Utils::sgn(getScale().x)*1.5, 1.5);
                canJump = false;
                lastHit.restart();
                velocity.x*=-2;
                velocity.x+=Utils::randFloat(-2, 2);
                velocity.y=-10;
                level--;
                setAnimation(spriteSheets[level][FALL]);
            }
        }

        animationType currentAnimationType=RUN;
        sf::Clock lastHit;
        sf::Time invincibilityTime = sf::milliseconds(500);
        int level = 0;
        int jumpHeight = 13;
        std::vector <std::vector <ARO::Anim*> > spriteSheets;
        bool isDead;
        sf::Clock deadTime;
    };

    class Enemy: public MovingEntity {
    public:
        Enemy (ARO::Anim* a, sf::Vector2f pos, MARIO2* g) :MovingEntity(g) {
            setAnimation(a);
            Utils::setOriginInCenter(*this);
            setPosition(pos);
            velocity = sf::Vector2f(1, 0);
        }
        virtual void updateEnemy(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(0, -Utils::sgn(velocity.y)*intersection.height);
                        velocity.y=0;
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            move(velocity.x, 0);
            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(-Utils::sgn(velocity.x)*intersection.width, 0);
                        velocity.x*=-1;
                        flipSprite();
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            velocity.y+=0.5;
            //velocity.x*=0.9;
        }
        virtual void onKilled() {
            game->vecEffects.push_back(Effect(*this, game, sf::Vector2f(Utils::randFloat(-5, 5), -5)));
            m_destroy = true;
            game->score+=200;
        }
        virtual void onHit() {

        }
    };

    class NME_Snek: public Enemy {
    public:
        NME_Snek (ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Enemy(a, pos, g) {
            velocity.x=0.5;
            setScale(2, 2);
        }
        void onHit() {
            onKilled();
        }
    };

    class NME_Armadillo: public Enemy {
    public:
        NME_Armadillo (ARO::Anim* a, sf::Vector2f pos, MARIO2* g, ARO::Anim* a1) : Enemy(a, pos, g) {
            setScale(2, 2);
            hiding = a1;
        }
        void updateEnemy(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(0, -Utils::sgn(velocity.y)*intersection.height);
                        velocity.y=0;
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            move(velocity.x, 0);
            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    if (a->collidable) {
                        move(-Utils::sgn(velocity.x)*intersection.width, 0);
                        velocity.x*=-1;
                        flipSprite();
                    }
                    a->onCollision(this, Utils::getArea(intersection));
                }
            }
            velocity.y+=0.5;
            if (wasHit) {
                rotate(velocity.x*deltaTime.asMilliseconds()*0.1);
                for (int i=0; i<game->vecEnemies.size(); i++) {
                    if (game->vecEnemies[i]!=this && Collision::PixelPerfectTest(*this, *(game->vecEnemies[i]))) {
                        game->vecEnemies[i]->onKilled();
                    }
                }
            }
        }
        void onHit() {
            if (wasHit) {
                if (velocity.x==0) {
                    velocity.x = Utils::sgn(Utils::randFloat(-1, 1))*10;
                    setScale(std::abs(getScale().x)*Utils::sgn(velocity.x), getScale().y);
                }
                else {
                    velocity.x = 0;
                }
            }
            else {
                wasHit = true;
                setAnimation(hiding);
                velocity.x = 0;
            }
        }
        ARO::Anim* hiding;
        bool wasHit = false;
    };

    void gameOver() {
        TADZIK.setScale(1.5, 1.5);
        TADZIK.setRotation(0);
        TADZIK.isDead = false;
        TADZIK.velocity = sf::Vector2f(0.001,0);
        for (int i=0; i<vecTiles.size(); i++)
            delete vecTiles[i];
        for (int i=0; i<vecEnemies.size(); i++)
            delete vecEnemies[i];
        for (int i=0; i<vecPowerups.size(); i++)
            delete vecPowerups[i];
        vecTiles.clear();
        vecEnemies.clear();
        vecPowerups.clear();
        vecEffects.clear();
        loadMap("files/maps/mario/map3.png");
    }

    bool colorCompare(sf::Color c1, sf::Color c2) {
        return ((c1.r==c2.r) && (c1.g==c2.g) && (c1.b==c2.b) && c1.a!=0);
    }

    void loadMap(std::string mapDir) {
        sf::Image mapa;
        mapa.loadFromFile(mapDir);
        mapSize = sf::Vector2f(mapa.getSize().x*tileSize, mapa.getSize().y*tileSize);
        for (int x=0; x<mapa.getSize().x; x++) {
            for (int y=0; y<mapa.getSize().y; y++) {
                sf::Color color = mapa.getPixel(x, y);
                if (color.b==255 && color.a!=0) {
                    if      (color.a == 255) vecTiles.push_back(new StaticTile(vecBlocks[color.r/30][color.g/30], sf::Vector2f(x*tileSize, y*tileSize)));
                    else if (color.a == 200) vecHitboxlessFront.push_back(StaticTile(vecBlocks[color.r/30][color.g/30], sf::Vector2f(x*tileSize, y*tileSize), sf::Color(200, 200, 200)));
                    else if (color.a == 100) vecHitboxlessBack.push_back(StaticTile(vecBlocks[color.r/30][color.g/30], sf::Vector2f(x*tileSize, y*tileSize)));
                }

                if(mapa.getPixel(x, y)==sf::Color(200, 100, 100)) {
                    vecTiles.push_back(new Tile_Breakable(&aBreakableTile, sf::Vector2f(x*tileSize, y*tileSize), this));
                }
                if(mapa.getPixel(x, y)==sf::Color(255, 0, 0)) {
                    vecEnemies.push_back(new NME_Snek(&aSnekWalk, sf::Vector2f(x*tileSize, y*tileSize), this));
                }
                if(mapa.getPixel(x, y)==sf::Color(255, 1, 0)) {
                    vecEnemies.push_back(new NME_Armadillo(&aArmadillo, sf::Vector2f(x*tileSize, y*tileSize), this, &aArmadillo_));
                }
                else if(mapa.getPixel(x, y)==sf::Color(0, 200, 0)) {
                    TADZIK.setPosition(x*tileSize, y*tileSize);
                }
                if(mapa.getPixel(x, y)==sf::Color(200, 0, 100)) {
                    vecTiles.push_back(new Tile_PowerUp(&aPowerUpTile, sf::Vector2f(x*tileSize, y*tileSize), &aPowerUpTile_, this));
                }
                if(mapa.getPixel(x, y)==sf::Color(0, 255, 0)) {
                    vecTiles.push_back(new Tile_Water(&aWater, sf::Vector2f(x*tileSize, y*tileSize), this));
                }
                if(mapa.getPixel(x, y)==sf::Color(255, 100, 0)) {
                    vecTiles.push_back(new Tile_Timed(&aTimedTile, sf::Vector2f(x*tileSize, y*tileSize), this));
                }
            }
        }
    }

    void onSceneLoadToMemory() {
        tileSpriteSheet.loadFromFile("files/textures/mario/tileSpritesheet.png", 32);

        vecBlocks.resize(5);
        for (int i=0; i<5; i++) {
            vecBlocks[i].resize(5);
        }
        for (int i=0; i<5; i++) {
            for (int j=0; j<5; j++) {
                vecBlocks[i][j]=BlockInSpritesheet(&tileSpriteSheet, sf::IntRect(i, j, 1, 1));
            }
        }

//        BISwood = BlockInSpritesheet(&tileSpriteSheet, sf::IntRect(1, 0, 1, 1));

        texBackground.loadFromFile("files/textures/mario/background.png");
        texBreakableTile.loadFromFile("files/textures/mario/breakable1.png");
        texPowerUpTile.loadFromFile("files/textures/mario/powerUpTileActive.png");
        texPowerUpTile_.loadFromFile("files/textures/mario/powerUpTileInactive.png");

        aBreakableTile.setSpriteSheet(&texBreakableTile, 32, sf::seconds(1000000));
        aPowerUpTile.setSpriteSheet(&texPowerUpTile, 32, sf::seconds(1000000));
        aPowerUpTile_.setSpriteSheet(&texPowerUpTile_, 32, sf::seconds(1000000));
        spsTimedTile.loadFromFile("files/textures/mario/timedTile.png");
        aTimedTile.setSpriteSheet(&spsTimedTile, 32, sf::seconds(1000000));

        spBackground.setTexture(texBackground);
        texBackground.setRepeated(true);
        spBackground.setTextureRect(sf::IntRect(0, 0, 1000000000, texBackground.getSize().y));
        spBackground.setScale((double)window->getSize().y/(double)texBackground.getSize().y,
                              (double)window->getSize().y/(double)texBackground.getSize().y);

        spsSnekWalk.loadFromFile("files/textures/mario/snekWalk.png");
        aSnekWalk.setSpriteSheet(&spsSnekWalk, 44, sf::milliseconds(100));
        spsArmadillo.loadFromFile("files/textures/mario/armadillo.png");
        aArmadillo.setSpriteSheet(&spsArmadillo, 30, sf::milliseconds(150));
        spsArmadillo_.loadFromFile("files/textures/mario/armadillo_ball.png");
        aArmadillo_.setSpriteSheet(&spsArmadillo_, 26, sf::milliseconds(100));

        spsPowerup.loadFromFile("files/textures/mario/powerup.png");
        aPowerup.setSpriteSheet(&spsPowerup, 32, sf::seconds(1000000));

        spsWater.loadFromFile("files/textures/mario/water.png");
        aWater.setSpriteSheet(&spsWater, 32, sf::seconds(1000000));

        spsTadzikFall.resize(3);
        spsTadzikIdle.resize(3);
        spsTadzikJump.resize(3);
        spsTadzikRun.resize(3);

        aTadzikFall.resize(3);
        aTadzikIdle.resize(3);
        aTadzikJump.resize(3);
        aTadzikRun.resize(3);

        for (int i=0; i<2; i++) {
            spsTadzikRun[i].loadFromFile("files/textures/mario/player/playerRun"+Utils::stringify(i)+".png");
            spsTadzikIdle[i].loadFromFile("files/textures/mario/player/playerIdle"+Utils::stringify(i)+".png");
            spsTadzikJump[i].loadFromFile("files/textures/mario/player/playerJump"+Utils::stringify(i)+".png");
            spsTadzikFall[i].loadFromFile("files/textures/mario/player/playerFall"+Utils::stringify(i)+".png");
            aTadzikRun[i].setSpriteSheet(&spsTadzikRun[i], 26, sf::milliseconds(500));
            aTadzikIdle[i].setSpriteSheet(&spsTadzikIdle[i], 26, sf::milliseconds(500));
            aTadzikJump[i].setSpriteSheet(&spsTadzikJump[i], 26, sf::milliseconds(500));
            aTadzikFall[i].setSpriteSheet(&spsTadzikFall[i], 26, sf::milliseconds(500));
            TADZIK.setTadzikSize(i, &aTadzikIdle[i], &aTadzikRun[i], &aTadzikJump[i], &aTadzikFall[i]);
        }

        TADZIK.setAnimation(&aTadzikRun[0]);
        TADZIK.setScale(1.5, 1.5);
        Utils::setOriginInCenter(TADZIK);
    }

    void onSceneActivate() {
        loadMap("files/maps/mario/map3.png");
        rGame.create(1280, 720);
        windowSize = sf::Vector2f(window->getSize());
        gameView.reset(sf::FloatRect(0, 0, rGame.getSize().x, rGame.getSize().y));
    }

    void onSceneDeactivate() {
        for (int i=0; i<vecTiles.size(); i++)
            delete vecTiles[i];
        for (int i=0; i<vecEnemies.size(); i++)
            delete vecEnemies[i];
        for (int i=0; i<vecPowerups.size(); i++)
            delete vecPowerups[i];
        vecTiles.clear();
        vecEnemies.clear();
        vecPowerups.clear();
        vecEffects.clear();
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void draw(sf::Time deltaTime) {
        ///OGARNIANIE VIEW
        sf::Vector2i pos = rGame.mapCoordsToPixel(TADZIK.getPosition(), gameView);
        int scrollAreaRight = windowSize.x*(4.0f/8.0f);
        int scrollAreaLeft = windowSize.y*(1.0f/5.0f);
        if (pos.x > windowSize.x-scrollAreaRight && Utils::getViewOffset(&rGame).x+windowSize.x<mapSize.x) {
            gameView.move(pos.x-windowSize.x+scrollAreaRight, 0);
            spBackground.move((pos.x-windowSize.x+scrollAreaRight)*(1.0-parallax), 0);
            if (Utils::getViewOffset(&rGame).x+windowSize.x>mapSize.x) {
                gameView.move(-Utils::getViewOffset(&rGame).x-windowSize.x+mapSize.x, 0);
            }
        }
        else if (pos.x < scrollAreaLeft && Utils::getViewOffset(&rGame).x>0) {
            gameView.move(pos.x-scrollAreaLeft, 0);
            spBackground.move((pos.x-scrollAreaLeft)*(1.0-parallax), 0);
            if (Utils::getViewOffset(&rGame).x<0) {
                gameView.move(-Utils::getViewOffset(&rGame).y, 0);
            }
        }
        if (Utils::getViewOffset(&rGame).x<0) {
            gameView.move(-Utils::getViewOffset(&rGame).x, 0);
        }
        rGame.setView(gameView);

        if (TADZIK.isDead) {
            if (TADZIK.deadTime.getElapsedTime().asSeconds()>1) {
                gameOver();
                rGame.clear();
            }
            else {
                TADZIK.setRotation((TADZIK.deadTime.getElapsedTime()/sf::seconds(1))*720);
                TADZIK.setScale(TADZIK.getScale().x*0.95, TADZIK.getScale().y*0.95);
            }
        }
        else {
            ///INPUT Z KLAWIATURY
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) {
                TADZIK.velocity.x-=1;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
                TADZIK.velocity.x+=1;
            }
            if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
                TADZIK.jump();
            }
            TADZIK.updatePlayer(deltaTime);
        }

        ///OGARNIANIE KLOCKUF
        for (auto a=vecTiles.begin(); a!=vecTiles.end(); a++) {
            (*a)->updateTile();
            if ((*a)->shouldDestroy()) {
                delete *a;
                vecTiles.erase(a);
            }
        }

        ///OGARNIANIE PRZECIWNIKOW
        for (int i=vecEnemies.size()-1; i>=0; i--) {
            vecEnemies[i]->updateEnemy(deltaTime);
            if (vecEnemies[i]->shouldDestroy()) {
                delete vecEnemies[i];
                vecEnemies.erase(vecEnemies.begin()+i);
            }
        }

        ///GAME OVER
        if (TADZIK.getGlobalBounds().top>windowSize.y) {
            TADZIK.isDead = true;
        }

        ///EFEKTY
        if (!vecEffects.empty()) {
            for (int i=vecEffects.size()-1; i>=0; i--) {
                vecEffects[i].update();
                if (vecEffects[i].shouldDestroy) {
                    vecEffects.erase(vecEffects.begin()+i);
                }
            }
        }

        ///POWERUPY
        if (!vecPowerups.empty()) {
            for (int i=vecPowerups.size()-1; i>=0; --i) {
                vecPowerups[i]->updatePowerup(deltaTime);
                if (Collision::BoundingBoxTest(TADZIK, *vecPowerups[i])) {
                    vecPowerups[i]->onPickup();
                    delete vecPowerups[i];
                    vecPowerups.erase(vecPowerups.begin()+i);
                }
            }
        }

        rGame.clear();
        rGame.draw(spBackground);
        for (auto a:vecTiles)
            rGame.draw(*a);
        for (auto a:vecHitboxlessBack)
            rGame.draw(a);
        for (auto a:vecPowerups)
            rGame.draw(*a);
        for (auto a:vecEnemies)
            rGame.draw(*a);
        rGame.draw(TADZIK);
        for (auto a:vecHitboxlessFront)
            rGame.draw(a);
        for (auto a:vecEffects)
            rGame.draw(a);
        if (debug) {
            for (auto a:vecEnemies) {
                Utils::drawBoundingBox(*a, &rGame);
            }
            Utils::drawBoundingBox(TADZIK, &rGame);
        }
        rGame.display();
        sf::Sprite tmp = sf::Sprite(rGame.getTexture());
        //tmp.setScale(window->getSize().y/(mapSize.y), window->getSize().y/(mapSize.y));
        window->draw(tmp);

        score-=deltaTime.asSeconds()*20;
    }

protected:
    sf::View gameView;
    sf::RenderTexture rGame;
    sf::Texture texBackground;
    sf::Texture texBreakableTile;
    sf::Texture texPowerUpTile;
    sf::Texture texPowerUpTile_;

    SpriteSheet tileSpriteSheet;

    sf::Texture spsSnekWalk;
    sf::Texture spsArmadillo;
    sf::Texture spsArmadillo_;

    sf::Texture spsPowerup;
    sf::Texture spsWater;
    sf::Texture spsTimedTile;

    std::vector <sf::Texture> spsTadzikRun;
    std::vector <sf::Texture> spsTadzikIdle;
    std::vector <sf::Texture> spsTadzikJump;
    std::vector <sf::Texture> spsTadzikFall;

    std::vector <ARO::Anim> aTadzikRun;
    std::vector <ARO::Anim> aTadzikIdle;
    std::vector <ARO::Anim> aTadzikJump;
    std::vector <ARO::Anim> aTadzikFall;

    ARO::Anim aBreakableTile;
    ARO::Anim aPowerUpTile;
    ARO::Anim aPowerUpTile_;
    ARO::Anim aWater;
    ARO::Anim aTimedTile;

    ARO::Anim aPowerup;

    ARO::Anim aSnekWalk;
    ARO::Anim aArmadillo;
    ARO::Anim aArmadillo_;

    Player TADZIK = Player(this);
    sf::Sprite spBackground;
    sf::Sprite spFloorTile;

    std::vector <Enemy*> vecEnemies;
    std::vector <Tile*> vecTiles;
    std::vector <StaticTile> vecStaticTiles;
    std::vector <Powerup*> vecPowerups;
    std::vector <Effect> vecEffects;
    std::vector <Tile> vecHitboxlessFront;
    std::vector <Tile> vecHitboxlessBack;

    sf::Vector2f windowSize;
    sf::Vector2f mapSize;
    int tileSize = 32;
    int score = 10000;
    bool debug = false;

    float parallax = 0.6;

    std::vector <std::vector <BlockInSpritesheet> > vecBlocks;
};
#endif //MARIO2
