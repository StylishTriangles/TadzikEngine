#ifndef ISAYPARTY_HPP
#define ISAYPARTY_HPP

#include <SFML/Audio.hpp>

#include "Scene.hpp"
#include "AnimatedSprite.hpp"
#include "Utils.hpp"

#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>

struct Track{
    Track(){};
    Track(std::string n, std::string a, int b)
    :name(n), author(a), bpm(b)
    {}
    std::string name;
    std::string author;
    int bpm;
};

//minecraft's style text
struct TextCraft{
    TextCraft(sf::Text t, float ti, int ty)
    :text(t), time(ti), type(ty)
    {}

    double time;
    sf::Text text;
    int type;
};

struct StepPose{
    StepPose(int tempo, int k)
    :tempoPosition(tempo), key(k)
    {}
    int tempoPosition;
    int key;
};

struct DanceStep{
    DanceStep(){}
    DanceStep(Animation* anim, const std::initializer_list<int>& tempoList, const std::initializer_list<int>& keyList)
    :animation(anim)
    {
        std::vector<int> vTempo, vKey;
        for(int tmp: tempoList)vTempo.push_back(tmp);
        for(int tmp: keyList)vKey.push_back(tmp);
        vecPoses.clear();
        for(int i = 0; i < vKey.size(); i++){
            vecPoses.push_back(StepPose(vTempo[i], vKey[i]));
        }
    }
    Animation* animation;
    std::vector<StepPose> vecPoses;
};

class ISAYPARTY: public Scene{
public:
    ISAYPARTY(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {}

    virtual void onSceneLoadToMemory(){
        texArrowLeft.loadFromFile("files/textures/isaydisco/arrow_left.png");
        texArrowUp.loadFromFile("files/textures/isaydisco/arrow_up.png");
        texArrowRight.loadFromFile("files/textures/isaydisco/arrow_right.png");
        texArrowDown.loadFromFile("files/textures/isaydisco/arrow_down.png");

        if (!font.loadFromFile("files/Carnevalee_Freakshow.ttf")){
            std::cout << "cannot load font\n";
        }

        double dist = window->getSize().x/(double)(tempo+1.0);
        for(int i = 0; i < tempo; i++){
            sf::CircleShape shp(10);
            shp.setFillColor(sf::Color::Blue);
            shp.setPosition(dist+i*dist, 50);
            vCircles.push_back(shp);
        }
        circlePointer = sf::CircleShape(10);
        circlePointer.setFillColor(sf::Color::Red);

        vecSteps.resize(2);
        vecSteps[0] = DanceStep(&animDance1, {3, 6}, {1, 2});
        vecSteps[1] = DanceStep(&animDance1, {1, 2, 5, 8}, {3, 2, 4, 4});

        vecTracks.resize(2);
        vecTracks[0] = Track("Da_Funk", "Daft Punk", 112);
        vecTracks[1] = Track("nieeee", "Gall Anonim", 42);

        vecTextures.resize(17);
        for(int i = 0; i < vecTextures.size(); i++){
            vecTextures[i].loadFromFile("files/textures/isaydisco/"+Utils::stringify(i)+".png");
            animDance1.addFrame(AnimationFrame(&vecTextures[i], 200));
        }
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~ISAydIScOyouSAYparTyDisOdisCOPaRtYparTy");

        actSprite.setAnimation(&animDance1);
        actSprite.sprite.setScale(4.f, 4.f);
        actSprite.sprite.setPosition(window->getSize().x/2-actSprite.sprite.getGlobalBounds().width/2, window->getSize().y/2);

        setDanceStep(&vecSteps[0]);
        loadNewTrack();
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::Left)
                keyPressed(1);
            if (event.key.code == sf::Keyboard::Up)
                keyPressed(2);
            if (event.key.code == sf::Keyboard::Right)
                keyPressed(3);
            if (event.key.code == sf::Keyboard::Down)
                keyPressed(4);
        }
    }

    virtual void draw(double deltaTime){
        tmpTime += deltaTime;
        actSprite.update(deltaTime);
        double percent = (tmpTime/1000.0)/(60.0/bpm);
        if(tmpTime/1000.0 > 60.0/bpm){
            tmpTime=0.0;
            //setDanceStep(&vecSteps[rand()%vecSteps.size()]);
        }

        for(int i = vecText.size()-1; i>=0; i--){
            vecText[i].time -= deltaTime;
            if(vecText[i].time < 0.0){
                vecText.erase(vecText.begin()+i);
            }
        }

        if(actMusic.getStatus() == sf::SoundSource::Status::Stopped){
            loadNewTrack();
        }

        window->clear(sf::Color::Black);
        for(int i = 0; i < vCircles.size(); i++){
            bool isStepPose=false;
            for(int j = 0; j < actStep->vecPoses.size(); j++){
                if(actStep->vecPoses[j].tempoPosition-1 == i){
                    isStepPose=true;
                }
            }
            if(isStepPose)
                vCircles[i].setFillColor(sf::Color::Green);
            else
                vCircles[i].setFillColor(sf::Color(128, 128, 128));

            for(int i = 0; i < vecArrows.size(); i++){
                window->draw(vecArrows[i]);
            }

            window->draw(vCircles[i]);
        }
        circlePointer.setPosition(window->getSize().x*percent, 50);
        for(auto txt: vecText){
            window->draw(txt.text);
        }
        window->draw(circlePointer);
        window->draw(actSprite.sprite);
    }

    void setDanceStep(DanceStep* d){
        actStep = d;
        actSprite.setAnimation(d->animation);
        vecArrows.clear();
        for(auto pose : actStep->vecPoses){
            sf::Sprite spTmp(texArrowLeft);
            if(pose.key == 2)
                spTmp.setTexture(texArrowUp);
            if(pose.key == 3)
                spTmp.setTexture(texArrowRight);
            if(pose.key == 4)
                spTmp.setTexture(texArrowDown);

            double dist = window->getSize().x/(double)(tempo+1.0);
            spTmp.setPosition(dist*pose.tempoPosition, 24);
            spTmp.setScale(0.5, 0.5);
            vecArrows.push_back(spTmp);
        }
    }

    void keyPressed(int k){
        double closest = 1000000000;
        double dist = window->getSize().x/(double)(tempo+1.0);
        double percent = (tmpTime/1000.0)/(60.0/bpm);
        for(auto pose: actStep->vecPoses){
            if(pose.key == k){
                closest = std::min(fabs(dist*pose.tempoPosition - window->getSize().x*percent), closest);
            }
        }
        closest = closest / window->getSize().x;

        if(closest <= 0.01){
            displayText("REKT!!!", 0);
        }
        else if(closest <= 0.05){
            displayText("Fantastic!", 1);
        }
        else if(closest <= 0.1){
            displayText("Not bad", 2);
        }
        else if(closest <= 0.3){
            displayText("Poor af", 3);
        }
        else{
            displayText("Go home, you are drunk!", 4);
        }
        //std::cout << closest << "\n";*/
    }

    void displayText(std::string str, int type){
        sf::Text txt;
        txt.setString(str);
        txt.setFont(font);
        txt.setPosition(window->getSize().x*Utils::randF(0.1, 0.9), window->getSize().y*Utils::randF(0.4, 0.9));
        txt.setRotation(360.0 * Utils::randF(0.0, 1.0));
        double scale = Utils::randF(0.5, 0.75);
        txt.setScale(scale, scale);
        vecText.push_back(TextCraft(txt, Utils::randF(500.0, 1000.0), type));
    }

    void loadNewTrack(){
        actTrack = &vecTracks[rand()%vecTracks.size()];
        bpm = actTrack->bpm;
        actMusic.openFromFile("files/audio/isaydisco/"+actTrack->name+".ogg");
        actMusic.play();
    }

protected:
    int tempo = 8;
    int score=0;
    double bpm=20;          //uderzenia na minute
    double tmpTime=0.0;     //w sekundach
    std::vector<sf::CircleShape> vCircles;
    sf::Font font;
    sf::CircleShape circlePointer;

    std::vector<DanceStep> vecSteps;
    DanceStep* actStep = nullptr;

    sf::Texture texArrowLeft, texArrowUp, texArrowRight, texArrowDown;
    std::vector<sf::Sprite> vecArrows;
    std::vector<TextCraft> vecText;
    std::vector<Track> vecTracks;
    std::vector<sf::Texture> vecTextures;
    Track* actTrack;

    sf::Music actMusic;

    Animation animDance1;
    AnimatedSprite actSprite;

};

#endif // ISAYPARTY_HPP





