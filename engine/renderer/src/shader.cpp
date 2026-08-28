#include "engine/renderer/shader.h"
#include <glad/gl.h>
#include <cstdio>

namespace engine::renderer
{

    static unsigned int CompileStage(unsigned int type, const char *src)
    {
        unsigned int id = glCreateShader(type);
        glShaderSource(id, 1, &src, nullptr);
        glCompileShader(id);

        int success = 0;
        glGetShaderiv(id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetShaderInfoLog(id, 512, nullptr, log);
            std::printf("[ERROR] Shader compile failed: %s\n", log);
        }
        return id;
    }

    Shader::Shader(const char *vertexSrc, const char *fragmentSrc)
    {
        unsigned int vs = CompileStage(GL_VERTEX_SHADER, vertexSrc);
        unsigned int fs = CompileStage(GL_FRAGMENT_SHADER, fragmentSrc);

        m_id = glCreateProgram();
        glAttachShader(m_id, vs);
        glAttachShader(m_id, fs);
        glLinkProgram(m_id);

        int success = 0;
        glGetProgramiv(m_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char log[512];
            glGetProgramInfoLog(m_id, 512, nullptr, log);
            std::printf("[ERROR] Shader link failed: %s\n", log);
        }

        glDeleteShader(vs);
        glDeleteShader(fs);
    }

    Shader::~Shader()
    {
        glDeleteProgram(m_id);
    }

    void Shader::Bind() const
    {
        glUseProgram(m_id);
    }

} // namespace engine::renderer
