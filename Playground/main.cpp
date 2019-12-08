// GLEW нужно подключать до GLFW.
// GLEW
#define GLEW_STATIC
#define cimg_use_jpeg
#define NR_POINT_LIGHTS 4

#include <iostream>
#include <math.h>
#include <GL/glew.h>
#include "shader.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <vector>
#include "lodepng.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

//Время, прошедшее между последним и текущим кадром
GLfloat deltaTime = 0.0f;
//Время вывода последнего кадра
GLfloat lastFrame = 0.0f;
//Степень смешивания тестур
//float mixRate = 0.0f;
//Буфер нажатий
bool keys[1024];
//Камера
glm::vec3 cameraPosition  = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraUp        = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 cameraTarget    = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget);
glm::vec3 cameraFront     = -cameraDirection;
//Углы
GLfloat yaw   = -90.0f;
GLfloat pitch = 0.0f;
//Смещение мыши
GLfloat lastX;
GLfloat lastY;
bool firstMouse = true;
GLfloat fov = 45.0f;
//Освещение
//glm::vec3 lightPosition(1.5f, 1.5f, 1.5f);
float projDiffuseRate = 1.0f;
float dirDiffuseRate = 0.3f;
glm::vec3 pointAmbient  = glm::vec3(0.05f);
glm::vec3 pointDiffuse  = glm::vec3(0.6f);
glm::vec3 pointSpecular = glm::vec3(0.7f);
glm::vec3 dirAmbient    = glm::vec3(0.05f);
glm::vec3 dirDiffuse    = glm::vec3(dirDiffuseRate);
glm::vec3 dirSpecular   = glm::vec3(0.4f);
glm::vec3 projAmbient   = glm::vec3(0.05f);
glm::vec3 projDiffuse   = glm::vec3(projDiffuseRate);
glm::vec3 projSpecular  = glm::vec3(0.7f);

bool flashlight = true;
bool sun = true;
bool lamps = true;

//Реализация нажатий
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
//Значения углов от мыши
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
//Колесико мыши
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void doMovement();

unsigned int loadCubemap(std::vector<std::string> faces);


int main()
{
    //Настройки GLFW и GLEW
    //Инициализация GLFW
    glfwInit();
    //Настройка GLFW
    //Задается минимальная требуемая версия OpenGL.
    //Мажорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    //Минорная
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //Установка профайла для которого создается контекст
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //Для Mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    //Выключение возможности изменения размера окна
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
   
    
    
    //Создание объекта окна
    GLFWwindow* window = glfwCreateWindow(800, 600, "Good_Morning", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    //Инициализация GLEW
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
        
    //Размер отрисовываемого окна
    int screenWidth, screenHeight;
    glfwGetFramebufferSize(window, &screenWidth, &screenHeight);
    
    glViewport(0, 0, screenWidth, screenHeight); //Левый нижний угол, ширина, высота
    
    //Отслеживание глубины
    glEnable(GL_DEPTH_TEST);
    //Заполнение полигонов
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    //Трафаретное тестирование
    //glEnable(GL_STENCIL_TEST);
    //Если оба теста пройдены, ставим значение из glStencilFunc (ниже)
    //glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    //Режим смешивания
    glEnable(GL_BLEND);
    //Прозрачность источника - для источника, 1-прозрачность источника - для приемника
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    //glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO);
    //glBlendEquation(GL_FUNC_SUBTRACT);
    
    //Шейдеры
    Shader ourShader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/light_shader.vs", "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/light_shader.frag");
    Shader screenShader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/screen_shader.vs", "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/screen_shader.frag");
    Shader skyShader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/sky_shader.vs", "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/sky_shader.frag");
    Shader mirrorShader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/mirror_shader.vs", "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/mirror_shader.frag");
    Shader shader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/simple_shader.vs","/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/simple_shader.frag");
    Shader lampShader("/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/lamp_shader.vs","/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/lamp_shader.frag");
    
    //Куб
    float vertices[] = {
        // Позиции           // Текстурные // Нормали
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,  0.0f, 0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,  0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 0.0f, 1.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f, 0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f, 0.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  1.0f, 0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  1.0f, 0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  0.0f, -1.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  0.0f, -1.0f, 0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,  0.0f, 1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,  0.0f, 1.0f, 0.0f
    };
    
    float floorVertices[] = {
        //Позиции               //Текстурные    //Нормали
         5.0f, -0.5f,  5.0f,    2.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f,  5.0f,    0.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,    0.0f, 2.0f,     0.0f, 1.0f, 0.0f,

         5.0f, -0.5f,  5.0f,    2.0f, 0.0f,     0.0f, 1.0f, 0.0f,
        -5.0f, -0.5f, -5.0f,    0.0f, 2.0f,     0.0f, 1.0f, 0.0f,
         5.0f, -0.5f, -5.0f,    2.0f, 2.0f,     0.0f, 1.0f, 0.0f,
        //Пытаюсь сделать пол двусторонним
         5.0f, -0.501f,  5.0f,    2.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        -5.0f, -0.501f,  5.0f,    0.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        -5.0f, -0.501f, -5.0f,    0.0f, 2.0f,     0.0f, -1.0f, 0.0f,

         5.0f, -0.501f,  5.0f,    2.0f, 0.0f,     0.0f, -1.0f, 0.0f,
        -5.0f, -0.501f, -5.0f,    0.0f, 2.0f,     0.0f, -1.0f, 0.0f,
         5.0f, -0.501f, -5.0f,    2.0f, 2.0f,     0.0f, -1.0f, 0.0f
    };
    
    float opacityVertices[] = {
       //Позиции             //Текстурные  //Нормали
       0.0f,  0.5f,  0.0f,   0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
       0.0f, -0.5f,  0.0f,   0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
       1.0f, -0.5f,  0.0f,   1.0f, 0.0f,   0.0f, 0.0f, 1.0f,

       0.0f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
       1.0f, -0.5f,  0.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
       1.0f,  0.5f,  0.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f
    };
    
    float quadVertices[] = { //fills the entire screen in Normalized Device Coordinates.
        // positions   // texCoords
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    float skyVertices[] = {
        // positions
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };
    
    //Сдвиги кубиков
    glm::vec3 cubePositions[] = {
      glm::vec3( 0.0f,  7.0f,  0.0f),
      glm::vec3( 2.0f,  5.0f, -15.0f),
      glm::vec3(-1.5f, -2.2f, -2.5f),
      glm::vec3(-3.8f, -2.0f, -12.3f),
      glm::vec3( 2.4f, -1.5f, -3.5f),
      glm::vec3(-1.7f,  3.0f, -7.5f),
      glm::vec3( 1.3f, -2.0f, -2.5f),
      glm::vec3( 1.5f,  2.0f, -2.5f),
      glm::vec3( 1.5f,  0.8f, -1.5f),
      glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.5f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  5.0f, -12.0f),
        glm::vec3( 0.0f,  4.0f, -3.0f)
    };
    
    //Позиции окошек
    std::vector<glm::vec3> windows
    {
        glm::vec3(-1.5f,  0.0f, -0.48f),
        glm::vec3( 1.5f,  0.0f,  0.51f),
        glm::vec3( 0.0f,  0.0f,  0.7f),
        glm::vec3(-0.3f,  0.0f, -2.3f),
        glm::vec3( 0.5f,  0.0f, -0.6f)
    };
    
    //VBO, VAO, EBO
    //Создаем Vertex Array Object
    GLuint objectVAO;
    glGenVertexArrays(1, &objectVAO);
    
    //Создаем Vertex Buffer Objects
    GLuint objectVBO;
    glGenBuffers(1, &objectVBO);
    //Привязка буфера к VBO
    glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
    //Копируем вершинные данные в буфер + GL_STATIC_DRAW/GL_DYNAMIC_DRAW/GL_STREAM_DRAW
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    //Привязка VAO
    glBindVertexArray(objectVAO);
    
    //Устанавливаем указатели на вершинные атрибуты //location = 0, vec3,,normalize,step_between_data_packs,смещение_начала_данных

    // Атрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Атрибут с текстурой
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    // Атрибут с нормалями
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    //Пол
    GLuint floorVAO;
    glGenVertexArrays(1, &floorVAO);
    
    GLuint floorVBO;
    glGenBuffers(1, &floorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, floorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floorVertices), floorVertices, GL_STATIC_DRAW);
    glBindVertexArray(floorVAO);
    //Устанавливаем указатели на вершинные атрибуты //location = 0, vec3,,normalize,step_between_data_packs,смещение_начала_данных
    // Атрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Атрибут с текстурой
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //Атрибут с нормалью
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    //Полупрозрачные
    GLuint opacityVAO;
    glGenVertexArrays(1, &opacityVAO);
    
    GLuint opacityVBO;
    glGenBuffers(1, &opacityVBO);
    glBindBuffer(GL_ARRAY_BUFFER, opacityVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(opacityVertices), opacityVertices, GL_STATIC_DRAW);
    glBindVertexArray(opacityVAO);
    //Устанавливаем указатели на вершинные атрибуты //location = 0, vec3,,normalize,step_between_data_packs,смещение_начала_данных
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Атрибут с текстурой
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    //Атрибут с нормалью
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)(5 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);
    
    //Лампа
    GLuint lampVAO;
    glGenVertexArrays(1, &lampVAO);
    //Привязка VAO
    glBindVertexArray(lampVAO);
    //Привязка буфера к VBO (VBO - тот же)
    glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
    // Атрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    //Квад
    GLuint quadVAO;
    glGenVertexArrays(1, &quadVAO);
    
    GLuint quadVBO;
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glBindVertexArray(quadVAO);
    //Устанавливаем указатели на вершинные атрибуты //location = 0, vec3,,normalize,step_between_data_packs,смещение_начала_данных
    // Атрибут с координатами
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    // Атрибут с текстурой
    glVertexAttribPointer(1, 2, GL_FLOAT,GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    
    glBindVertexArray(0);
    
    
    //Скайбокс
    GLuint skyVAO;
    glGenVertexArrays(1, &skyVAO);
    
    GLuint skyVBO;
    glGenBuffers(1, &skyVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);
    glBindVertexArray(skyVAO);
    //Устанавливаем указатели на вершинные атрибуты //location = 0, vec3,,normalize,step_between_data_packs,смещение_начала_данных
    // Атрибут с координатами
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    
    glBindVertexArray(0);
    
    
    //Кадровый буфер (текстурный, для цвета)
    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    
    //Текстура для кадрового буфера
    unsigned int texColorBuffer;
    glGenTextures(1, &texColorBuffer);
    glBindTexture(GL_TEXTURE_2D, texColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, screenWidth, screenHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL); //NULL - тк только выделяем память
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //Прикрепление текстуры
    //Чтение/запись,тип подключения(цвет/глубина/трафарет),тип текстур,текстура,МИП-уровень для вывода
    //GL_DEPTH_ATTACHMENT -> format и internalformat объекта текстуры должны принять значение GL_DEPTH_COMPONENT
    //GL_STENCIL_ATTACHMENT -> параметры формата текстуры – GL_STENCIL_INDEX
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texColorBuffer, 0);
    
    //Рендербуфер для глубины и трафарета)
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    //Созидание
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, screenWidth, screenHeight);
    //Подключение к кадровому буферу
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    //Присоединяем объект рендербуфера к совмещенной точке прикрепления глубины и трафарета буфера кадра
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
    //Ошибки
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    
    //___Текстуры___
    //Идентификатор текстуры
    GLuint texObject;
    glGenTextures(1, &texObject);
    //Привязка конкретной текстуры
    //glActiveTexture(GL_TEXTURE0); //Активируем текстурный блок
    glBindTexture(GL_TEXTURE_2D, texObject);
    
    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    //Параметры МипМапов
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //GL_LINEAR_MIPMAP_LINEAR / GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //Загрузка lodepng
    std::vector<unsigned char> image; //Пиксели будут тут
    unsigned texwidth, texheight;

    //Декодирование
    unsigned error = lodepng::decode(image, texwidth, texheight, "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/stone.png");

    //Ошибки
    if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
    
    unsigned char* data = &image[0]; //RGBARGBARGBA...
    
    //Генерация текстуры
    //Цель,уровень мипмапа,формат хранения,ширина,высота,прост,формат и тип данных исходника,данные изображения
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //Генерация мипмапов
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR IN TEXTURE LOADING" << std::endl;
    }
    
    //Освобождение памяти и отвзяка от изображения
    std::vector<unsigned char>().swap(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //----------------------------------------------------------
    
    //Идентификатор текстуры
    GLuint texFloor;
    glGenTextures(1, &texFloor);
    //Привязка конкретной текстуры
    //glActiveTexture(GL_TEXTURE0); //Активируем текстурный блок
    glBindTexture(GL_TEXTURE_2D, texFloor);
    
    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
       
    //Параметры МипМапов
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR / GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //Загрузка lodepng
    //Декодирование
    error = lodepng::decode(image, texwidth, texheight, "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/grass.png");

    //Ошибки
    if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
    
    data = &image[0]; //RGBARGBARGBA...
    
    //Генерация текстуры
    //Цель,уровень мипмапа,формат хранения,ширина,высота,прост,формат и тип данных исходника,данные изображения
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //Генерация мипмапов
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR IN TEXTURE LOADING" << std::endl;
    }
    
    //Освобождение памяти и отвзяка от изображения
    std::vector<unsigned char>().swap(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //---------------------------------------------------------
    //Идентификатор текстуры
    GLuint texWindow;
    glGenTextures(1, &texWindow);
    //Привязка конкретной текстуры
    //glActiveTexture(GL_TEXTURE0); //Активируем текстурный блок
    glBindTexture(GL_TEXTURE_2D, texWindow);
    
    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       
    //Параметры МипМапов
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR / GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //Загрузка lodepng
    //Декодирование
    error = lodepng::decode(image, texwidth, texheight, "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/bulb.png");

    //Ошибки
    if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
    
    data = &image[0]; //RGBARGBARGBA...
    
    //Генерация текстуры
    //Цель,уровень мипмапа,формат хранения,ширина,высота,прост,формат и тип данных исходника,данные изображения
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //Генерация мипмапов
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR IN TEXTURE LOADING" << std::endl;
    }
    
    //Освобождение памяти и отвзяка от изображения
    std::vector<unsigned char>().swap(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //---------------------------------------------------------
    
    //Идентификатор текстуры
    GLuint texObjectSpecular;
    glGenTextures(1, &texObjectSpecular);
    //Привязка конкретной текстуры
    //glActiveTexture(GL_TEXTURE0); //Активируем текстурный блок
    glBindTexture(GL_TEXTURE_2D, texObjectSpecular);
    
    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       
    //Параметры МипМапов
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR / GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //Загрузка lodepng
    //Декодирование
    error = lodepng::decode(image, texwidth, texheight, "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/container2_specular.png");

    //Ошибки
    if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
    
    data = &image[0]; //RGBARGBARGBA...
    
    //Генерация текстуры
    //Цель,уровень мипмапа,формат хранения,ширина,высота,прост,формат и тип данных исходника,данные изображения
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //Генерация мипмапов
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR IN TEXTURE LOADING" << std::endl;
    }
    
    //Освобождение памяти и отвзяка от изображения
    std::vector<unsigned char>().swap(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //---------------------------------------------------------
    
    //Идентификатор текстуры
    GLuint texFloorSpecular;
    glGenTextures(1, &texFloorSpecular);
    //Привязка конкретной текстуры
    //glActiveTexture(GL_TEXTURE0); //Активируем текстурный блок
    glBindTexture(GL_TEXTURE_2D, texFloorSpecular);
    
    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
       
    //Параметры МипМапов
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_LINEAR_MIPMAP_LINEAR / GL_LINEAR / GL_NEAREST
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    
    //Загрузка lodepng
    //Декодирование
    error = lodepng::decode(image, texwidth, texheight, "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/wood_specular.png");

    //Ошибки
    if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
    
    data = &image[0]; //RGBARGBARGBA...
    
    //Генерация текстуры
    //Цель,уровень мипмапа,формат хранения,ширина,высота,прост,формат и тип данных исходника,данные изображения
    if (data){
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        //Генерация мипмапов
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "ERROR IN TEXTURE LOADING" << std::endl;
    }
    
    //Освобождение памяти и отвзяка от изображения
    std::vector<unsigned char>().swap(image);
    glBindTexture(GL_TEXTURE_2D, 0);
    
    //---------------------------------------------------------
    
    //Кубическая карта
    std::vector<std::string> faces
    {
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/right.png",
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/left.png",
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/top.png",
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/bottom.png",
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/front.png",
        "/Users/JulieClark/Documents/ВМК/graphics/Playground/Playground/back.png"
    };
    
    unsigned int cubemapTexture = loadCubemap(faces);
    
    //---Texture::END---
    
    
    //Игровой цикл
    while(!glfwWindowShouldClose(window))
    {
        //Клавиатура
        glfwPollEvents();
        glfwSetKeyCallback(window, key_callback);
        //Захват мыши
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetCursorPosCallback(window, mouse_callback);
        //Колесико
        glfwSetScrollCallback(window, scroll_callback);
        //WASD
        doMovement();
        
        //Вычисление времени
        GLfloat currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        
        // Первый проход
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // буфер трафарета не используется
        glEnable(GL_DEPTH_TEST);
        
        //Скайбокс
        glDepthMask(GL_FALSE);
        skyShader.Use();
        skyShader.setVec3("brightness", dirDiffuse);
        //Видовая
        glm::mat4 view = glm::mat4(1.0f);
        //Камера (Грама-Шмидта)
        GLfloat radius = 2.0f;
        glm::vec3 cameraDirection = glm::normalize(cameraPosition - cameraTarget); // Камера -> Z+
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection)); // Камера -> X+
        cameraUp = glm::cross(cameraDirection, cameraRight); // Камера -> Y+
        //Позиция камеры, Цель камеры, Орт вверх
        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, up);
        //Убрать составляющую передвижения
        view = glm::mat4(glm::mat3(view));
        //Проекция
        glm::mat4 projection = glm::mat4(1.0f);
        //fov,,distance_min,distance_max
        projection = glm::perspective(glm::radians(fov), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);

        skyShader.setMat4("view", view);
        skyShader.setMat4("projection", projection);
        
        glBindVertexArray(skyVAO);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthMask(GL_TRUE);
        
        //Сортировка по расстоянию до наблюдателя прозрачных объектов
        std::map<float, glm::vec3> sorted;
        for (unsigned int i = 0; i < windows.size(); i++)
        {
            float distance = glm::length(cameraPosition - windows[i]);
            sorted[distance] = windows[i];
        }

        //Шейдер
        
        mirrorShader.Use();
        mirrorShader.setVec3("cameraPosition", cameraPosition);
        mirrorShader.setVec3("sunBright", dirDiffuse);
        //mirrorShader.setVec3("lampBright", pointDiffuse);

        //view
        view = glm::mat4(1.0f);
        //Камера (Грама-Шмидта)
        radius = 2.0f;
        cameraDirection = glm::normalize(cameraPosition - cameraTarget); // Камера -> Z+
        up = glm::vec3(0.0f, 1.0f, 0.0f);
        cameraRight = glm::normalize(glm::cross(up, cameraDirection)); // Камера -> X+
        cameraUp = glm::cross(cameraDirection, cameraRight); // Камера -> Y+
        //Позиция камеры, Цель камеры, Орт вверх
        view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, up);
        // [R_x R_y R_z 0]   [1 0 0 -P_x]
        // [U_x U_y U_z 0] * [0 1 0 -P_y]
        // [D_x D_y D_z 0]   [0 0 1 -P_z]
        // [0   0   0   1]   [0 0 0   1 ]
        
        //Проекция
        projection = glm::mat4(1.0f);
        //fov,,distance_min,distance_max
        projection = glm::perspective(glm::radians(fov), float(screenWidth) / float(screenHeight), 0.1f, 100.0f);
        
        mirrorShader.setMat4("view", view);
        mirrorShader.setMat4("projection", projection);
        
        
        //Объекты
        glBindVertexArray(objectVAO);
        //Текстура
        //glBindTexture(GL_TEXTURE_2D, texture1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        //Куб 1
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.0f, -1.0f));
        mirrorShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        //Куб2
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.0f, 0.0f));
        mirrorShader.setMat4("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        //Пол
        ourShader.Use();
        
        //Материал
        ourShader.setInt("material.ambient", 0);
        ourShader.setInt("material.diffuse", 0);
        ourShader.setInt("material.specular", 1);
        ourShader.setFloat("material.shininess", 32.0f);
        
        //Направленный свет
        ourShader.setVec3("dirLight.direction", 0.2f, -1.0f, 0.3f);
        ourShader.setVec3("dirLight.ambient", dirAmbient);
        ourShader.setVec3("dirLight.diffuse", dirDiffuse);
        ourShader.setVec3("dirLight.specular", dirSpecular);
        
        //Точечный свет
        ourShader.setVec3("light[0].position", pointLightPositions[0]);
        ourShader.setVec3("light[0].specular", pointSpecular);
        ourShader.setVec3("light[0].ambient", pointAmbient);
        ourShader.setVec3("light[0].diffuse", pointDiffuse);
        ourShader.setFloat("light[0].constant", 1.0f);
        ourShader.setFloat("light[0].linear", 0.09f);
        ourShader.setFloat("light[0].quadratic", 0.032f);
        
        ourShader.setVec3("light[1].position", pointLightPositions[1]);
        ourShader.setVec3("light[1].specular", pointSpecular);
        ourShader.setVec3("light[1].ambient", pointAmbient);
        ourShader.setVec3("light[1].diffuse", pointDiffuse);
        ourShader.setFloat("light[1].constant", 1.0f);
        ourShader.setFloat("light[1].linear", 0.09f);
        ourShader.setFloat("light[1].quadratic", 0.032f);
        
        ourShader.setVec3("light[2].position", pointLightPositions[2]);
        ourShader.setVec3("light[2].specular", pointSpecular);
        ourShader.setVec3("light[2].ambient", pointAmbient);
        ourShader.setVec3("light[2].diffuse", pointDiffuse);
        ourShader.setFloat("light[2].constant", 1.0f);
        ourShader.setFloat("light[2].linear", 0.09f);
        ourShader.setFloat("light[2].quadratic", 0.032f);
        
        ourShader.setVec3("light[3].position", pointLightPositions[3]);
        ourShader.setVec3("light[3].specular", pointSpecular);
        ourShader.setVec3("light[3].ambient", pointAmbient);
        ourShader.setVec3("light[3].diffuse", pointDiffuse);
        ourShader.setFloat("light[3].constant", 1.0f);
        ourShader.setFloat("light[3].linear", 0.09f);
        ourShader.setFloat("light[3].quadratic", 0.032f);
        
        //Фонарик
        ourShader.setVec3("projLight.position", cameraPosition);
        ourShader.setVec3("projLight.direction", cameraFront);
        ourShader.setFloat("projLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("projLight.outerCutOff", glm::cos(glm::radians(15.5f)));
        ourShader.setVec3("projLight.ambient",  projAmbient);
        ourShader.setVec3("projLight.diffuse", projDiffuse);
        ourShader.setVec3("projLight.specular", projDiffuse);
        ourShader.setFloat("projLight.constant", 1.0f);
        ourShader.setFloat("projLight.linear", 0.09f);
        ourShader.setFloat("projLight.quadratic", 0.032f);
        
        ourShader.setVec3("viewPosition", cameraPosition);
        
        ourShader.setMat4("view", view);
        ourShader.setMat4("projection", projection);
        
       
        glBindVertexArray(objectVAO);
        //Текстура
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texObject);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texObjectSpecular);
        for(GLuint i = 0; i < 10; i++)
        {
            glm::mat4 model(1.0f);
            model = glm::translate(model, cubePositions[i]);
            GLfloat angle = (GLfloat)glfwGetTime() * glm::radians(15.0f + i*10.0f);
            model = glm::rotate(model, angle, glm::vec3(1.0f, 0.3f, 0.5f));
            ourShader.setMat4("model", model);

          glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glBindVertexArray(floorVAO);
        //Текстура
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texFloor);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texFloorSpecular);
        //Пол 1
        ourShader.setMat4("model", glm::mat4(1.0f));
        ourShader.setFloat("shiftX", 0.0f);
        ourShader.setFloat("shiftY", 0.0f);
        ourShader.setFloat("shiftZ", 0.0f);
        glDrawArrays(GL_TRIANGLES, 0, 12);
        
        //Травка
        shader.Use();
        shader.setMat4("view", view);
        shader.setMat4("projection", projection);
        glBindVertexArray(opacityVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texWindow);
        //glActiveTexture(GL_TEXTURE1);
        //glBindTexture(GL_TEXTURE_2D, texWindow);
        for(std::map<float,glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, it->second);
            shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 6);
        }
        
        //Лампа
        lampShader.Use();
        lampShader.setMat4("view", view);
        lampShader.setMat4("projection", projection);
        
        glBindVertexArray(lampVAO);
        for (int i = 0; i < NR_POINT_LIGHTS; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            lampShader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        glBindVertexArray(0);
        
        // второй проход
        glBindFramebuffer(GL_FRAMEBUFFER, 0); // возвращаем буфер кадра по умолчанию
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        screenShader.Use();
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, texColorBuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Смена буферов
        glfwSwapBuffers(window);
    }
    
    
    //Очищаем ресурсы
    glfwTerminate();
    
    return 0;
}

//Реализация нажатий
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{
    if(action == GLFW_PRESS)
      keys[key] = true;
    else if(action == GLFW_RELEASE)
      keys[key] = false;
    
    // Когда пользователь нажимает ESC, мы устанавливаем свойство WindowShouldClose в true,
    // и приложение после этого закроется
    if(key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
    
    // Изменение яркости фонарика (если включен) кнопками-стрелками (вверх и вниз)
    if(key == GLFW_KEY_UP && action == GLFW_PRESS && flashlight)
    {
        projDiffuseRate += 0.1f;
        if(projDiffuseRate >= 0.7f)
            projDiffuseRate = 0.7f;
        projDiffuse = glm::vec3(projDiffuseRate);
    }
    
    if(key == GLFW_KEY_DOWN && action == GLFW_PRESS && flashlight)
    {
        projDiffuseRate -= 0.1f;
        if(projDiffuseRate <= 0.0f)
            projDiffuseRate = 0.0f;
        projDiffuse = glm::vec3(projDiffuseRate);
    }
    
    //Изменение яркости Солнца
    if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS && sun)
    {
        dirDiffuseRate += 0.1f;
        if(dirDiffuseRate >= 1.0f)
            dirDiffuseRate = 1.0f;
        dirDiffuse = glm::vec3(dirDiffuseRate);
    }
    
    if(key == GLFW_KEY_LEFT && action == GLFW_PRESS && sun)
    {
        dirDiffuseRate -= 0.1f;
        if(dirDiffuseRate <= 0.0f)
            dirDiffuseRate = 0.0f;
        dirDiffuse = glm::vec3(dirDiffuseRate);
    }
    
    //Освещение
    if(key == GLFW_KEY_1 && action == GLFW_PRESS)
    {
        if (lamps)
        {
            pointAmbient      = glm::vec3(0.0f);
            pointDiffuse      = glm::vec3(0.0f);
            pointSpecular     = glm::vec3(0.0f);
            lamps = false;
        } else
        {
            pointAmbient      = glm::vec3(0.05f);
            pointDiffuse      = glm::vec3(0.8f);
            pointSpecular     = glm::vec3(1.0f);
            lamps = true;
        }
    }
    
    if(key == GLFW_KEY_2 && action == GLFW_PRESS)
    {
        if (sun)
        {
            dirAmbient      = glm::vec3(0.0f);
            dirDiffuse      = glm::vec3(0.0f);
            dirSpecular     = glm::vec3(0.0f);
            sun = false;
        } else
        {
            dirAmbient      = glm::vec3(0.05f);
            dirDiffuse      = glm::vec3(0.4f);
            dirSpecular     = glm::vec3(0.5f);
            sun = true;
        }
    }
    
    if(key == GLFW_KEY_3 && action == GLFW_PRESS)
    {
        if (flashlight)
        {
            projAmbient      = glm::vec3(0.0f);
            projDiffuse      = glm::vec3(0.0f);
            projSpecular     = glm::vec3(0.0f);
            flashlight = false;
        } else
        {
            projAmbient      = glm::vec3(0.05f);
            projDiffuse      = glm::vec3(1.0f);
            projSpecular     = glm::vec3(1.0f);
            flashlight = true;
        }
    }
}

void doMovement()
{
  // Управление камерой
  GLfloat cameraSpeed = 5.0f * deltaTime;
  if(keys[GLFW_KEY_W])
      cameraPosition += cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_S])
      cameraPosition -= cameraSpeed * cameraFront;
  if(keys[GLFW_KEY_A])
      cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
  if(keys[GLFW_KEY_D])
      cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
      lastX = xpos;
      lastY = ypos;
      firstMouse = false;
    }
    GLfloat xoffset = xpos - lastX;
    //Оконные Y-координаты возрастают с верху вниз
    GLfloat yoffset = -(ypos - lastY);
    lastX = xpos;
    lastY = ypos;

    GLfloat sensitivity = 0.05f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;
    
    yaw   += xoffset;
    pitch += yoffset;
    
    if(pitch > 89.0f)
      pitch =  89.0f;
    if(pitch < -89.0f)
      pitch = -89.0f;
    
    //Вычисление угла
    glm::vec3 front;
    front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
    front.y = sin(glm::radians(pitch));
    front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
    cameraFront = glm::normalize(front);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
  if(fov >= 1.0f && fov <= 45.0f)
      fov -= yoffset;
  if(fov <= 1.0f)
      fov = 1.0f;
  if(fov >= 45.0f)
      fov = 45.0f;
}

unsigned int loadCubemap(std::vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    //Параметры семплинга
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    
    for(GLuint i = 0; i < faces.size(); i++)
    {
        //Загрузка lodepng
        std::vector<unsigned char> image; //Пиксели будут тут
        unsigned texwidth, texheight;
        //Декодирование
        unsigned error = lodepng::decode(image, texwidth, texheight, faces[i]);

        //Ошибки
        if(error) std::cout << "DECODER::ERROR " << error << ": " << lodepng_error_text(error) << std::endl;
        
        unsigned char* data = &image[0]; //RGBARGBARGBA...
        
        if (data){
            //Генерация текстуры
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, texwidth, texheight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            //Генерация мипмапов
            glGenerateMipmap(GL_TEXTURE_2D);
        }
        else
        {
            std::cout << "ERROR IN CUBE TEXTURE LOADING" << std::endl;
        }
        
        //Освобождение памяти и отвзяка от изображения
           std::vector<unsigned char>().swap(image);
           glBindTexture(GL_TEXTURE_2D, 0);
    }

    return textureID;
}
