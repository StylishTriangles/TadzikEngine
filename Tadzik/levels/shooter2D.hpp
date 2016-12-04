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
        for (int i=0; i<mapa.getSize().x; i++) {
            for (int j=0; j<mapa.getSize().y; j++) {
                if (mapa.getPixel(i, j)==sf::Color(255, 255, 255)) {
                    spWall.setPosition((i+0.5)*tileSize, (j+0.5)*tileSize);
                    vecWalls.push_back(spWall);
                }
                else if (mapa.getPixel(i, j) == sf::Color(0, 0, 255)) {
                    spTadzik.setPosition(i*tileSize, j*tileSize);
                }
            }
        }
    }

    void handleCollision(MovingEntity1& s1, std::vector <sf::Sprite>& vecSprites) {
        for (int i=0; i<vecSprites.size(); ++i) {
            sf::FloatRect intersection;
            if (Collision::PixelPerfectTest(vecSprites[i], s1) && s1.getGlobalBounds().intersects(vecSprites[i].getGlobalBounds(), intersection)) {
                sf::Vector2f direction = vecSprites[i].getPosition() - s1.getPosition();
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

        }
    }

    sf::Vector2f getPoint(sf::Vector2f p2) {
        sf::Vector2f p1 = spTadzik.getPosition();
        double a = (p1.y-p2.y) / (p2.x-p1.x);
        if (p2.x-p1.x>0) {
            return sf::Vector2f(window->getSize().x, p1.y-a*(window->getSize().x-p1.x));
        }
        else {
            return sf::Vector2f(0, p1.y+a*(p1.x));
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

    void drawShadow(std::vector <sf::Sprite> vec) {
        for (int i=0; i<vec.size(); i++) {
            sf::ConvexShape convex;
            convex.setFillColor(sf::Color::Black);
            convex.setPointCount(4);

            convex.setPoint(0, sf::Vector2f(vec[i].getGlobalBounds().left, vec[i].getGlobalBounds().top));
            convex.setPoint(1, getPoint(sf::Vector2f(vec[i].getGlobalBounds().left, vec[i].getGlobalBounds().top)));
            convex.setPoint(2, getPoint(sf::Vector2f(vec[i].getGlobalBounds().left+vec[i].getGlobalBounds().width, vec[i].getGlobalBounds().top+vec[i].getGlobalBounds().height)));
            convex.setPoint(3, sf::Vector2f(vec[i].getGlobalBounds().left+vec[i].getGlobalBounds().width, vec[i].getGlobalBounds().top+vec[i].getGlobalBounds().height));

            window->draw(convex);
            convex.setPoint(0, sf::Vector2f(vec[i].getGlobalBounds().left+vec[i].getGlobalBounds().width, vec[i].getGlobalBounds().top));
            convex.setPoint(1, getPoint(sf::Vector2f(vec[i].getGlobalBounds().left+vec[i].getGlobalBounds().width, vec[i].getGlobalBounds().top)));
            convex.setPoint(2, getPoint(sf::Vector2f(vec[i].getGlobalBounds().left, vec[i].getGlobalBounds().top+vec[i].getGlobalBounds().height)));
            convex.setPoint(3, sf::Vector2f(vec[i].getGlobalBounds().left, vec[i].getGlobalBounds().top+vec[i].getGlobalBounds().height));
            window->draw(convex);
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

        spTadzik.move(spTadzik.velocity.x, spTadzik.velocity.y);

        handleCollision(spTadzik, vecWalls);

        if (health<0) gameOver();
        spHealthBar.setScale(health/100, 1);

        spTadzik.velocity.x*=0.8;
        spTadzik.velocity.y*=0.8;
        spTadzik.setRotation(atan2(sf::Mouse::getPosition(*window).y-spTadzik.getPosition().y, sf::Mouse::getPosition(*window).x-spTadzik.getPosition().x)*180/M_PI);


        window->clear(sf::Color::White);

        //window->draw(getPlayerShadow());
        window->draw(spTadzik);
        for (auto a:vecWalls) window->draw(a);
        drawShadow(vecWalls);
        window->draw(spHealthBar);
        window->draw(spHealthFrame);
        for (auto a:bullets) window->draw(a);
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

    std::vector <sf::Sprite> vecWalls;

    std::vector <sf::CircleShape> bullets;

    double speedX = 0, speedY = 0;
    double maxSpeed = 5;
    double acceleration = 2;
    double health = 100;
    int tileSize = 40;
    double angle = M_PI/6;
};

#endif //SHOOTER2D
