#pragma once
#include <string>

namespace engine::renderer {

class Shader {
public:
    Shader(const char* vertexSrc, const char* fragmentSrc);
    ~Shader();

    void Bind() const;
    unsigned int Id() const { return m_id; }

private:
    unsigned int m_id = 0;
};

} // namespace engine::renderer
