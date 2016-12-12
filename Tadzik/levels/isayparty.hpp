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

enum Limb {
    HAND_LEFT   = 135,
    HAND_RIGHT  = 45,
    LEG_LEFT    = 225,
    LEG_RIGHT   = 315,
    HEAD        = 90,
    HIPS_LEFT   = 180,
    HIPS_RIGHT  = 0
};

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

struct KeyText: public sf::Text{
public:
    KeyText(const std::string& str, const sf::Font& f, float timeDuration, int l, float t)
    :sf::Text(str, f), duration(timeDuration), limb(l), timeCreated(t)
    {
        sf::FloatRect textRect =getLocalBounds();
        setOrigin(textRect.left + textRect.width/2.0f,
               textRect.top  + textRect.height/2.0f);
    }

    float timeCtr=0;
    float duration=0;
    float scale=1.0;
    float timeCreated=0.0;
    float scaleSpeed=0.01;
    int limb;
    void update(float dt){
        timeCtr += dt;
        scale+=scaleSpeed;
        if(scale>=1.2){
            scale=1.2;
            scaleSpeed*=-1;
        }
        if(scale <= 0.8){
            scale=0.8;
            scaleSpeed*=-1;
        }
        setRotation((scale-1.0)*30);
        setScale(scale, scale);
        setColor(sf::Color(getColor().r, getColor().g, getColor().b, (sf::Uint8)(255-(timeCtr/duration)*180.0)));
    }
};

struct StepPose{
    StepPose(int tempo, int l)
    :tempoPosition(tempo), limb(l)
    {}
    int tempoPosition;
    int limb;
    bool used=false;
};

struct DanceStep{
    DanceStep(){}
    DanceStep(Animation* anim, const std::initializer_list<int>& tempoList, const std::initializer_list<int>& limbList)
    :animation(anim)
    {
        std::vector<int> vTempo, vKey;
        for(int tmp: tempoList)vTempo.push_back(tmp);
        for(int tmp: limbList)vKey.push_back(tmp);
        vecPoses.clear();
        for(int i = 0; i < vKey.size(); i++){
            vecPoses.push_back(StepPose(vTempo[i], vKey[i]));
        }
    }

    void clearUsedPoses(){
        for(StepPose& p: vecPoses)
            p.used=false;
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

        vecSteps.resize(2);
        vecSteps[0] = DanceStep(&animDance1, {3, 3}, {HAND_LEFT, HAND_RIGHT});
        vecSteps[1] = DanceStep(&animDance1, {1, 4, 10, 14}, {LEG_LEFT, LEG_LEFT, LEG_RIGHT, LEG_RIGHT});

        vecTracks.resize(2);
        vecTracks[0] = Track("Da_Funk", "Daft Punk", 112);
        vecTracks[1] = Track("nieeee", "Gall Anonim", 42);

        vecTextures.resize(17);
        for(int i = 0; i < vecTextures.size(); i++){
            vecTextures[i].loadFromFile("files/textures/isaydisco/"+Utils::stringify(i)+".png");
            animDance1.addFrame(AnimationFrame(&vecTextures[i], 112*2));
        }
        dbgText.move(100, 0);       //martwe pixele ftw
        dbgText.setFont(font);

        vecDiscoColors.push_back(sf::Color(64, 0, 77));
        vecDiscoColors.push_back(sf::Color(104, 0, 95));
        vecDiscoColors.push_back(sf::Color(190,75,121));
        vecDiscoColors.push_back(sf::Color(252,162,101));
        vecDiscoColors.push_back(sf::Color(238,190,67));

        vecDiscoReactionsGood.push_back("WOAH!");
        vecDiscoReactionsGood.push_back("Superb!");
        vecDiscoReactionsGood.push_back("Smooth!");
        vecDiscoReactionsGood.push_back("Fantastic!");
        vecDiscoReactionsGood.push_back("COMBO x42");

        vecDiscoReactionsBad.push_back("Go home, you are drunk");
        vecDiscoReactionsBad.push_back("Boooooo");
        vecDiscoReactionsBad.push_back("Awful");
        vecDiscoReactionsBad.push_back("Absolutely Disgusting");
        vecDiscoReactionsBad.push_back("Disgusting");
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~ISAydIScOyouSAYparTyDisOdisCOPaRtYparTy");

        actSprite.setAnimation(&animDance1);
        actSprite.sprite.setScale(4.f, 4.f);
        actSprite.sprite.setPosition(window->getSize().x/2-actSprite.sprite.getGlobalBounds().width/2, window->getSize().y/2);

        setDanceStep(&vecSteps[0]);
        loadNewTrack();
        vecKeyTexts.clear();
    }

    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::KeyReleased) {
            if (event.key.code == sf::Keyboard::W)
                keyPressed(HEAD);
            else if (event.key.code == sf::Keyboard::A)
                keyPressed(HAND_LEFT);
            else if (event.key.code == sf::Keyboard::D)
                keyPressed(HAND_RIGHT);
            else if (event.key.code == sf::Keyboard::H)
                keyPressed(HIPS_LEFT);
            else if (event.key.code == sf::Keyboard::J)
                keyPressed(HIPS_RIGHT);
            else if (event.key.code == sf::Keyboard::B)
                keyPressed(LEG_LEFT);
            else if (event.key.code == sf::Keyboard::M)
                keyPressed(LEG_RIGHT);
            else
                keyPressed(0xdeadc0de);
        }
    }

    virtual void draw(double deltaTime){
        tmpTime += deltaTime;
        for(KeyText& kt: vecKeyTexts){
            kt.update(deltaTime);
        }
        actSprite.update(deltaTime);
        if(actSprite.getReplays() != 0){
            setDanceStep(&vecSteps[rand()%vecSteps.size()]);
        }
        for(StepPose& sp: actStep->vecPoses){
            if(actSprite.getFrame() == sp.tempoPosition && !sp.used){
                addKey(sp.limb);
                sp.used=true;
            }
        }

        for(int i = vecText.size()-1; i>=0; i--){
            vecText[i].time -= deltaTime;
            if(vecText[i].time < 0.0){
                vecText.erase(vecText.begin()+i);
            }
        }
        for(int i = vecKeyTexts.size()-1; i >= 0; i--){
            if(vecKeyTexts[i].timeCtr > vecKeyTexts[i].duration){
                vecKeyTexts.erase(vecKeyTexts.begin()+i);
                displayText(vecDiscoReactionsBad[rand()%vecDiscoReactionsBad.size()], 0);
            }
        }

        if(actMusic.getStatus() == sf::SoundSource::Status::Stopped){
            loadNewTrack();
        }

        window->clear(sf::Color::Black);
        for(auto txt: vecText){
            window->draw(txt.text);
        }
        window->draw(actSprite.sprite);
        for(KeyText ks: vecKeyTexts)
            window->draw(ks);

        dbgText.setString(Utils::stringify(actSprite.getFrame())+"/"+Utils::stringify(actSprite.framesSize()));
        window->draw(dbgText);
    }

    void setDanceStep(DanceStep* d){
        if(actStep != nullptr)
            actStep->clearUsedPoses();
        actStep = d;
        actSprite.setAnimation(d->animation);
    }

    void keyPressed(int k){
        int miniIdx=-1;
        float miniTime=1000000000.0;
        for(int i = 0; i < vecKeyTexts.size(); i++){
            if(vecKeyTexts[i].limb == k){
                if(miniTime > vecKeyTexts[i].timeCreated){
                    miniIdx = i;
                    miniTime = vecKeyTexts[i].timeCreated;
                }
            }
        }
        if(miniIdx != -1){
            displayText(vecDiscoReactionsGood[rand()%vecDiscoReactionsGood.size()], 0);
            vecKeyTexts.erase(vecKeyTexts.begin()+miniIdx);
        }
        else{
            displayText(vecDiscoReactionsBad[rand()%vecDiscoReactionsBad.size()], 0);
        }
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
        //actMusic.openFromFile("files/audio/isaydisco/"+actTrack->name+".ogg");
        //actMusic.play();
    }

    void addKey(int limb){
        int radius = 120;
        float radian = ((float)limb/180.0) * 3.1415f;
        sf::Vector2f pos = sf::Vector2f(cos(radian)*radius, -sin(radian)*radius);
        pos += actSprite.sprite.getPosition();
        pos += sf::Vector2f(actSprite.sprite.getGlobalBounds().width/2.f, actSprite.sprite.getGlobalBounds().height/2.f);
        std::string txt="W";
        if(limb == HEAD)
            txt = "W";
        if(limb == HAND_LEFT)
            txt = "A";
        if(limb == HAND_RIGHT)
            txt = "D";
        if(limb == HIPS_LEFT)
            txt = "H";
        if(limb == HIPS_RIGHT)
            txt = "J";
        if(limb == LEG_LEFT)
            txt = "B";
        if(limb == LEG_RIGHT)
            txt = "M";
        KeyText ks(txt, font, 1000, limb, tmpTime);
        ks.setColor(vecDiscoColors[rand()%vecDiscoColors.size()]);
        ks.setPosition(pos);
        vecKeyTexts.push_back(ks);
    }

protected:
    int tempo = 8;
    int score=0;
    double bpm=20;          //uderzenia na minute
    double tmpTime=0.0;     //w sekundach
    sf::Font font;

    std::vector<DanceStep> vecSteps;
    DanceStep* actStep = nullptr;

    sf::Texture texArrowLeft, texArrowUp, texArrowRight, texArrowDown;
    std::vector<sf::Sprite> vecArrows;
    std::vector<TextCraft> vecText;
    std::vector<Track> vecTracks;
    std::vector<sf::Texture> vecTextures;
    std::vector<KeyText> vecKeyTexts;
    std::vector<sf::Color> vecDiscoColors;
    std::vector<std::string> vecDiscoReactionsGood;
    std::vector<std::string> vecDiscoReactionsBad;

    Track* actTrack;

    sf::Music actMusic;
    sf::Text dbgText;
    Animation animDance1;
    AnimatedSprite actSprite;

};

#endif // ISAYPARTY_HPP





