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

    }

    void onSceneActivate(){
        vao = new chaos::VertexArray(3, 0, 0, 0, &cube_Pos);
        sh1 = new chaos::ShaderProgram({std::make_pair("chaos/files/shaders/shader2.vs", GL_VERTEX_SHADER),
                    std::make_pair("chaos/files/shaders/shader2.fs", GL_FRAGMENT_SHADER)});
        transformCube = new chaos::Transform();

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
            transformCube->moveX(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            transformCube->moveX(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            transformCube->moveY(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            transformCube->moveY(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Q))
            transformCube->rotateY(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            transformCube->rotateY(-0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
            transformCube->rotateX(0.001*deltaTime);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::F))
            transformCube->rotateX(-0.001*deltaTime);

        sh1->run();
        sh1->setUniform("uniColor", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
        sh1->setUniform("mx",transformCube->getGlobalTransformMatrix());
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        vao->unbind();
        glUseProgram(0);
    }
    void deliverEvent(sf::Event&){}

protected:
    chaos::VertexArray* vao = nullptr;
    chaos::ShaderProgram* sh1 = nullptr;
    chaos::Transform* transformCube = nullptr;

};

#endif // ARRRR_HPP

