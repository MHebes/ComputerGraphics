#include <cassert>
#include <cstdio>
#include <fstream>
#include <iostream>
#include <limits>
#include <sstream>

#include "PPM.h"

std::istream& cmt(std::istream& is)
{
    is >> std::ws;
    while (is.peek() == '#') {
        // advance to EOL or EOF
        is.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        is >> std::ws;  // skip leading whitespace
    }

    return is;
}

// scales x from range [0, a] to range [0, b]
size_t scale(const size_t x, const size_t a, size_t b)
{
    return b * ((float)x) / ((float)a);
}

// Constructor loads a filename with the .ppm extension
PPM::PPM(std::string fileName) : m_PixelData(nullptr)
{
    std::ifstream infile(fileName);

    if (!infile) {
        std::cout << "Error: could not open input file" << std::endl;
        exit(1);
    }
    
    // read header
    std::string magic;
    infile >> magic; // TODO this won't allow things like P3#comment at the start

    if (magic.compare(PPM_MAGIC) != 0) {
        std::cout << "Error: not a supported file format" << std::endl;
        exit(1);
    }

    // read width, height
    infile >> cmt >> m_width >> cmt >> m_height;

    m_PixelData = new unsigned char[m_width * m_height * 3];

    // read max value
    size_t max;
    infile >> cmt >> max;

    // read data
    for (int y = 0; y < m_height; y++) {
        for (int x = 0; x < m_width; x++) {
            size_t r, g, b;
            infile >> cmt >> r >> cmt >> g >> cmt >> b;
            // TODO error handling when no data

            // scale from maximum to PPM_MAX
            r = scale(r, max, PPM_MAX);
            g = scale(g, max, PPM_MAX);
            b = scale(b, max, PPM_MAX);
            setPixel(x, y, r, g, b);
        }
    }
}

// Destructor clears any memory that has been allocated
PPM::~PPM() { delete[] m_PixelData; }

// Saves a PPM Image to a new file.
void PPM::savePPM(std::string outputFileName) const
{
    assert(m_PixelData);  // invariant

    std::ofstream outfile(outputFileName);

    if (!outfile) {
        std::cout << "Error: could not open output file" << std::endl;
        exit(1);
    }

    outfile << *this;
}

// Darken subtracts 50 from each of the red, green
// and blue color components of all of the pixels
// in the PPM. Note that no values may be less than
// 0 in a ppm.
void PPM::darken()
{
    for (size_t i = 0; i < m_width * m_height * 3; i++) {
        m_PixelData[i] =
            (m_PixelData[i] <= PPM_DARKEN_AMT ? 0 : m_PixelData[i] - 50);
    }
}

unsigned char* PPM::getPixel(size_t x, size_t y) const
{
    return &m_PixelData[3 * (y * m_width + x % m_width)];
}

// Sets a pixel to a specific R,G,B value
void PPM::setPixel(size_t x, size_t y, unsigned char R, unsigned char G,
                   unsigned char B)
{
    size_t basei = 3 * (y * m_width + x % m_width);
    m_PixelData[basei] = R;
    m_PixelData[basei + 1] = G;
    m_PixelData[basei + 2] = B;
}
