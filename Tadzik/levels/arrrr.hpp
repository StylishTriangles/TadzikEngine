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

std::vector<GLfloat> cube_Pos = {
            -1.f, -1.f, -1.f,
             1.f, -1.f, -1.f,
             1.f,  1.f, -1.f,
             1.f,  1.f, -1.f,
            -1.f,  1.f, -1.f,
            -1.f, -1.f, -1.f,

            -1.f, -1.f,  1.f,
             1.f, -1.f,  1.f,
             1.f,  1.f,  1.f,
             1.f,  1.f,  1.f,
            -1.f,  1.f,  1.f,
            -1.f, -1.f,  1.f,

            -1.f,  1.f,  1.f,
            -1.f,  1.f, -1.f,
            -1.f, -1.f, -1.f,
            -1.f, -1.f, -1.f,
            -1.f, -1.f,  1.f,
            -1.f,  1.f,  1.f,

             1.f,  1.f,  1.f,
             1.f,  1.f, -1.f,
             1.f, -1.f, -1.f,
             1.f, -1.f, -1.f,
             1.f, -1.f,  1.f,
             1.f,  1.f,  1.f,

            -1.f, -1.f, -1.f,
             1.f, -1.f, -1.f,
             1.f, -1.f,  1.f,
             1.f, -1.f,  1.f,
            -1.f, -1.f,  1.f,
            -1.f, -1.f, -1.f,

            -1.f,  1.f, -1.f,
             1.f,  1.f, -1.f,
             1.f,  1.f,  1.f,
             1.f,  1.f,  1.f,
            -1.f,  1.f,  1.f,
            -1.f,  1.f, -1.f
        };

class ARRRR: public Scene{
public:
    ARRRR(std::string _name, SceneManager* mgr, sf::RenderWindow* w)
    :Scene(_name, mgr, w)
    {
    }

    ~ARRRR(){
        delete vao;
        delete sh1;
        delete transformCube;
    }

    void onSceneLoadToMemory(){
        renderer = new chaos::Renderer();
    }

    void onSceneActivate(){
        vao = new chaos::VertexArray(3, 0, 0, 0, &cube_Pos);
        sh1 = new chaos::ShaderProgram({std::make_pair("files/shaders/shader2.vs", GL_VERTEX_SHADER),
                    std::make_pair("files/shaders/shader2.fs", GL_FRAGMENT_SHADER)});
        transformCube = new chaos::Transform();
        transformCube->setScale(0.25f, 0.25f, 0.25f);
        circle = new chaos::Circle(renderer);
        circle->setScale(0.3f,0.5f,0.3f);
        camera = new chaos::Camera(renderer, chaos::PERSPECTIVE, glm::perspective(glm::radians(45.0f), (GLfloat)window->getSize().x/window->getSize().y, 0.1f, 100.0f));;
        camera->moveZ(5.f);

        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    }

    void onSceneDeactivate(){
        window->popGLStates();
    }

    void draw(double deltaTime){
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            transformCube->moveX(-0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            transformCube->moveX(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            transformCube->moveY(-0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            transformCube->moveY(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            transformCube->rotateY(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            transformCube->rotateY(-0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            transformCube->rotateX(0.003*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
            transformCube->rotateX(-0.003*deltaTime);

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::H))
            camera->moveX(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::K))
            camera->moveX(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::U))
            camera->moveY(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::J))
            camera->moveY(0.001*deltaTime);

        renderer->setCamCombined(camera->getProjectionMatrix()*camera->getViewMatrix());

        sh1->run();
        sh1->setUniform("uniColor", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
        sh1->setUniform("mx",renderer->getCamCombined()*transformCube->getGlobalTransformMatrix());
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        vao->unbind();
        glUseProgram(0);

        circle->setColor(fabs(transformCube->getX())/3.f,1.0f,0.2f,0.6f);
        circle->draw();

        camera->update();
    }
    void deliverEvent(sf::Event&){}

protected:
    chaos::Renderer* renderer = nullptr;
    chaos::VertexArray* vao = nullptr;
    chaos::ShaderProgram* sh1 = nullptr;
    chaos::Transform* transformCube = nullptr;
    chaos::Camera* camera=nullptr;
    chaos::Circle* circle;
    int circR=0;

};

#endif // ARRRR_HPP

