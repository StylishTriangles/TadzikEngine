#ifndef SHOOTER2D_HPP
#define SHOOTER2D_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"
#include "../include/Common.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>
#include <fstream>


class SHOOTER2D: public Scene{
public:
    SHOOTER2D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

    class HUD {
    public:
        HUD(SHOOTER2D* g) {
            game = g;
            tAmmo.setPosition(game->windowSize.x-80, game->windowSize.y-30);
            tAllAmmo.setPosition(game->windowSize.x-30, game->windowSize.y-30);
            tScore.setPosition(game->windowSize.x-30, 0);
            tLights.setPosition(game->windowSize.x-130, game->windowSize.y-25);
            tLights.setScale(0.75, 0.75);
            healthBar.setPosition(10, 5);
            healthFrame.setPosition(10, 5);
            activeWeapon.setPosition(game->windowSize.x-130, game->windowSize.y-120);

            frameTopLeft.setPoint(0, sf::Vector2f(0, 0));
            frameTopLeft.setPoint(1, sf::Vector2f(120, 0));
            frameTopLeft.setPoint(2, sf::Vector2f(90, 30));
            frameTopLeft.setPoint(3, sf::Vector2f(0, 30));
            frameTopLeft.setFillColor(sf::Color::Yellow);

            frameTopRight.setPoint(0, sf::Vector2f(game->windowSize.x, 0));
            frameTopRight.setPoint(1, sf::Vector2f(game->windowSize.x-120, 0));
            frameTopRight.setPoint(2, sf::Vector2f(game->windowSize.x-90, 30));
            frameTopRight.setPoint(3, sf::Vector2f(game->windowSize.x, 30));
            frameTopRight.setFillColor(sf::Color::Yellow);

            frameBotLeft.setPoint(0, sf::Vector2f(0, 0));
            frameBotLeft.setPoint(1, sf::Vector2f(120, 0));
            frameBotLeft.setPoint(2, sf::Vector2f(90, 30));
            frameBotLeft.setPoint(3, sf::Vector2f(0, 30));
            frameBotLeft.setFillColor(sf::Color::Yellow);

            frameBotRight.setPoint(0, sf::Vector2f(game->windowSize.x, game->windowSize.y));
            frameBotRight.setPoint(1, sf::Vector2f(game->windowSize.x-120, game->windowSize.y));
            frameBotRight.setPoint(2, sf::Vector2f(game->windowSize.x-90, game->windowSize.y-30));
            frameBotRight.setPoint(3, sf::Vector2f(game->windowSize.x, game->windowSize.y-30));
            frameBotRight.setFillColor(sf::Color::Yellow);
        }
        sf::Sprite healthBar;
        sf::Sprite healthFrame;
        sf::Sprite activeWeapon;
        sf::Text tScore;
        sf::Text tAmmo;
        sf::Text tAllAmmo;
        sf::Text tLights;

        sf::ConvexShape frameTopLeft = sf::ConvexShape(4);
        sf::ConvexShape frameTopRight = sf::ConvexShape(4);
        sf::ConvexShape frameBotLeft = sf::ConvexShape(4);
        sf::ConvexShape frameBotRight = sf::ConvexShape(4);
        void update() {
            tScore.setString(Utils::stringify(game->score));
            tAmmo.setString(Utils::stringify(game->TADZIK.weapons[game->TADZIK.currentWeapon].ammo));
            tAllAmmo.setString(Utils::stringify(game->TADZIK.weapons[game->TADZIK.currentWeapon].mags));
            tLights.setString(Utils::stringify(game->TADZIK.lights));
            healthBar.setTextureRect(sf::IntRect(0, 0, game->TADZIK.health/100*game->texHealthBar.getSize().x, healthBar.getTextureRect().height));
            activeWeapon.setTexture(game->TADZIK.weapons[game->TADZIK.currentWeapon].texture);

            draw();
        }
        void draw() {
            game->window->draw(frameTopLeft);
            game->window->draw(frameTopRight);
            game->window->draw(frameBotRight);
            game->window->draw(healthFrame);
            game->window->draw(healthBar);
            game->window->draw(tScore);
            game->window->draw(tAmmo);
            game->window->draw(tAllAmmo);
            game->window->draw(tLights);
            game->window->draw(activeWeapon);
        }
        void setFont(sf::Font* f, sf::Color c) {
            tScore.setFont(*f);
            tAmmo.setFont(*f);
            tAllAmmo.setFont(*f);
            tLights.setFont(*f);
            tScore.setColor(c);
            tAmmo.setColor(c);
            tAllAmmo.setColor(c);
            tLights.setColor(c);
        }
        SHOOTER2D* game;
    };

    class Wave {
    public:

        Wave(float t, int e) {
            time = t; maxEnemies = e;
        }
        float time;
        int maxEnemies;
    };

    class MovingEntity: public sf::Sprite {
    public:
        MovingEntity() {
            hitbox.setOrigin(hitbox.getRadius(), hitbox.getRadius());
        }
        sf::Vector2f velocity;
        virtual void update() {
            move(velocity);
            hitbox.setPosition(getPosition());
            velocity.x*=0.8;
            velocity.y*=0.8;
        }
        sf::CircleShape hitbox = sf::CircleShape(10);
    };

    class Object {
    public:
        std::vector <sf::Vector2f> points;
    };

    class lightSource: public sf::Vector2f{
    public:
        lightSource(SHOOTER2D* g){
            game = g;
        }
        lightSource(sf::Vector2f pos, SHOOTER2D* g) {
            x=pos.x, y=pos.y;
            game = g;
        }
        lightSource(sf::Vector2f pos, sf::Texture* t1, sf::Texture* t2, SHOOTER2D* g, sf::Color color = sf::Color::White) {
            x=pos.x, y=pos.y;
            glow.setTexture(*t2);
            glow.setOrigin(glow.getTextureRect().width/2, glow.getTextureRect().height/2);
            glow.setPosition(sf::Vector2f(x, y));
            glow.setColor(color);

            sprite.setTexture(*t1);
            sprite.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height/2);
            sprite.setPosition(sf::Vector2f(x, y));
            //sprite.setColor(sf::Color(255-(255-color.r)/50, 255-(255-color.g)/50, 255-(255-color.b)/50));
            sprite.setColor(color);

            game = g;
        }
        void update() {
            sortPoints();
            sprite.setPosition(x, y);
            glow.setPosition(x, y);
        }
        void sortPoints(){
            std::sort(points.begin(), points.end(), [this] (sf::Vector2f a, sf::Vector2f b) {
                return atan2(a.y-this->y, a.x-this->x) < atan2(b.y-this->y, b.x-this->x);
            });
        }
        void getShadowSprite() {
            game->rTextureTmp.clear(sf::Color(0, 0, 0, 255));
            game->rShadows.clear(sf::Color(0, 0, 0, 255));
            game->rTextureTmp.draw(shadow, sf::BlendNone);
            game->rTextureTmp.display();
            game->rShadows.draw(glow, sf::BlendAdd);
            game->rShadows.draw(sf::Sprite(game->rTextureTmp.getTexture()));
            game->rShadows.display();
            drawableShadow = game->rShadows.getTexture();
        }
        std::vector <sf::Vector2f> points;
        sf::VertexArray shadow = sf::VertexArray(sf::TrianglesFan, 0);
        sf::Sprite glow;
        sf::Sprite sprite;
        sf::Texture drawableShadow;
        SHOOTER2D* game;
    };

    class Bullet: public sf::Sprite {
    public:
        Bullet() {

        }
        Bullet(sf::Vector2f p, float speed) {
            setPosition(p);
            v = speed;
        }
        sf::Vector2f velocity;
        void update() {
            move(velocity);
            setRotation(atan2(velocity.y, velocity.x)*180/M_PI);
        }
        void create(sf::Vector2f p, sf::Vector2f d, float speed) {
            setPosition(p);
            v = speed;
            Utils::normalize(d);
            velocity = d*speed;
        }
        void setDirection(sf::Vector2f d) {
            velocity.x = cos (atan2(d.y-getPosition().y, d.x-getPosition().x))*v;
            velocity.y = sin (atan2(d.y-getPosition().y, d.x-getPosition().x))*v;
        }
        float damage = 100;
        float knockback = -2;
        bool penetrating = false;
        int maxPenetrating = 1;
        bool bouncy = false;
        int bounces = 2;
        bool exploding = true;
        bool friendly = true;
        float v = 5;
    };

    class Weapon {
    public:
        Weapon() {};
        Weapon(MovingEntity* p): parent(p) {};
        void setParent(MovingEntity* p) {parent = p;}
        void setBullet() {
            bullet.damage = damage;
            bullet.knockback = knockback;
            bullet.v = velocity;
        }
        MovingEntity* parent;
        std::string name;
        float reloadTime = 1;
        float fireTime = 1;
        unsigned int magSize = 30;
        unsigned int magAmount = 3;
        unsigned int maxMagAmount = 3;
        unsigned int ammo;
        unsigned int mags;
        int damage = 100;
        float knockback = -2;
        float velocity = 5;
        bool automatic = true;
        sf::Texture texture;
        Bullet bullet;
    };

    class Player: public MovingEntity {
    public:
        Player(SHOOTER2D* g) {
            game = g;
            reloadBar.setSize(sf::Vector2f(30, 10));
            reloadBar.setFillColor(sf::Color::Green);
        }
        void update() {
            angle = atan2(velocity.y, velocity.x);
            speed = sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
            sf::Vector2f m = {std::min(speed, maxSpeed)*cos(angle), std::min(speed, maxSpeed)*sin(angle)};
            move(m);
            ls.x = getPosition().x;
            ls.y = getPosition().y;
            ls.update();
            hitbox.setPosition(getPosition());
            if (isReloading) {
                reloadBar.setScale(1-reloadFor.getElapsedTime().asMilliseconds()/weapons[currentWeapon].reloadTime, 1);
                reloadBar.setPosition(getGlobalBounds().left, getPosition().y);
                if (reloadFor.getElapsedTime().asMilliseconds()>weapons[currentWeapon].reloadTime) {
                    isReloading = false;
                    weapons[currentWeapon].ammo = weapons[currentWeapon].magSize;
                    weapons[currentWeapon].mags--;
                }
            }
            velocity*=0.8f;
        }
        void nextWeapon() {
            currentWeapon++;
            if (currentWeapon==weapons.size()) currentWeapon = 0;
        }
        void prevWeapon() {
            currentWeapon--;
            if (currentWeapon==-1) currentWeapon = weapons.size()-1;
        }
        void addWeapon(Weapon w) {
            w.setParent(this);
            bool t = true;
            for (unsigned int i=0; i<weapons.size(); i++) {
                if (w.name == weapons[i].name) {
                    t = false;
                    weapons[i].mags = weapons[i].magAmount;
                }
            }
            if (t) weapons.push_back(w);
        }
        void reload() {
            if (weapons[currentWeapon].ammo < weapons[currentWeapon].magSize && !isReloading && weapons[currentWeapon].mags > 0) {
                isReloading = true;
                reloadFor.restart();
            }
        }
        void shoot() {
            if (lastShot.getElapsedTime().asMilliseconds() > weapons[currentWeapon].fireTime && weapons[currentWeapon].ammo>0 && !isReloading) {
                lastShot.restart();
                weapons[currentWeapon].ammo--;
                game->tmpBullet = weapons[currentWeapon].bullet;
                game->tmpBullet.setPosition(getPosition());
                game->tmpBullet.setDirection(sf::Vector2f(game->rGame.mapPixelToCoords(sf::Mouse::getPosition(*game->window))));
                game->vecBullets.push_back(game->tmpBullet);
                if (weapons[currentWeapon].ammo == 0)
                    reload();
                }
        }
        lightSource ls = lightSource(getPosition(), game);
        float health = 100;
        sf::Clock sinceHit;
        sf::Time invincibilityTime = sf::seconds(1);
        sf::RectangleShape reloadBar;
        sf::Clock reloadFor;
        sf::Clock lastShot;
        float maxSpeed = 10;
        float angle;
        float speed;
        Bullet playerBullet;
        std::vector <Weapon> weapons;
        int currentWeapon = 0;
        unsigned int lights = 30;
        bool isReloading = false;
        bool isDead = false;
        SHOOTER2D* game;
    };

    class EnemyMaker {
    public:
        EnemyMaker() {};
        EnemyMaker(sf::Texture* texture, sf::Texture* textureDead, MovingEntity* tar, SHOOTER2D* g) {
            enemyTexture = texture;
            deadTexture = textureDead;
            target = tar;
            game = g;
        }
        sf::Texture* enemyTexture;
        sf::Texture* deadTexture;
        MovingEntity* target;
        SHOOTER2D* game;
    };

    class Enemy: public MovingEntity {
    public:
        Enemy(sf::Vector2f position, EnemyMaker EM) {
            setPosition(position);
            setTexture(*EM.enemyTexture);
            Utils::setOriginInCenter(*this);
            hitbox.setRadius(getGlobalBounds().width/2);
            hitbox.setOrigin(getGlobalBounds().width/2, getGlobalBounds().height/2);
            target = EM.target;
            healthBar.setFillColor(sf::Color::Red);
            healthBar.setSize(sf::Vector2f(getGlobalBounds().width, 5));
            game = EM.game;
            deadTexture = EM.deadTexture;
        }

        void setDeadTexture(sf::Texture* d) {
            deadTexture = d;
        }

        virtual void update() {
            angle = atan2(destination.y-getPosition().y, destination.x-getPosition().x);
            setRotation(angle*180/M_PI);
            velocity.x += speed* cos(angle);
            velocity.y += speed* sin(angle);
            move(velocity);
            Utils::normalize(velocity);
            velocity *= speed;
            fixElements();
            if (!m_flying)
                findPath();
            else
                destination = target->hitbox.getPosition();
        }

        virtual void onHit (Bullet* bullet) {
            health-=bullet->damage;
            velocity.x*=bullet->knockback;
            velocity.y*=bullet->knockback;
        }

        void setTarget (MovingEntity* ME) {
            target = ME;
        }

        void fixElements() {
            hitbox.setPosition(getPosition());
            healthBar.setPosition(sf::Vector2f(getGlobalBounds().left, getGlobalBounds().top-10));
        }

        virtual void findPath() {
            sf::Vector2f rd = target->hitbox.getPosition()-getPosition();
            sf::Vector2f sp;
            sf::Vector2f sd;
            sf::Vector2f p2;
            sf::Vector2f rp = getPosition();
            int furthestObject = -1;
            float rMag;
            float sMag;
            float T1Min=999999;
            float T1, T2;

            for (unsigned int i=0; i<game->vecWalls.size()-1; i++) {
                for (unsigned int j=0; j<game->vecWalls[i].points.size(); j++) {
                    sp = game->vecWalls[i].points[j];
                    p2 = game->vecWalls[i].points[(j+1)%game->vecWalls[i].points.size()];
                    sd ={p2.x-sp.x, p2.y-sp.y};
                    T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                    T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                    rMag = sqrt(rd.x*rd.x+rd.y*rd.y);
                    sMag = sqrt(sd.x*sd.x+sd.y*sd.y);
                    if (T1<T1Min && T1>0 && T1<1 && T2>0 && T2<1 && rd.x/rMag!=sd.x/sMag && rd.y/rMag!=sd.y/sMag) {
                        T1Min=T1;
                        furthestObject = i;
                    }
                }
            }
            if (furthestObject == -1) {
                destination = target->hitbox.getPosition();
                return;
            }
            else {
                float rayAngle = Utils::getAngle(getPosition(), target->hitbox.getPosition());
                float maxAngle = -20;
                float minAngle = 20;
                int pLeft = -1;
                int pRight = -1;
                for (unsigned int i=0; i<game->vecWalls[furthestObject].points.size(); i++) {
                    float tmpAngle = Utils::getAngle(getPosition(), game->vecWalls[furthestObject].points[i]);
                    tmpAngle-=rayAngle;
                    if (tmpAngle<-M_PI) tmpAngle +=2*M_PI;
                    if (tmpAngle>=M_PI) tmpAngle -=2*M_PI;
                    if (tmpAngle>maxAngle) {
                        pRight = i;
                        maxAngle = tmpAngle;
                    }
                    if (tmpAngle<minAngle) {
                        pLeft = i;
                        minAngle = tmpAngle;
                    }
                }
                if (Utils::getMagnitude(target->getPosition(), game->vecWalls[furthestObject].points[pLeft]) <
                    Utils::getMagnitude(target->getPosition(), game->vecWalls[furthestObject].points[pRight]))
                {
                    destination = Utils::rotatedPoint(game->vecWalls[furthestObject].points[pLeft], getPosition(), 20);
                }
                else destination = Utils::rotatedPoint(game->vecWalls[furthestObject].points[pRight], getPosition(), -20);
            }
        }

        sf::VertexArray getDebugPath() {
            sf::VertexArray a (sf::LinesStrip, 3);
                a[0]=sf::Vertex(getPosition(), sf::Color::Red);
                a[1]=sf::Vertex(destination, sf::Color::Red);
                a[2]=sf::Vertex(target->hitbox.getPosition(), sf::Color::Red);
            return a;
        }

        virtual void onDrop() {
            if (Utils::chance(0.1)) {
                game->vecPowerups.push_back(new PUPGiveHealth(game, &game->texPUPGiveHealth, getPosition()));
            }
            else if (Utils::chance(0.11)) {
                game->vecPowerups.push_back(new PUPGiveAmmo(game, &game->texPUPGiveAmmo, getPosition()));
            }
            else if (Utils::chance(0.5)) {
                int t = Utils::randInt(1, 4);
                game->vecPowerups.push_back(new PUPGiveWeapon(game, &game->vecWeapons[t].texture, getPosition(), t));
            }
        }

        virtual void onKilled() {
            setTexture(*deadTexture);
            game->rMisc.draw(*this);
            game->rMisc.display();
            delete(this);
        }

        float health = 100;
        float maxSpeed = 5;
        float damage = 10;
        float speed = 1;
        float angle;
        bool m_flying = false;
        MovingEntity* target;
        SHOOTER2D* game;
        sf::Vector2f destination;
        sf::RectangleShape healthBar;
        sf::Texture* deadTexture;
    };

    class AIZombie: public Enemy {
    public:
        AIZombie(sf::Vector2f position, EnemyMaker EM) : Enemy(position, EM) {
            speed = 2;
        }
        void update() {
            angle = atan2(destination.y-getPosition().y, destination.x-getPosition().x);
            setRotation(angle*180/M_PI);
            velocity.x += speed* cos(angle);
            velocity.y += speed* sin(angle);
            move(velocity);
            Utils::normalize(velocity);
            velocity *= speed;
            fixElements();
            findPath();
        }

    };

    class AIGhost: public Enemy {
    public:
        AIGhost(sf::Vector2f position, EnemyMaker EM) : Enemy(position, EM) {
            speed = 1;
            m_flying = true;
        }
    };

    class AIRunner: public Enemy {
    public:
        AIRunner(sf::Vector2f position, EnemyMaker EM) : Enemy(position, EM) {
            speed = 4;
            health = 10;
        };
        void update() {
            angle = atan2(destination.y-getPosition().y, destination.x-getPosition().x);
            setRotation(angle*180/M_PI);
            velocity.x += speed* cos(angle);
            velocity.y += speed* sin(angle);
            move(velocity);
            Utils::normalize(velocity);
            velocity *= speed;
            fixElements();
            findPath();
        }

    };

    class Powerup: public sf::Sprite {
    public:
        Powerup(SHOOTER2D* p) {
            parent = p;
        }
        Powerup(SHOOTER2D* p, sf::Texture* t, sf::Vector2f pos) {
            parent = p;
            setTexture(*t);
            setPosition(pos);
        }
        SHOOTER2D* parent;
        std::string name;
        float duration;
        float time;
        virtual void onPickup() {

        }
    };

    class PUPGiveHealth: public Powerup {
    public:
        PUPGiveHealth(SHOOTER2D* p, sf::Texture* t, sf::Vector2f pos) : Powerup(p) {
            setTexture(*t);
            setPosition(pos);
            Utils::setOriginInCenter(*this);
        }
        PUPGiveHealth(SHOOTER2D* p) : Powerup(p) {};
        void onPickup() {
            parent->TADZIK.health = 100;
        }
    };

    class PUPGiveAmmo: public Powerup {
    public:
        PUPGiveAmmo(SHOOTER2D* p) : Powerup(p) {};
        PUPGiveAmmo(SHOOTER2D* p, sf::Texture* t, sf::Vector2f pos) : Powerup(p) {
            setTexture(*t);
            setPosition(pos);
            Utils::setOriginInCenter(*this);
        }
        void onPickup() {
            parent->TADZIK.weapons[parent->TADZIK.currentWeapon].mags = parent->TADZIK.weapons[parent->TADZIK.currentWeapon].magAmount;
        }
    };

    class PUPGiveWeapon: public Powerup {
        public:
        PUPGiveWeapon(SHOOTER2D* p) : Powerup(p) {};
        PUPGiveWeapon(SHOOTER2D* p, sf::Texture* t, sf::Vector2f pos, int id) : Powerup(p) {
            setTexture(*t);
            setPosition(pos);
            Utils::setOriginInCenter(*this);
            ID = id;
        }
        int ID = 1;
        void onPickup() {
            parent->TADZIK.addWeapon(parent->vecWeapons[ID]);
        }
    };

    void updateShadow(lightSource& ls) {
        ls.points.clear();
        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++){
                ls.points.push_back(getIntersection(ls, vecWalls[i].points[j]));
                ls.points.push_back(getIntersection(ls, Utils::rotatedPoint(vecWalls[i].points[j], ls, 0.0001)));
                ls.points.push_back(getIntersection(ls, Utils::rotatedPoint(vecWalls[i].points[j], ls, -0.0001)));
                sf::CircleShape c(2);
                c.setOrigin(1, 1);
                c.setPosition(Utils::rotatedPoint(vecWalls[i].points[j], ls, 1));
                rDebug.draw(c);
                c.setPosition(Utils::rotatedPoint(vecWalls[i].points[j], ls, -1));
                rDebug.draw(c);
            }
        }
        ls.sortPoints();
        ls.shadow.clear();
        sf::Color c = sf::Color(0, 0, 0, 0);
        ls.shadow.append(sf::Vertex(ls, c));
        for (unsigned int i=0; i<ls.points.size(); i++) {
            ls.shadow.append(sf::Vertex(ls.points[i], c));
        }
        ls.shadow.append(sf::Vertex(ls.points[0], c));
    }

    sf::VertexArray getLine(sf::Vector2f p1, sf::Vector2f p2, sf::Color c = sf::Color::White) {
        sf::VertexArray a (sf::Lines, 2);
        a[0]=p1;
        a[1]=p2;
        return a;
    }

    sf::Vector2f getIntersection (sf::Vector2f rp, sf::Vector2f p2) {
        sf::Vector2f rd = {p2.x-rp.x, p2.y-rp.y};
        sf::Vector2f sp;
        sf::Vector2f sd;
        float rMag;
        float sMag;
        float T1Min=1000;
        float T1, T2;

        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                sp = vecWalls[i].points[j];
                p2 = vecWalls[i].points[(j+1)%vecWalls[i].points.size()];
                sd ={p2.x-sp.x, p2.y-sp.y};
                T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                rMag = rd.x*rd.x+rd.y*rd.y;
                sMag = sd.x*sd.x+sd.y*sd.y;
                if (T1<T1Min && T1>0 && T2>0 && T2<1 && rd.x/rMag!=sd.x/sMag && rd.y/rMag!=sd.y/sMag)
                    T1Min=T1;
            }
        }
        return sf::Vector2f(rp.x+rd.x*T1Min, rp.y+rd.y*T1Min);
    }

    void onSceneLoadToMemory() {
        mapa.loadFromFile("files/maps/shooter2D/map1.png");

        mapSize = {mapa.getSize().x*tileSize, mapa.getSize().y*tileSize};

        gameView.reset(sf::FloatRect(0, 0, mapSize.x, mapSize.y));

        deathMessage.setFont(Common::Font::Days_Later);
        deathMessage.setPosition(640, 360);

        texBullet1.loadFromFile("files/textures/shooter2D/bullet1.png");
        tmpBullet.setTexture(texBullet1);
        TADZIK.playerBullet.setTexture(texBullet1);
        tmpBullet.setScale(0.2, 0.2);
        TADZIK.playerBullet.setScale(0.2, 0.2);

        texBlood.loadFromFile("files/textures/shooter2D/blood.png");
        spBlood.setTexture(texBlood);
        Utils::setOriginInCenter(spBlood);

        texWall.loadFromFile("files/textures/shooter2D/wall1.png");
        spWall.setTexture(texWall);
        spWall.setOrigin(tileSize/2, tileSize/2);
        texCandle.loadFromFile("files/textures/shooter2D/candle.png");

        texPlayer.loadFromFile("files/textures/shooter2D/player.png");
        TADZIK.setTexture(texPlayer);
        Utils::setOriginInCenter(TADZIK);

        texHealthBar.loadFromFile("files/textures/shooter2D/healthBar.png");
        texHealthFrame.loadFromFile("files/textures/shooter2D/healthFrame.png");

        texShadow.loadFromFile("files/textures/shooter2D/shadow.png");

        TADZIK.ls.glow.setTexture(texShadow);
        Utils::setOriginInCenter(TADZIK.ls.glow);

        loadMap();
        loadWaves();

        rDebug.create(mapSize.x, mapSize.y);
        rShadows.create(mapSize.x, mapSize.y);
        rMisc.create(mapSize.x, mapSize.y);
        rGame.create(mapSize.x, mapSize.y);
        rTextureTmp.create(mapSize.x, mapSize.y);

        Object tmpObject;
        tmpObject.points.push_back(sf::Vector2f(tileSize, tileSize));
        tmpObject.points.push_back(sf::Vector2f(mapSize.x-tileSize, tileSize));
        tmpObject.points.push_back(sf::Vector2f(mapSize.x-tileSize, mapSize.y-tileSize));
        tmpObject.points.push_back(sf::Vector2f(tileSize, mapSize.y-tileSize));

        vecWalls.push_back(tmpObject);

        rTextureTmp.clear(sf::Color(0, 0, 0, 0));
        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                rTextureTmp.draw(getLine(vecWalls[i].points[j], vecWalls[i].points[(j+1)%vecWalls[i].points.size()]));
            }
        }
        rTextureTmp.display();
        texLines = rTextureTmp.getTexture();

        hud.healthBar.setTexture(texHealthBar);
        hud.healthFrame.setTexture(texHealthFrame);
        hud.setFont(&Common::Font::Days_Later, sf::Color::Black);

        texCrosshair.loadFromFile("files/textures/shooter2D/crosshair.png");
        spCrosshair.setTexture(texCrosshair);
        Utils::setOriginInCenter(spCrosshair);

        texZombie.loadFromFile("files/textures/shooter2D/nmeZombie.png");
        texGhost.loadFromFile("files/textures/shooter2D/nmeGhost.png");
        texRunner.loadFromFile("files/textures/shooter2D/nmeRunner.png");

        loadWeapons();
        TADZIK.addWeapon(vecWeapons[0]);
        //TADZIK.addWeapon(vecWeapons[1]);
        //TADZIK.addWeapon(vecWeapons[2]);
        TADZIK.addWeapon(vecWeapons[3]);
        TADZIK.currentWeapon = 0;


        texPUPGiveHealth.loadFromFile("files/textures/shooter2D/texPUPGiveHealth.png");
        texPUPGiveAmmo.loadFromFile("files/textures/shooter2D/texPUPGiveAmmo.png");

        spsExplosion.loadFromFile("files/textures/shooter2D/spsExplosion.png");
        animExplosion.setSpriteSheet(&spsExplosion, 134, sf::milliseconds(10));
        aExplosion.setAnimation(&animExplosion);
        aExplosion.setLooped(false);
        aExplosion.centerOrigin();

        ZombieMaker = EnemyMaker(&texZombie, &texBlood, &TADZIK, this);
        GhostMaker = EnemyMaker(&texGhost, &texBlood, &TADZIK, this);
        RunnerMaker = EnemyMaker(&texRunner, &texBlood, &TADZIK, this);

        consoleCommands.push_back("debug");
        consoleCommands.push_back("killall");
        consoleCommands.push_back("stopspawning");
    }

    void onSceneActivate() {
        window->setMouseCursorVisible(false);
        ImGui::SetMouseCursor(ImGuiMouseCursor_None);
        waveClock.restart();
    }

    void onSceneDeactivate() {
        for (unsigned int i=vecEnemies.size()-1; i>=0; i--) {
            delete vecEnemies[i];
            vecEnemies.erase(vecEnemies.begin()+i);
        }
        for (unsigned int i=vecPowerups.size()-1; i>=0; i--) {
            delete vecPowerups[i];
            vecPowerups.erase(vecPowerups.begin()+i);
        }
    }

    void loadMap() {
        bool t[mapa.getSize().x][mapa.getSize().y];
        sf::Color objectColor = sf::Color(255, 255, 255);
        for (unsigned int i=0; i<mapa.getSize().x; i++)
            for (unsigned int j=0; j<mapa.getSize().y; j++) {
                t[i][j]=0;
                if (mapa.getPixel(i, j) == objectColor || mapa.getPixel(i, j) == sf::Color(0, 0, 0)) {
                    spWall.setPosition((i+0.5)*tileSize, (j+0.5)*tileSize);
                    vecSprites.push_back(spWall);
                }
            }
        for (unsigned int i=1; i<mapa.getSize().x-1; i++) {
            for (unsigned int j=1; j<mapa.getSize().y-1; j++) {
                if (mapa.getPixel(i, j)==objectColor && t[i][j]==0) {
                    if (t[i-1][j]==1 || t[i][j-1]==1 || t[i+1][j]==1 || t[i][j+1]==1) {
                        t[i][j]=1;
                    }
                    else {
                        Object tmpWall;
                        sf::Vector2i pocz = {i, j};
                        sf::Vector2i on = pocz;
                        int d = 0;
                        int dPrev = 3;
                        bool did = false;
                        do {
                            if (did) {
                                dPrev = d;
                                d=(d+3)%4;
                                did = false;
                            }
                            if (d==0) {
                                if (mapa.getPixel(on.x, on.y) == objectColor || mapa.getPixel(on.x, on.y-1) == objectColor) {
                                    if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                    on.x++;
                                    did = true;
                                    t[on.x][on.y] = 1;
                                }
                            }
                            else if (d==1) {
                                if (mapa.getPixel(on.x, on.y) == objectColor || mapa.getPixel(on.x-1, on.y) == objectColor) {
                                    if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                    on.y++;
                                    did = true;
                                    t[on.x][on.y] = 1;
                                }
                            }
                            else if (d==2) {
                                if (mapa.getPixel(on.x-1, on.y) == objectColor || mapa.getPixel(on.x-1, on.y-1) == objectColor) {
                                    if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                    on.x--;
                                    did = true;
                                    t[on.x][on.y] = 1;
                                }
                            }
                            else if (d==3) {
                                if (mapa.getPixel(on.x, on.y-1) == objectColor || mapa.getPixel(on.x-1, on.y-1) == objectColor) {
                                    if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                    on.y--;
                                    did = true;
                                    t[on.x][on.y] = 1;
                                }
                            }
                            if (!did) d=(d+1)%4;
                        }
                        while (on!=pocz);
                        vecWalls.push_back(tmpWall);
                    }
                }
                else if (mapa.getPixel(i, j) == sf::Color(0, 0, 255)) {
                    TADZIK.setPosition(i*tileSize, j*tileSize);
                }
            }
        }
    }

    void loadWeapons() {
        std::ifstream file;
        std::string texDir;
        Weapon tmpWeapon;
        tmpWeapon.bullet.setTexture(texBullet1);
        tmpWeapon.bullet.setScale(0.2, 0.2);
        tmpWeapon.bullet.setOrigin(tmpWeapon.bullet.getTextureRect().width/2, tmpWeapon.bullet.getTextureRect().height/2);
        file.open("files/resource/shooter2D/weapons.dat");
        while (!file.eof()) {
            file >> tmpWeapon.name;
            file >> texDir;
            tmpWeapon.texture.loadFromFile(texDir);
            file >> tmpWeapon.damage;
            file >> tmpWeapon.magSize; tmpWeapon.ammo = tmpWeapon.magSize;
            file >> tmpWeapon.magAmount; tmpWeapon.mags = tmpWeapon.magAmount;
            file >> tmpWeapon.automatic;
            file >> tmpWeapon.reloadTime;
            file >> tmpWeapon.fireTime;
            file >> tmpWeapon.velocity;
            file >> tmpWeapon.knockback;
            tmpWeapon.setBullet();
            vecWeapons.push_back(tmpWeapon);
        }
    }

    void loadWaves() {
        std::ifstream file;
        file.open("files/resource/shooter2D/waves.dat");
        Wave tmpWave(0, 0);
        while (!file.eof()) {
            file >> tmpWave.time;
            file >> tmpWave.maxEnemies;
            vecWaves.push_back(tmpWave);
        }
    }

    void handleCollision(MovingEntity& s1, std::vector <sf::Sprite>& walls) {
        for (unsigned int i=0; i<walls.size(); ++i) {
            sf::FloatRect intersection;
            if (Collision::PixelPerfectTest(walls[i], s1) && s1.getGlobalBounds().intersects(walls[i].getGlobalBounds(), intersection)) {
                sf::Vector2f direction = walls[i].getPosition() - s1.getPosition();
                sf::Vector2f offset;
                // X collision
                if (abs(direction.x) > abs(direction.y))
                    offset.x = ((direction.x<0)?-1:1)*intersection.width;
                // Y collision
                if (abs(direction.x) < abs(direction.y))
                    offset.y = ((direction.y<0)?-1:1)*intersection.height;
                s1.velocity.x -= offset.x * abs(offset.x)/50.0f;
                s1.velocity.y -= offset.y * abs(offset.y)/50.0f;
            }
        }
    }

    void handleEntityCollision (MovingEntity& e1, MovingEntity &e2, double knockback) {
        sf::FloatRect intersection;
        if (e1.hitbox.getGlobalBounds().intersects(e2.hitbox.getGlobalBounds(), intersection)) {
            sf::Vector2f direction = e2.getPosition() - e1.getPosition();
            sf::Vector2f offset;
            offset.x = ((direction.x<0)?-1:1)*intersection.width;
            offset.y = ((direction.y<0)?-1:1)*intersection.height;
            offset.x*=knockback;
            offset.y*=knockback;
            e1.velocity -= offset;
            e2.velocity += offset;
        }
    }

    void gameOver() {
        TADZIK.isDead = true;
        deathMessage.setString("YOU SUCK");
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left && !TADZIK.weapons[TADZIK.currentWeapon].automatic) {
                TADZIK.shoot();
            }
            if (event.mouseButton.button == sf::Mouse::Right && TADZIK.lights>0) {
                vecLights.push_back(lightSource(TADZIK.getPosition(), &texCandle, &texShadow, this, Utils::randColor()));
                updateShadow(vecLights[vecLights.size()-1]);
                vecLights[vecLights.size()-1].getShadowSprite();
                TADZIK.lights--;
            }
        }
        if (event.type == sf::Event::MouseWheelMoved) {
            TADZIK.currentWeapon-=event.mouseWheel.delta;
            while (TADZIK.currentWeapon<0) TADZIK.currentWeapon += TADZIK.weapons.size();
            TADZIK.currentWeapon = TADZIK.currentWeapon%TADZIK.weapons.size();
            TADZIK.isReloading = false;
            if (TADZIK.weapons[TADZIK.currentWeapon].ammo == 0) TADZIK.reload();
        }
    }

    void updateEnemies() {
        for (int i=vecEnemies.size()-1; i>=0; i--) {
            if (debug)
                rDebug.draw(vecEnemies[i]->getDebugPath());
            if (!vecEnemies[i]->m_flying)
                handleCollision(*vecEnemies[i], vecSprites);
            for (int j=vecBullets.size()-1; j>=0; j--) {
                if (Collision::BoundingBoxTest(*vecEnemies[i], vecBullets[j]) && vecBullets[j].friendly) {
                    vecEnemies[i]->onHit(&vecBullets[j]);
                    if (vecBullets[j].penetrating && vecBullets[j].maxPenetrating > 0) {
                        vecBullets[j].maxPenetrating--;
                    }
                    else
                        if (vecBullets[j].exploding) {
                            aExplosion.setPosition(vecBullets[j].getPosition());
                            vecExplosions.push_back(aExplosion);
                        }
                        vecBullets.erase(vecBullets.begin()+j);
                    vecEnemies[i]->healthBar.setScale(vecEnemies[i]->health/100.0, 1);
                    if (vecEnemies[i]->health<=0) {
                        vecEnemies[i]->onDrop();
                        vecEnemies[i]->onKilled();
                        vecEnemies.erase(vecEnemies.begin()+i);
                        score++;
                        break;
                    }
                }
            }
        }
        for (unsigned int i=0; i+1<vecEnemies.size(); i++) {
            for (unsigned int j=i+1; j<vecEnemies.size(); j++) {
                handleEntityCollision(*vecEnemies[i], *vecEnemies[j], 0.02);
            }
        }
        if (waveClock.getElapsedTime().asSeconds()>vecWaves[currentWave].time) {
            sf::Image img;
            img = rTextureTmp.getTexture().copyToImage();
            for (int i=0; i<vecWaves[currentWave].maxEnemies; i++) {
                sf::Vector2i t = Utils::randVector2i(sf::IntRect(10, 10, mapSize.x-10, mapSize.y-10));
                if (img.getPixel(t.x, t.y) == sf::Color::Black)
                    if (mapa.getPixel(t.x/tileSize, t.y/tileSize)!=sf::Color(0, 0, 0) && mapa.getPixel(t.x/tileSize, t.y/tileSize)!=sf::Color(255, 255, 255)) {
                        if (Utils::chance(0.75))
                            vecEnemies.push_back(new AIZombie(sf::Vector2f(t), ZombieMaker));
                        else if (Utils::chance(0.33))
                            vecEnemies.push_back(new AIGhost(sf::Vector2f(t), GhostMaker));
                        else
                            vecEnemies.push_back(new AIRunner(sf::Vector2f(t), RunnerMaker));
                    }
            }
            waveClock.restart();
            currentWave++;
            currentWave=currentWave%vecWaves.size();
        }
    }

    bool onConsoleUpdate(std::vector<std::string> args){
        if (args[0] == "debug") {
            debug = !debug;
            rDebug.setActive(debug);
        }
        else if (args[0] == "gib") {
            for (unsigned int i=0; i<TADZIK.weapons.size(); i++) {
                TADZIK.weapons[i].ammo = TADZIK.weapons[i].magSize;
                TADZIK.weapons[i].mags = TADZIK.weapons[i].magAmount;
                TADZIK.health = 100;
                TADZIK.lights = 30;
            }
        }
        else if (args.size()==2 && args[0] == "spawn") {
            for (int i=0; i<atoi(args[1].c_str()); i++)
                vecEnemies.push_back(new AIZombie(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)), ZombieMaker));
        }
        else if (args[0] == "killall") {
            for (unsigned int i = 0; i<vecEnemies.size(); i++)
                delete vecEnemies[i];
            vecEnemies.clear();
        }
        else if (args[0] == "stopspawning") {
            for (unsigned int i = 0; i<vecEnemies.size(); i++)
                delete vecEnemies[i];
            vecEnemies.clear();
            currentWave = 0;
            vecWaves[0].time = 1000000;
        }
        else if (args[0] == "lightsoff") {
            vecLights.clear();
        }
        else if (args[0] == "clear") {
            rMisc.clear(sf::Color(0, 0, 0, 0));
        }
        else
            return false;
        return true;
    }

    virtual void getKeyboardStuff() {
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -TADZIK.velocity.x < TADZIK.maxSpeed)
            TADZIK.velocity.x -= acceleration;
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && -TADZIK.velocity.y < TADZIK.maxSpeed)
            TADZIK.velocity.y -= acceleration;
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && TADZIK.velocity.y < TADZIK.maxSpeed)
            TADZIK.velocity.y += acceleration;
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && TADZIK.velocity.x < TADZIK.maxSpeed)
            TADZIK.velocity.x += acceleration;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) TADZIK.health-=10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::R)) TADZIK.reload();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            vecEnemies.push_back(new AIGhost(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)), GhostMaker));
    }

    virtual void draw(double deltaTime) {
        window->clear();
        ///VIEW HANDLING
        sf::Vector2i pos = rGame.mapCoordsToPixel(TADZIK.getPosition(), gameView);
        int scrollArea = 200;
        if (pos.y > windowSize.y-scrollArea && viewOffset.y+windowSize.y<mapSize.y) {
            gameView.move(0, pos.y-windowSize.y+scrollArea);
            viewOffset.y+=pos.y-windowSize.y+scrollArea;
            if (viewOffset.y+windowSize.y>mapSize.y) {
                gameView.move(0, -viewOffset.y-windowSize.y+mapSize.y);
                viewOffset.y=mapSize.y-windowSize.y;
            }
        }
        else if (pos.y < scrollArea && viewOffset.y>0) {
            gameView.move(0, pos.y-scrollArea);
            viewOffset.y+=pos.y-scrollArea;
            if (viewOffset.y<0) {
                gameView.move(0, -viewOffset.y);
                viewOffset.y=0;
            }
        }
        rGame.setView(gameView);

        /// INPUT MYSZKI
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left) && TADZIK.weapons[TADZIK.currentWeapon].automatic) {
            TADZIK.shoot();
        }

        /// INPUT Z KLAWIATURY
        getKeyboardStuff();

        /// OGARNIANIE KOLIZJI
        for (unsigned int i=0; i<vecEnemies.size(); i++) {
            if (Collision::PixelPerfectTest(TADZIK, *vecEnemies[i])) {
                handleEntityCollision(TADZIK, *vecEnemies[i], 0.5);
                if (TADZIK.sinceHit.getElapsedTime() > TADZIK.invincibilityTime) {
                    TADZIK.health-=vecEnemies[i]->damage;
                    TADZIK.sinceHit.restart();
                }
            }
        }
        handleCollision(TADZIK, vecSprites);

        if (TADZIK.health<0) gameOver();

        TADZIK.update();
        updateShadow(TADZIK.ls);
        if (debug) TADZIK.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

        sf::Vector2f tmpDirect = sf::Vector2f(rGame.mapPixelToCoords(sf::Mouse::getPosition(*window))) - TADZIK.getPosition();
        TADZIK.setRotation(atan2(tmpDirect.y, tmpDirect.x)*180/M_PI);

        rGame.clear(sf::Color(255,
                              255-100*(waveClock.getElapsedTime().asSeconds()/vecWaves[currentWave].time),
                              255-100*(waveClock.getElapsedTime().asSeconds()/vecWaves[currentWave].time)));

        rGame.draw(sf::Sprite(rMisc.getTexture())); //krew i te sprawy

        /// OGARNIANIE PRZECIWNIKÓW
        for (unsigned int i=0; i<vecEnemies.size(); i++) {
            vecEnemies[i]->update();
            if (debug)
                rDebug.draw(vecEnemies[i]->hitbox);
            rGame.draw(*vecEnemies[i]);
            rGame.draw(vecEnemies[i]->healthBar);
        }
        updateEnemies();

        /// OGARNIANIE POCISKÓW
        if (vecBullets.size()>0) {
            for (int i=vecBullets.size()-1; i>=0; i--) {
                vecBullets[i].update();
                rGame.draw(vecBullets[i]);
                sf::FloatRect tmp;
                for (unsigned int j=0; j<vecSprites.size(); j++) {
                    if (vecSprites[j].getGlobalBounds().intersects(vecBullets[i].getGlobalBounds(), tmp)) {
                        if (vecBullets[i].bouncy && vecBullets[i].bounces > 0) {
                            vecBullets[i].bounces--;
                            if (tmp.height<tmp.width) {
                                vecBullets[i].velocity.y = -vecBullets[i].velocity.y;
                                vecBullets[i].move(0, -tmp.height*Utils::sgn(vecBullets[i].velocity.y));
                            }
                            else {
                                vecBullets[i].velocity.x = -vecBullets[i].velocity.x;
                                vecBullets[i].move(tmp.width*Utils::sgn(vecBullets[i].velocity.x), 0);
                            }
                        }
                        else {
                            if (vecBullets[i].exploding) {
                                aExplosion.setPosition(vecBullets[i].getPosition());
                                vecExplosions.push_back(aExplosion);
                            }
                            vecBullets.erase(vecBullets.begin()+i);
                            break;
                        }
                    }
                }
            }
        }

        /// OGARNIANIE POWERUPOW
        for (int i=vecPowerups.size()-1; i>=0; --i) {
            rGame.draw(*vecPowerups[i]);
            if (Collision::BoundingBoxTest(TADZIK, *vecPowerups[i])) {
                vecPowerups[i]->onPickup();
                delete(vecPowerups[i]);
                vecPowerups.erase(vecPowerups.begin()+i);
            }
        }

        /// W SUMIE NIE TAKI TEMPORARY LIGHTNING
        rTextureTmp.clear(sf::Color(0, 0, 0, 255));
        rShadows.clear(sf::Color(0, 0, 0));
        rTextureTmp.draw(TADZIK.ls.shadow, sf::BlendNone);
        rTextureTmp.display();
        rShadows.draw(TADZIK.ls.glow, sf::BlendAdd);
        rShadows.draw(sf::Sprite(rTextureTmp.getTexture()));
        rShadows.display();
        rTextureTmp.clear(sf::Color(0, 0, 0, 0));
        for (unsigned int i = 0; i<vecLights.size(); i++)
            rTextureTmp.draw(vecLights[i].sprite, sf::BlendAdd);
        rTextureTmp.draw(sf::Sprite(rShadows.getTexture()), sf::BlendAdd);
        for (unsigned int i=0; i<vecLights.size(); i++)
            rTextureTmp.draw(sf::Sprite(vecLights[i].drawableShadow), sf::BlendAdd);
        rTextureTmp.display();

        rGame.draw(sf::Sprite(rTextureTmp.getTexture()), sf::BlendMultiply);
        rGame.draw(sf::Sprite(texLines));

        ///EKSPLOZJE
        for (int i=vecExplosions.size()-1; i>=0; --i) {
            for (int j=vecEnemies.size()-1; j>=0; --j) {
                if (Collision::PixelPerfectTest(*vecEnemies[j], vecExplosions[i])) {
                    vecEnemies[j]->onDrop();
                    vecEnemies[j]->onKilled();
                    vecEnemies.erase(vecEnemies.begin()+j);
                }
            }
            vecExplosions[i].update(deltaTime);
            if (!vecExplosions[i].shouldDestroy()) {
                rGame.draw(vecExplosions[i]);
            }
            else {
                vecExplosions.erase(vecExplosions.begin()+i);
            }
        }

        /// DEBUG
        if (debug) {
            for (unsigned int i=0; i<TADZIK.ls.points.size(); i++) {
                sf::CircleShape c(2);
                c.setOrigin(1, 1);
                c.setFillColor(sf::Color::Green);
                c.setPosition(TADZIK.ls.points[i]);
                rDebug.draw(c);
            }
            rDebug.display();
            rGame.draw(sf::Sprite(rDebug.getTexture()));
            rDebug.clear(sf::Color(0, 0, 0, 0));
        }

        ///RYSOWANIE RESZTY
        rGame.draw(TADZIK);
        if (TADZIK.isReloading)
            rGame.draw(TADZIK.reloadBar);

        rGame.display();

        window->draw(sf::Sprite(rGame.getTexture()));
        hud.update();
        spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));
        window->draw(spCrosshair);
        if (TADZIK.isDead) {
            window->draw(deathMessage);
        }
    }

protected:
    sf::Texture texBackground;
    sf::Texture texWall;
    sf::Texture texPlayer;
    sf::Texture texHealthBar;
    sf::Texture texHealthFrame;
    sf::Texture texBullet1;
    sf::Texture texCandle;
    sf::Texture texZombie;
    sf::Texture texGhost;
    sf::Texture texRunner;
    sf::Texture texCrosshair;
    sf::Texture texShadow;
    sf::Texture texPUPGiveHealth;
    sf::Texture texPUPGiveAmmo;
    sf::Texture texBlood;
    sf::Texture spsExplosion;

    sf::Texture texLines;

    sf::Sprite spCrosshair;
    sf::Sprite spBlood;
    sf::Sprite spWall;

    ARO::Anim animExplosion;
    ARO::AnimSprite aExplosion;

    sf::RenderTexture rDebug;
    sf::RenderTexture rShadows;
    sf::RenderTexture rMisc;
    sf::RenderTexture rGame;
    sf::RenderTexture rTextureTmp;
    sf::View gameView;
    sf::Image mapa;
    sf::Text deathMessage;

    sf::Vector2f viewOffset = {0, 0};
    sf::Vector2f mapSize;
    sf::Vector2f windowSize = sf::Vector2f(window->getSize());

    HUD hud = HUD(this);

    Player TADZIK = Player(this);

    Bullet tmpBullet;

    EnemyMaker ZombieMaker;
    EnemyMaker GhostMaker;
    EnemyMaker RunnerMaker;

    std::vector <ARO::AnimSprite> vecExplosions;

    std::vector <lightSource> vecLights;

    std::vector <Object> vecWalls;

    std::vector <Enemy*> vecEnemies;

    std::vector <Bullet> vecBullets;

    std::vector <sf::Sprite> vecSprites;

    std::vector <Weapon> vecWeapons;

    std::vector <Wave> vecWaves;

    std::vector <Powerup*> vecPowerups;

    float acceleration = 2;
    int tileSize = 20;
    int score = 0;

    sf::Clock waveClock;

    int currentWave = 0;

    bool debug = false;

};

#endif //SHOOTER2D
