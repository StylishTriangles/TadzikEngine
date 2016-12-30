#ifndef ARRRR_HPP
#define ARRRR_HPP

//#include "../glad/glad.h"
#include "../GL/glew.h"

#include "../include/Scene.hpp"
#include "../include/AnimatedSprite.hpp"
#include "../include/Utils.hpp"

#include "../chaos/VertexArray.hpp"
#include "../chaos/ShaderProgram.hpp"

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
    }

    void onSceneLoadToMemory(){
        vao = new chaos::VertexArray(3, 0, 2, 0, &cube_Pos);
        sh1 = new chaos::ShaderProgram({std::make_pair("chaos/files/shaders/shader2.vs", GL_VERTEX_SHADER),
                    std::make_pair("chaos/files/shaders/shader2.fs", GL_FRAGMENT_SHADER)});
    }

    void onSceneActivate(){

    }

    void onSceneDeactivate(){
        window->popGLStates();
    }

    void draw(double deltaTime){
        /*sh1->run();
        sh1->setUniform("uniColor", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
        sh1->setUniform("mx",glm::mat4());
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        vao->unbind();
        glUseProgram(0);*/
        sh1->run();
        sh1->setUniform("uniColor", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
        sh1->setUniform("mx",glm::mat4());
        vao->bind();
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        vao->unbind();
        glUseProgram(0);
    }
    void deliverEvent(sf::Event&){}

protected:
    chaos::VertexArray* vao = nullptr;
    chaos::ShaderProgram* sh1 = nullptr;

};

#endif // ARRRR_HPP

