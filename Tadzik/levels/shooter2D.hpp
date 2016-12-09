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

        bool operator<(CrossingPoint& p){
            return angle < p.angle;
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

    class Bullet: public sf::Sprite {
    public:
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
        bool penetrating = false;
        bool bouncy = false;
    };

    class Player: public MovingEntity {
    public:
        Player() {

        }
        void update() {
            move(velocity);
            ls = getPosition();
            hitbox.setPosition(getPosition());
        }
        lightSource ls = lightSource(getPosition(), {0, 0}, sf::Color(255, 255, 255, 100));
        double health = 100;
        sf::Clock sinceHit;
        sf::Time invincibilityTime = sf::seconds(1);

    };

    class Enemy: public MovingEntity {
    public:
        Enemy(sf::Vector2f p, sf::Texture* t) {
            setPosition(p);
            setTexture(*t);
        }
        void update() {
            move(velocity);
            hitbox.setPosition(getPosition());
        }
        double health = 100;
        double maxSpeed = 5;
        double damage = 10;

    };

    void updateShadow(lightSource& ls) {
        //if (ls.velocity==sf::Vector2f(0, 0)) return;
        ls.points.clear();
        ls.points.push_back(getIntersection(ls, sf::Vector2f(0, 0)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(window->getSize().x, 0)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(window->getSize().x, window->getSize().y)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(0, window->getSize().y)));
        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++){
                ls.points.push_back(getIntersection(ls, vecWalls[i].points[j]));
                //ls.points.push_back(getIntersection(ls, rotatedPoint(vecWalls[i].points[j], ls, 1)));
                //ls.points.push_back(getIntersection(ls, rotatedPoint(vecWalls[i].points[j], ls, -1)));
                ls.points.push_back(getIntersection(ls, sf::Vector2f(vecWalls[i].points[j].x-0.01, vecWalls[i].points[j].y)));
                ls.points.push_back(getIntersection(ls, sf::Vector2f(vecWalls[i].points[j].x+0.01, vecWalls[i].points[j].y)));
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
        //ls.color.a = alpha;
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
        r.x = p.x+5*d*sin(a);
        r.y = p.y-5*d*cos(a);
        return r;
    }

    CrossingPoint getIntersection (sf::Vector2f rp, sf::Vector2f p2) {
        sf::Vector2f rd = {p2.x-rp.x, p2.y-rp.y};
        sf::Vector2f sp;
        sf::Vector2f sd;
        sf::Vector2f v2;
        double T1Min=10;
        double T1, T2;
        sf::Vector2f MIN = p2;

        sp = {0, 0};
        v2 = {window->getSize().x, 0};
        sd = {v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>=0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {window->getSize().x, 0};
        v2 = {window->getSize().x, window->getSize().y};
        sd ={v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>=0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {window->getSize().x, window->getSize().y};
        v2 = {0, window->getSize().y};
        sd ={v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>=0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {0, window->getSize().y};
        v2 = {0, 0};
        sd = {v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>=0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                sp = vecWalls[i].points[j];
                v2 = vecWalls[i].points[(j+1)%vecWalls[i].points.size()];
                sd ={v2.x-sp.x, v2.y-sp.y};
                T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                if (T1<T1Min && T1>0 && T2>=0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
                    T1Min=T1;
                    MIN.x = sp.x+sd.x*T2;
                    MIN.y = sp.y+sd.y*T2;
                }
            }
        }
        return CrossingPoint(MIN, atan2(MIN.y-rp.y, MIN.x-rp.x));
    }

    virtual void onSceneLoadToMemory() {
        font.loadFromFile("files/Carnevalee_Freakshow.ttf");
        mapa.loadFromFile("files/maps/shooter2D/map1.png");

        texBullet1.loadFromFile("files/textures/shooter2D/bullet1.png");
        tmpBullet.setTexture(texBullet1);
        tmpBullet.setScale(0.2, 0.2);
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
                            for (int i=0; i<sprites.size(); i++)
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
                        if (d==1) {
                            if (mapa.getPixel(on.x, on.y) == objectColor || mapa.getPixel(on.x-1, on.y) == objectColor) {
                                if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                on.y++;
                                did = true;
                                t[on.x][on.y] = 1;
                            }
                        }
                        if (d==2) {
                            if (mapa.getPixel(on.x-1, on.y) == objectColor || mapa.getPixel(on.x-1, on.y-1) == objectColor) {
                                if (d!=dPrev && abs(d-dPrev)!=2) tmpWall.points.push_back(sf::Vector2f(on.x*tileSize, on.y*tileSize));
                                on.x--;
                                did = true;
                                t[on.x][on.y] = 1;
                            }
                        }
                        if (d==3) {
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
                std::cout << std::endl;
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
                s1.velocity -= offset / 5.0f;
            }
        }
    }

    void handleEntityCollision (MovingEntity& e1, MovingEntity &e2) {
        sf::FloatRect intersection;
        if (e1.hitbox.getGlobalBounds().intersects(e2.hitbox.getGlobalBounds(), intersection)) {
            sf::Vector2f direction = e2.getPosition() - e1.getPosition();
            sf::Vector2f offset;
            // X collision
            if (abs(direction.x) > abs(direction.y))
            offset.x = ((direction.x<0)?-1:1)*intersection.width;
            // Y collision
            if (abs(direction.x) < abs(direction.y))
                offset.y = ((direction.y<0)?-1:1)*intersection.height;
            e1.velocity -= offset/2.0f;
            e2.velocity += offset/2.0f;
        }
    }

    void gameOver() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                tmpBullet.create(spTadzik.getPosition(), sf::Vector2f(sf::Mouse::getPosition(*window)), 5.0);
                vecBullets.push_back(tmpBullet);
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                vecLights.push_back(lightSource(sf::Vector2f(sf::Mouse::getPosition(*window)), sf::Vector2f(0, 0), sf::Color(255, 255, 0, 100), &texCandle));
                updateShadow(vecLights[vecLights.size()-1]);
            }
        }
    }

    virtual void draw(double deltaTime) {
        //input z klawiatury
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) && -spTadzik.velocity.x < maxSpeed) {
            spTadzik.velocity.x -= acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) || sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) && -spTadzik.velocity.y < maxSpeed) {
            spTadzik.velocity.y -= acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) || sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) && spTadzik.velocity.y < maxSpeed) {
            spTadzik.velocity.y += acceleration;
        }
        if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) && spTadzik.velocity.x < maxSpeed) {
            spTadzik.velocity.x += acceleration;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) spTadzik.health-=10;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::O)) {
            vecEnemies.push_back(Enemy(sf::Vector2f(Utils::randInt(10, window->getSize().x-10), Utils::randInt(10, window->getSize().y)), &texEnemy1));
        }

        if (spTadzik.sinceHit.getElapsedTime() > spTadzik.invincibilityTime) {
            for (unsigned i=0; i<vecEnemies.size(); i++) {
                vecEnemies[i].update();
                if (Collision::PixelPerfectTest(spTadzik, vecEnemies[i])) {
                    spTadzik.health-=vecEnemies[i].damage;
                    spTadzik.sinceHit.restart();
                    handleEntityCollision(spTadzik, vecEnemies[i]);
                }
            }
        }

        spHealthBar.setScale(spTadzik.health/100, 1);
        if (spTadzik.health<0) gameOver();

        std::cout << spTadzik.health << std::endl;
        spTadzik.update();
        //spTadzik.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));
        handleCollision(spTadzik, vecSprites);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);

        updateShadow(spTadzik.ls);
        for (unsigned int i=0; i<vecLights.size(); i++)
            updateShadow(vecLights[i]);

        window->clear(sf::Color(50, 50, 100));

        for (unsigned int i=0; i<vecSprites.size(); i++)
            window->draw(vecSprites[i]);

        for (auto a:vecEnemies) window->draw(a);
        //std::cout << vecEnemies.size();

        rTexture.clear(sf::Color(0, 0, 0, 0));
        for (int i=0; i<spTadzik.ls.points.size(); i++) {
            sf::CircleShape c(2);
            c.setOrigin(1, 1);
            c.setFillColor(sf::Color::Green);
            c.setPosition(spTadzik.ls.points[i].point);
            rDebug.draw(c);
        }
        for (unsigned int i=0; i<vecLights.size(); i++)
            rTexture.draw(vecLights[i].shadow, sf::BlendAdd);
        rTexture.draw(spTadzik.ls.shadow, sf::BlendAdd);
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
        //window->draw(sf::Sprite(rDebug.getTexture()));
        rDebug.clear(sf::Color(0, 0, 0, 0));
        for (unsigned int i = 0; i<vecLights.size(); i++) window->draw(vecLights[i].sprite);
        window->draw(spTadzik);
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

    Player spTadzik;
    sf::Sprite spWall;
    sf::Sprite spHealthBar;
    sf::Sprite spHealthFrame;
    sf::CircleShape spBullet;

    std::vector <lightSource> vecLights;

    std::vector <Object> vecWalls;

    std::vector <Enemy> vecEnemies;

    Bullet tmpBullet;

    std::vector <Bullet> vecBullets;

    std::vector <sf::Sprite> vecSprites;
    double speedX = 0, speedY = 0;
    double maxSpeed = 5;
    double acceleration = 2;
    int tileSize = 20;
    double angle = M_PI/6;
};

#endif //SHOOTER2D
