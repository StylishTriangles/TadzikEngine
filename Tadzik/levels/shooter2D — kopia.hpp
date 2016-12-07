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

sf::Vector2f getPoint(sf::Vector2f p1, sf::Vector2f p2, sf::RenderWindow* window) {
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
        int getRightPoint(sf::Vector2f p1) {
            sf::Vector2f v1 = {sprites[sprites.size()/2].getPosition().x - p1.x , sprites[sprites.size()/2].getPosition().y-p1.y};
            int RightPoint;
            double angle = -M_PI;
            double angle2;
            for (int i=0; i<points.size(); i++) {
                sf::Vector2f& p2 = points[i];
                angle2 = atan2(v1.y, v1.x) - atan2(p2.y-p1.y, p2.x-p1.x);
                if (angle2>M_PI) angle2-=M_PI;
                if (angle2<-M_PI) angle2+=M_PI;
                if (angle2 > angle) {
                    angle = angle2;
                    RightPoint = i;
                }
            }
            return RightPoint;
        }
        int getLeftPoint(sf::Vector2f p1) {
            sf::Vector2f v1 = {sprites[sprites.size()/2].getPosition().x - p1.x , sprites[sprites.size()/2].getPosition().y-p1.y};
            int LeftPoint;
            double angle = M_PI;
            double angle2;
            for (int i=0; i<points.size(); i++) {
                sf::Vector2f& p2 = points[i];
                angle2 = atan2(v1.y, v1.x) - atan2(p2.y-p1.y, p2.x-p1.x);
                if (angle2 < angle) {
                    angle = angle2;
                    LeftPoint = i;
                }
            }
            return LeftPoint;
        }
        sf::VertexArray getShadow(sf::Vector2f p1, sf::RenderWindow* window) {
            int right = getLeftPoint(p1);
            int left = getRightPoint(p1);
            sf::VertexArray a(sf::TrianglesFan);
            a.resize(4);
            a[0]=getPoint(p1, points[left], window);
            a[1]=getPoint(p1, points[right], window);
            a[2]=points[right];
            int p=3;
            int i=right;
            while (i!=left) {
                i--;
                if (i<0) i+=points.size();
                a.resize(p+1);
                a[p]=points[i];
                p++;
            }
            a.resize(p+1);
            a[p]=points[left];

            return a;
        }
    };

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


    sf::ConvexShape getPlayerShadow() {
        sf::ConvexShape convex;
        convex.setFillColor(sf::Color::Green);
        sf::Vector2f tmp;
        sf::Vector2f p1 = spTadzik.getPosition();
        sf::Vector2i p2 = sf::Mouse::getPosition(*window);
        convex.setPointCount(4);
        convex.setPoint(0, p1);
        convex.setPoint(3, p1);
        double a = (p1.y-p2.y) / (p2.x-p1.x);
        double a1 = tan(atan(a)-angle);
        if (p2.x-p1.x>0)
            tmp = sf::Vector2f(window->getSize().x, p1.y-a1*(window->getSize().x-p1.x));
        else
            tmp = sf::Vector2f(0, p1.y+a1*(p1.x));
        convex.setPoint(1, tmp);
        a1 = tan(atan(a)+angle);
        if (p2.x-p1.x>0)
            tmp = sf::Vector2f(window->getSize().x, p1.y-a1*(window->getSize().x-p1.x));
        else
            tmp = sf::Vector2f(0, p1.y+a1*(p1.x));
        convex.setPoint(2, tmp);
        return convex;
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

        spTadzik.move(spTadzik.velocity.x, spTadzik.velocity.y);

        handleCollision(spTadzik, vecWalls);

        if (health<0) gameOver();
        spHealthBar.setScale(health/100, 1);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);


        window->clear(sf::Color(50, 50, 100));

        //window->draw(getPlayerShadow());
        window->draw(spTadzik);
        for (int i=0; i<vecWalls.size(); i++)
            for (int j=0; j<vecWalls[i].sprites.size(); j++)
                window->draw(vecWalls[i].sprites[j]);
        //drawShadow(vecWalls);
        window->draw(spHealthBar);
        window->draw(spHealthFrame);
        for (auto a:bullets) window->draw(a);
        spBullet.setFillColor(sf::Color::White);
        spBullet.setRadius(3);
        for (int i=0; i<vecWalls.size(); i++) {
            window->draw(vecWalls[i].getShadow(spTadzik.getPosition(), window));
            for (int j=0; j<vecWalls[i].points.size(); j++) {
                spBullet.setPosition(vecWalls[i].points[j]);
                window->draw(spBullet);
            }
        }
        //window->draw(vecWalls[0].getShadow(spTadzik.getPosition(), window));
        //std::cout << vecWalls.size();
    }

protected:
    sf::Texture texBackground;
    sf::Texture texWall;
    sf::Texture texPlayer;
    sf::Texture texHealthBar;
    sf::Texture texHealthFrame;
    sf::Image mapa;
    sf::Font font;

    MovingEntity1 spTadzik;
    sf::Sprite spWall;
    sf::Sprite spHealthBar;
    sf::Sprite spHealthFrame;
    sf::CircleShape spBullet;

    std::vector <Object> vecWalls;

    std::vector <sf::CircleShape> bullets;

    double speedX = 0, speedY = 0;
    double maxSpeed = 5;
    double acceleration = 2;
    double health = 100;
    int tileSize = 40;
    double angle = M_PI/6;
};

#endif //SHOOTER2D
