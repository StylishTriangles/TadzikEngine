#ifndef ISAYPARTY_HPP
#define ISAYPARTY_HPP

#include <SFML/Audio.hpp>

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"

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

        vecTracks.resize(1);
        vecTracks[0] = Track("Da_Funk", "Daft Punk", 112);

        vecTextures.resize(17);
        for(int i = 0; i < vecTextures.size(); i++){
            vecTextures[i].loadFromFile("files/textures/isaydisco/"+Utils::stringify(i)+".png");
            if(i != 16)
            animDanceAll.addFrame(AnimationFrame(&vecTextures[i], 120*2));
        }

        texIdle.loadFromFile("files/textures/isaydisco/idle.png");

        animHands.addFrame(AnimationFrame(&vecTextures[2], 120*2));
        animHands.addFrame(AnimationFrame(&vecTextures[8], 120*2));
        animHands.addFrame(AnimationFrame(&vecTextures[4], 120*2));
        animHands.addFrame(AnimationFrame(&vecTextures[6], 120*2));

        animHips.addFrame(AnimationFrame(&vecTextures[9], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[10], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[11], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[12], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[9], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[10], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[11], 120*2));
        animHips.addFrame(AnimationFrame(&vecTextures[12], 120*2));

        animWindmill.addFrame(AnimationFrame(&vecTextures[0], 120*2));
        animWindmill.addFrame(AnimationFrame(&vecTextures[8], 120*2));
        animWindmill.addFrame(AnimationFrame(&vecTextures[0], 120*2));
        animWindmill.addFrame(AnimationFrame(&texIdle, 120*4));
        animWindmill.addFrame(AnimationFrame(&vecTextures[6], 120*2));
        animWindmill.addFrame(AnimationFrame(&vecTextures[15], 120*2));
        animWindmill.addFrame(AnimationFrame(&vecTextures[6], 120*2));

        vecTexturesDaftDJ.resize(8);
            for(int i = 0; i < vecTexturesDaftDJ.size(); i++){
                vecTexturesDaftDJ[i].loadFromFile("files/textures/isaydisco/DaftPunk1_DJ"+Utils::stringify(i)+".png");
            }

            vecTexturesDaftGuitar.resize(8);
            for(int i = 0; i < 4; i++){
                vecTexturesDaftGuitar[i].loadFromFile("files/textures/isaydisco/DaftPunk1_GF"+Utils::stringify(i)+".png");
            }
            for(int i = 0; i < 4; i++){
                vecTexturesDaftGuitar[i+4].loadFromFile("files/textures/isaydisco/DaftPunk1_GG"+Utils::stringify(i)+".png");
        }

        for(int i = 0; i < 4; i++){
            animDJ1.addFrame(AnimationFrame(&vecTexturesDaftDJ[i], 112*2));
            animGuitar1.addFrame(AnimationFrame(&vecTexturesDaftGuitar[i], 112*2));
            animDJ2.addFrame(AnimationFrame(&vecTexturesDaftDJ[i+4], 112*2));
            animGuitar2.addFrame(AnimationFrame(&vecTexturesDaftGuitar[i+4], 112*2));
        }

        vecSteps.resize(4);
        vecSteps[0] = DanceStep(&animHands, {0,2,4,6}, {HAND_LEFT, HAND_RIGHT, HAND_LEFT, HAND_RIGHT});
        vecSteps[1] = DanceStep(&animHips,  {0,2,4,6}, {HIPS_LEFT, HIPS_RIGHT, HIPS_LEFT, HIPS_RIGHT});
        vecSteps[2] = DanceStep(&animWindmill,  {0,2,3,3,4,6}, {HAND_LEFT,HAND_LEFT,LEG_LEFT,LEG_RIGHT,HAND_RIGHT,HAND_RIGHT});
        //vecSteps[3] = DanceStep(&animDanceAll, {3, 3}, {HAND_LEFT, HAND_RIGHT});
        vecSteps[3] = DanceStep(&animDanceAll, {0, 4, 6, 8, 13, 15}, {HAND_LEFT,LEG_RIGHT,HAND_RIGHT,HAND_LEFT,LEG_RIGHT,HAND_RIGHT});


        dbgText.move(100, 0);       //martwe pixele ftw
        dbgText.setFont(Common::Font::Comic_Sans);

        vecDiscoColors.push_back(sf::Color(255, 255, 255));
        vecDiscoColors.push_back(sf::Color(0, 255, 0));
        vecDiscoColors.push_back(sf::Color(255, 255, 0));
        vecDiscoColors.push_back(sf::Color(0 ,255,255));
        //vecDiscoColors.push_back(sf::Color(238,190,67));

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

        texBkgrnd.loadFromFile("files/textures/isaydisco/background.png");
        background.setTexture(texBkgrnd);
        background.setScale((float)window->getSize().x/(float)texBkgrnd.getSize().x, (float)window->getSize().y/(float)texBkgrnd.getSize().y);
    }

    virtual void onSceneActivate(){
        window->setTitle("Tadzik ~ISAydIScOyouSAYparTyDisOdisCOPaRtYparTy");

        //actSprite.setAnimation(&animDanceAll);
        setDanceStep(&vecSteps[rand()%vecSteps.size()]);
        actSprite.sprite.setScale(4.f, 4.f);
        actSprite.sprite.setPosition(window->getSize().x/2-actSprite.sprite.getGlobalBounds().width/2, window->getSize().y/2);

        spriteDaft1.setAnimation(&animDJ1);
        spriteDaft2.setAnimation(&animGuitar1);
        spriteDaft1.sprite.setScale(4.f, 4.f);
        spriteDaft2.sprite.setScale(4.f, 4.f);
        spriteDaft1.sprite.setPosition(actSprite.sprite.getPosition());
        spriteDaft2.sprite.setPosition(actSprite.sprite.getPosition());
        spriteDaft1.sprite.move(-200, 0);
        spriteDaft2.sprite.move(-300, 0);

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

    virtual void draw(sf::Time deltaTime){
        double dT = deltaTime.asMilliseconds();
        tmpTime += dT;
        for(KeyText& kt: vecKeyTexts){
            kt.update(dT);
        }
        actSprite.update(dT);
        spriteDaft1.update(dT);
        spriteDaft2.update(dT);
        if(actSprite.getReplays() != 0){
            setDanceStep(&vecSteps[rand()%vecSteps.size()]);
        }
        if(spriteDaft1.getReplays() != 0){
            setDanceStepDP(1);
        }
        if(spriteDaft2.getReplays() != 0){
            setDanceStepDP(2);
        }
        for(StepPose& sp: actStep->vecPoses){
            if(actSprite.getFrame() == sp.tempoPosition && !sp.used){
                addKey(sp.limb);
                sp.used=true;
            }
        }

        for(int i = vecText.size()-1; i>=0; i--){
            vecText[i].time -= dT;
            if(vecText[i].time < 0.0){
                vecText.erase(vecText.begin()+i);
            }
        }
        for(int i = vecKeyTexts.size()-1; i >= 0; i--){
            if(vecKeyTexts[i].timeCtr > vecKeyTexts[i].duration){
                vecKeyTexts.erase(vecKeyTexts.begin()+i);
                displayText(vecDiscoReactionsBad[rand()%vecDiscoReactionsBad.size()], 0);
                score--;
            }
        }

        if(actMusic.getStatus() == sf::SoundSource::Status::Stopped){
            loadNewTrack();
        }

        window->clear(sf::Color::Black);
        window->draw(background);
        window->draw(spriteDaft1.sprite);
        window->draw(spriteDaft2.sprite);

        for(auto txt: vecText){
            window->draw(txt.text);
        }
        window->draw(actSprite.sprite);

        sf::RectangleShape rectangle(sf::Vector2f(2*actSprite.sprite.getGlobalBounds().width, 20.0f));
        rectangle.setPosition(actSprite.sprite.getPosition());
        rectangle.move(-actSprite.sprite.getGlobalBounds().width/2, -radius-20);
        rectangle.setFillColor(sf::Color(40.0, 150.0, 40.0));
        window->draw(rectangle);
        rectangle.setFillColor(sf::Color::Green);
        rectangle.setSize(sf::Vector2f((score/100.0)*(2*actSprite.sprite.getGlobalBounds().width), 20.0f));
        window->draw(rectangle);

        for(KeyText ks: vecKeyTexts)
            window->draw(ks);

        dbgText.setString(Utils::stringify(actSprite.getFrame())+"/"+Utils::stringify(actSprite.framesSize()));
        window->draw(dbgText);

        if(score >= 100){
            sceneManager->callMeBaby();
            actMusic.stop();
        }
    }

    void setDanceStep(DanceStep* d){
        if(actStep != nullptr)
            actStep->clearUsedPoses();
        actStep = d;
        actSprite.setAnimation(d->animation);
    }

    void setDanceStepDP(int idx){
        if(idx==1){
            Animation* a = &animDJ1;
            if(Utils::randFloat(0.0, 1.0) >= 0.5)
                a = &animDJ2;
            spriteDaft1.setAnimation(a);
        }
        if(idx==2){
            Animation* a = &animGuitar1;
            if(Utils::randFloat(0.0, 1.0) >= 0.5)
                a = &animGuitar2;
            spriteDaft2.setAnimation(a);
        }
    }

    virtual bool onConsoleUpdate(std::vector<std::string> args){
        if(args.size() == 2){
            if(args[0] == "addscore" || args[0] == "ads"){
                score += atoi(args[1].c_str());
                return true;
            }
        }
        return false;
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
            score++;
        }
        else{
            displayText(vecDiscoReactionsBad[rand()%vecDiscoReactionsBad.size()], 0);
            score--;
        }
    }

    void displayText(std::string str, int type){
        sf::Text txt;
        txt.setString(str);
        txt.setFont(Common::Font::Comic_Sans);
        txt.setPosition(window->getSize().x*Utils::randFloat(0.1, 0.9), window->getSize().y*Utils::randFloat(0.4, 0.9));
        txt.setRotation(360.0 * Utils::randFloat(0.0, 1.0));
        double scale = Utils::randFloat(0.5, 0.75);
        txt.setScale(scale, scale);
        vecText.push_back(TextCraft(txt, Utils::randFloat(500.0, 1000.0), type));
    }

    void loadNewTrack(){
        actTrack = &vecTracks[rand()%vecTracks.size()];
        bpm = actTrack->bpm;
        actMusic.openFromFile("files/audio/isaydisco/"+actTrack->name+".ogg");
        actMusic.play();
    }

    void addKey(int limb){
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
        KeyText ks(txt, Common::Font::Comic_Sans, 1000, limb, tmpTime);
        ks.setColor(vecDiscoColors[rand()%vecDiscoColors.size()]);
        ks.setPosition(pos);
        vecKeyTexts.push_back(ks);
    }

protected:
    int tempo = 8;
    int score=50;           //score in [0,100]
    double bpm=20;          //uderzenia na minute
    double tmpTime=0.0;     //w sekundach
    int radius = 120;       //promien klawiszy wokol tadzika

    std::vector<DanceStep> vecSteps;
    DanceStep* actStep = nullptr;

    sf::Texture texArrowLeft, texArrowUp, texArrowRight, texArrowDown;
    std::vector<sf::Sprite> vecArrows;
    std::vector<TextCraft> vecText;
    std::vector<Track> vecTracks;
    std::vector<sf::Texture> vecTextures;
    sf::Texture texIdle;
    std::vector<KeyText> vecKeyTexts;
    std::vector<sf::Color> vecDiscoColors;
    std::vector<std::string> vecDiscoReactionsGood;
    std::vector<std::string> vecDiscoReactionsBad;
    std::vector<sf::Texture> vecTexturesDaftDJ;
    std::vector<sf::Texture> vecTexturesDaftGuitar;

    Track* actTrack;

    sf::Music actMusic;
    sf::Text dbgText;
    Animation animDanceAll;
    Animation animHands;
    Animation animHips;
    Animation animWindmill;

    Animation animGuitar1;
    Animation animGuitar2;
    Animation animDJ1;
    Animation animDJ2;

    AnimatedSprite actSprite;
    AnimatedSprite spriteDaft1;
    AnimatedSprite spriteDaft2;


    sf::Texture texBkgrnd;
    sf::Sprite background;

};

#endif // ISAYPARTY_HPP
