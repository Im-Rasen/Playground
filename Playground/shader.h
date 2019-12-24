#ifndef shader_h
#define shader_h

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <GL/glew.h> // Подключаем glew для того, чтобы получить все необходимые заголовочные файлы OpenGL

class Shader
{
public:
    // Идентификатор программы
    GLuint Program;
    // Конструктор считывает и собирает шейдер
    Shader(const GLchar* vertexPath, const GLchar* fragmentPath){
        
        
        // 1. Получаем исходный код шейдера из filePath
        
        std::string vertexCode;
        std::string fragmentCode;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        //Удостоверимся, что ifstream объекты могут выкидывать исключения
        vShaderFile.exceptions(std::ifstream::failbit);
        fShaderFile.exceptions(std::ifstream::failbit);
        try
        {
            //Открываем файлы
            vShaderFile.open(vertexPath);
            fShaderFile.open(fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            //Считываем данные в потоки
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            //Закрываем файлы
            vShaderFile.close();
            fShaderFile.close();
            //Преобразовываем потоки в массив GLchar
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
        }
        catch(std::ifstream::failure e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const GLchar* vShaderCode = vertexCode.c_str();
        const GLchar* fShaderCode = fragmentCode.c_str();
        
        
        // 2. Сборка шейдеров
        
        GLuint vertex, fragment;
        GLint success; // Для ошибок
        GLchar infoLog[512]; // Для ошибок

        //Вершинный шейдер
        vertex = glCreateShader(GL_VERTEX_SHADER); // Идентификатор
        glShaderSource(vertex, 1, &vShaderCode, NULL); // Привязка исходного кода
        glCompileShader(vertex); // Компиляция
        //Ошибки
        glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(vertex, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
        };

        //Фрагментный шейдер
        fragment = glCreateShader(GL_FRAGMENT_SHADER); // Идентификатор
        glShaderSource(fragment, 1, &fShaderCode, NULL); // Привязка исходного кода
        glCompileShader(fragment);
        //Ошибки
        glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragment, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            std::cout << fragmentPath << std::endl;
        }

        // Шейдерная программа
        this->Program = glCreateProgram();
        glAttachShader(this->Program, vertex);
        glAttachShader(this->Program, fragment);
        glLinkProgram(this->Program);
        //Ошибки
        glGetProgramiv(this->Program, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(this->Program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            std::cout << "ERROR::SHADER " << fragmentPath << std::endl;
        }
        
        glDeleteShader(vertex);
        glDeleteShader(fragment);
    };
    
    // Использование программы
    void Use() { glUseProgram(this->Program); }
    
    
    void setInt(const std::string &name, int value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform1i(nameLocation, value);
    }
    void setBool(const std::string &name, bool value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform1i(nameLocation, value);
    }
    void setFloat(const std::string &name, float value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform1f(nameLocation, value);
    }
    //-------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform2fv(nameLocation, 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform2f(nameLocation, x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform3fv(nameLocation, 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform3f(nameLocation, x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform4fv(nameLocation, 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniform4f(nameLocation, x, y, z, w);
    }
    //-------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniformMatrix2fv(nameLocation, 1, GL_FALSE, &mat[0][0]);
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniformMatrix3fv(nameLocation, 1, GL_FALSE, &mat[0][0]);
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        GLuint nameLocation = glGetUniformLocation(this->Program, name.c_str());
        glUniformMatrix4fv(nameLocation, 1, GL_FALSE, &mat[0][0]);
    }
};

#endif
