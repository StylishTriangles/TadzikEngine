#ifndef ARRRR_HPP
#define ARRRR_HPP

 #define GLEW_STATIC
#include "../GL/glew.h"

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"

#include "../chaos/VertexArray.hpp"
#include "../chaos/ShaderProgram.hpp"
#include "../chaos/Transform.hpp"
#include "../chaos/Camera.hpp"
#include "../chaos/primitives.hpp"
#include "../chaos/Texture.hpp"
#include "../chaos/Model.hpp"
#include "../chaos/MeshPrefab.hpp"
#include "../chaos/ResourceManager.hpp"
#include "../chaos/Water.hpp"

class ARRRR: public Scene{
public:
    ARRRR(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {
    }

    ~ARRRR(){
        delete cubeTest;
        delete water;
        delete camera;
        delete renderer;
        delete shipModel;
        delete sailsModel;

        delete rscManager; //tego ziomeczka na koncu trzeba zniszczyc, to final boss jest
    }

    void onSceneLoadToMemory(){
        renderer = new chaos::Renderer();
        rscManager = new chaos::ResourceManager();
        shipPrefab = rscManager->loadResource<chaos::MeshPrefab>("files/models3d/GhostShip_ship.obj", "ghost_ship");
        sailsPrefab = rscManager->loadResource<chaos::MeshPrefab>("files/models3d/GhostShip_sails.obj", "ghost_ship_sails");
        shipTexture = rscManager->loadResource<chaos::Texture>("files/textures/arrrr/Ship_diff.jpg", "ghost_ship_texture");
        sailsTexture = rscManager->loadResource<chaos::Texture>("files/textures/arrrr/Sails_diff.jpg", "ghost_ship_sails_texture");
        renderer->addMeshVAO(shipPrefab);
        renderer->addMeshVAO(sailsPrefab);
    }

    void onSceneActivate(){
        cubeTest = new chaos::Cube(renderer);
        cubeTest->setScale(0.1f, 0.1f, 0.1f);
        cubeTest->translate(-5,-1,5);
        cubeTest->setColor(glm::vec4(1.0,0.0,0.0,0.5));
        water = new chaos::Water(renderer,60,60,0.2,0.2);
        water->setColor(0.0,0.0,1.0,0.5);
        water->rotateX(M_PI/2.0);
        water->translate(-30,-1,30);
        camera = new chaos::Camera(renderer, chaos::PERSPECTIVE, glm::perspective(glm::radians(45.0f), (GLfloat)window->getSize().x/window->getSize().y, 0.1f, 100.0f));;
        camera->moveZ(5.f);
        camera->yaw -= 60;

        shipModel = new chaos::Model(renderer, shipPrefab);
        sailsModel = new chaos::Model(renderer, sailsPrefab);
        shipModel->setColor(1.0,1.0,1.0,0.85);
        sailsModel->setColor(1.0,1.0,1.0,0.6);
        shipModel->setScale(0.1,0.1,0.1);
        shipModel->moveX(2.0);
        sailsModel->setParent(shipModel);
        shipModel->translate(-3,-1,-3);
        shipModel->rotateY(M_PI*1.2);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        window->setMouseCursorVisible(false);
    }

    void onSceneDeactivate(){
        window->popGLStates();
    }

    void draw(double deltaTime){
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cubeTest->moveX(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cubeTest->moveX(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cubeTest->moveY(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cubeTest->moveY(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Z))
            cubeTest->moveZ(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            cubeTest->moveZ(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            cubeTest->rotateY(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            cubeTest->rotateY(-0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            cubeTest->rotateX(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
            cubeTest->rotateX(-0.003*deltaTime);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::H))
            camera->moveX(-0.002*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            camera->moveX(0.002*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::U))
            camera->moveZ(-0.002*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            camera->moveZ(0.002*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::L))
            camera->moveY(-0.002*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::O))
            camera->moveY(0.002*deltaTime);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num1))
            water->renderType = GL_LINES;
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Num2))
            water->renderType = GL_TRIANGLES;

        using namespace std::chrono;
        milliseconds ms = duration_cast< milliseconds >(
        system_clock::now().time_since_epoch());
        GLfloat t = ((ms.count()-renderer->startTime))/1000.0;
        cubeTest->setY(-1.2+  0.2*sin(sqrt(cubeTest->getX()*cubeTest->getX()+cubeTest->getZ()*cubeTest->getZ())+t));
        shipModel->setY(-1.2+ 0.2*sin(sqrt(shipModel->getX()*shipModel->getX()+shipModel->getZ()*shipModel->getZ())+t));

        GLfloat rad = shipModel->getRotY()-M_PI/3.0;
        GLfloat sp = 0.0005;
        shipModel->translate(cos(rad)*deltaTime*sp, 0, sin(rad)*deltaTime*sp);

        renderer->setCamCombined(camera->getProjectionMatrix()*camera->getViewMatrix());
        window->clear(sf::Color(0.05,0.05,0.05,1.0));
        water->draw();
        camera->update();
        cubeTest->draw();
        shipTexture->bind();
        shipModel->draw();
        sailsTexture->bind();
        sailsModel->draw();
    }
    void deliverEvent(sf::Event& event){
        if (event.type == sf::Event::MouseMoved) {
            static glm::vec2 lastMousePos;
            glm::vec2 mousePos(event.mouseMove.x, event.mouseMove.y);

            glm::vec2 deltaPos(mousePos - lastMousePos);
            const float mouseSensitivity = 1.0f;
            deltaPos *= mouseSensitivity;
            deltaPos.y *= -1;
            camera->processMouse(deltaPos.x, deltaPos.y);

            auto windowSize = window->getSize();

            uint32_t maxMousePosRadius = glm::min(windowSize.x, windowSize.y) / 3;
            glm::vec2 windowCenter(windowSize.x / 2, windowSize.y / 2);

            if (glm::length(mousePos - windowCenter) > maxMousePosRadius) {
                sf::Mouse::setPosition(sf::Vector2i((int)windowCenter.x, (int)windowCenter.y), *window);
                lastMousePos = windowCenter;
            }
            else {
                lastMousePos = mousePos;
            }
        }
    }

protected:
    chaos::Renderer* renderer = nullptr;
    chaos::ResourceManager* rscManager = nullptr;
    chaos::Transform* transformCube = nullptr;
    chaos::Camera* camera=nullptr;
    chaos::Water* water;
    chaos::Cube* cubeTest;
    chaos::MeshPrefab* shipPrefab;
    chaos::MeshPrefab* sailsPrefab;
    chaos::Model* shipModel;
    chaos::Model* sailsModel;
    chaos::Texture* shipTexture;
    chaos::Texture* sailsTexture;

    int circR=0;
};

#endif // ARRRR_HPP

