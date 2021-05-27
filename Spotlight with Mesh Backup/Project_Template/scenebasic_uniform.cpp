#include "scenebasic_uniform.h"

#include <iostream>
#include <sstream>
using std::cerr;
using std::endl;

#include <glm/gtc/matrix_transform.hpp>
using glm::vec3;
using glm::vec4;
using glm::mat4;
using glm::mat3;

SceneBasic_Uniform::SceneBasic_Uniform() : plane(10.0f, 10.0f, 100, 100)
{
    mesh = ObjMesh::load("media/great_helm_base.obj", true);
}
void SceneBasic_Uniform::initScene()
{
    compile();
	glEnable(GL_DEPTH_TEST);

    view = glm::lookAt(vec3(5.0f, 5.0f, 7.5f), vec3(0.0f, 0.75f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    projection = mat4(1.0f);

    tPrev = 0;
    angle = 0;

    prog1.setUniform("Fog.MaxDist", 30.0f);
    prog1.setUniform("Fog.MinDist", 1.0f);
    prog1.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));


    prog1.setUniform("Light.L", 1.0f, 1.0f, 1.0f);
    prog1.setUniform("Light.La", 0.0f, 0.0f, 1.0f);
    prog1.setUniform("Light.Position", view * glm::vec4(5.0f, 5.0f, 2.0f, 0.0f));

    prog2.setUniform("Fog.MaxDist", 30.0f);
    prog2.setUniform("Fog.MinDist", 1.0f);
    prog2.setUniform("Fog.Color", vec3(0.5f, 0.5f, 0.5f));


    prog2.setUniform("Light.L", 1.0f, 1.0f, 1.0f);
    prog2.setUniform("Light.La", 0.0f, 0.0f, 1.0f);
    prog2.setUniform("Light.Position", view * glm::vec4(5.0f, 5.0f, 2.0f, 0.0f));

    angle = glm::half_pi<float>();

    //prog.setUniform("Spot.L", vec3(0.9f));
    //prog.setUniform("Spot.La", vec3(0.5f));
    //prog.setUniform("Spot.Exponent", 50.0f);
    //prog.setUniform("Spot.Cutoff", glm::radians(15.0f));

    GLuint texID = Texture::loadTexture("media/texture/rough.png");

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texID);
}

void SceneBasic_Uniform::compile()
{
	try {
		prog1.compileShader("shader/basic_uniform.vert");
		prog1.compileShader("shader/basic_uniform.frag");
		prog1.link();
		prog1.use();

        prog2.compileShader("shader/base_uniform.vert");
        prog2.compileShader("shader/base_uniform.frag");
	} catch (GLSLProgramException &e) {
		cerr << e.what() << endl;
		exit(EXIT_FAILURE);
	}
}

void SceneBasic_Uniform::update( float t )
{
    time = t;

    float deltaT = t - tPrev;
    if (tPrev == 0.0f)
        deltaT = 0.0f;

    tPrev = t;
    angle += 0.25f * deltaT;
    if (angle > glm::two_pi<float>())
        angle -= glm::two_pi<float>();
}

void SceneBasic_Uniform::render()
{
    prog1.setUniform("Time", time);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    //vec4 lightPos = vec4(0.0f, 10.0f, 0.0f, 1.0f);
    //prog.setUniform("Spot.Position", vec3(view * lightPos));
    //mat3 normalMatrix = mat3(vec3(view[0]), vec3(view[1]), vec3(view[2]));
    //prog.setUniform("Spot.Direction", normalMatrix * vec3(-lightPos));

    prog2.setUniform("Material.Kd", 0.2f, 0.55f, 0.9f);
    prog2.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog2.setUniform("Material.Ka", 0.2f * 0.3f, 0.55f * 0.3f, 0.9f * 0.3f);
    prog2.setUniform("Material.Shininess", 100.0f);

    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, 0.0f, 0.0f));
    model = glm::scale(model, vec3(0.1, 0.1, 0.1));
    setMatrices();
    mesh->render();

    prog1.setUniform("Material.Kd", 0.7f, 0.7f, 0.7f);
    prog1.setUniform("Material.Ks", 0.9f, 0.9f, 0.9f);
    prog1.setUniform("Material.Ka", 0.2f, 0.2f, 0.2f);
    prog1.setUniform("Material.Shininess", 180.0f);
 
    model = mat4(1.0f);
    model = glm::translate(model, vec3(0.0f, -2.0f, 0.0f));
    setMatrices();
    plane.render();

}

void SceneBasic_Uniform::setMatrices()
{
    mat4 mv = view * model; //we create a model view matrix
    
    prog1.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    prog1.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix
    prog1.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix

    prog2.setUniform("ModelViewMatrix", mv); //set the uniform for the model view matrix
    prog2.setUniform("NormalMatrix", glm::mat3(vec3(mv[0]), vec3(mv[1]), vec3(mv[2]))); //we set the uniform for normal matrix
    prog2.setUniform("MVP", projection * mv); //we set the model view matrix by multiplying the mv with the projection matrix
}

void SceneBasic_Uniform::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(70.0f), (float)w / h, 0.3f, 100.0f);
}
