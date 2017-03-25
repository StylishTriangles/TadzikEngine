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

    class Tile: public ARO::AnimSprite {
    public:
        Tile(ARO::Anim* a, sf::Vector2f pos) {
            setPosition(pos);
            setAnimation(a);
        }
        virtual void onHitBelow() {

        }
        virtual void onHitAbove() {

        }
    };

    class Tile_Breakable: public Tile {
    public:
        Tile_Breakable(ARO::Anim* a, sf::Vector2f pos) : Tile(a, pos) {
            setLooped(false);
        }
        void onHitBelow() {
            nextFrame();
        }
    };

    class Tile_PowerUp: public Tile {
    public:
        Tile_PowerUp(ARO::Anim* a, sf::Vector2f pos, MARIO2* g, ARO::Anim* d) : Tile(a, pos) {
            setLooped(false);
            deactivated = d;
            game = g;
        }
        void onHitBelow() {
            if (active) {
                setAnimation(deactivated);
                active = false;
                game->vecPowerups.push_back(new Powerup(game, getPosition()-sf::Vector2f(0, game->tileSize), sf::Vector2f(3, -2 )));
            }
        }
        ARO::Anim* deactivated;
        MARIO2* game;
        bool active = true;
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
    };

    class Powerup: public MovingEntity {
    public:
        Powerup(MARIO2* g, sf::Vector2f pos, sf::Vector2f v) : MovingEntity(g, pos, v) {
            setAnimation(&g->aPowerup);
        }
        void updatePowerup(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(0, -Utils::sgn(velocity.y)*intersection.height);
                    velocity.y*=-1;
                }
            }
            move(velocity.x, 0);
            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(-Utils::sgn(velocity.x)*intersection.width, 0);
                    velocity.x*=-1;
                    flipSprite();
                }
            }
            velocity.y+=0.5;
            //velocity.x*=0.9;
        }
        void onPickup() {
            game->TADZIK.setScale(2, 2);
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
                    float t = Utils::getMagnitude(getPosition(), a->getPosition());
                    if (t<minDistance) {
                        minDistance = t;
                        tileCollided = a;
                        intersection = intersectionTMP;
                    }
                }
            }
            if (minDistance!=999999) {
                move(0, -Utils::sgn(velocity.y)*intersection.height);
                if (velocity.y>0) {
                    tileCollided->onHitAbove();
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
                    tileCollided->onHitBelow();
                velocity.y=0;
            }
            move(velocity.x, 0);

            ///KOLIZJA Z PRZECIWNIKAMI
            for (int i=game->vecEnemies.size()-1; i>=0; i--) {
                if (getGlobalBounds().intersects(game->vecEnemies[i]->getGlobalBounds(), intersection) && lastHit.getElapsedTime()>invincibilityTime) {
                    if (velocity.y>0 && prevGlobalBounds.top+prevGlobalBounds.height<game->vecEnemies[i]->getGlobalBounds().top) {
                        velocity.y=-10;
                        game->vecEnemies[i]->onKilled();
                        game->vecEnemies.erase(game->vecEnemies.begin()+i);
                        break;
                    }
                    else {
                        setScale(1.5, 1.5);
                        canJump = false;
                        lastHit.restart();
                        velocity.x*=-2;
                        velocity.x+=Utils::randFloat(-2, 2);
                        velocity.y=-10;
                    }
                }
            }

            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(-Utils::sgn(velocity.x)*intersection.width, 0);
                    velocity.x=0;
                }
            }
            velocity.y+=0.5;
            velocity.x*=0.9;
            if ((velocity.x>0 && currentDirection!=RIGHT) || (velocity.x<0 && currentDirection!=LEFT))
                flipSprite();
            if (Utils::sgn(velocity.y)!=Utils::sgn(prevVelocity.y) && velocity.y>0 && currentAnimationType!=FALL) {
                setAnimation(spriteSheets[level][FALL]);
                currentAnimationType=FALL;
            }
            prevVelocity=velocity;
        }

        void jump() {
            if (canJump) {
                velocity.y = -13;
                canJump = false;
                setAnimation(spriteSheets[level][JUMP]);
                currentAnimationType=JUMP;
            }
        }

        bool canJump = true;
        animationType currentAnimationType=RUN;
        sf::Clock lastHit;
        sf::Time invincibilityTime = sf::milliseconds(500);
        int level = 0;
        std::vector <std::vector <ARO::Anim*> > spriteSheets;
    };

    class Enemy: public MovingEntity {
    public:
        Enemy (ARO::Anim* a, sf::Vector2f pos, MARIO2* g) :MovingEntity(g) {
            setAnimation(a);
            Utils::setOriginInCenter(*this);
            setPosition(pos);
        }
        virtual void updateEnemy(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(0, -Utils::sgn(velocity.y)*intersection.height);
                    velocity.y=0;
                }
            }
            move(velocity.x, 0);
            intersection = sf::FloatRect(0, 0, 0, 0);
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(-Utils::sgn(velocity.x)*intersection.width, 0);
                    velocity.x*=-1;
                    flipSprite();
                }
            }
            velocity.y+=0.5;
            //velocity.x*=0.9;
        }
        virtual void onKilled() {
            game->vecEffects.push_back(Effect(*this, game, sf::Vector2f(Utils::randFloat(-5, 5), -5)));
            delete this;
        }

    };

    class NME_Snek: public Enemy {
    public:
        NME_Snek (ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Enemy(a, pos, g) {
            velocity.x=0.5;
            setScale(2, 2);
        }
    };

    void gameOver() {
        for (int i=0; i<vecTiles.size(); i++)
            delete vecTiles[i];
        for (int i=0; i<vecEnemies.size(); i++)
            delete vecEnemies[i];
        vecTiles.clear();
        vecEnemies.clear();
        loadMap("files/maps/mario/map2.png");
    }

    void loadMap(std::string mapDir) {
        sf::Image mapa;
        mapa.loadFromFile(mapDir);
        mapSize = sf::Vector2f(mapa.getSize().x*tileSize, mapa.getSize().y*tileSize);
        for (int x=0; x<mapa.getSize().x; x++) {
            for (int y=0; y<mapa.getSize().y; y++) {
                if (mapa.getPixel(x, y)==sf::Color(0, 0, 0)) {
                    vecTiles.push_back(new Tile(&aFloorTile, sf::Vector2f(x*tileSize, y*tileSize)));
                }
                else if(mapa.getPixel(x, y)==sf::Color(100, 100, 100)) {
                    spFloorTile.setPosition(x*tileSize, y*tileSize);
                    vecHitboxlessBack.push_back(spFloorTile);
                }
                else if(mapa.getPixel(x, y)==sf::Color(200, 200, 200)) {
                    spFloorTile.setPosition(x*tileSize, y*tileSize);
                    vecHitboxlessFront.push_back(spFloorTile);
                }
                else if(mapa.getPixel(x, y)==sf::Color(200, 100, 100)) {
                    vecTiles.push_back(new Tile_Breakable(&aBreakableTile, sf::Vector2f(x*tileSize, y*tileSize)));
                }
                else if(mapa.getPixel(x, y)==sf::Color(255, 0, 0)) {
                    vecEnemies.push_back(new NME_Snek(&aSnekWalk, sf::Vector2f(x*tileSize, y*tileSize), this));
                }
                else if(mapa.getPixel(x, y)==sf::Color(0, 0, 255)) {
                    TADZIK.setPosition(x*tileSize, y*tileSize);
                }
                else if(mapa.getPixel(x, y)==sf::Color(200, 0, 100)) {
                    vecTiles.push_back(new Tile_PowerUp(&aPowerUpTile, sf::Vector2f(x*tileSize, y*tileSize), this, &aPowerUpTile_ ));
                }
            }
        }
    }

    void onSceneLoadToMemory() {
        texBackground.loadFromFile("files/textures/mario/background.png");
        texFloorTile.loadFromFile("files/textures/mario/floor1.png");
        texBreakableTile.loadFromFile("files/textures/mario/breakable1.png");
        texPowerUpTile.loadFromFile("files/textures/mario/powerUpTileActive.png");
        texPowerUpTile_.loadFromFile("files/textures/mario/powerUpTileInactive.png");

        aFloorTile.setSpriteSheet(&texFloorTile, 32, sf::seconds(1000000));
        aBreakableTile.setSpriteSheet(&texBreakableTile, 32, sf::seconds(1000000));
        aPowerUpTile.setSpriteSheet(&texPowerUpTile, 32, sf::seconds(1000000));
        aPowerUpTile_.setSpriteSheet(&texPowerUpTile_, 32, sf::seconds(1000000));
        spFloorTile.setTexture(texFloorTile);

        spBackground.setTexture(texBackground);
        texBackground.setRepeated(true);
        spBackground.setTextureRect(sf::IntRect(0, 0, 1000000000, texBackground.getSize().y));
        spBackground.setScale((double)window->getSize().y/(double)texBackground.getSize().y,
                              (double)window->getSize().y/(double)texBackground.getSize().y);

        spsSnekWalk.loadFromFile("files/textures/mario/snekWalk.png");
        aSnekWalk.setSpriteSheet(&spsSnekWalk, 44, sf::milliseconds(100));

        spsPowerup.loadFromFile("files/textures/mario/powerup.png");
        aPowerup.setSpriteSheet(&spsPowerup, 32, sf::seconds(1000000));

        spsTadzikFall.resize(3);
        spsTadzikIdle.resize(3);
        spsTadzikJump.resize(3);
        spsTadzikRun.resize(3);

        aTadzikFall.resize(3);
        aTadzikIdle.resize(3);
        aTadzikJump.resize(3);
        aTadzikRun.resize(3);

        for (int i=0; i<3; i++) {
            spsTadzikRun[i].loadFromFile("files/textures/universal/playerRun"+Utils::stringify(i)+".png");
            spsTadzikIdle[i].loadFromFile("files/textures/universal/playerIdle"+Utils::stringify(i)+".png");
            spsTadzikJump[i].loadFromFile("files/textures/universal/playerJump"+Utils::stringify(i)+".png");
            spsTadzikFall[i].loadFromFile("files/textures/universal/playerFall"+Utils::stringify(i)+".png");
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
        loadMap("files/maps/mario/map2.png");
        rGame.create(window->getSize().x, window->getSize().y);
        windowSize = sf::Vector2f(window->getSize());
        gameView.reset(sf::FloatRect(0, 0, window->getSize().x, window->getSize().y));
    }

    void onSceneDeactivate() {
        for (int i=0; i<vecTiles.size(); i++)
            delete vecTiles[i];
        for (int i=0; i<vecEnemies.size(); i++)
            delete vecEnemies[i];
        vecTiles.clear();
        vecEnemies.clear();
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    void draw(double deltaTime) {
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
        TADZIK.updatePlayer(sf::milliseconds(deltaTime));

        ///OGARNIANIE KLOCKUF
        for (auto a=vecTiles.begin(); a!=vecTiles.end(); a++) {
            if ((*a)->shouldDestroy()) {
                delete *a;
                vecTiles.erase(a);
            }
        }

        ///OGARNIANIE PRZECIWNIKOW
        for (auto a=vecEnemies.begin(); a!=vecEnemies.end(); a++) {
            (*a)->updateEnemy(sf::milliseconds(deltaTime));
        }

        ///GAME OVER
        if (TADZIK.getGlobalBounds().top>windowSize.y) {
            gameOver();
            rGame.clear();
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
                vecPowerups[i]->updatePowerup(sf::milliseconds(deltaTime));
                if (Collision::BoundingBoxTest(TADZIK, *vecPowerups[i])) {
                    vecPowerups[i]->onPickup();
                    delete vecPowerups[i];
                    vecPowerups.erase(vecPowerups.begin()+i);
                }
            }
        }

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
        rGame.display();
        window->draw(sf::Sprite(rGame.getTexture()));
    }

protected:
    sf::View gameView;
    sf::RenderTexture rGame;
    sf::Texture texBackground;
    sf::Texture texFloorTile;
    sf::Texture texBreakableTile;
    sf::Texture texPowerUpTile;
    sf::Texture texPowerUpTile_;

    sf::Texture spsSnekWalk;
    sf::Texture spsPowerup;

    std::vector <sf::Texture> spsTadzikRun;
    std::vector <sf::Texture> spsTadzikIdle;
    std::vector <sf::Texture> spsTadzikJump;
    std::vector <sf::Texture> spsTadzikFall;

    std::vector <ARO::Anim> aTadzikRun;
    std::vector <ARO::Anim> aTadzikIdle;
    std::vector <ARO::Anim> aTadzikJump;
    std::vector <ARO::Anim> aTadzikFall;

    ARO::Anim aFloorTile;
    ARO::Anim aBreakableTile;
    ARO::Anim aPowerUpTile;
    ARO::Anim aPowerUpTile_;

    ARO::Anim aPowerup;

    ARO::Anim aSnekWalk;
    Player TADZIK = Player(this);
    sf::Sprite spBackground;
    sf::Sprite spFloorTile;

    std::vector <Enemy*> vecEnemies;
    std::vector <Tile*> vecTiles;
    std::vector <Powerup*> vecPowerups;
    std::vector <Effect> vecEffects;
    std::vector <sf::Sprite> vecHitboxlessFront;
    std::vector <sf::Sprite> vecHitboxlessBack;

    sf::Vector2f windowSize;
    sf::Vector2f mapSize;
    int tileSize = 32;

    float parallax = 0.6;
};
#endif //MARIO2
