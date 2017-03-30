
#ifndef RPG_HPP

#define RPG_HPP

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Collision.hpp"
#include "../include/Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <cmath>

class RPG : public Scene {
 public:
  RPG(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
      : Scene(_name, mgr, w) {}

  enum Direction { None = 0, Down = 1, Up = -1, Right = 1, Left = -1 };

  class Entity {
   public:
    ARO::AnimSprite aSprite;
    int health = 100;
    int damage = 10;
    float basespeed = 10;
    float speedX = 0;
    float speedY = 0;
    char dir = 'N';
    int side = Right;
    bool isAttacking;

    void draw(sf::RenderWindow* w) { w->draw(aSprite); }
    void move(sf::Vector2f a) { aSprite.move(a); }
    void move(float x, float y) {
      if (x == 0 || y == 0)
        aSprite.move(sf::Vector2f(x, y));
      else
        aSprite.move(
            sf::Vector2f(x * (double)0.70710678, y * (double)0.70710678));
      std::cout << dir << "\t" << speedY << "\n";
    }
  };

  class Slice : public ARO::AnimSprite {
   public:
    Slice(sf::Vector2f V, ARO::Anim* A, int dmg, bool b) {
      setAnimation(A);
      setPosition(V);
      Damage = dmg;
      isPlayer = b;
    }
    int Damage = 10;
    bool isPlayer = 0;
    void collide(std::vector<sf::Sprite>& V) {
      for (int i = V.size(); i >= 0; i--)
        if (Collision::PixelPerfectTest(*this, V[i])) V.erase(V.begin() + i);
    }
  };
  void loadMap(sf::Image mapa) {
    for (int i = 0; i < mapa.getSize().x; i++) {
      for (int j = 0; j < mapa.getSize().y; j++) {
        if (mapa.getPixel(i, j) == sf::Color(0, 0, 0)) {
          tempWall.setPosition(i * tilesize, j * tilesize);
          spWall.push_back(tempWall);
        } else {
          tempGrass.setPosition(i * tilesize, j * tilesize);
          spGrass.push_back(tempGrass);
        }

        if (mapa.getPixel(i, j) == sf::Color(0, 0, 255)) {
          Player.aSprite.setPosition(i * tilesize, j * tilesize);
        }

        if (mapa.getPixel(i, j) == sf::Color(255, 0, 0)) {
          tempEnemy.setPosition(i * tilesize, j * tilesize);
          spEnemy.push_back(tempEnemy);
        }
      }
    }
  }

  float distance(sf::Sprite spA, sf::Sprite spB) {
    return sqrt((spA.getPosition().x - spB.getPosition().x) *
                    (spA.getPosition().x - spB.getPosition().x) +
                (spA.getPosition().y - spB.getPosition().y) *
                    (spA.getPosition().y - spB.getPosition().y));
  }

  void staticCollision(Entity& E) {
    sf::FloatRect R = E.aSprite.getGlobalBounds();
    bool b = 0;
    int w;
    for (int i = 0; i < spWall.size(); i++)

      if (spWall[i].getGlobalBounds().intersects(R)) {
        b = 1;
        w = i;
        break;
      }
    if (b) {
      E.speedX = 0;
      E.speedY = 0;
      E.move((spWall[w].getPosition().x - E.aSprite.getPosition().x) * 2,
             (spWall[w].getPosition().y - E.aSprite.getPosition().y) * 2);
    } else {
      /// x
      R.left += E.speedX;
      for (int i = 0; i < spWall.size(); i++)
        if (spWall[i].getGlobalBounds().intersects(R)) {
          E.speedX = 0;
          break;
        }
      R = E.aSprite.getGlobalBounds();
      /// y
      R.top += E.speedY;
      for (int i = 0; i < spWall.size(); i++)
        if (spWall[i].getGlobalBounds().intersects(R)) {
          E.speedY = 0;
          break;
        }
    }
  }

  virtual void onSceneLoadToMemory() {
    window->setView(view);

    texUpRun.loadFromFile("files/textures/rpg/up/run.png");
    upRun.setSpriteSheet(&texUpRun, 17, sf::milliseconds(60));

    texDownRun.loadFromFile("files/textures/rpg/down/run.png");
    downRun.setSpriteSheet(&texDownRun, 17, sf::milliseconds(60));

    texRightRun.loadFromFile("files/textures/rpg/right/run.png");
    rightRun.setSpriteSheet(&texRightRun, 23, sf::milliseconds(60));

    texLeftRun.loadFromFile("files/textures/rpg/left/run.png");
    leftRun.setSpriteSheet(&texLeftRun, 23, sf::milliseconds(60));

    texDownAttack.loadFromFile("files/textures/rpg/down/attack.png");
    downAttack.setSpriteSheet(&texDownAttack, 30, sf::milliseconds(100));

    texUpAttack.loadFromFile("files/textures/rpg/up/attack.png");
    upAttack.setSpriteSheet(&texUpAttack, 24, sf::milliseconds(100));

    texRightAttack.loadFromFile("files/textures/rpg/right/attack.png");
    rightAttack.setSpriteSheet(&texRightAttack, 30, sf::milliseconds(100));

    texSliceDown.loadFromFile("files/textures/rpg/down/projectile.png");
    sliceDown.setSpriteSheet(&texSliceDown, 50, sf::milliseconds(12));

    texSliceRight.loadFromFile("files/textures/rpg/right/projectile.png");
    sliceRight.setSpriteSheet(&texSliceRight, 66, sf::milliseconds(12));

    texIdle.loadFromFile("files/textures/rpg/down/idle.png");
    Idle.setSpriteSheet(&texIdle, 15, sf::milliseconds(300));

    Player.aSprite.setAnimation(&rightRun);
    Player.aSprite.setScale(5, 5);

    texWall.loadFromFile("files/textures/rpg/Wall.png");
    tempWall.setTexture(texWall);
    tempWall.setScale(1, 1);

    texEnemy.loadFromFile("files/textures/rpg/enemyStand.png");
    tempEnemy.setTexture(texEnemy);
    tempEnemy.setOrigin(tempEnemy.getTextureRect().width * 0.5,
                        tempEnemy.getTextureRect().height * 0.5);
    tempEnemy.setScale(5, 5);

    texGrass.loadFromFile("files/textures/rpg/grass.png");
    tempGrass.setTexture(texGrass);

    texDoor.loadFromFile("files/textures/rpg/door.png");
    texDoorOpen.loadFromFile("files/textures/rpg/doorOpen.png");

    window->setMouseCursorVisible(0);
    texCrosshair.loadFromFile("files/textures/rpg/crosshair.png");
    spCrosshair.setTexture(texCrosshair);
    spCrosshair.setOrigin(spCrosshair.getTextureRect().height * 0.5,
                          spCrosshair.getTextureRect().width * 0.5);

    mapa.loadFromFile("files/maps/rpg/mapa1.png");
    loadMap(mapa);
  }

  virtual void onSceneActivate() {}

  void deliverEvent(sf::Event& event) {}

  virtual void draw(sf::Time deltaTime) {
    float dT = deltaTime.asMilliseconds();

    // Player.speedX=0, Player.speedY=0;
    /// Movement
    Player.speedX = Player.speedX * 0.5;
    Player.speedY = Player.speedY * 0.5;
    /// Kierunki
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) &&
        Player.isAttacking == false) {
      Player.isAttacking = true;
      if (Player.dir == 'D') {
        Player.aSprite.setAnimation(&downAttack);
        sliceVec.push_back(
            Slice(Player.aSprite.getPosition(), &sliceDown, 10, true));
      } else if (Player.dir == 'R') {
        Player.aSprite.setAnimation(&rightAttack);
        sliceVec.push_back(
            Slice(Player.aSprite.getPosition(), &sliceRight, 10, true));
      } else {
        Player.aSprite.setAnimation(&upAttack);
        sliceVec.push_back(
            Slice(Player.aSprite.getPosition(), &sliceRight, 10, true));
      }

      Player.aSprite.reset();
      Player.aSprite.setLooped(false);

      sliceVec[sliceVec.size() - 1].setLooped(false);
      sliceVec[sliceVec.size() - 1].setScale(5, 5);
    }
    if (Player.aSprite.shouldDestroy()) {
      Player.isAttacking = false;
      Player.aSprite.setLooped(true);
    }
    if (Player.isAttacking == false) {
      if (Player.dir != 'N') {
        if ((Player.dir == 'U' || Player.dir == 'D') &&
            std::abs(Player.speedY) < std::abs(Player.speedX))
          Player.dir = 'N', Player.aSprite.setAnimation(&Idle);
        if ((Player.dir == 'R' || Player.dir == 'L') &&
            std::abs(Player.speedY) > std::abs(Player.speedX))
          Player.dir = 'N', Player.aSprite.setAnimation(&Idle);
        else if (std::abs(Player.speedX) < 0.5 && std::abs(Player.speedY) < 0.5)
          Player.dir = 'N', Player.aSprite.setAnimation(&Idle);
      }
      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Up))) {
        Player.speedY -= 0.6 * Player.basespeed;
        if (Player.dir == 'N' || Player.dir == 'D') {
          Player.dir = 'U';
          Player.aSprite.setAnimation(&upRun);
        }
      }
      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Down))) {
        Player.speedY += 0.6 * Player.basespeed;
        if (Player.dir == 'N' || Player.dir == 'U') {
          Player.dir = 'D';
          Player.aSprite.setAnimation(&downRun);
        }
      }
      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Left))) {
        Player.speedX -= 0.6 * Player.basespeed;
        if (Player.dir == 'N' || Player.dir == 'R') {
          Player.dir = 'L';
          Player.aSprite.setAnimation(&leftRun);
        }
      }
      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Right))) {
        Player.speedX += 0.6 * Player.basespeed;
        if (Player.dir == 'N' || Player.dir == 'L') {
          Player.dir = 'R';
          Player.aSprite.setAnimation(&rightRun);
        }
      }

      if (Player.speedX < -Player.basespeed) Player.speedX = -Player.basespeed;
      if (Player.speedY < -Player.basespeed) Player.speedY = -Player.basespeed;
      if (Player.speedX > Player.basespeed) Player.speedX = Player.basespeed;
      if (Player.speedY > Player.basespeed) Player.speedY = Player.basespeed;

      staticCollision(Player);
      if (Player.aSprite.getAnim() == &Idle)
        Player.aSprite.update(dT), std::cout << "arek;";
      else if (((std::abs(Player.speedX) + std::abs(Player.speedY)) /
                Player.basespeed) > 1)
        Player.aSprite.update(dT);
      else
        Player.aSprite.update(
            dT * (std::abs(Player.speedX) + std::abs(Player.speedY)) /
            (2 * Player.basespeed));
      Player.move(Player.speedX, Player.speedY);
      view.setCenter(Player.aSprite.getPosition());
      window-> setView(view);
    } else {
      staticCollision(Player);
      if (Player.aSprite.shouldDestroy() == 0) Player.aSprite.update(dT);
      if ((sf::Keyboard::isKeyPressed(sf::Keyboard::D) ||
           sf::Keyboard::isKeyPressed(sf::Keyboard::Right)))
        Player.dir = 'R';
      else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Left)))
        Player.dir = 'L';
      else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::S) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Down)))
        Player.dir = 'D';
      else if ((sf::Keyboard::isKeyPressed(sf::Keyboard::W) ||
                sf::Keyboard::isKeyPressed(sf::Keyboard::Up)))
        Player.dir = 'U';
    }
    /// Kolizja gdzies tu

    if (Player.speedX < -Player.basespeed) Player.speedX = -Player.basespeed;
    if (Player.speedY < -Player.basespeed) Player.speedY = -Player.basespeed;
    if (Player.speedX > Player.basespeed) Player.speedX = Player.basespeed;
    if (Player.speedY > Player.basespeed) Player.speedY = Player.basespeed;

    spCrosshair.setPosition(sf::Vector2f(sf::Mouse::getPosition(*window)));

    // OBRACANIE

    // SKELETON MOVEMENT
    for (int i = 0; i < spEnemy.size(); i++)
      spEnemy[i].move(sf::Vector2f(Utils::randInt(-50, 50) * tilesize * 0.001,
                                   Utils::randInt(-50, 50) * tilesize * 0.001));

    // DRAW STARTS
    window-> clear(sf::Color());
    for (int i = 0; i < spGrass.size(); i++) window-> draw(spGrass[i]);
    for (int i = 0; i < spWall.size(); i++)  window-> draw(spWall[i]);
    for (int i = 0; i < spEnemy.size(); i++) window-> draw(spEnemy[i]);

    window->draw(spCrosshair);

    Player.draw(window);

    for (int i = sliceVec.size() - 1; i >= 0; i--)
      if (sliceVec[i].isPlayer == true) {
        if (Player.isAttacking == 0) sliceVec.erase(sliceVec.begin() + i);
      } else if (sliceVec[i].shouldDestroy())
        sliceVec.erase(sliceVec.begin() + i);

    for (int i = 0; i < sliceVec.size(); i++) {
      sliceVec[i].update(dT);
      window->draw(sliceVec[i]);
      sliceVec[i].collide(spEnemy);
    }
  }

 protected:
  sf::View view = sf::View(sf::FloatRect(0, 0, 1280, 720));

  sf::Image mapa;

  sf::Texture texWall;
  std::vector<sf::Sprite> spWall;
  sf::Sprite tempWall;

  int tilesize = 30;

  sf::Texture texEnemy;
  std::vector<sf::Sprite> spEnemy;
  sf::Sprite tempEnemy;

  sf::Texture texGrass;
  std::vector<sf::Sprite> spGrass;
  sf::Sprite tempGrass;

  sf::Texture texDoor;
  sf::Texture texDoorOpen;
  std::vector<sf::Sprite> spDoor;
  sf::Sprite tempDoor;

  sf::Texture texCrosshair;
  sf::Sprite spCrosshair;

  std::vector<Slice> sliceVec;
  sf::Texture texSliceDown;
  ARO::Anim sliceDown;
  sf::Texture texSliceRight;
  ARO::Anim sliceRight;

  Entity Player;
  sf::Texture texIdle;
  ARO::Anim Idle;
  sf::Texture texDownRun;
  ARO::Anim downRun;
  sf::Texture texUpRun;
  ARO::Anim upRun;
  sf::Texture texRightRun;
  ARO::Anim rightRun;
  sf::Texture texLeftRun;
  ARO::Anim leftRun;
  sf::Texture texDownAttack;
  ARO::Anim downAttack;
  sf::Texture texUpAttack;
  ARO::Anim upAttack;
  sf::Texture texRightAttack;
  ARO::Anim rightAttack;
};
#endif  // RPG
