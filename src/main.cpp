#include <bits/stdc++.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "header/cube.h"
#include "header/object.h"
#include "header/shader.h"
#include "header/stb_image.h"

void framebufferSizeCallback(GLFWwindow *window, int width, int height);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
unsigned int loadCubemap(std::vector<string> &mFileName);

struct material_t{
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float gloss;
};

struct light_t{
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
};

struct model_t{
    glm::vec3 position;
    glm::vec3 scale;
    glm::vec3 rotation;
    Object* object;
};

struct camera_t{
    glm::vec3 position;
    glm::vec3 up;
    float rotationY;
};

// settings
int SCR_WIDTH = 800;
int SCR_HEIGHT = 600;

// cube map 
unsigned int cubemapTexture;
unsigned int cubemapVAO, cubemapVBO;

// shader programs 
int shaderProgramIndex = 0;
std::vector<shader_program_t*> shaderPrograms;
shader_program_t* cubemapShader;

// global time variable
float currentTime = 0.0f;
float initTime = glfwGetTime() + 15.0f;

// rowlet rotation degree
float RowletDeltax = 0.0f;
float RowletDeltay = 0.0f;

// pokeball parameters
float throwStartTime = -1.0f; // -1 means the ball is not currently in motion
glm::vec3 ballInitialPosition = glm::vec3(0.0f, 0.42f, 13.58f); // Initial position (x, y, z)
glm::vec3 ballVelocity = glm::vec3(0.0f, 15.0f, -12.0f); // Velocity vector (x, y, z)
float gravity = 9.8f;

// additional dependencies
light_t light;
material_t material;
camera_t camera;
model_t Rowlet;
model_t Rowlet_evolve;
model_t Groudon;
model_t Pokeball;

// model matrix
glm::mat4 cameraModel;
glm::mat4 RowletModel;
glm::mat4 RowletModel_evolve;
glm::mat4 GroudonModel;
glm::mat4 PokeballModel;

void camera_setup(){
    camera.position = glm::vec3(0.0, 20.0, 100.0);
    camera.up = glm::vec3(0.0, 1.0, 0.0);
    camera.rotationY = 0;
}

void light_setup(){
    light.position = glm::vec3(0.0, 1000.0, 0.0);
    light.ambient = glm::vec3(1.0);
    light.diffuse = glm::vec3(1.0);
    light.specular = glm::vec3(1.0);
}

void material_setup(){
    material.ambient = glm::vec3(1.0);
    material.diffuse = glm::vec3(1.0);
    material.specular = glm::vec3(0.7);
    material.gloss = 10.5;
}

void model_setup(){

#if defined(__linux__) || defined(__APPLE__)
    std::string objDir = "../../src/asset/obj/";
    std::string textureDir = "../../src/asset/texture/";
#else
    std::string objDir = "..\\..\\src\\asset\\obj\\";
    std::string textureDir = "..\\..\\src\\asset\\texture\\";
#endif

    RowletModel = glm::mat4(1.0f);
    Rowlet.position = glm::vec3(0.0f, 0.0f, 10.0f);
    Rowlet.scale = glm::vec3(4.0f, 4.0f, 4.0f);
    Rowlet.rotation = glm::vec3(0.0f, 90.0f, 0.0f);
    Rowlet.object = new Object(objDir + "rowlet.obj");
    Rowlet.object->load_to_buffer();
    Rowlet.object->load_texture(textureDir + "rowlet.jpg");

    GroudonModel = glm::mat4(1.0f);
    Groudon.position = glm::vec3(0.0f, 0.0f, -500.0f);
    Groudon.scale = glm::vec3(0.1f, 0.1f, 0.1f);
    Groudon.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Groudon.object = new Object(objDir + "groudon.obj");
    Groudon.object->load_to_buffer();
    Groudon.object->load_texture(textureDir + "groudon.jpg");

    PokeballModel = glm::mat4(1.0f);
    Pokeball.position = glm::vec3(0.0f, -1.0f, 15.0f);
    Pokeball.scale = glm::vec3(3.0f, 3.0f, 3.0f);
    Pokeball.rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    Pokeball.object = new Object(objDir + "pokeball.obj");
    Pokeball.object->load_to_buffer();
    Pokeball.object->load_texture(textureDir + "pokeball.jpg");

    RowletModel_evolve = glm::mat4(1.0f);
    Rowlet_evolve.position = glm::vec3(0.0f, 0.0f, 90.0f);
    Rowlet_evolve.scale = glm::vec3(0.5f, 0.5f, 0.5f);
    Rowlet_evolve.rotation = glm::vec3(0.0f, 180.0f, 0.0f);
    Rowlet_evolve.object = new Object(objDir + "rowlet_evolve.obj");
    Rowlet_evolve.object->load_to_buffer();
    Rowlet_evolve.object->load_texture(textureDir + "rowlet_evolve.jpg");
}

void shader_setup(){

// Setup the shader program for each shading method

#if defined(__linux__) || defined(__APPLE__)
    std::string shaderDir = "../../src/shaders/";
#else
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    std::vector<std::string> shadingMethod = {
        "default",                            
        "bling-phong", "gouraud", "metallic",  
        "glass_schlick", "glass_empricial",    
    };

    for(int i=0; i<shadingMethod.size(); i++){
        std::string vpath = shaderDir + shadingMethod[i] + ".vert";
        std::string fpath = shaderDir + shadingMethod[i] + ".frag";

        shader_program_t* shaderProgram = new shader_program_t();
        shaderProgram->create();
        shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
        shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
        shaderProgram->link_shader();
        shaderPrograms.push_back(shaderProgram);
    } 
    std::string vpath = shaderDir + "evolve.vert";
    std::string fpath = shaderDir + "evolve.frag";
    std::string gpath = shaderDir + "evolve.geom";

    shader_program_t* shaderProgram = new shader_program_t();
    shaderProgram->create();
    shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
    shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
    shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
    shaderProgram->link_shader();
    shaderPrograms.push_back(shaderProgram);

    vpath = shaderDir + "explosion.vert";
    fpath = shaderDir + "explosion.frag";
    gpath = shaderDir + "explosion.geom";

    shaderProgram = new shader_program_t();
    shaderProgram->create();
    shaderProgram->add_shader(vpath, GL_VERTEX_SHADER);
    shaderProgram->add_shader(fpath, GL_FRAGMENT_SHADER);
    shaderProgram->add_shader(gpath, GL_GEOMETRY_SHADER);
    shaderProgram->link_shader();
    shaderPrograms.push_back(shaderProgram);
}


void cubemap_setup(){

#if defined(__linux__) || defined(__APPLE__)
    std::string cubemapDir = "../../src/asset/texture/skybox/";
    std::string shaderDir = "../../src/shaders/";
#else
    std::string cubemapDir = "..\\..\\src\\asset\\texture\\skybox\\";
    std::string shaderDir = "..\\..\\src\\shaders\\";
#endif

    // setup texture for cubemap
    std::vector<std::string> faces
    {
        cubemapDir + "right.jpg",
        cubemapDir + "left.jpg",
        cubemapDir + "top.jpg",
        cubemapDir + "bottom.jpg",
        cubemapDir + "front.jpg",
        cubemapDir + "back.jpg"
    };
    cubemapTexture = loadCubemap(faces);   

    // setup shader for cubemap
    std::string vpath = shaderDir + "cubemap.vert";
    std::string fpath = shaderDir + "cubemap.frag";
    
    cubemapShader = new shader_program_t();
    cubemapShader->create();
    cubemapShader->add_shader(vpath, GL_VERTEX_SHADER);
    cubemapShader->add_shader(fpath, GL_FRAGMENT_SHADER);
    cubemapShader->link_shader();

    glGenVertexArrays(1, &cubemapVAO);
    glGenBuffers(1, &cubemapVBO);
    glBindVertexArray(cubemapVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubemapVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVertices), &cubemapVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glBindVertexArray(0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
}

void setup(){

    // Initialize shader model camera light material
    light_setup();
    model_setup();
    shader_setup();
    camera_setup();
    cubemap_setup();
    material_setup();

    // Enable depth test, face culling ...
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_CULL_FACE);
    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);

    // Debug: enable for debugging
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback([](  GLenum source, GLenum type, GLuint id, GLenum severity, 
                                 GLsizei length, const GLchar* message, const void* userParam) {

    std::cerr << "GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "") 
               << "type = " << type 
               << ", severity = " << severity 
               << ", message = " << message << std::endl;
     }, nullptr);
}

void update(){
    
    // Update the global time variable
    currentTime = glfwGetTime();

    Rowlet.rotation.x = 0.0f;
    if (currentTime - initTime < 0) {
        // no action
    }
    else if ((currentTime - initTime < 4.0f)) {
        // rotate camera
        camera.rotationY += max(0.5f, (6.0f - (currentTime - initTime)) * (6.0f - (currentTime - initTime)) / 18.0f);
    }
    else if (currentTime - initTime < 9.0f) {
        // camera view set to 30 degree
        if (abs(camera.rotationY - 30.0f) > 0.5f) {
            camera.rotationY += 0.5f;
        }
        else {
            // rowlet y-axis oscillation
            Rowlet.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
        }
    }
    else if ((currentTime - initTime < 10.0f) && Rowlet.rotation.y > -15.0f) {
        // rowlet turn around to stare
        Rowlet.rotation.y -= 1.0f;
    }
    else if (currentTime - initTime < 10.0f) {
        // rowlet stop and stare
    }
    else if (currentTime - initTime < 11.0f) {
        // rowlet turn back
        if (abs(Rowlet.rotation.y - 90.0f) > 0.5f) {
            Rowlet.rotation.y += 1.5f;
        }
        else {
            // rowlet y-axis oscillation
            Rowlet.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
        }
    }
    else if (currentTime - initTime < 12.0f) {
        // rowlet attack
        Rowlet.position.z -= 0.15f;
        Rowlet.rotation.x = 30.0f;
    }
    else if (currentTime - initTime < 12.5f) {
        // rowlet stop and groudon mock
        // Groudon fast oscillation along x-axis
        Groudon.position.x += 0.2f * sin(10.0f * (currentTime - initTime));
    }
    else if (currentTime - initTime < 13.5f) {
        // rowlet return
        Rowlet.position.z += 0.15f;
    }
    else if (currentTime - initTime < 14.0f) {
        // waiting for groudon's turn
    }
    else if (currentTime - initTime < 15.0f) {
        // groudon attack
        Groudon.position.z += 5.0f;
        Groudon.rotation.y -= 1.0f;
        Rowlet.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
    }
    else if (currentTime - initTime < 15.5f) {
        // groudon stop and rowlet fall
        RowletDeltax -= 0.6f;
        RowletDeltay -= 0.6f;
    }
    else if (currentTime - initTime < 16.5f) {
        // groudon returns and rowlet arises
        Groudon.position.z -= 5.0f;
        Groudon.rotation.y += 1.0f;
        RowletDeltax += 0.3f;
        RowletDeltay += 0.3f;
    }
    else if (currentTime - initTime < 18.5f) {
        // shimmering rowlet
        Rowlet.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
        Rowlet.rotation.y += 1.0f * (currentTime - initTime);
    }
    else if (currentTime - initTime < 19.5f) {
        // show the evolved rowlet
        Rowlet_evolve.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
    }
    else if (currentTime - initTime < 20.5f) {
        // rotate to throw the bomb pokeball
        Rowlet_evolve.rotation.x -= 0.3f;
        Pokeball.position.z -= 0.01f;
        Pokeball.position.y += 0.01f;
        ballInitialPosition = Pokeball.position;
        throwStartTime = currentTime;
    }
    else if ((currentTime - initTime < 35.5f) && Pokeball.position.y > 0.0f) {
        // rotate the rowlet back
        if (currentTime - initTime < 21.5f) {
           Rowlet_evolve.rotation.x += 0.3f;
        }
        else {
            Rowlet_evolve.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
        }
        // throw the bomb pokeball
        float t = currentTime - throwStartTime;
        Pokeball.position.x = ballInitialPosition.x;
        Pokeball.position.y = ballInitialPosition.y + ballVelocity.y * t - 0.5f * gravity * t * t;
        Pokeball.position.z = ballInitialPosition.z + ballVelocity.z * t;
    }
    else {
        // rowlet y-axis oscillation
        Rowlet.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
        Rowlet_evolve.position.y += 0.01f * sin(2.0f * (currentTime - initTime));
    }
    
    RowletModel = glm::mat4(1.0f);
    RowletModel = glm::scale(RowletModel, Rowlet.scale);
    RowletModel = glm::translate(RowletModel, Rowlet.position);
    RowletModel = glm::rotate(RowletModel, glm::radians(Rowlet.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    RowletModel = glm::rotate(RowletModel, glm::radians(Rowlet.rotation.x), glm::vec3(1.0, 0.0, 0.0));
    if ((currentTime - initTime > 15.0f) && (currentTime - initTime < 16.5f)) {
        RowletModel = glm::rotate(RowletModel, glm::radians(RowletDeltay), glm::vec3(1.0, 0.0, 0.0));
        RowletModel = glm::rotate(RowletModel, glm::radians(RowletDeltax), glm::vec3(1.0, 0.0, 0.0));
    }

    RowletModel_evolve = glm::mat4(1.0f);
    RowletModel_evolve = glm::scale(RowletModel_evolve, Rowlet_evolve.scale);
    RowletModel_evolve = glm::translate(RowletModel_evolve, Rowlet_evolve.position);
    RowletModel_evolve = glm::rotate(RowletModel_evolve, glm::radians(Rowlet_evolve.rotation.y), glm::vec3(0.0, 1.0, 0.0));
    RowletModel_evolve = glm::rotate(RowletModel_evolve, glm::radians(Rowlet_evolve.rotation.x), glm::vec3(1.0, 0.0, 0.0));

    GroudonModel = glm::mat4(1.0f);
    GroudonModel = glm::scale(GroudonModel, Groudon.scale);
    GroudonModel = glm::translate(GroudonModel, Groudon.position);
    GroudonModel = glm::rotate(GroudonModel, glm::radians(Groudon.rotation.y), glm::vec3(0.0, 1.0, 0.0));

    PokeballModel = glm::mat4(1.0f);
    PokeballModel = glm::scale(PokeballModel, Pokeball.scale);
    PokeballModel = glm::translate(PokeballModel, Pokeball.position);

    camera.rotationY = (camera.rotationY > 360.0) ? 0.0 : camera.rotationY;
    cameraModel = glm::mat4(1.0f);
    cameraModel = glm::rotate(cameraModel, glm::radians(camera.rotationY), camera.up);
    cameraModel = glm::translate(cameraModel, camera.position);
}

void render(){
    glClearColor(0.0, 0.0, 50.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Calculate view, projection matrix
    glm::mat4 view = glm::lookAt(glm::vec3(cameraModel[3]), glm::vec3(0.0), camera.up);
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);

    // Set matrix for view, projection, model transformation
    shaderPrograms[6]->use();
    shaderPrograms[6]->set_uniform_value("model", RowletModel);
    shaderPrograms[6]->set_uniform_value("view", view);
    shaderPrograms[6]->set_uniform_value("projection", projection);
    if (currentTime - initTime - 16.5f > 0) {
        shaderPrograms[6]->set_uniform_value("shimmer", 0.5f + 0.5f * sin((currentTime - initTime - 16.5f) * 5.0f));    
    }
    else {
        shaderPrograms[6]->set_uniform_value("shimmer", 0.0f);
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Rowlet.object->get_texture_id());
    shaderPrograms[6]->set_uniform_value("ourTexture", 0);
    
    if (currentTime - initTime < 19.5f) {
        Rowlet.object->render();
        shaderPrograms[6]->release();
    }

    shaderPrograms[3]->use();
    shaderPrograms[3]->set_uniform_value("model", RowletModel_evolve);
    shaderPrograms[3]->set_uniform_value("view", view);
    shaderPrograms[3]->set_uniform_value("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Rowlet_evolve.object->get_texture_id());
    shaderPrograms[3]->set_uniform_value("ourTexture", 0);
    
    if (currentTime - initTime > 19.5f) {
        Rowlet_evolve.object->render();
        shaderPrograms[3]->release();
    }

    if (currentTime - initTime < 22.5) {
        shaderProgramIndex = 0;
    }
    else {
        shaderProgramIndex = 7;
    }
    shaderPrograms[shaderProgramIndex]->use();
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", GroudonModel);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("time", (currentTime - initTime - 22.5f));

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Groudon.object->get_texture_id());
    shaderPrograms[shaderProgramIndex]->set_uniform_value("ourTexture", 0);

    Groudon.object->render();
    shaderPrograms[shaderProgramIndex]->release();

    shaderProgramIndex = 3;
    shaderPrograms[shaderProgramIndex]->use();
    shaderPrograms[shaderProgramIndex]->set_uniform_value("model", PokeballModel);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("view", view);
    shaderPrograms[shaderProgramIndex]->set_uniform_value("projection", projection);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, Pokeball.object->get_texture_id());
    shaderPrograms[shaderProgramIndex]->set_uniform_value("ourTexture", 0);

    if ((currentTime - initTime > 19.5f) && (currentTime - initTime < 22.5f)) {
        Pokeball.object->render();
        shaderPrograms[shaderProgramIndex]->release();
    }

    glDepthFunc(GL_LEQUAL); // Allow the skybox to render behind everything else
    glDepthMask(GL_FALSE); // Disable depth buffer writing
    cubemapShader->use();
    glm::mat4 viewSkybox = glm::mat4(glm::mat3(view)); // Remove translation for the cubemap
    cubemapShader->set_uniform_value("view", viewSkybox);
    cubemapShader->set_uniform_value("projection", projection);

    glBindVertexArray(cubemapVAO);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    cubemapShader->set_uniform_value("skybox", 1);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
    glDepthMask(GL_TRUE); // Re-enable depth buffer writing
    glDepthFunc(GL_LESS); // Restore default depth 
}


int main() {
    
    // glfw: initialize and configure
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "The Revenge of Rowlet", NULL, NULL);
    if (window == NULL) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSwapInterval(1);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // set viewport
    glfwGetFramebufferSize(window, &SCR_WIDTH, &SCR_HEIGHT);
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    // Setup texture, model, shader ...e.t.c
    setup();
    
    // Render loop, main logic can be found in update, render function
    while (!glfwWindowShouldClose(window)) {
        update(); 
        render(); 
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

// Add key callback
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // shader program selection
    if (key == GLFW_KEY_0 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 0;
    if (key == GLFW_KEY_1 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 1;
    if (key == GLFW_KEY_2 && (action == GLFW_REPEAT || action == GLFW_PRESS)) 
        shaderProgramIndex = 2;
    if (key == GLFW_KEY_3 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 3;
    if (key == GLFW_KEY_4 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 4;
    if (key == GLFW_KEY_5 && (action == GLFW_REPEAT || action == GLFW_PRESS))
        shaderProgramIndex = 5;

    // camera movement
    float cameraSpeed = 0.5f;
    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z -= 10.0;
    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.position.z += 10.0;
    if (key == GLFW_KEY_A && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY -= 10.0;
    if (key == GLFW_KEY_D && (action == GLFW_REPEAT || action == GLFW_PRESS))
        camera.rotationY += 10.0;
    
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
    SCR_WIDTH = width;
    SCR_HEIGHT = height;
}

// Loading cubemap texture
unsigned int loadCubemap(vector<std::string>& faces){

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        stbi_set_flip_vertically_on_load(false);
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                         0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
            );
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap tex failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}  
