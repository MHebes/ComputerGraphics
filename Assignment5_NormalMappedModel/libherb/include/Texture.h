#pragma once

#include <string>

class Texture {
    public:
    Texture();

    ~Texture();

    void LoadTexture(const std::string filename);

    void Bind(unsigned int slot=0) const;

    void Unbind();

    void loadPpm(bool flip);

    int getWidth() { return m_width; }
    int getHeight() { return m_height; }

    int getBpp() { return m_bpp; }

    void setPixel(int x, int y, int r, int g, int b);

    void printPixels();
protected:
    unsigned int m_width;
    unsigned int m_height;
    char m_bpp;
};