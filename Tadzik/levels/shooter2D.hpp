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

    class MovingEntity1: public sf::Sprite {
    public:
        sf::Vector2f velocity;
        void update() {
            move(velocity);
        }
    };

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
            sprite.setPosition(x, y);
        }
        void update() {

        }
        std::vector <std::pair <sf::Vector2f, double> > points;
        sf::Vector2f velocity;
        sf::VertexArray shadow = sf::VertexArray(sf::TrianglesFan, 0);
        sf::Sprite sprite;
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

    void updateShadow(lightSource& ls, int alpha) {
        ls.points.clear();
        ls.points.push_back(getIntersection(ls, sf::Vector2f(0, 0)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(window->getSize().x, 0)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(window->getSize().x, window->getSize().y)));
        ls.points.push_back(getIntersection(ls, sf::Vector2f(0, window->getSize().y)));
        for (unsigned int i=0; i<vecWalls.size(); i++)
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
            std::sort(ls.points.begin(), ls.points.end(), [](const std::pair<sf::Vector2f, double> &left, const std::pair<sf::Vector2f,double> &right) {
        return left.second < right.second;
        });
        ls.shadow.clear();
        ls.shadow.append(sf::Vertex(ls, sf::Color(0, 0, 0, alpha)));
        for (int i=0; i<ls.points.size(); i++) {
            ls.shadow.append(sf::Vertex(ls.points[i].first, sf::Color(0, 0, 0, alpha)));
        }
        ls.shadow.append(sf::Vertex(ls.points[0].first, sf::Color(0, 0, 0, alpha)));
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
        //a=1/a;
        sf::Vector2f r;
        r.x = p.x+5*d*sin(a);
        r.y = p.y-5*d*cos(a);
        return r;
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
                sp = vecWalls[i].points[j];
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
        tmpLightsource.sprite.setTexture(texCandle);
        tmpLightsource.sprite.setOrigin(tmpLightsource.sprite.getTextureRect().width/2, tmpLightsource.sprite.getTextureRect().height/2);

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

    }

    virtual void onSceneActivate() {

    }

    void loadMap() {
        bool t[mapa.getSize().x][mapa.getSize().y];
        sf::Color objectColor = sf::Color(255, 255, 255);
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
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

    void handleCollision(MovingEntity1& s1, std::vector <sf::Sprite>& walls) {
        for (int i=0; i<walls.size(); ++i) {
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

    void gameOver() {

    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseButtonPressed) {
            if (event.mouseButton.button == sf::Mouse::Left) {
                tmpBullet.create(spTadzik.getPosition(), sf::Vector2f(sf::Mouse::getPosition(*window)), 5.0);
                vecBullets.push_back(tmpBullet);
            }
            if (event.mouseButton.button == sf::Mouse::Right) {
                tmpLightsource = sf::Vector2f(sf::Mouse::getPosition(*window));
                vecLights.push_back(tmpLightsource);
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) health-=10;
        spHealthBar.setScale(health/100, 1);
        if (health<0) gameOver();

        spTadzik.update();
        handleCollision(spTadzik, vecSprites);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);

        lsTadzik = spTadzik.getPosition();
        updateShadow(lsTadzik, 0);
        for (unsigned int i=0; i<vecLights.size(); i++)
            updateShadow(vecLights[i], 10);

        window->clear(sf::Color(50, 50, 100));

        for (unsigned int i=0; i<vecSprites.size(); i++)
            window->draw(vecSprites[i]);

        for (int i=vecBullets.size()-1; i>=0; i--) {
            vecBullets[i].update();
            window->draw(vecBullets[i]);
            for (unsigned int j=0; j<vecSprites.size(); j++) {
                if (Collision::BoundingBoxTest(vecBullets[i], vecSprites[j]))
                    if (vecBullets.size()!=0) vecBullets.erase(vecBullets.begin()+i);
            }
        }

        rTexture.clear(sf::Color::Black);
        for (int i=0; i<lsTadzik.points.size(); i++) {
            sf::CircleShape c(2);
            c.setOrigin(1, 1);
            c.setFillColor(sf::Color::Green);
            c.setPosition(lsTadzik.points[i].first);
            rDebug.draw(c);
        }
        for (int i=0; i<vecLights.size(); i++)
            rTexture.draw(vecLights[i].shadow, sf::BlendNone);
        rTexture.draw(lsTadzik.shadow, sf::BlendNone);
        rTexture.display();
        window->draw(sf::Sprite(rTexture.getTexture()));
        sf::Vector2f enemy = sf::Vector2f(Utils::randInt(10, 1270), Utils::randInt(10, 710));
        //if (sf::Sprite(rTexture.getTexture()))
        for (unsigned int i=0; i<vecWalls.size(); i++) {
            for (unsigned int j=0; j<vecWalls[i].points.size(); j++) {
                drawLine(vecWalls[i].points[j], vecWalls[i].points[(j+1)%vecWalls[i].points.size()]);
            }
        }
        rDebug.display();
        window->draw(sf::Sprite(rDebug.getTexture()));
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

    sf::Image mapa;
    sf::Font font;
    sf::Transform trans;
    sf::RenderTexture rTexture;
    sf::RenderTexture rDebug;

    MovingEntity1 spTadzik;
    sf::Sprite spWall;
    sf::Sprite spHealthBar;
    sf::Sprite spHealthFrame;
    sf::CircleShape spBullet;

    lightSource lsTadzik;
    lightSource tmpLightsource;
    std::vector <lightSource> vecLights;

    std::vector <Object> vecWalls;

    Bullet tmpBullet;

    std::vector <Bullet> vecBullets;

    std::vector <sf::CircleShape> bullets;

    std::vector <sf::Sprite> vecSprites;
    double speedX = 0, speedY = 0;
    double maxSpeed = 5;
    double acceleration = 2;
    double health = 100;
    int tileSize = 20;
    double angle = M_PI/6;
};

#endif //SHOOTER2D
