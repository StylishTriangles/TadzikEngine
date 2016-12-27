#ifndef ARRRR_HPP
#define ARRRR_HPP

#include "glad/glad.h"

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
        window->pushGLStates();
    }

    void onSceneDeactivate(){
        window->popGLStates();
    }

    void draw(double deltaTime){
        int v1=31337, v2=1337;
        glGetIntegerv(GL_MAJOR_VERSION, &v1);
        glGetIntegerv(GL_MINOR_VERSION, &v2);
        std::cout << "versions: " << v1 << " " << v2 << "\n";

        sh1->run();
        std::cout << "lol1\n";
        sh1->setUniform("uniColor", glm::vec4(1.0f, 0.0f, 0.5f, 1.0f));
        std::cout << "lol2\n";
        sh1->setUniform("mx",glm::mat4());
        std::cout << "lol3\n";
        vao->bind();
        std::cout << "lol4\n";
        if(vao == nullptr)
            std::cout << "wtf\n";
        glDrawArrays(GL_TRIANGLES, 0, vao->countVertices());
        std::cout << "lol5\n";
        vao->unbind();
        std::cout << "lol6\n";
    }
    void deliverEvent(sf::Event&){}

protected:
    chaos::VertexArray* vao = nullptr;
    chaos::ShaderProgram* sh1 = nullptr;

};

#endif // ARRRR_HPP

