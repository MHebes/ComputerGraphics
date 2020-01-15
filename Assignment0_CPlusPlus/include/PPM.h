/** @file PPM.h
 *  @brief Class for working with PPM images
 *
 *  Class for working with P3 PPM images specifically.
 *
 *  @author Michael Hebert
 *  @bug No known bugs.
 */
#ifndef PPM_H
#define PPM_H

#include <iostream>
#include <string>

static const char* PPM_MAGIC = "P3";
static const size_t PPM_MAX = 255;
static const int PPM_PIXELS_PER_LINE = 8;
static const unsigned char PPM_DARKEN_AMT = 50;

class PPM {
public:
    // Constructor loads a filename with the .ppm extension
    PPM(std::string fileName);

    // Destructor clears any memory that has been allocated
    ~PPM();

    // overload << operator to print to streams
    inline friend std::ostream& operator<<(std::ostream& os, const PPM& ppm)
    {
        os << "P3" << std::endl;
        os << ppm.getWidth() << ' ' << ppm.getHeight() << std::endl;
        os << PPM_MAX << std::endl;

        for (int y = 0; y < ppm.getHeight(); y++) {
            for (int x = 0; x < ppm.getWidth(); x++) {
                unsigned char* pixel = ppm.getPixel(x, y);
                os << +pixel[0] << " " << +pixel[1] << " " << +pixel[2];

                // add newline if at max, or space between them
                if (x % PPM_PIXELS_PER_LINE == PPM_PIXELS_PER_LINE - 1) os << std::endl;
                else if (x < ppm.getWidth() - 1) os << "  ";
            }
            os << std::endl;
        }

        return os;
    }

    // Saves a PPM Image to a new file.
    void savePPM(std::string outputFileName) const;

    // Darken subtracts 50 from each of the red, green
    // and blue color components of all of the pixels
    // in the PPM. Note that no values may be less than
    // 0 in a ppm.
    void darken();

    // Sets a pixel to a specific R,G,B value
    void setPixel(size_t x, size_t y, unsigned char R, unsigned char G,
                  unsigned char B);

    unsigned char* getPixel(size_t x, size_t y) const;

    // Returns the raw pixel data in an array.
    // You may research what 'inline' does.
    inline unsigned char* pixelData() const { return m_PixelData; }

    // Returns image width
    inline size_t getWidth() const { return m_width; }

    // Returns image height
    inline size_t getHeight() const { return m_height; }

    // NOTE:    You may add any helper functions you like in the
    //          private section.
private:
    // Store the raw pixel data here
    // Data is R,G,B format row-major order
    unsigned char* m_PixelData;

    // Store width and height of image.
    size_t m_width{0};
    size_t m_height{0};
};

#endif
