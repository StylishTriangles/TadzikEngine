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
    }

    void onSceneLoadToMemory(){
        renderer = new chaos::Renderer();
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

        renderer->setCamCombined(camera->getProjectionMatrix()*camera->getViewMatrix());

        water->draw();
        camera->update();
        cubeTest->draw();
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
                sf::Mouse::setPosition(sf::Vector2i((int)windowCenter.x, (int)windowCenter.y));
                lastMousePos = windowCenter;
            }
            else {
                lastMousePos = mousePos;
            }
        }
    }

protected:
    chaos::Renderer* renderer = nullptr;
    chaos::Transform* transformCube = nullptr;
    chaos::Camera* camera=nullptr;
    chaos::Water* water;
    chaos::Cube* cubeTest;
    int circR=0;
};

#endif // ARRRR_HPP

