#ifndef SHOOTER2D_HPP
#define SHOOTER2D_HPP

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Collision.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class SHOOTER2D: public Scene{
public:
    SHOOTER2D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

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
        //std::vector <sf::Sprite> sprites;
    };

    class CrossingPoint{
    public:
        CrossingPoint(sf::Vector2f v, double a)
        :point(v), angle(a)
        {}

        bool operator<(const CrossingPoint& p) const {
            return angle > p.angle;
        }

        sf::Vector2f point;
        double angle;
    };

    class lightSource: public sf::Vector2f {
    public:

        lightSource(){
            color = sf::Color(Utils::randInt(0, 255), Utils::randInt(0, 255), Utils::randInt(0, 255), Utils::randInt(0, 255));
        }
        lightSource(sf::Vector2f f) {
            x=f.x, y=f.y;
            color = sf::Color(Utils::randInt(0, 255), Utils::randInt(0, 255), Utils::randInt(0, 255), Utils::randInt(0, 255));
        }
        lightSource(sf::Vector2f p, sf::Vector2f v, sf::Color c) {
            x=p.x, y=p.y;
            color = c;
            velocity = v;
        }
        lightSource(sf::Vector2f p, sf::Vector2f v, sf::Color c, sf::Texture* t) {
            x=p.x, y=p.y;
            color = c;
            velocity = v;
            sprite.setTexture(*t);
            sprite.setOrigin(sprite.getTextureRect().width/2, sprite.getTextureRect().height/2);
            sprite.setPosition(sf::Vector2f(x, y));
        }
        void operator= (sf::Vector2f v) {
            x = v.x;
            y = v.y;
            sprite.setPosition(x, y);
        }
        void update() {
            x+=velocity.x;
            y+=velocity.y;
            velocity.x*=0.9;
            velocity.y*=0.9;
            sortPoints();
        }
        void sortPoints(){
            std::sort(points.begin(), points.end());
        }
        std::vector <CrossingPoint> points;
        sf::Vector2f velocity;
        sf::Vector2f acceleration;
        sf::VertexArray shadow = sf::VertexArray(sf::TrianglesFan, 0);
        sf::Sprite sprite;
        sf::Color color;
    };

    class Weapon {
    public:
        Weapon(MovingEntity* p): parent(p) {};
        void shoot() {

        }
        MovingEntity* parent;
        double reloadTime = 1;
        unsigned int magSize = 30;
        unsigned int maxAmmo = 90;
        sf::Sprite sprite;
    };

    class Bullet: public sf::Sprite {
    public:
        Bullet() {

        }
        Bullet(sf::Vector2f p, double v) {
            setPosition(p);
            speed = v;
        }
        sf::Vector2f velocity;
        void update() {

            move(velocity);
            setRotation(atan2(velocity.y, velocity.x)*180/M_PI);
        }
        void create(sf::Vector2f p, sf::Vector2f d, double v) {
            setPosition(p);
            velocity.x = cos (atan2(d.y-p.y, d.x-p.x))*v;
            velocity.y = sin (atan2(d.y-p.y, d.x-p.x))*v;
        }
        void setDirection(sf::Vector2f d) {
            velocity.x = cos (atan2(d.y-getPosition().y, d.x-getPosition().x))*speed;
            velocity.y = sin (atan2(d.y-getPosition().y, d.x-getPosition().x))*speed;
        }
        double damage = 10;
        double knockback = -2;
        bool penetrating = false;
        bool bouncy = false;
        bool friendly = true;
        double speed = 5;
    };

    class Player: public MovingEntity {
    public:
        Player() {

        }
        void update() {
            angle = atan2(velocity.y, velocity.x);
            speed = sqrt(velocity.x*velocity.x+velocity.y*velocity.y);
            sf::Vector2f m = {std::min(speed, maxSpeed)*cos(angle), std::min(speed, maxSpeed)*sin(angle)};
            move(m);
            ls = getPosition();
            hitbox.setPosition(getPosition());
        }
        lightSource ls = lightSource(getPosition(), {0, 0}, sf::Color(255, 255, 255, 100));
        double health = 100;
        sf::Clock sinceHit;
        sf::Time invincibilityTime = sf::seconds(1);
        double maxSpeed = 10;
        double angle;
        double speed;
        Bullet playerBullet;
        std::vector <Weapon> weapons;
    };

    class Enemy: public MovingEntity {
    public:
        Enemy(sf::Vector2f p, sf::Texture* t, MovingEntity* ME) {
            setPosition(p);
            setTexture(*t);
            setOrigin(getTextureRect().width/2, getTextureRect().height/2);
            hitbox.setRadius(getGlobalBounds().width/2);
            hitbox.setOrigin(getGlobalBounds().width/2, getGlobalBounds().height/2);
            target = ME;
            healthBar.setFillColor(sf::Color::Red);
            healthBar.setSize(sf::Vector2f(getGlobalBounds().width, 5));
        }
        void update() {
            angle = atan2(destination.y-getPosition().y, destination.x-getPosition().x);
            velocity.x += speed* cos(angle);
            velocity.y += speed* sin(angle);
            move(velocity);
            hitbox.setPosition(getPosition());
            velocity.x*=0.8;
            velocity.y*=0.8;
            healthBar.setPosition(sf::Vector2f(getGlobalBounds().left, getGlobalBounds().top-10));
        }
        void onHit (Bullet bullet) {
            health-=bullet.damage;
            velocity.x*=bullet.knockback;
            velocity.y*=bullet.knockback;
        }
        void trackEntity (MovingEntity* ME) {
            target = ME;
        }
        void getDestination() {

        }
        double health = 100;
        double maxSpeed = 5;
        double damage = 10;
        double speed = 1;
        double angle;
        MovingEntity* target;
        sf::Vector2f destination;
        sf::RectangleShape healthBar;
    };

    void getEnemyDestination(Enemy &e) {
        sf::Vector2f rd = e.target->hitbox.getPosition()-e.getPosition();
        sf::Vector2f sp;
        sf::Vector2f sd;
        sf::Vector2f p2;
        sf::Vector2f rp = e.getPosition();
        int furthestObject = -1;
        double rMag;
        double sMag;
        double T1Min=999999;
        double T1, T2;

        for (unsigned int i=0; i<vecWalls.size()-1; i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                sp = vecWalls[i].points[j];
                p2 = vecWalls[i].points[(j+1)%vecWalls[i].points.size()];
                sd ={p2.x-sp.x, p2.y-sp.y};
                T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                rMag = sqrt(rd.x*rd.x+rd.y*rd.y);
                sMag = sqrt(sd.x*sd.x+sd.y*sd.y);
                if (T1<T1Min && T1>0 && T1<1 && T2>0 && T2<1 && rd.x/rMag!=sd.x/sMag && rd.y/rMag!=sd.y/sMag) {
                    T1Min=T1;
                    furthestObject = i;
                    //std::cout << i << "     " << T1 << std::endl;
                }
            }
        }
        //std::cout << furthestObject << std::endl;
        if (furthestObject == -1) {
            e.destination = e.target->hitbox.getPosition();
            return;
        }
        else {
            double rayAngle = 0.5*(atan2(vecWalls[furthestObject].points[0].y-e.getPosition().y, vecWalls[furthestObject].points[0].x-e.getPosition().x) +
                                   atan2(vecWalls[furthestObject].points[1].y-e.getPosition().y, vecWalls[furthestObject].points[1].x-e.getPosition().x));
            double maxAngle = 0;
            double minAngle = 20;
            double tmpMaxAngle;
            double tmpMinAngle;
            int pLeft = -1;
            int pRight = -1;
            for (int i=0; i<vecWalls[furthestObject].points.size(); i++) {
                double tmpAngle = atan2(vecWalls[furthestObject].points[i].y-e.getPosition().y, vecWalls[furthestObject].points[i].x-e.getPosition().x);
                tmpMaxAngle = tmpAngle - rayAngle; tmpMinAngle = tmpAngle-rayAngle;
                if (tmpMaxAngle<-M_PI) tmpMaxAngle +=M_PI;
                if (tmpMaxAngle>=M_PI) tmpMaxAngle -=M_PI;
                if (tmpMinAngle<-M_PI) tmpMaxAngle +=M_PI;
                if (tmpMinAngle>=M_PI) tmpMaxAngle -=M_PI;
                if (tmpMaxAngle>maxAngle) {
                    pRight = i;
                    maxAngle = tmpMaxAngle;
                }
                if (tmpMinAngle<minAngle) {
                    pLeft = i;
                    minAngle = tmpMinAngle;
                }
            }
            std::cout << pLeft << " " << pRight << std::endl;
            //if (maxAngle-atan2(e.target->hitbox.getPosition().y-e.getPosition().y, e.target->hitbox.getPosition().x-e.getPosition().x)-M_PI <
            //    atan2(e.target->hitbox.getPosition().y-e.getPosition().y, e.target->hitbox.getPosition().x-e.getPosition().x)+M_PI-minAngle) {
            //      e.destination = vecWalls[furthestObject].points[pLeft];
            //}
            //else e.destination = vecWalls[furthestObject].points[pRight];
            if ((e.target->getPosition().x-vecWalls[furthestObject].points[pLeft].x)*(e.target->getPosition().x-vecWalls[furthestObject].points[pLeft].x)+
                (e.target->getPosition().y-vecWalls[furthestObject].points[pLeft].y)*(e.target->getPosition().y-vecWalls[furthestObject].points[pLeft].y) <
                (e.target->getPosition().x-vecWalls[furthestObject].points[pRight].x)*(e.target->getPosition().x-vecWalls[furthestObject].points[pRight].x)+
                (e.target->getPosition().y-vecWalls[furthestObject].points[pRight].y)*(e.target->getPosition().y-vecWalls[furthestObject].points[pRight].y))
            {
                e.destination = rotatedPoint(vecWalls[furthestObject].points[pLeft], e.target->hitbox.getPosition(), -20);
            }
            else e.destination = rotatedPoint(vecWalls[furthestObject].points[pRight], e.target->hitbox.getPosition(), 20);
        }

    }

    void AStar (Enemy& e){
        sf::Vector2i p1 = sf::Vector2i(e.getPosition().x / tileSize, e.getPosition().y / tileSize);
        sf::Vector2i p2 = sf::Vector2i(e.target->hitbox.getPosition().x / tileSize, e.target->hitbox.getPosition().y / tileSize);
        std::vector <sf::Vector2f> path;

    }
    void updateShadow(lightSource& ls) {
        ls.points.clear();
        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++){
                ls.points.push_back(getIntersection(ls, vecWalls[i].points[j]));
                ls.points.push_back(getIntersection(ls, rotatedPoint(vecWalls[i].points[j], ls, 0.0001)));
                ls.points.push_back(getIntersection(ls, rotatedPoint(vecWalls[i].points[j], ls, -0.0001)));
                sf::CircleShape c(2);
                c.setOrigin(1, 1);
                c.setPosition(rotatedPoint(vecWalls[i].points[j], ls, 1));
                rDebug.draw(c);
                c.setPosition(rotatedPoint(vecWalls[i].points[j], ls, -1));
                rDebug.draw(c);
            }
        }
        ls.sortPoints();
        ls.shadow.clear();
        ls.shadow.append(sf::Vertex(ls, ls.color));
        for (int i=0; i<ls.points.size(); i++) {
            ls.shadow.append(sf::Vertex(ls.points[i].point, ls.color));
        }
        ls.shadow.append(sf::Vertex(ls.points[0].point, ls.color));
    }

    void drawLine(sf::Vector2f p1, sf::Vector2f p2) {
        sf::VertexArray a (sf::Lines, 2);
        a[0]=p1;
        a[1]=p2;
        window->draw(a);
    }

    sf::Vector2f rotatedPoint(sf::Vector2f p, sf::Vector2f center, double d) {
        double a = atan2(p.y-center.y, p.x-center.x);
        double l = sqrt((p.y-center.y)*(p.y-center.y)+(p.x-center.x)*(p.x-center.x));
        sf::Vector2f r;
        r.x = p.x+d*sin(a);
        r.y = p.y-d*cos(a);
        return r;
    }

    CrossingPoint getIntersection (sf::Vector2f rp, sf::Vector2f p2) {
        sf::Vector2f rd = {p2.x-rp.x, p2.y-rp.y};
        sf::Vector2f sp;
        sf::Vector2f sd;
        double rMag;
        double sMag;
        double T1Min=1000;
        double T1, T2;

        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                sp = vecWalls[i].points[j];
                p2 = vecWalls[i].points[(j+1)%vecWalls[i].points.size()];
                sd ={p2.x-sp.x, p2.y-sp.y};
                T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                rMag = sqrt(rd.x*rd.x+rd.y*rd.y);
                sMag = sqrt(sd.x*sd.x+sd.y*sd.y);
                if (T1<T1Min && T1>0 && T2>0 && T2<1 && rd.x/rMag!=sd.x/sMag && rd.y/rMag!=sd.y/sMag)
                    T1Min=T1;
            }
        }
        return CrossingPoint(sf::Vector2f(rp.x+rd.x*T1Min, rp.y+rd.y*T1Min), atan2(rd.y, rd.x));
    }

    virtual void onSceneLoadToMemory() {
        font.loadFromFile("files/Carnevalee_Freakshow.ttf");
        mapa.loadFromFile("files/maps/shooter2D/map1.png");

        texBullet1.loadFromFile("files/textures/shooter2D/bullet1.png");
        tmpBullet.setTexture(texBullet1);
        spTadzik.playerBullet.setTexture(texBullet1);
        tmpBullet.setScale(0.2, 0.2);
        spTadzik.playerBullet.setScale(0.2, 0.2);

        texWall.loadFromFile("files/textures/shooter2D/wall1.png");
        spWall.setTexture(texWall);
        spWall.setOrigin(tileSize/2, tileSize/2);
        texCandle.loadFromFile("files/textures/shooter2D/candle.png");

        texPlayer.loadFromFile("files/textures/shooter2D/player.png");
        spTadzik.setTexture(texPlayer);
        spTadzik.setOrigin(spTadzik.getTextureRect().width/2, spTadzik.getTextureRect().height/2);

        texHealthBar.loadFromFile("files/textures/shooter2D/healthBar.png");
        texHealthFrame.loadFromFile("files/textures/shooter2D/healthFrame.png");
        spHealthBar.setTexture(texHealthBar);
        spHealthFrame.setTexture(texHealthFrame);

        loadMap();

        spBullet.setFillColor(sf::Color::White);
        spBullet.setRadius(3);
        spBullet.setOrigin(3, 3);

        rTexture.create(window->getSize().x, window->getSize().y);
        rDebug.create(window->getSize().x, window->getSize().y);

        texEnemy1.loadFromFile("files/textures/shooter2D/enemy1.png");

        Object tmpObject;
        tmpObject.points.push_back(sf::Vector2f(tileSize, tileSize));
        tmpObject.points.push_back(sf::Vector2f(window->getSize().x-tileSize, tileSize));
        tmpObject.points.push_back(sf::Vector2f(window->getSize().x-tileSize, window->getSize().y-tileSize));
        tmpObject.points.push_back(sf::Vector2f(tileSize, window->getSize().y-tileSize));

        vecWalls.push_back(tmpObject);

    }

    virtual void onSceneActivate() {

    }

    void loadMap() {
        bool t[mapa.getSize().x][mapa.getSize().y];
        sf::Color objectColor = sf::Color(255, 255, 255);
        for (unsigned int i=0; i<mapa.getSize().x; i++) {
            for (unsigned int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==objectColor && t[i][j]==0) {
                    Object tmpWall;
                    sf::Vector2i pocz = {i, j};
                    sf::Vector2i on = pocz;
                    std::vector <sf::Vector2i> sprites;
                    sprites.push_back(on);
                    int d = 0;
                    int dPrev = 3;
                    bool did = false;
                    do {
                        bool repeated = false;
                        if (mapa.getPixel(on.x, on.y) == objectColor) {
                            for (unsigned int i=0; i<sprites.size(); i++)
                                if (sprites[i]==on)
                                    repeated = true;
                            if (!repeated) sprites.push_back(on);
                        }
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
                    for (auto a:sprites) {
                        spWall.setPosition((a.x+0.5)*tileSize, (a.y+0.5)*tileSize);
                        vecSprites.push_back(spWall);
                    }
                    vecWalls.push_back(tmpWall);
                }
                else if (mapa.getPixel(i, j) == sf::Color(0, 0, 0)) {
                    spWall.setPosition((i+0.5)*tileSize, (j+0.5)*tileSize);
                    vecSprites.push_back(spWall);
                }
                else if (mapa.getPixel(i, j) == sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*tileSize, j*tileSize);
                }
            }
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

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                shoot();
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                vecLights.push_back(lightSource(sf::Vector2f(sf::Mouse::getPosition(*window)), sf::Vector2f(0, 0), sf::Color(255, 255, 0, 100), &texCandle));
                updateShadow(vecLights[vecLights.size()-1]);
            }
        }
    }

    void shoot() {
        tmpBullet = spTadzik.playerBullet;
        tmpBullet.setPosition(spTadzik.getPosition());
        tmpBullet.setDirection(sf::Vector2f(sf::Mouse::getPosition(*window)));
        vecBullets.push_back(tmpBullet);
    }

    void updateEnemies() {
        for (int i=vecEnemies.size()-1; i>=0; i--) {
            getEnemyDestination(vecEnemies[i]);
            sf::VertexArray a (sf::LinesStrip, 3);
            a[0]=sf::Vertex(vecEnemies[i].getPosition(), sf::Color::Red);
            a[1]=sf::Vertex(vecEnemies[i].destination, sf::Color::Red);
            a[2]=sf::Vertex(vecEnemies[i].target->hitbox.getPosition(), sf::Color::Red);
            rDebug.draw(a);
            handleCollision(vecEnemies[i], vecSprites);
            for (int j=vecBullets.size()-1; j>=0; j--) {
                if (Collision::BoundingBoxTest(vecEnemies[i], vecBullets[j]) && vecBullets[j].friendly) {
                    vecEnemies[i].onHit(vecBullets[j]);
                    vecBullets.erase(vecBullets.begin()+j);
                    vecEnemies[i].healthBar.setScale(vecEnemies[i].health/100.0, 1);
                    if (vecEnemies[i].health<=0) vecEnemies.erase(vecEnemies.begin()+i);
                }
            }
        }
        for (int i=0; i+1<vecEnemies.size(); i++) {
            for (int j=i+1; j<vecEnemies.size(); j++) {
                handleEntityCollision(vecEnemies[i], vecEnemies[j], 0.02);
            }
        }
        if (vecEnemies.size()<1) {
            //sf::Image img = sf::Sprite(rTexture.getTexture());
            //do {
            //    tmpEnemy.setPosition(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)));
            //}
            //while (rTexture.)
            vecEnemies.push_back(Enemy(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)), &texEnemy1, &spTadzik));
        }
    }

    virtual void draw(double deltaTime) {
        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -spTadzik.velocity.x < spTadzik.maxSpeed) {
            spTadzik.velocity.x -= acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && -spTadzik.velocity.y < spTadzik.maxSpeed) {
            spTadzik.velocity.y -= acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && spTadzik.velocity.y < spTadzik.maxSpeed) {
            spTadzik.velocity.y += acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && spTadzik.velocity.x < spTadzik.maxSpeed) {
            spTadzik.velocity.x += acceleration;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) spTadzik.health-=10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            vecEnemies.push_back(Enemy(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)), &texEnemy1, &spTadzik));
        }

        updateEnemies();
        for (unsigned i=0; i<vecEnemies.size(); i++) {
            if (Collision::PixelPerfectTest(spTadzik, vecEnemies[i])) {
                handleEntityCollision(spTadzik, vecEnemies[i], 0.5);
                if (spTadzik.sinceHit.getElapsedTime() > spTadzik.invincibilityTime) {
                    spTadzik.health-=vecEnemies[i].damage;
                    spTadzik.sinceHit.restart();
                }
            }
        }

        if (spTadzik.health<0) gameOver();

        spTadzik.update();
        if (debug) spTadzik.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));
        handleCollision(spTadzik, vecSprites);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);

        updateShadow(spTadzik.ls);
        for (unsigned int i=0; i<vecLights.size(); i++)
            updateShadow(vecLights[i]);

        window->clear(sf::Color(0, 0, 255));

        for (unsigned int i=0; i<vecSprites.size(); i++)
            window->draw(vecSprites[i]);

        for (unsigned int i=0; i<vecEnemies.size(); i++) {
            vecEnemies[i].update();
            window->draw(vecEnemies[i].hitbox);
            window->draw(vecEnemies[i]);
            window->draw(vecEnemies[i].healthBar);
        }
        //std::cout << vecEnemies.size();

        rTexture.clear(sf::Color(0, 0, 0, 255));
        for (int i=0; i<spTadzik.ls.points.size(); i++) {
            sf::CircleShape c(2);
            c.setOrigin(1, 1);
            c.setFillColor(sf::Color::Green);
            c.setPosition(spTadzik.ls.points[i].point);
            rDebug.draw(c);
        }
        for (unsigned int i=0; i<vecLights.size(); i++)
            rTexture.draw(vecLights[i].shadow, sf::BlendNone);
        rTexture.draw(spTadzik.ls.shadow, sf::BlendNone);
        rTexture.display();
        window->draw(sf::Sprite(rTexture.getTexture()));

        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                drawLine(vecWalls[i].points[j], vecWalls[i].points[(j+1)%vecWalls[i].points.size()]);
            }
        }

        if (vecBullets.size()>0) {
            for (int i=vecBullets.size()-1; i>=0; i--) {
                vecBullets[i].update();
                window->draw(vecBullets[i]);
                for (unsigned int j=0; j<vecSprites.size(); j++) {
                    if (Collision::BoundingBoxTest(vecBullets[i], vecSprites[j]))
                        if (vecBullets.size()!=0) vecBullets.erase(vecBullets.begin()+i);
                }
            }
        }

        rDebug.display();
        if (debug) window->draw(sf::Sprite(rDebug.getTexture()));
        rDebug.clear(sf::Color(0, 0, 0, 0));
        for (unsigned int i = 0; i<vecLights.size(); i++) window->draw(vecLights[i].sprite);
        window->draw(spTadzik);

        //std::cout << vecWalls[5].points.size();
        //std::cout << vecLights.size();
    }

protected:
    sf::Texture texBackground;
    sf::Texture texWall;
    sf::Texture texPlayer;
    sf::Texture texHealthBar;
    sf::Texture texHealthFrame;
    sf::Texture texBullet1;
    sf::Texture texCandle;
    sf::Texture texEnemy1;

    sf::Image mapa;
    sf::Font font;
    sf::RenderTexture rTexture;
    sf::RenderTexture rDebug;
    sf::BlendMode myBlendMode;

    Player spTadzik;
    sf::Sprite spWall;
    sf::Sprite spHealthBar;
    sf::Sprite spHealthFrame;
    sf::CircleShape spBullet;

    Bullet tmpBullet;
    Enemy tmpEnemy = Enemy(sf::Vector2f(500, 500), &texEnemy1, &spTadzik);

    std::vector <lightSource> vecLights;

    std::vector <Object> vecWalls;

    std::vector <Enemy> vecEnemies;

    std::vector <Bullet> vecBullets;

    std::vector <sf::Sprite> vecSprites;
    double acceleration = 2;
    int tileSize = 20;

    bool debug = false;
};

#endif //SHOOTER2D
