#ifndef ANIMATED_SPRITE_H
#define ANIMATED_SPRITE_H

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

struct AnimationFrame{
    sf::Texture* texture;
    float duration;
    AnimationFrame(sf::Texture* t, float d)
    :duration(d)  //[ms]
    {
        texture = t;
    }
};

class Animation
{
public:
    std::string getName()           {return name;}
    void setName(std::string _name) {name = _name;}
    void addFrame(AnimationFrame f) {vecFrames.push_back(f);}
    std::vector<AnimationFrame> vecFrames;                   //nie chce mi sie calego vectora pisac

protected:
    std::string name;
};

class AnimatedSprite
{
public:
    void setAnimation(Animation* a){
        currentAnimation = a;
        actFrame=0;

        if(currentAnimation->vecFrames.size() == 0){
            std::cout << "dafuq? AnimatedSprite::setAnimation()\n";
        }
        sprite.setTexture(*currentAnimation->vecFrames[0].texture, true);
        countReplays=0;
    }

    void update(float delta){
        currTime += delta;
        if(currTime > currentAnimation->vecFrames[actFrame].duration){
            actFrame=(actFrame+1)%currentAnimation->vecFrames.size();
            sprite.setTexture(*currentAnimation->vecFrames[actFrame].texture);
            currTime=0;
            if(actFrame==0)
                countReplays++;
        }
    }

    void move(double x, double y) {
        sprite.move(x, y);
    }

    void setPosition(float x, float y) {
        sprite.setPosition(x, y);
    }
    void setPosition(sf::Vector2f pos) {
        sprite.setPosition(pos);
    }
    Animation* getAnimation() {
        return currentAnimation;
    }

    int getFrame(){
        return actFrame;
    }

    int framesSize(){
        return currentAnimation->vecFrames.size();
    }

    int getReplays(){
        return countReplays;
    }

    sf::Sprite sprite;
    Animation* currentAnimation;

protected:
    float currTime=0;
    int actFrame=0;
    int countReplays=0;
};

namespace ARO {
    class Anim {
    public:
        Anim () {};
        Anim (sf::Texture* SpriteSheet, int frameWidth, int dur) {
            spriteSheet = SpriteSheet;
            width = frameWidth;
            frames = spriteSheet->getSize().x/width;
            height = spriteSheet->getSize().y;
            duration = sf::milliseconds(dur);
        }
        sf::Texture* spriteSheet;
        sf::Time duration = sf::seconds(2);
        int frames;
        int width;
        int height;
        void setSpriteSheet (sf::Texture* t, int frameWidth, int dur) {
            spriteSheet = t;
            width = frameWidth;
            frames = spriteSheet->getSize().x/width;
            height = spriteSheet->getSize().y;
            duration = sf::milliseconds(dur);
        }
    };

    class AnimSprite: public sf::Sprite {
    public:
        AnimSprite() {};
        AnimSprite(Anim* anim, bool looped = true, sf::Vector2f vel = sf::Vector2f(0, 0)) {
            setAnimation(anim);
            m_looped = looped;
            velocity = vel;
        }
        bool isLooped() {return m_looped;};
        bool shouldDestroy() {return m_destroy;};
        int currentFrame = 0;
        void update (double delta) {
            runTime+=delta;
            move(velocity);
            if (runTime*playSpeed>animation->duration.asMilliseconds()) {
                runTime = 0;
                currentFrame++;
                if (currentFrame>=animation->frames) {
                    loops++;
                    currentFrame = 0;
                    if (!m_looped)
                        m_destroy = true;
                }
                setTextureRect(sf::IntRect(currentFrame*animation->width, 0, animation->width, animation->height));
            }
        }
        void setAnimation(Anim* a) {
            animation = a;
            currentFrame = 0;
            runTime = 0;
            setTexture(*(animation->spriteSheet));
            setTextureRect(sf::IntRect(0, 0, animation->width, animation->height));
        }
        void setVelocity(sf::Vector2f vel) {
            velocity = vel;
        }
        void setVelocity(float vx, float vy) {
            velocity = sf::Vector2f(vx, vy);
        }
        void setLooped (bool t) {
            m_looped = t;
        }
        int getLoops () {
            return loops;
        }
        void setPlaySpeed (float speed) {
            playSpeed = speed;
        }
        void centerOrigin() {
            setOrigin(getTextureRect().width/2, getTextureRect().height/2);
        }
    protected:
        bool m_destroy = false;
        bool m_looped = true;
        Anim* animation;
        double runTime = 0;
        int loops = 0;
        sf::Vector2f velocity = sf::Vector2f(0, 0);
        float playSpeed = 1;
    };
}


#endif // ANIMATED_SPRITE_H
