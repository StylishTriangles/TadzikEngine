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

class MARIO2: public Scene{
public:
    MARIO2(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

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

    class MovingEntity: public ARO::AnimSprite {
    public:
        enum direction {
            LEFT, RIGHT
        };
        MovingEntity(MARIO2* g) {
            game = g;
        }
        virtual void updateEntity() {
            velocity.y+=0.5;
            move(velocity);
        }
        void flipSprite() {
            setScale(std::abs(getScale().x)*Utils::sgn(velocity.x), getScale().y);
            if (Utils::sgn(getScale().x)>0) {
                currentDirection = RIGHT;
            }
            else
                currentDirection = LEFT;
        }
        sf::Vector2f velocity;
        sf::Vector2f prevVelocity = {0.1, 0};
        MARIO2* game;
        direction currentDirection = RIGHT;
    };

    class Player: public MovingEntity {
    public:
        enum animationType {
            IDLE, RUN, JUMP, FALL
        };
        Player(MARIO2* g) : MovingEntity(g) {
            game = g;
        }
        void updatePlayer(sf::Time deltaTime) {
            update(deltaTime.asMilliseconds()*velocity.x);
            move(0, velocity.y);
            sf::FloatRect intersection;
            for (auto a:game->vecTiles) {
                if (getGlobalBounds().intersects(a->getGlobalBounds(), intersection)) {
                    move(0, -Utils::sgn(velocity.y)*intersection.height);
                    if (velocity.y>0) {
                        a->onHitAbove();
                        //spTadzik.isStanding = true;
                        if (currentAnimationType!=RUN) {
                            setAnimation(&game->aTadzikRun);
                            currentAnimationType=RUN;
                        }
                        canJump = true;
                        if (std::abs(velocity.x)<0.1 && currentAnimationType!=IDLE) {
                            setAnimation(&game->aTadzikIdle);
                            currentAnimationType=IDLE;
                        }
                    }
                    else
                        a->onHitBelow();
                    velocity.y=0;
                }
            }
            move(velocity.x, 0);
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
                setAnimation(&game->aTadzikFall);
                currentAnimationType=FALL;
            }
            prevVelocity=velocity;
        }

        void jump() {
            if (canJump) {
                velocity.y = -13;
                canJump = false;
                setAnimation(&game->aTadzikJump);
                currentAnimationType=JUMP;
            }
        }

        bool canJump = true;
        animationType currentAnimationType=RUN;

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
        virtual void onHit() {

        }

    };

    class NME_Snek: public Enemy {
    public:
        NME_Snek (ARO::Anim* a, sf::Vector2f pos, MARIO2* g) : Enemy(a, pos, g) {
            velocity.x=0.5;
        }
        void onHit() {

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
            }
        }
    }

    void onSceneLoadToMemory() {
        texBackground.loadFromFile("files/textures/mario/background.png");
        texFloorTile.loadFromFile("files/textures/mario/floor1.png");
        texBreakableTile.loadFromFile("files/textures/mario/breakable1.png");

        aFloorTile.setSpriteSheet(&texFloorTile, 32, sf::seconds(1000000));
        aBreakableTile.setSpriteSheet(&texBreakableTile, 32, sf::seconds(1000000));
        spFloorTile.setTexture(texFloorTile);

        spBackground.setTexture(texBackground);
        texBackground.setRepeated(true);
        spBackground.setTextureRect(sf::IntRect(0, 0, 1000000000, texBackground.getSize().y));
        spBackground.setScale((double)window->getSize().y/(double)texBackground.getSize().y,
                              (double)window->getSize().y/(double)texBackground.getSize().y);

        spsSnekWalk.loadFromFile("files/textures/mario/snekWalk.png");
        aSnekWalk.setSpriteSheet(&spsSnekWalk, 44, sf::milliseconds(100));

        spsTadzikRun.loadFromFile("files/textures/universal/playerRun.png");
        spsTadzikIdle.loadFromFile("files/textures/universal/playerIdle.png");
        spsTadzikJump.loadFromFile("files/textures/universal/playerJump.png");
        spsTadzikFall.loadFromFile("files/textures/universal/playerFall.png");
        aTadzikRun.setSpriteSheet(&spsTadzikRun, 26, sf::milliseconds(500));
        aTadzikIdle.setSpriteSheet(&spsTadzikIdle, 26, sf::milliseconds(500));
        aTadzikJump.setSpriteSheet(&spsTadzikJump, 26, sf::milliseconds(500));
        aTadzikFall.setSpriteSheet(&spsTadzikFall, 26, sf::milliseconds(500));

        TADZIK.setAnimation(&aTadzikRun);
        TADZIK.setScale(2, 2);
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
        int scrollArea = 200;
        if (pos.x > windowSize.x-scrollArea && viewOffset.x+windowSize.x<mapSize.x) {
            gameView.move(pos.x-windowSize.x+scrollArea, 0);
            spBackground.move((pos.x-windowSize.x+scrollArea)*(1.0-parallax), 0);
            viewOffset.x+=pos.x-windowSize.x+scrollArea;
            if (viewOffset.x+windowSize.x>mapSize.x) {
                gameView.move(-viewOffset.x-windowSize.x+mapSize.x, 0);
                viewOffset.x=mapSize.x-windowSize.x;
            }
        }
        else if (pos.x < scrollArea && viewOffset.x>0) {
            gameView.move(pos.x-scrollArea, 0);
            spBackground.move((pos.x-scrollArea)*(1.0-parallax), 0);
            viewOffset.x+=pos.x-scrollArea;
            if (viewOffset.x<0) {
                gameView.move(-viewOffset.y, 0);
                viewOffset.x=0;
            }
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
        for (int i=vecTiles.size()-1; i>=0; i--) {
            if (vecTiles[i]->shouldDestroy()) {
                delete vecTiles[i];
                vecTiles.erase(vecTiles.begin()+i);
            }
        }

        ///OGARNIANIE PRZECIWNIKOW
        for (int i=vecEnemies.size()-1; i>=0; i--) {
            vecEnemies[i]->updateEnemy(sf::milliseconds(deltaTime));
        }

        /// GAME OVER
        if (TADZIK.getGlobalBounds().top>windowSize.y) {
            gameOver();
        }

        rGame.draw(spBackground);
        for (auto a:vecTiles)
            rGame.draw(*a);
        for (auto a:vecHitboxlessBack)
            rGame.draw(a);
        for (auto a:vecEnemies)
            rGame.draw(*a);
        rGame.draw(TADZIK);
        for (auto a:vecHitboxlessFront)
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

    sf::Texture spsSnekWalk;

    sf::Texture spsTadzikRun;
    sf::Texture spsTadzikIdle;
    sf::Texture spsTadzikJump;
    sf::Texture spsTadzikFall;

    ARO::Anim aTadzikRun;
    ARO::Anim aTadzikIdle;
    ARO::Anim aTadzikJump;
    ARO::Anim aTadzikFall;

    ARO::Anim aFloorTile;
    ARO::Anim aBreakableTile;

    ARO::Anim aSnekWalk;
    Player TADZIK = Player(this);
    sf::Sprite spBackground;
    sf::Sprite spFloorTile;

    std::vector <Enemy*> vecEnemies;
    std::vector <Tile*> vecTiles;
    std::vector <sf::Sprite> vecHitboxlessFront;
    std::vector <sf::Sprite> vecHitboxlessBack;

    sf::Vector2f windowSize;
    sf::Vector2f viewOffset;
    sf::Vector2f mapSize;
    int tileSize = 32;

    float parallax = 0.6;
};
#endif //MARIO2
