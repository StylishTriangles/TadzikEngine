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


class MovingEntity1: public sf::Sprite {
public:
    sf::Vector2f velocity;
};
class SHOOTER2D: public Scene{
public:
    SHOOTER2D(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
        :Scene(_name, mgr, w)
    {}

    class Object {
    public:
        std::vector <sf::Vector2f> points;
        std::vector <sf::Sprite> sprites;
    };

    class lightSource: public sf::Vector2f {
    public:
        void operator= (sf::Vector2f v) {
            x = v.x;
            y = v.y;
        }
        void update() {
        };
        std::vector <std::pair <sf::Vector2f, double> > points;
        sf::Vector2f velocity;
    };

    void drawLine(sf::Vector2f p1, sf::Vector2f p2) {
        sf::VertexArray a (sf::Lines, 2);
        a[0]=p1;
        a[1]=p2;
        window->draw(a);
    }

    sf::Vector2f getPoint(sf::Vector2f p1, sf::Vector2f p2) {
        double a = (p1.y-p2.y) / (p2.x-p1.x);
        sf::Vector2f onX;
        sf::Vector2f onY;
        if (p2.x-p1.x>0)
            onY = sf::Vector2f(window->getSize().x, p1.y-a*(window->getSize().x-p1.x));
        else
            onY = sf::Vector2f(0, p1.y+a*(p1.x));
        if (p2.y-p1.y>0)
            onX = sf::Vector2f(p1.x-(window->getSize().y-p1.y)/a, window->getSize().y);
        else
            onX = sf::Vector2f(p1.x+p1.y/a, 0);
        if (onX.x<0 || onX.x>window->getSize().x)
            return onX;
        return onY;
    }


    std::pair <sf::Vector2f, double> getIntersection (sf::Vector2f rp, sf::Vector2f p2) {
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
        if (T1<T1Min && T1>0 && T2>0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {window->getSize().x, 0};
        v2 = {window->getSize().x, window->getSize().y};
        sd ={v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {window->getSize().x, window->getSize().y};
        v2 = {0, window->getSize().y};
        sd ={v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        sp = {0, window->getSize().y};
        v2 = {0, 0};
        sd = {v2.x-sp.x, v2.y-sp.y};
        T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
        T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
        if (T1<T1Min && T1>0 && T2>0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
            T1Min=T1;
            MIN.x = sp.x+sd.x*T2;
            MIN.y = sp.y+sd.y*T2;
        }

        for (int i=0; i<vecWalls.size(); i++) {
            for (int j=0; j<vecWalls[i].points.size(); j++) {
                sp = vecWalls[i].points[j%vecWalls[i].points.size()];
                v2 = vecWalls[i].points[(j+1)%vecWalls[i].points.size()];
                sd ={v2.x-sp.x, v2.y-sp.y};
                T2 = (rd.x*(sp.y-rp.y) + rd.y*(rp.x-sp.x))/(sd.x*rd.y - sd.y*rd.x);
                T1 = (sp.x+sd.x*T2-rp.x)/rd.x;
                if (T1<T1Min && T1>0 && T2>0 && T2<1 && atan(rd.y/rd.x)!=atan(sd.y/sd.x)) {
                    T1Min=T1;
                    MIN.x = sp.x+sd.x*T2;
                    MIN.y = sp.y+sd.y*T2;
                }
            }
        }
        return {MIN, atan2(MIN.y-rp.y, MIN.x-rp.x)};
    }

    sf::Vector2f rotated (sf::Vector2f c, sf::Vector2f p, double angle) {
        double a = atan2(c.y-p.y, c.x-p.x);
        a+=angle;
        return sf::Vector2f(c.x+1, c.y+tan(a));
    }

    virtual void onSceneLoadToMemory() {
        font.loadFromFile("files/Carnevalee_Freakshow.ttf");
        mapa.loadFromFile("files/maps/shooter2D/map1.png");

        texWall.loadFromFile("files/textures/shooter2D/wall1.png");
        spWall.setTexture(texWall);
        spWall.setOrigin(tileSize/2, tileSize/2);

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
    }

    virtual void onSceneActivate() {

    }

    void loadMap() {
        bool t[mapa.getSize().x][mapa.getSize().y];
        sf::Color objectColor = sf::Color(255, 255, 255);
        //for (int i=0; i<mapa.getSize().x; i++)
        //    for (int j=0; j<mapa.getSize().y; j++)
        //        t[i][j]=0;
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==objectColor && t[i][j]==0) {
                    Object tmpWall;
                    spWall.setPosition((i+0.5)*tileSize, (j+0.5)*tileSize);
                    //tmpWall.sprites.push_back(spWall);
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
                        tmpWall.sprites.push_back(spWall);
                    }
                    vecWalls.push_back(tmpWall);
                std::cout << std::endl;
                }
                else if (mapa.getPixel(i, j) == sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*tileSize, j*tileSize);
                }
            }
        }
    }

    void handleCollision(MovingEntity1& s1, std::vector <Object>& objects) {
        for (int i=0; i<objects.size(); ++i) {
            for (int j=0; j<objects[i].sprites.size(); j++) {
                sf::FloatRect intersection;
                if (Collision::PixelPerfectTest(objects[i].sprites[j], s1) && s1.getGlobalBounds().intersects(objects[i].sprites[j].getGlobalBounds(), intersection)) {
                    sf::Vector2f direction = objects[i].sprites[j].getPosition() - s1.getPosition();
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
    }

    void gameOver() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {

        }
    }

    virtual void draw(double deltaTime) {
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) health-=10;
        //if (sf::Mouse::)

        spTadzik.move(spTadzik.velocity.x, spTadzik.velocity.y);

        handleCollision(spTadzik, vecWalls);

        if (health<0) gameOver();
        spHealthBar.setScale(health/100, 1);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);

        lsTadzik = spTadzik.getPosition();
        rTexture.clear(sf::Color::Black);
        window->clear(sf::Color(50, 50, 100));

        for (int i=0; i<vecWalls.size(); i++)
            for (int j=0; j<vecWalls[i].sprites.size(); j++)
                window->draw(vecWalls[i].sprites[j]);

        criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(0, 0)));
        criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(window->getSize().x, 0)));
        criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(window->getSize().x, window->getSize().y)));
        criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(0, window->getSize().y)));
        for (int i=0; i<vecWalls.size(); i++)
            for (int j=0; j<vecWalls[i].points.size(); j++){
                criticalPoints.push_back(getIntersection(lsTadzik, vecWalls[i].points[j]));
                criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(vecWalls[i].points[j].x+0.01, vecWalls[i].points[j].y)));
                criticalPoints.push_back(getIntersection(lsTadzik, sf::Vector2f(vecWalls[i].points[j].x-0.01, vecWalls[i].points[j].y)));
            }
        std::sort(criticalPoints.begin(), criticalPoints.end(), [](const std::pair<sf::Vector2f, double> &left, const std::pair<sf::Vector2f,double> &right) {
            return left.second < right.second;
        });
        sf::VertexArray shadow(sf::TrianglesFan, 0);
        shadow.append(sf::Vertex(lsTadzik, sf::Color(0, 0, 0, 0)));
        for (int i=0; i<criticalPoints.size(); i++) {
            shadow.append(sf::Vertex(criticalPoints[i].first, sf::Color(0, 0, 0, 0)));
        }
        shadow.append(sf::Vertex(criticalPoints[0].first, sf::Color(0, 0, 0, 0)));
        criticalPoints.clear();

        rTexture.draw(shadow, sf::BlendNone);
        rTexture.display();
        window->draw(sf::Sprite(rTexture.getTexture()));
        for (int i=0; i<vecWalls.size(); i++) {
            for (int j=0; j<vecWalls[i].points.size(); j++) {
                drawLine(vecWalls[i].points[j], vecWalls[i].points[(j+1)%vecWalls[i].points.size()]);
            }
        }
        window->draw(spTadzik);

    }

protected:
    sf::Texture texBackground;
    sf::Texture texWall;
    sf::Texture texPlayer;
    sf::Texture texHealthBar;
    sf::Texture texHealthFrame;
    sf::Image mapa;
    sf::Font font;
    sf::Transform trans;
    sf::RenderTexture rTexture;

    MovingEntity1 spTadzik;
    sf::Sprite spWall;
    sf::Sprite spHealthBar;
    sf::Sprite spHealthFrame;
    sf::CircleShape spBullet;

    lightSource lsTadzik;
    std::vector <lightSource> lsLights;

    std::vector <Object> vecWalls;

    std::vector <std::pair <sf::Vector2f, double> > criticalPoints;

    std::vector <sf::CircleShape> bullets;

    double speedX = 0, speedY = 0;
    double maxSpeed = 5;
    double acceleration = 2;
    double health = 100;
    int tileSize = 40;
    double angle = M_PI/6;
};

#endif //SHOOTER2D
