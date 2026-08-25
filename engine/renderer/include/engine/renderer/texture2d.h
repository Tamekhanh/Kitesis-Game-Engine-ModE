#pragma once
#include <string>

namespace engine::renderer {

class Texture2D {
public:
    explicit Texture2D(const std::string& path);
    ~Texture2D();

    void Bind(unsigned int slot = 0) const;

    int Width() const { return m_width; }
    int Height() const { return m_height; }

private:
    unsigned int m_id = 0;
    int m_width = 0;
    int m_height = 0;
};

} // namespace engine::renderer